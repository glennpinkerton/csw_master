
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
 This class has the data for a particular fault patch.  All the data
 needed to draw the fault in 3D and to report back a pick of the
 fault are supplied via instances of this class.
*/
public class J3DFault extends J3DSurfacePatch
{

/*
 * Members have package scope so they can be directly
 * accessed by the J3DPanel class.
 */
    double          youngestAge,
                    oldestAge,
                    youngestMotionAge,
                    oldestMotionAge;


/**
 Create an empty fault patch object for use in the 3D display.
 Default values are set for all the
 members.  Unless you fill this with data via the various set calls, the
 fault will not be drawn.
*/
    public J3DFault () {
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
  public J3DFault (
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
      youngestAge = 0.0;
      oldestAge = 10000.0;
      youngestMotionAge = 0.0;
      oldestMotionAge = 0.0;

      this.setGlobalColor(new Color (170, 140, 100, 220));

      type = FAULT;
    }

    /**
     */
    public double getOldestAge() {
        return oldestAge;
    }

    /**
     */
    public double getOldestMotionAge() {
        return oldestMotionAge;
    }

    /**
     */
    public double getYoungestAge() {
        return youngestAge;
    }

    /**
     */
    public double getYoungestMotionAge() {
        return youngestMotionAge;
    }

    /**
     * @param d
     */
    public void setOldestAge(double d) {
        oldestAge = d;
    }

    /**
     * @param d
     */
    public void setOldestMotionAge(double d) {
        oldestMotionAge = d;
    }

    /**
     * @param d
     */
    public void setYoungestAge(double d) {
        youngestAge = d;
    }

    /**
     * @param d
     */
    public void setYoungestMotionAge(double d) {
        youngestMotionAge = d;
    }


}  // end of class definition
