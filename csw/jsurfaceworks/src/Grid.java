
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;

import java.util.ArrayList;

import csw.jutils.src.XYZPolyline;

/**
 * The base class for any grid data class.  This has the minimum data
 * to visualize a grid and any faults used to calculate the grid.
 *
 * @author Glenn Pinkerton
 */
public class Grid {

  int nCols,
      nRows;
  double xMin;
  double width;
  double yMin;
  double height;
  double[] nodeZArray;
  double rotationAngle;
  ArrayList<XYZPolyline> faults;

/*---------------------------------------------------------------------------*/

/**
 Construct an empty grid object.
*/
  public Grid() {
  }

/*---------------------------------------------------------------------------*/

/**
 Construct a grid object with or without faults. Typically, xmin, ymin, height,
 and width should be in program units (meters), fault constraints should be in
 program units; however, grid data can be in user or program units.

 @param data Data for the grid, from lower left to upper right by row.
 @param ncol Number of columns in the data.
 @param nrow Number of rows in the data
 @param xmin X of lower left corner (before rotation).
 @param ymin Y of lower left corner (before rotation).
 @param width Width of a row of the grid (before rotation).
 @param height Height of a column of the grid (before rotation).
 @param angle Rotation angle in degrees, counterclockwise from the positive x axis.
 @param faults  An {@link ArrayList}<XYZPolyline> object containing the fault lines.
*/
  public Grid (
    double[] data,
    int ncol,
    int nrow,
    double xmin,
    double ymin,
    double width,
    double height,
    double angle,
    ArrayList<XYZPolyline> faults
  )
  {
    setData (data,
             ncol,
             nrow,
             xmin,
             ymin,
             width,
             height,
             angle,
             faults);
  }

/*---------------------------------------------------------------------------*/

  /**
   * Copy constructor
   * @param copyGrid Grid to copy from
   */
  public Grid(Grid copyGrid) {
    this.nRows = copyGrid.nRows;
    this.nCols = copyGrid.nCols;
    this.xMin = copyGrid.xMin;
    this.width = copyGrid.width;
    this.yMin = copyGrid.yMin;
    this.height = copyGrid.height;
    this.rotationAngle = copyGrid.rotationAngle;
    if (copyGrid.nodeZArray != null) {
      int numNodes = copyGrid.nodeZArray.length;
      this.nodeZArray = new double[numNodes];
      System.arraycopy(copyGrid.nodeZArray, 0, this.nodeZArray, 0, numNodes);
    }
    if (copyGrid.faults != null)
      this.faults = new ArrayList<XYZPolyline> (copyGrid.faults);
  }

/*---------------------------------------------------------------------------*/

/**
 Set the data in a grid object.  Typically xmin, ymin, height, and width should
 be in program units (meters), fault constraints should be in program units;
 however, grid data can be in user or program units.

 @param data Data for the grid, from lower left to upper right by row.
 @param ncol Number of columns in the data.
 @param nrow Number of rows in the data
 @param xmin X of lower left corner (before rotation).
 @param ymin Y of lower left corner (before rotation).
 @param width Width of a row of the grid (before rotation).
 @param height Height of a column of the grid (before rotation).
 @param angle Rotation angle in degrees, counterclockwise from the positive x axis.
 @param faults  An {@link ArrayList}XYZPolyline object containing the fault lines.
*/

  public void setData (
    double[] data,
    int ncol,
    int nrow,
    double xmin,
    double ymin,
    double width,
    double height,
    double angle,
    ArrayList<XYZPolyline> faults
  )
  {
    nodeZArray = data;
    nCols = ncol;
    nRows = nrow;
    xMin = xmin;
    yMin = ymin;
    this.width = width;
    this.height = height;
    rotationAngle = angle;
    this.faults = faults;

    calcMinMax ();
  }

/*-----------------------------------------------------------------------*/

  private double        zmin = 1.e30,
                        zmax = -1.e30;

  private void calcMinMax ()
  {
    int      i;

    zmin = 1.e30;
    zmax = -1.e30;
    for (i=0; i<nCols * nRows; i++) {
      if (nodeZArray[i] > 1.e20  ||
          nodeZArray[i] < -1.e20) {
        continue;
      }
      if (nodeZArray[i] < zmin) zmin = nodeZArray[i];
      if (nodeZArray[i] > zmax) zmax = nodeZArray[i];
    }
  }

/*---------------------------------------------------------------------------*/

