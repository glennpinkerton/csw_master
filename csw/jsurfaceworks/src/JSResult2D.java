
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
 This is an extremely simple 
 class, having just an x array, a y array and the number of points.
 The application code can use these data to populate data structures
 appropriately.  The public cannot create an instance of this class
 directly.  The public can only access the results created by various
 JSurfaceWorks methods.
*/
public class JSResult2D {


  double[] x;
  double[] y;
  int      npts;

  JSResult2D () {
  }

/*------------------------------------------------------------------------------------*/

/**
Return the result x array, already filled in.
*/
  public double[] getXArray ()
  {
    return x;
  }

/**
Return the result y array, already filled in.
*/
  public double[] getYArray ()
  {
    return y;
  }

/**
Return the number of points in the result x and y arrays.
*/
  public int getNpts ()
  {
    return npts;
  }

}  // end of JSResult2D class
