
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
  This is a specific type of TriMeshConstraint that is always flagged as a fault.
  It can be used in trimesh or grid calculations.

  @author Glenn Pinkerton
*/
public class FaultConstraint extends TriMeshConstraint {

/**
 Build an empty FaultConstraint object.
*/
  public FaultConstraint () {
    flagValue = TriMeshConstraint.FAULT_CONSTRAINT;
  }

/**
 Build a FaultConstraint object with the specified points.
*/
  public FaultConstraint (double[] x, double[] y, double[] z, int n)
  {
    xArray = x;
    yArray = y;
    zArray = z;

    numPoints = n;
    flagValue = TriMeshConstraint.FAULT_CONSTRAINT;
  }

/**
 Overrides the base class setFlag method to insure the flag is always correct
 for a fault constraint.
*/
  public void setFlag (int flag)
  {
    flagValue = TriMeshConstraint.FAULT_CONSTRAINT;
  }
    
}  // end of FaultConstraint class
