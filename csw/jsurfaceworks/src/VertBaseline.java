
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;

/**
 This class holds the data needed to rotate a plane so its coordinates coordinates
 are fairly flat.  The data are the 3 coefficients of the plane and the xyz origin
 of the rotation.
*/


public class VertBaseline {

    double c1, c2, c3;
    double x0, y0, z0;
    boolean  used;

/*
 * The constructor has package scope, so only JSurfaceWorks or
 * its peers can create an instance.
 */
    VertBaseline ()
    {
        c1 = 1.e30;
        c2 = 1.e30;
        c3 = 1.e30;
        used = false;
        x0 = 1.e30;
        y0 = 1.e30;
        z0 = 1.e30;
    }

    VertBaseline (VertBaseline copyFrom)
    {
      c1 = copyFrom.c1;
      c2 = copyFrom.c2;
      c3 = copyFrom.c3;
      x0 = copyFrom.x0;
      y0 = copyFrom.y0;
      z0 = copyFrom.z0;
      used = copyFrom.used;
    }

/**
 Get the first coeficient of the plane used to rotate the points.
 If this baseline is not to be used, the value is 1.e30.
*/
    public double getC1 ()
    {
        return c1;
    }

/**
 Get the second coeficient of the plane used to rotate the points.
 If this baseline is not to be used, the value is 1.e30.
*/
    public double getC2 ()
    {
        return c2;
    }

/**
 Get the third coeficient of the plane used to rotate the points.
 If this baseline is not to be used, the value is 1.e30.
*/
    public double getC3 ()
    {
        return c3;
    }

/**
 Get the x coordinate of the rotation origin.
 If this baseline is not to be used, the value is 1.e30.
*/
    public double getX0 ()
    {
        return x0;
    }

/**
 Get the flag for using the baseline or not.  If the flag is false, this tells
 the {@link JSurfaceWorks}
 calculations that the baseline has been calculated, but no vertical surface
 handling is needed.  If the flag is true, the calculations will use the
 endpoints of the baseline in handling "vertical" surfaces.
*/
    public boolean isUsed ()
    {
        return used;
    }

/**
 Get the y coordinate of the rotation origin.
 same as the x,y range.
*/
    public double getY0 ()
    {
        return y0;
    }

/**
 Get the x coordinate of the rotation origin.
 Get the scale factor used for scaling the z range to be roughly the
 same as the x,y range.
*/
    public double getZ0 ()
    {
        return z0;
    }

}
