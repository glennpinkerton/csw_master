
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
 * 2D bounding values.
 */
public class Bounds2D {

    private double minX, minY, maxX, maxY;
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
    public Bounds2D() {
    }

  /**
   * Constructor.
   *
   * @param minX minimum X value.
   * @param minY minimum Y value.
   * @param maxX maximum X value.
   * @param maxY maximum Y value.
   */
    public Bounds2D( double minX, double minY,
                     double maxX, double maxY)
    {
        this.minX = minX;
        this.minY = minY;
        this.maxX = maxX;
        this.maxY = maxY;
    }

  /**
   * Constructor from three arrays.
   *
   * @param xs array of xs.
   * @param ys array of ys.
   */
    public Bounds2D(double[] xs, double[] ys)
    {

        if (xs.length != ys.length)
            throw new IllegalArgumentException("Bounds2D array lengths differ");

        for (int index=0; index<xs.length; index++) {

            if (xs[index] > -1.e20  &&  xs[index] < 1.e20) {
                if (xs[index] < minX) minX = xs[index];
                if (xs[index] > maxX) maxX = xs[index];
            }

            if (ys[index] > -1.e20  &&  ys[index] < 1.e20) {
                if (ys[index] < minY) minY = ys[index];
                if (ys[index] > maxY) maxY = ys[index];
            }

        }

    }

  /**
   * Copy constructor.
   * @param bounds Bounds2D from which to copy.
   */
    public Bounds2D(Bounds2D bounds)
    {
        this.minX = bounds.minX;
        this.minY = bounds.minY;
        this.maxX = bounds.maxX;
        this.maxY = bounds.maxY;
    }

  /**
   * Constructor from an {@link XYZPolyline}.
   * @param polyline the XYZPolyline from which to construct a new Bounds2D.
   */
    public Bounds2D(XYZPolyline polyline)
    {
        this.minX = polyline.getMinX();
        this.minY = polyline.getMinY();
        this.maxX = polyline.getMaxX();
        this.maxY = polyline.getMaxY();
    }

  /**
   * Updates the bounding x,y,z values from the {@link XYZPolyline}.  More
   * specifically, if any x,y,z values in the {@link XYZPolyline} lie outside
   * of the existing values for the Bounds2D, the XYZPolyline values will
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
    }

  /**
   * Clears out all values.
   */
    public void reset()
    {
        minX = DEFAULT_VALUE;
        minY = DEFAULT_VALUE;
        maxX = -DEFAULT_VALUE;
        maxY = -DEFAULT_VALUE;
    }

  /**
   * Sets the bounding x,y values explicitly, regardless of whether these
   * parameters lie inside or outside the existing x,y ranges.
   *
   * @param minX new value for minX.
   * @param minY new value for minY.
   * @param maxX new value for maxX.
   * @param maxY new value for maxY.
   */
    public void setValues(double minX, double minY,
                          double maxX, double maxY)
    {
        this.minX = minX;
        this.minY = minY;
        this.maxX = maxX;
        this.maxY = maxY;
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

        minX -= xFactor;
        minY -= yFactor;
        maxX += xFactor;
        maxY += yFactor;

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

}
