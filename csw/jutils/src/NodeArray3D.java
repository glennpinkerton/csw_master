
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
 * Stores x, y, z values for 3d points.  Used for input to grid
 * and trimesh calculation, amomg other things.
 */
public class NodeArray3D {

  public double[] xArray = null;
  public double[] yArray = null;
  public double[] zArray = null;
  public int[] flagArray = null;

  /**
   * Constructor
   * @param size initial size
   */
  public NodeArray3D(int size) {
    init(size);
  }

  /**
   * Constructor
   * @param x array of x values
   * @param y array of y values
   * @param z array of z values
   */
  public NodeArray3D(double[] x, double[] y, double[] z) {
    if (x.length != y.length || x.length != z.length || y.length != z.length)
      throw new IllegalArgumentException(
        "x, y, and z array lengths are different"
      );
    copyArrays(x.length, x, y, z, null);
  }

  /**
   * Copy constructor.
   *
   * @param na {@link NodeArray3D} from which to copy.
   */
  public NodeArray3D (NodeArray3D na) {
    copyArrays(na.size(), na.xArray, na.yArray, na.zArray, na.flagArray);
  }

  private void copyArrays(
    int size,
    double xs[],
    double ys[],
    double zs[],
    int flags[]
  ) {
    init(size);
    System.arraycopy(xs, 0, xArray, 0, size);
    System.arraycopy(ys, 0, yArray, 0, size);
    System.arraycopy(zs, 0, zArray, 0, size);
    if (flags != null) {
      flagArray = new int[size];
      System.arraycopy(flags, 0, flagArray, 0, size);
    }
  }

  private void init(int size) {
    xArray = new double[size];
    yArray = new double[size];
    zArray = new double[size];
	MathUtils.fill (xArray, 1.e30);
	MathUtils.fill (yArray, 1.e30);
	MathUtils.fill (zArray, 1.e30);
  }

  /**
  Copy the specified x, y and z arrays into this NodeArray3D.  Any data previously
  stored in the NodeArray3D will be deleted when this is called.  If the 3 arrays are of
  different lengths, the shortest length is used.  This method is used, for
  example, when a fault line is being added to the model.  The
  points for the fault line are already in arrays, so it is
  easier to just copy them over than to add them one at a time.
   * @param x x array from which to copy
   * @param y y array from which to copy
   * @param z z array from which to copy
  */
  public void copyXYZ(double[] x, double[] y, double[] z) {
    if (x.length != y.length || x.length != z.length || y.length != z.length)
      throw new IllegalArgumentException("x, y, and z array lengths are different");
    copyArrays(x.length, x, y, z, null/*flags*/);
  }

  public void appendXYZ(double[] xs, double[] ys, double[] zs) {
    int origSize = size();
    /*
     * If this NodeArray3D's internal arrays have yet to be initialized,
     * initialize them and just copy the values from the NodeArray
     * passed in.
     */
    if (origSize <= 0 || xArray == null) {
      int size = xs.length;
      init(size);
      for (int i = 0; i < size; i++) {
        xArray[i] = xs[i];
        yArray[i] = ys[i];
        zArray[i] = zs[i];
      }
    } else {
      double[] xArrayCopy = new double[origSize];
      double[] yArrayCopy = new double[origSize];
      double[] zArrayCopy = new double[origSize];
      System.arraycopy(xArray, 0, xArrayCopy, 0, origSize);
      System.arraycopy(yArray, 0, yArrayCopy, 0, origSize);
      System.arraycopy(zArray, 0, zArrayCopy, 0, origSize);
      int addedSize = xs.length;
      int totalSize = addedSize + origSize;
      xArray = new double[totalSize];
      yArray = new double[totalSize];
      zArray = new double[totalSize];
      System.arraycopy(xArrayCopy, 0, xArray, 0, origSize);
      System.arraycopy(yArrayCopy, 0, yArray, 0, origSize);
      System.arraycopy(zArrayCopy, 0, zArray, 0, origSize);
      System.arraycopy(xs, 0, xArray, origSize, addedSize);
      System.arraycopy(ys, 0, yArray, origSize, addedSize);
      System.arraycopy(zs, 0, zArray, origSize, addedSize);
    }
  }

