
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
 * 3D bounding values.
 */
public class Bounds3D {

    private final double DEFAULT_VALUE = 1.e30;
    private final double DD_TINY = 1.e-10;

    private double minX = DEFAULT_VALUE,
                   minY = DEFAULT_VALUE,
                   minZ = DEFAULT_VALUE,
                   maxX = -DEFAULT_VALUE,
                   maxY = -DEFAULT_VALUE,
                   maxZ = -DEFAULT_VALUE;

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
        this.minX = Math.min (minX, maxX);
        this.minY = Math.min (minY, maxY);
        this.minZ = Math.min (minZ, maxZ);
        this.maxX = Math.max (maxX, minX);
        this.maxY = Math.max (maxY, minY);
        this.maxZ = Math.max (maxZ, minZ);
        correctZeroDimensions ();
    }

  /**
   * Constructor from three arrays.
   *
   * @param xa array of x values.
   * @param ya array of y values.
   * @param za array of z values.
   */
    public Bounds3D(double[] xa, double[] ya, double za[])
        throws  IllegalArgumentException
    {

        if (xa == null  ||  ya == null  ||  za == null) {
            throw new IllegalArgumentException("Bounds3D array is null");
        }
        if (xa.length != ya.length || xa.length != za.length) {
            throw new IllegalArgumentException("Bounds3D array lengths differ");
        }
        if (xa.length < 2) {
            throw new IllegalArgumentException("Bounds3D array has < 2 points");
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

            if (za[index] > -1.e20  &&  za[index] < 1.e20) {
                if (za[index] < minZ) minZ = za[index];
                if (za[index] > maxZ) maxZ = za[index];
            }

        }
        correctZeroDimensions ();

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
        throws IllegalArgumentException
    {
        if (polyline == null) {
            throw new IllegalArgumentException("Bounds3D XYZPolyline is null");
        }
        if (polyline.size() < 2) {
            throw new IllegalArgumentException
              ("Bounds3D less than 2 points in XYZPolyline");
        }

        this.minX = polyline.getMinX();
        this.minY = polyline.getMinY();
        this.minZ = polyline.getMinZ();
        this.maxX = polyline.getMaxX();
        this.maxY = polyline.getMaxY();
        this.maxZ = polyline.getMaxZ();
        correctZeroDimensions ();
    }


  /**
   * If minX is equal to maxX or minY is equal to maxY,
   * or minZ is equal to maxZ 
   * modify the problematic values to be sane.  The min is
   * lowered a little and the max is raised a little, with
   * the "little" value determined by any non problematic
   * values of min and max.
   */
    private void correctZeroDimensions ()
    {
        double   dx = maxX - minX;
        double   dy = maxY - minY;
        double   dz = maxZ - minZ;

        if (dx <= 0.0) {
            double  ddx;
            if (dy <= 0.0  &&  dz <= 0.0) {
                ddx = maxX / 100000.0;
            }
            else {
                ddx = (dy + dz) / 100000.0;
            }
            if (ddx < 0.0) ddx = -ddx;
            if (ddx < DD_TINY) ddx = DD_TINY;
            minX -= ddx;
            maxX += ddx;
        }
        if (dy <= 0.0) {
            double  ddy;
            if (dx <= 0.0  &&  dz <= 0.0) {
                ddy = maxY / 100000.0;
            }
            else {
                ddy = (dx + dz) / 100000.0;
            }
            if (ddy < 0.0) ddy = -ddy;
            if (ddy < DD_TINY) ddy = DD_TINY;
            minY -= ddy;
            maxY += ddy;
        }
        if (dz <= 0.0) {
            double  ddz;
            if (dx <= 0.0  &&  dy <= 0.0) {
                ddz = maxZ / 100000.0;
            }
            else {
                ddz = (dx + dy) / 100000.0;
            }
            if (ddz < 0.0) ddz = -ddz;
            if (ddz < DD_TINY) ddz = DD_TINY;
            minY -= ddz;
            maxY += ddz;
        }

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
      throws IllegalArgumentException
    {
        if (polyline == null) {
            throw new IllegalArgumentException
              ("Bounds3D XYZPolyline is null");
        }
        if (polyline.size() < 2) {
            throw new IllegalArgumentException
              ("Bounds3D XYZPolyline has less than 2 points");
        }

        reset ();
        minX = Math.min(minX, polyline.getMinX());
        maxX = Math.max(maxX, polyline.getMaxX());
        minY = Math.min(minY, polyline.getMinY());
        maxY = Math.max(maxY, polyline.getMaxY());
        minZ = Math.min(minZ, polyline.getMinZ());
        maxZ = Math.max(maxZ, polyline.getMaxZ());
        correctZeroDimensions ();
    }

  /**
   * Clears out all values.
   */
    private void reset()
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
        double factor = pct / 200.0;

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
   * @param xpct the percentage to expand the width.
   * @param ypct the percentage to expand the height.
   */
    public void expandByPercentage(double xpct, double ypct, double zpct)
    {
        double xfactor = xpct / 200.0;
        double yfactor = ypct / 200.0;
        double zfactor = zpct / 200.0;

        double xFactor = getWidth() * xfactor;
        double yFactor = getHeight() * yfactor;
        double zFactor = getDepth() * zfactor;

        minX -= xFactor;
        minY -= yFactor;
        minZ -= zFactor;
        maxX += xFactor;
        maxY += yFactor;
        maxZ += zFactor;
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
