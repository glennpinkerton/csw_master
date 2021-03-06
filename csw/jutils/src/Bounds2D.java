
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;

/**
 * 2D bounding values.
 */
public class Bounds2D {

    private final double DEFAULT_VALUE = 1.e30;
    private final double DD_TINY = 1.e-10;

    private double minX = DEFAULT_VALUE,
                   minY = DEFAULT_VALUE,
                   maxX = -DEFAULT_VALUE,
                   maxY = -DEFAULT_VALUE;


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
        this.minX = Math.min(minX, maxX);
        this.minY = Math.min(minY, maxY);
        this.maxX = Math.max(minX, maxX);
        this.maxY = Math.max(minY, maxY);
        correctZeroDimensions ();
    }

  /**
   * Constructor from two arrays.
   *
   * @param xa array of x values.
   * @param ya array of y values.
   */
    public Bounds2D(double[] xa, double[] ya)  throws IllegalArgumentException
    {

        if (xa == null  ||  ya == null) {
            throw new IllegalArgumentException("Bounds2D array is null");
        }
        if (xa.length != ya.length) {
            throw new IllegalArgumentException("Bounds2D array lengths differ");
        }
        if (xa.length < 2) {
            throw new IllegalArgumentException("Bounds2D array with < 2 points");
        }

        for (int index=0; index<xa.length; index++) {

            if (xa[index] > -1.e20  &&  xa[index] < 1.e20) {
                if (xa[index] < minX) minX = xa[index];
                if (xa[index] > maxX) maxX = xa[index];
            }

            if (ya[index] > -1.e20  &&  ya[index] < 1.e20) {
                if (ya[index] < minY) minY = ya[index];
                if (ya[index] > maxY) maxY = ya[index];
            }

        }
        correctZeroDimensions ();

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
    public Bounds2D(XYZPolyline polyline)  throws IllegalArgumentException
    {
        if (polyline == null) {
            throw new IllegalArgumentException("Bounds2D XYZPolyline is null");
        }
        if (polyline.size() < 2) {
            throw new IllegalArgumentException
              ("Bounds2D XYZPolyline has less than 2 points");
        }

        reset ();
        this.minX = Math.min(minX, polyline.getMinX());
        this.maxX = Math.max(maxX, polyline.getMaxX());
        this.minY = Math.min(minY, polyline.getMinY());
        this.maxY = Math.max(maxY, polyline.getMaxY());
        correctZeroDimensions ();
    }


  /**
   * If minX is equal to maxX or minY is equal to maxY,
   * modify the problematic values to be sane.  The min is
   * lowered a little and the max is raised a little, with
   * the "little" value determined by any non problematic
   * values of min and max.
   */
    private void correctZeroDimensions ()
    {
        double   dx = maxX - minX;
        double   dy = maxY - minY;

        if (dx <= 0.0) {
            double  ddx;
            if (dy <= 0.0) {
                ddx = maxX / 100000.0;
            }
            else {
                ddx = dy / 100000.0;
            }
            if (ddx < 0.0) ddx = -ddx;
            if (ddx < DD_TINY) ddx = DD_TINY;
            minX -= ddx;
            maxX += ddx;
        }
        if (dy <= 0.0) {
            double  ddy;
            if (dx <= 0.0) {
                ddy = maxY / 100000.0;
            }
            else {
                ddy = dx / 100000.0;
            }
            if (ddy < 0.0) ddy = -ddy;
            if (ddy < DD_TINY) ddy = DD_TINY;
            minY -= ddy;
            maxY += ddy;
        }

    }



  /**
   * Updates the bounding x,y values from the {@link XYZPolyline}.  More
   * specifically, if any x,y values in the {@link XYZPolyline} lie outside
   * of the existing values for the Bounds2D, the XYZPolyline values will
   * be used.
   *
   * @param polyline {@link XYZPolyline} used to update bounds.
   */
    public void expandTo(XYZPolyline polyline)
      throws  IllegalArgumentException
    {
        if (polyline == null) {
            throw new IllegalArgumentException("Bounds2D XYZPolyline is null");
        }
        if (polyline.size() < 2) {
            throw new IllegalArgumentException
              ("Bounds2D XYZPolyline has less than 2 points");
        }

        reset ();
        minX = Math.min(minX, polyline.getMinX());
        maxX = Math.max(maxX, polyline.getMaxX());
        minY = Math.min(minY, polyline.getMinY());
        maxY = Math.max(maxY, polyline.getMaxY());
        correctZeroDimensions ();
    }

  /**
   * sets all values back to the defaults
   */
    private void reset()
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
        this.minX = Math.min(minX, maxX);
        this.minY = Math.min(minY, maxY);
        this.maxX = Math.max(minX, maxX);
        this.maxY = Math.max(minY, maxY);
        correctZeroDimensions ();
    }

  /**
   *
   * @param pct the percentage to expand.
   */
    public void expandByPercentage(double pct)
    {
        double factor = pct / 200.0;

        double xFactor = getWidth() * factor;
        double yFactor = getHeight() * factor;

        minX -= xFactor;
        minY -= yFactor;
        maxX += xFactor;
        maxY += yFactor;
        correctZeroDimensions ();

        return;
    }

  /**
   * @param xpct the percentage to expand the width.
   * @param ypct the percentage to expand the height.
   */
    public void expandByPercentage(double xpct, double ypct)
    {
        double xfactor = xpct / 200.0;
        double yfactor = ypct / 200.0;

        double xFactor = getWidth() * xfactor;
        double yFactor = getHeight() * yfactor;

        minX -= xFactor;
        minY -= yFactor;
        maxX += xFactor;
        maxY += yFactor;
        correctZeroDimensions ();

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
