
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
 * A grid created by the csw grid library is represented in this class.
 */
public class CSWGrid extends Grid {

  byte[]         nodeMask;
  double[]       pointErrors;

/*---------------------------------------------------------------------------*/

/**
 Construct an empty grid object.
*/
  public CSWGrid() {
  }

/*---------------------------------------------------------------------------*/

/**
 Construct a grid object with or without faults.
 @param data Data for the grid, from lower left to upper right by row.
 @param ncol Number of columns in the data.
 @param nrow Number of rows in the data
 @param xmin X of lower left corner (before rotation).
 @param ymin Y of lower left corner (before rotation).
 @param width Width of a row of the grid (before rotation).
 @param height Height of a column of the grid (before rotation).
 @param angle Rotation angle in degrees, counterclockwise from the positive x axis.
 @param faults  An {@link XYZPolylineList} object containing the fault lines.
 @param mask Array with the "how good is the control" info at each node.
*/
  public CSWGrid (
    double[] data,
    int ncol,
    int nrow,
    double xmin,
    double ymin,
    double width,
    double height,
    double angle,
    ArrayList<XYZPolyline> faults,
    byte[] mask
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
             faults
            );
    nodeMask = mask;
    pointErrors = null;
  }

/*---------------------------------------------------------------------------*/
  public CSWGrid(CSWGrid copyGrid) {
    super(copyGrid);
    if (copyGrid.nodeMask != null) {
      int nodeMaskSize = copyGrid.nodeMask.length;
      this.nodeMask = new byte[nodeMaskSize];
      System.arraycopy(copyGrid.nodeMask, 0, this.nodeMask, 0, nodeMaskSize);
    }
    if (copyGrid.pointErrors != null) {
      int size = copyGrid.pointErrors.length;
      this.pointErrors = new double[size];
      System.arraycopy(copyGrid.pointErrors, 0, this.pointErrors, 0, size);
    }
  }
/*---------------------------------------------------------------------------*/

 /**
 Make a shallow copy of this grid into the specified new grid.  Only references
 to arrays are put into the new grid.
 */
  public void transfer (CSWGrid newGrid)
  {
    if (newGrid == null) {
      return;
    }
    newGrid.nCols = nCols;
    newGrid.nRows = nRows;
    newGrid.xMin = xMin;
    newGrid.width = width;
    newGrid.yMin = yMin;
    newGrid.height = height;
    newGrid.nodeZArray = nodeZArray;
    newGrid.rotationAngle = rotationAngle;
    newGrid.faults = faults;
    newGrid.nodeMask = nodeMask;
    newGrid.pointErrors = pointErrors;

    return;
  }


/*---------------------------------------------------------------------------*/

/**
 If the grid has a valid node Mask, convert the z array values coincident
 with non zero node mask values to 1.e30.
*/
  public void setHardNulls ()
  {
    int          i;

    if (nodeMask == null) return;

    for (i=0; i<nCols * nRows; i++) {
      if (nodeMask[i] != 0) {
        nodeZArray[i] = 1.e30;
      }
    }

    return;

  }

/*---------------------------------------------------------------------------*/

/**
 Returns the array with info on "how good is the control" at each node.
 */
public byte[] getNodeMask() {
    return nodeMask;
}

/*---------------------------------------------------------------------------*/

/**
 Returns an array of errors between the input data points and the grid estimate
 at those input data points.  This is only pertinent in a CSWGrid object returned
 from the calcGrid method of the {@link JSurfaceWorks} class.
 */
public double[] getPointErrors() {
    return pointErrors;
}

/*---------------------------------------------------------------------------*/

/**
 Set the mask array (which has info on how well each node is controlled by the
 input data points).  The mask array is calculated by the calcGrid method of the
 {@link JSurfaceWorks} class.   It may also be available from a file so I have
 kept this as a public method.
 */
public void setNodeMask(byte[] bs) {
    nodeMask = bs;
}

/*---------------------------------------------------------------------------*/

/*
 * This set function has been changed to package scope.  The only time the
 * point errors are set is from the calcGrid method in the JSurfaceWorks class.
 */
void setPointErrors(double[] ds) {
    pointErrors = ds;
}

} // end of class CSWGrid definition