  /**
   * Append points to the end of the NodeArray3D, maintaining original
   * order of points in each NodeArray3D.
   *
   * @param na the NodeArray to append.
   */
  public void appendNodeArray(NodeArray3D na) {
    int origSize = size();
    int appendSize = na.size();
    int totalSize = appendSize + origSize;

    appendXYZ(na.xArray, na.yArray, na.zArray);

    // handle flag array
    if (na.flagArray != null) {
      if (flagArray == null) {
        flagArray = new int[appendSize];
        for (int i = 0; i < appendSize; i++) {
          flagArray[i] = na.flagArray[i];
        }
      } else {
        /*
         * Only if flagArray of BOTH NodeArray3Ds are non-null do we append
         * the flagArray values.  In other words, if the existing NodeArrays
         * (this) flagArray is null but the appending NodeArrays (na)
         * flagArray is non-null, appending those flags makes no sense.
         *
         * The opposite case is worthy of an exception, because if
         * the existing NodeArray has a valid flagArray but the appending
         * NodeArray does not, that would effectively invalidate the
         * existing NodeArrays flagArray.
         */
        if (flagArray != null) {
          if (na.flagArray == null)
            throw new IllegalStateException(
              "Attempting to append NodeArray whose flags are null to a "+
              "NodeArray whose flags are non-null"
            );
          double[] flagArrayCopy = new double[origSize];
          System.arraycopy(flagArray, 0, flagArrayCopy, 0, origSize);
          flagArray = new int[totalSize];
          System.arraycopy(flagArrayCopy, 0, flagArray, 0, origSize);
          System.arraycopy(na.flagArray, 0, flagArray, origSize, appendSize);
        }
      }
    }
  }

  /**
   * Prepend points to the beginning of a NodeArray, maintaining the
   * original order of points in each NodeArray.
   *
   * @param na the NodeArray to append.
   */
  public void prependNodeArray(NodeArray3D na) {
    int origSize = size();
    /*
     * If this NodeArray's internal arrays have yet to be initialized,
     * initialize them and just copy the values from the NodeArray
     * passed in.
     */
    if (origSize <= 0 || xArray == null) {
      init(na.size());
      for(int i = 0; i < na.size(); i++) {
        xArray[i] = na.xArray[i];
        yArray[i] = na.yArray[i];
        zArray[i] = na.zArray[i];
      }
    } else {
      double[] xArrayCopy = new double[origSize];
      double[] yArrayCopy = new double[origSize];
      double[] zArrayCopy = new double[origSize];
      System.arraycopy(xArray, 0, xArrayCopy, 0, origSize);
      System.arraycopy(yArray, 0, yArrayCopy, 0, origSize);
      System.arraycopy(zArray, 0, zArrayCopy, 0, origSize);
      int addedSize = na.size();
      int totalSize = addedSize + origSize;
      xArray = new double[totalSize];
      yArray = new double[totalSize];
      zArray = new double[totalSize];
      System.arraycopy(na.xArray, 0, xArray, 0, addedSize);
      System.arraycopy(na.yArray, 0, yArray, 0, addedSize);
      System.arraycopy(na.zArray, 0, zArray, 0, addedSize);
      System.arraycopy(xArrayCopy, 0, xArray, addedSize, origSize);
      System.arraycopy(yArrayCopy, 0, yArray, addedSize, origSize);
      System.arraycopy(zArrayCopy, 0, zArray, addedSize, origSize);
    }
  }

  public void appendXYZ(double x, double y, double z) {
    double[] xs = new double[1];
    double[] ys = new double[1];
    double[] zs = new double[1];
    xs[0] = x;
    ys[0] = y;
    zs[0] = z;
    appendXYZ(xs, ys, zs);
  }

  public double[] getXArray(){ return xArray; }
  public double[] getYArray(){ return yArray; }
  public double[] getZArray(){ return zArray; }
  public int[] getFlagArray() {
    return(flagArray);
  }

  /**
   * @return number of points contained in the array.
   */
  public int size(){
    if (xArray == null)
      return(0);
    return xArray.length;
  }

  /**
   * Initialize all flag values with value.
   * @param value value to initialize all flags.
   */
  public void initializeFlags(int value) {
    if (flagArray == null)
      flagArray = new int[size()];
    MathUtils.fill(flagArray, value);
  }

  /**
   * Sets a flag for a node with a value.
   * @param index index of node.
   * @param value value for flag at node.
   */
  public void setFlag(int index, int value) {
    if (index < 0 || index >= size())
      throw new IllegalArgumentException(
        "index "+index+" out of range [0-"+size()+"]"
      );
    if (flagArray == null)
      flagArray = new int[size()];
    flagArray[index] = value;
  }

  /**
   * Get flag value for node at index.
   * @param index index of node.
   * @return flag value for node at index.
   */
  public int getFlag(int index) {
    if (flagArray == null)
      return(-1);
    if (index < 0 || index >= size())
      throw new IllegalArgumentException("index "+index+" out of range [0-"+size()+"]");
    return(flagArray[index]);
  }

  public double getMinX() {
    return(MathUtils.getMin(xArray));
  }
  public double getMinY() {
    return(MathUtils.getMin(yArray));
  }
  public double getMinZ() {
    return(MathUtils.getMin(zArray));
  }

  public double getMaxX() {
    return(MathUtils.getMax(xArray));
  }
  public double getMaxY() {
    return(MathUtils.getMax(yArray));
  }
  public double getMaxZ() {
    return(MathUtils.getMax(zArray));
  }

}
