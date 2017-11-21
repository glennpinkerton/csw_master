
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
 * 3D bounding values.
 */
public class Bounds3D {

    private double minX, minY, maxX, maxY, minZ, maxZ;
	private final double DEFAULT_VALUE = 1.e30;

  /*
   * Instance initializer--start with inverted ranges.
   */
    {
        reset();
    }

  /**
   * Default constructor.
   */
    public Bounds3D() {
    }

  /**
   * Constructor.
   *
   * @param minX minimum X value.
   * @param minY minimum Y value.
   * @param minZ minimum Z value.
   * @param maxX maximum X value.
   * @param maxY maximum Y value.
   * @param maxZ maximum Z value.
   */
    public Bounds3D( double minX, double minY, double minZ,
                     double maxX, double maxY, double maxZ)
    {
        this.minX = minX;
        this.minY = minY;
        this.minZ = minZ;
        this.maxX = maxX;
        this.maxY = maxY;
        this.maxZ = maxZ;
    }

  /**
   * Constructor from three arrays.
   *
   * @param xs array of xs.
   * @param ys array of ys.
   * @param zs array of ys.
   */
    public Bounds3D(double[] xs, double[] ys, double zs[])
    {

        if (xs.length != ys.length || xs.length != zs.length)
            throw new IllegalArgumentException("Bounds3D array lengths differ");

        for (int index=0; index<xs.length; index++) {

            if (xs[index] > -1.e20  &&  xs[index] < 1.e20) {
                if (xs[index] < minX) minX = xs[index];
                if (xs[index] > maxX) maxX = xs[index];
            }

            if (ys[index] > -1.e20  &&  ys[index] < 1.e20) {
                if (ys[index] < minY) minY = ys[index];
                if (ys[index] > maxY) maxY = ys[index];
            }

            if (zs[index] > -1.e20  &&  zs[index] < 1.e20) {
                if (zs[index] < minZ) minZ = zs[index];
                if (zs[index] > maxZ) maxZ = zs[index];
            }

        }

    }

  /**
   * Copy constructor.
   * @param bounds Bounds3D from which to copy.
   */
    public Bounds3D(Bounds3D bounds)
    {
        this.minX = bounds.minX;
        this.minY = bounds.minY;
        this.minZ = bounds.minZ;
        this.maxX = bounds.maxX;
        this.maxY = bounds.maxY;
        this.maxZ = bounds.maxZ;
    }

  /**
   * Constructor from an {@link XYZPolyline}.
   * @param polyline the XYZPolyline from which to construct a new Bounds3D.
   */
    public Bounds3D(XYZPolyline polyline)
    {
        this.minX = polyline.getMinX();
        this.minY = polyline.getMinY();
        this.minZ = polyline.getMinZ();
        this.maxX = polyline.getMaxX();
        this.maxY = polyline.getMaxY();
        this.maxZ = polyline.getMaxZ();
    }

  /**
   * Updates the bounding x,y,z values from the {@link XYZPolyline}.  More
   * specifically, if any x,y,z values in the {@link XYZPolyline} lie outside
   * of the existing values for the Bounds3D, the XYZPolyline values will
   * be used.
   *
   * @param polyline {@link XYZPolyline} used to update bounds.
   */
    public void expandTo(XYZPolyline polyline)
    {
        if (polyline == null) return;
        minX = Math.min(minX, polyline.getMinX());
        maxX = Math.max(maxX, polyline.getMaxX());
        minY = Math.min(minY, polyline.getMinY());
        maxY = Math.max(maxY, polyline.getMaxY());
        minZ = Math.min(minZ, polyline.getMinZ());
        maxZ = Math.max(maxZ, polyline.getMaxZ());
    }

  /**
   * Clears out all values.
   */
    public void reset()
    {
        minX = DEFAULT_VALUE;
        minY = DEFAULT_VALUE;
        minZ = DEFAULT_VALUE;
        maxX = -DEFAULT_VALUE;
        maxY = -DEFAULT_VALUE;
        maxZ = -DEFAULT_VALUE;
    }

  /**
   * Sets the bounding x,y values explicitly, regardless of whether these
   * parameters lie inside or outside the existing x,y ranges.
   *
   * @param minX new value for minX.
   * @param minY new value for minY.
   * @param minZ new value for minZ.
   * @param maxX new value for maxX.
   * @param maxY new value for maxY.
   * @param maxZ new value for maxZ.
   */
    public void setValues(double minX, double minY, double minZ,
                          double maxX, double maxY, double maxZ)
    {
        this.minX = minX;
        this.minY = minY;
        this.minZ = minZ;
        this.maxX = maxX;
        this.maxY = maxY;
        this.maxZ = maxZ;
    }

  /**
   * Expand the bounds by a percentage.  Each dimension is expanded by
   * the specified percentage of itself.
   * For example, passing in 10.0 for the
   * pct parameter will change extents from [0.0, 0.0, 0.0, 100.0, 200.0, 300.0] to
   * [-5.0, -10.0, -15.0, 105.0, 210.0, 315.0].  For a positive pactor, the limits
   * are expanded.  For a negative factor, the limits are shrunk.
   *
   * @param pct the percentage to expand.
   */
    public void expandByPercentage(double pct)
    {
        double factor = pct / 100.0;

        factor *= 0.5;

        double xFactor = getWidth() * factor;
        double yFactor = getHeight() * factor;
        double zFactor = getDepth() * factor;

        minX -= xFactor;
        minY -= yFactor;
        minZ -= zFactor;
        maxX += xFactor;
        maxY += yFactor;
        maxZ += zFactor;

        return;
    }

  /**
   * @return minX (value of minimum x)
   */
    public double getMinX()
    {
        return minX;
    }

  /**
   * @return maxX (value of maximum x)
   */
    public double getMaxX()
    {
        return maxX;
    }

  /**
   * @return minY (value of minimum y)
   */
    public double getMinY()
    {
        return minY;
    }

  /**
   * @return maxY (value of maximum y)
   */
    public double getMaxY()
    {
        return maxY;
    }

  /**
   * @return minZ (value of minimum z)
   */
    public double getMinZ()
    {
        return minZ;
    }

  /**
   * @return maxZ (value of maximum y)
   */
    public double getMaxZ()
    {
        return maxZ;
    }

  /**
   * @return width (maxX - minX)
   */
    public double getWidth()
    {
        return(maxX - minX);
    }

  /**
   * @return height (maxY - minY)
   */
    public double getHeight()
    {
        return(maxY - minY);
    }

  /**
   * @return depth (maxZ - minZ)
   */
    public double getDepth()
    {
        return(maxZ - minZ);
    }

}
