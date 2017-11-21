
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.awt.Color;

/**
 This class has the data for a particular horizon patch.  All the data
 needed to draw the horizon in 3D and to report back a pick of the
 horizon are supplied via instances of this class.
*/
public class J3DHorizonPatch extends J3DSurfacePatch
{

/*
 * members have package scope so they can be directly
 * accessed by the J3DPanel class.
 */
    double          age;

    // Contains the type of line that is being picked on the horizon patch,
    // either J3DLine.POLYLINE_TYPE, J3DLine.POLYGON_TYPE, or 0 for not
    // defined.
    int pickedLineType;

/**
 Create an empty horizon patch object for use in the 3D display.  Default
 values are set for all the
 members.  Unless you fill this with data via the various set calls, the
 horizon will not be drawn.
*/
    public J3DHorizonPatch () {
      super();
      init();
    }

/**
 Constructor that takes arrays of node x,y,z coordinates and
 arrays of triangle node1, node2 and node3 numbers.  This is
 specifically meant for adding patches via the simulation
 results viewer.
 @param nodeX Array of triangle node x coordinates.
 @param nodeY Array of triangle node y coordinates.
 @param nodeZ Array of triangle node z coordinates.
 @param triNode1 Array of node 1 numbers for each triangle.
 @param triNode2 Array of node 2 numbers for each triangle.
 @param triNode3 Array of node 3 numbers for each triangle.
*/
  public J3DHorizonPatch (
    double[] nodeX,
    double[] nodeY,
    double[] nodeZ,
    int[] triNode1,
    int[] triNode2,
    int[] triNode3)
  {
    super (nodeX, nodeY, nodeZ,
           triNode1, triNode2, triNode3);
    init ();
  }

  private void init() {
    age = 0.0;
    this.setGlobalColor(Color.lightGray);
    type = HORIZON;
    pickedLineType = 0;
  }

    /**
    Return the age of the horizon patch.  If this has not been set,
    its value is zero.
     */
    public double getAge() {
        return age;
    }

    /**
    Set the age for the horizon that the patch is part of.  The default value is zero.
     * @param d  The age of the horizon, usually in millions of years.
     */
    public void setAge(double d) {
        age = d;
    }

    /**
     * Sets the type of line that is being picked on the horizon patch,
     * either Line3D.POLYLINE_TYPE or Line3D.POLYGON_TYPE.
     *
     * @param pickedLineType Line3D.POLYLINE_TYPE or Line3D.POLYGON_TYPE
     */
    public void setPickedLineType(int pickedLineType) {
        this.pickedLineType = pickedLineType;
    }

}  // end of class definition
