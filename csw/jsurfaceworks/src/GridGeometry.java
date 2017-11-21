
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
 * A temporary storage class for data read from a grid file.
 */
public class GridGeometry {
  int nCols,
      nRows;
  double xMin;
  double yMin;
  double xMax;
  double yMax;
  double avSpace;

  public GridGeometry() {
    init ();
  }

  private void init () {
    nCols = 0;
    nRows = 0;
    xMin = 1.e30;
    yMin = 1.e30;
    xMax = -1.e30;
    yMax = -1.e30;
    avSpace = 1.e30;
  }

  public GridGeometry(int ncol,
                      int nrow,
                      double xmin,
                      double ymin,
                      double xmax,
                      double ymax)
  {
    setGeometry (ncol, nrow,
                 xmin, ymin, xmax, ymax);
  }


  public GridGeometry (double avspace)
  {
    init ();
    if (avspace <= 0.0) avspace = 1.e30;
    avSpace = avspace;
  }

  public void setAvSpace (double avspace)
  {
    if (avspace <= 0.0) avspace = 1.e30;
    avSpace = avspace;
  }

  public void setGeometry (int ncol,
                           int nrow,
                           double xmin,
                           double ymin,
                           double xmax,
                           double ymax)
  {
    nCols = ncol;
    nRows = nrow;
    xMin = xmin;
    yMin = ymin;
    xMax = xmax;
    yMax = ymax;
  }

  public int getColumns ()
  {
    return nCols;
  }

  public int getRows ()
  {
    return nRows;
  }

  public double getXmin ()
  {
    return xMin;
  }

  public double getYmin ()
  {
    return yMin;
  }

  public double getXmax ()
  {
    return xMax;
  }

  public double getYmax ()
  {
    return yMax;
  }

  public double getAvSpace ()
  {
    return avSpace;
  }

}