  public double getZMin ()
  {
    if (zmin > zmax) {
        calcMinMax ();
    }
    return zmin;
  }

/*---------------------------------------------------------------------------*/

  public double getZMax ()
  {
    if (zmin > zmax) {
        calcMinMax ();
    }
    return zmax;
  }

/*---------------------------------------------------------------------------*/

/**
 Return a reference to the fault array.  Returns null if the fault array
 has not been defined.
 */
public ArrayList<XYZPolyline> getFaults() {
    return faults;
}

/*---------------------------------------------------------------------------*/

/**
 Return the unrotated height of the grid.
 */
public double getHeight() {
    return height;
}

/*---------------------------------------------------------------------------*/

/**
 Return the number of columns in the grid.
 */
public int getNCols() {
    return nCols;
}

/*---------------------------------------------------------------------------*/

/**
 Return a reference to the grid data array.
 */
public double[] getNodeZArray() {
    return nodeZArray;
}

/*---------------------------------------------------------------------------*/

/**
 Return the number of rows in the grid.
 */
public int getNRows() {
    return nRows;
}

/*---------------------------------------------------------------------------*/

/**
 Return the grid rotation angle.
 */
public double getRotationAngle() {
    return rotationAngle;
}

/*---------------------------------------------------------------------------*/

/**
 Return the unrotated width of the grid.
 */
public double getWidth() {
    return width;
}

/*---------------------------------------------------------------------------*/

/**
 Return the X coordinate of the unrotated lower left corner.
 */
public double getXMin() {
    return xMin;
}

/*---------------------------------------------------------------------------*/

/**
 Return the Y coordinate of the unrotated lower left corner.
 */
public double getYMin() {
    return yMin;
}

/*---------------------------------------------------------------------------*/

/**
 * Return the X coordinate of the unrotated upper right corner.
 */
public double getXMax() {
  return (xMin + width);
}

/*---------------------------------------------------------------------------*/

/**
 * Return the Y coordinate of the unrotated upper right corner.
 */
public double getYMax() {
  return (yMin + height);
}


/*---------------------------------------------------------------------------*/

/**
  Set the array of {@link FaultConstraint} objects used to calculate the grid.
  Typically these should be in program units.
  If there are no faults associated with the grid, set this to null.
 */
public void setFaults(ArrayList<XYZPolyline> faults) {
    this.faults = faults;
}

/*---------------------------------------------------------------------------*/

/**
 Set the unrotated height of the grid.  Typically height should be in
 program units.
 */
public void setHeight(double height) {
    this.height = height;
}

/*---------------------------------------------------------------------------*/

/**
 Set the number of columns in the grid.
 */
public void setNCols(int ncol) {
    nCols = ncol;
}

/*---------------------------------------------------------------------------*/

/**
 Set the array of grid data.  This is not copied.  The Grid object uses the
 reference specified here.  Grid data can be in either program or user units,
 but the owner should know what it is in.
 */
public void setNodeZArray(double[] data) {
    nodeZArray = data;
    calcMinMax ();
}

/*---------------------------------------------------------------------------*/

/**
 Set the number of rows in the grid.
 */
public void setNRows(int nrow) {
    nRows = nrow;
}

/*---------------------------------------------------------------------------*/

/**
 Set the grid rotation angle.  This is in degrees, counterclockwise from
 the positive x axis.
 */
public void setRotationAngle(double angle) {
    rotationAngle = angle;
}

/*---------------------------------------------------------------------------*/

/**
 Set the unrotated width of the grid. Typically width should be in
 program units.
 */
public void setWidth(double width) {
    this.width = width;
}

/*---------------------------------------------------------------------------*/

/**
 Set the X coordinate of the unrotated lower left corner of the grid.
 Typically should be in program units.
 */
public void setXMin(double xmin) {
    this.xMin = xmin;
}

/*---------------------------------------------------------------------------*/

/**
 Set the Y coordinate of the unrotated lower left corner of the grid.
 Typically should be in program units.
 */
public void setYMin(double d) {
    yMin = d;
}

/*---------------------------------------------------------------------------*/

} // end of class Grid definition
