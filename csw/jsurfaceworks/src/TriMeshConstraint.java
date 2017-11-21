
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
  Store a sequence of points to be used as a constraint line when
  calculating a TriMesh.  The constraint line can have an optional
  flag identifying it.  This flag will be attached to all triangle
  edges that lie on this constraint line.

  @author Glenn Pinkerton
*/
public class TriMeshConstraint {

  static public final int FAULT_CONSTRAINT         = -319;  //use an unlikely number
  static public final int BOUNDARY_CONSTRAINT      = -419;  //use an unlikely number
  static public final int CONTOUR_CONSTRAINT       = -519;  //use an unlikely number

/*
 * Arrays with node by node information.
 */
  protected double[] xArray;
  protected double[] yArray;
  protected double[] zArray;
  protected int numPoints;
  protected int flagValue;

/**
 Build an empty TriMeshConstraint object.
*/
  public TriMeshConstraint () {
    numPoints = 0;
    flagValue = 0;
    xArray = null;
    yArray = null;
    zArray = null;
  }

/**
 Build a TriMeshConstraint object with the specified points and a zero flag.
*/
  public TriMeshConstraint (double[] x, double[] y, double[] z, int n)
  {
    xArray = x;
    yArray = y;
    zArray = z;

    numPoints = n;
    flagValue = 0;
  }

/**
 Build a TriMeshConstraint object with the specified points and flag.
*/
  public TriMeshConstraint (double[] x, double[] y, double[] z, int n, int flag)
  {
    xArray = x;
    yArray = y;
    zArray = z;

    numPoints = n;
    flagValue = flag;
  }


  

/**
 Add point data to the trimesh constraint.
*/
  public void setPointArrays (double[] x,
                              double[] y,
                              double[] z,
                              int n)
  {
    xArray = x;
    yArray = y;
    zArray = z;
    numPoints = n;
  }

/**
 Set the flag value in the trimesh constraint.
*/
  public void setFlag (int flag)
  {
    flagValue = flag;
  }

/**
 Return a reference to the node x coordinates array.
*/
  public double[] getXArray ()
  {
    return xArray;
  }

/**
 Return a reference to the node y coordinates array.
*/
  public double[] getYArray ()
  {
    return yArray;
  }

/**
 Return a reference to the node z coordinates array.
*/
  public double[] getZArray ()
  {
    return zArray;
  }

/**
 Return the number of points.
*/
  public int getNumPoints ()
  {
    return numPoints;
  }

/**
 Return the current flag set for the constraint.
*/
  public int getFlag ()
  {
    return flagValue;
  }

}  // end of TriMeshConstraint class
