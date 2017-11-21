
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
 * Represents a blended grid for an NDP overlay scenario.
 */
public class BlendedGrid {

  public static final int NUM_IN_MIXTURE = 4;
  private int nCols;
  private int nRows;
  private double xMin;
  private double width;
  private double yMin;
  private double height;
  private Mixture[] nodeZArray;

  private double[] xFault;
  private double[] yFault;
  private int[] nFaultPoints;
  private int   nFaults;
  private int   nFaultTotal;

  /**
   * Constructor
   */
  public BlendedGrid(
      int nRows,
      int nCols,
      double xMin,
      double yMin,
      double width,
      double height,
      CSWBlendedNodes nodes,
      int backgroundValue,
      double[] xf,
      double[] yf,
      int[]    nfp,
      int      nf,
      int      ntot
  ) {
    this.nCols = nCols;
    this.nRows = nRows;
    this.xMin = xMin;
    this.yMin = yMin;
    this.width = width;
    this.height = height;

    // Create an array of mixtures from the return values.
    int size = nodes.size();
    nodeZArray = new Mixture[size];
    double[] fractions;
    int[] valueInts;
    int[] values = new int[NUM_IN_MIXTURE];
    for (int i = 0; i < size; i++) {
      fractions = nodes.getFractionsForNode(i);
      valueInts = nodes.getValueIDsForNode(i);
      // Sum the fractions
      double sum = 0;
      for (int j = 0; j < fractions.length; j++) {
        sum += fractions[j];
      }
      if (sum <= 0.00001) {
        values[0] = backgroundValue;
        fractions[0] = 1.0;
      } else {
        for (int j = 0; j < NUM_IN_MIXTURE; j++) {
          values[j] = valueInts[j];
        }
      }
      nodeZArray[i] = new Mixture(fractions, values);
    }

    /* Set the fault (boundary) data.  The fault data, if the grid used it,
     * is needed to make a blended grid image.
     */
    xFault = xf;
    yFault = yf;
    nFaultPoints = nfp;
    nFaults = nf;
    nFaultTotal = ntot;
  }

  /**
   * Copy Constructor.
   *
   * <P>The Mixture nodes are returned as a shallow copy since they are
   * calculated by JSurfaceWorks and not changeable.
   * @param copyGrid blended grid to copy
   */
  private BlendedGrid(BlendedGrid copyGrid) {
    // primitives
    this.nRows = copyGrid.nRows;
    this.nCols = copyGrid.nCols;
    this.xMin = copyGrid.xMin;
    this.yMin = copyGrid.yMin;
    this.width = copyGrid.width;
    this.height = copyGrid.height;

    // Shallow copy since this is computed by JSurfaceWorks and shouldn't
    // be changed.
    this.nodeZArray = copyGrid.nodeZArray;

    // Deep copy since these might be changed, for instance changing units
    if (copyGrid.xFault != null && copyGrid.yFault != null) {
      int length = copyGrid.xFault.length;
      this.xFault = new double[length];
      System.arraycopy(copyGrid.xFault, 0, this.xFault, 0, length);
      length = copyGrid.yFault.length;
      this.yFault = new double[length];
      System.arraycopy(copyGrid.yFault, 0, this.yFault, 0, length);
    } else {
      this.xFault = null;
      this.yFault = null;
    }

    // more primitives
    this.nFaults = copyGrid.nFaults;
    this.nFaultTotal = copyGrid.nFaultTotal;

    // shallow copy becuase this shouldn't  be changed.
    this.nFaultPoints = copyGrid.nFaultPoints;
  }

/*---------------------------------------------------------*/

/*
 * Methods to get the fault data.
 */
  public double[] getXFault ()
  {
    return xFault;
  }

  public double[] getYFault ()
  {
    return yFault;
  }

  public int[] getNFaultPoints ()
  {
    return nFaultPoints;
  }

  public int getNFaults ()
  {
    return nFaults;
  }

  public int getNFaultTotal ()
  {
    return nFaultTotal;
  }

/*--------------------------------------------------------*/

  public double getHeight() {
    return height;
  }

  public double getWidth() {
    return width;
  }

  public Mixture getMixture(int row, int col) {
    int index = (row * nCols) + col;
    return nodeZArray[index];
  }

  public void setMixtureToNull (int row, int col)
  {
    int index = (row * nCols) + col;
    nodeZArray[index] = null;
  }

/*-------------------------------------------------------------------*/

  /**
  Retrieve the number of columns.
  */
  public int getNCols ()
  {
    return nCols;
  }

  /**
  Retrieve the number of rows.
  */
  public int getNRows ()
  {
    return nRows;
  }

  /**
  Retrieve the minimum x.
  */
  public double getXMin ()
  {
    return xMin;
  }

  /**
  Retrieve the minimum y.
  */
  public double getYMin ()
  {
    return yMin;
  }

  /**
  Retrieve the maximum x.
  */
  public double getXMax ()
  {
    return xMin + width * (nCols - 1);
  }

  /**
  Retrieve the maximum y.
  */
  public double getYMax ()
  {
    return yMin + height * (nRows - 1);
  }

  /**
  Retrieve a value for a mixture component at a node.
  @param nodenum  Number of node in the blended grid.
  @param nval Number of value at the node (0 to 3)
  */
  public int getMixtureValue (int nodenum, int nval)
  {
    int ival;

    if (nodeZArray == null) {
      return -1;
    }

    Mixture mix = nodeZArray[nodenum];
    if (mix == null) {
      ival = -1;
    }
    else {
      ival = mix.getValue (nval);
    }

    return ival;
  }

/*-------------------------------------------------------------------------*/

  /**
  Retrieve a fraction for a mixture component at a node.
  @param nodenum  Number of node in the blended grid.
  @param nval Number of value at the node (0 to 3)
  */
  public double getMixtureFraction (int nodenum, int nval)
  {
    double fval;

    if (nodeZArray == null) {
      return -1;
    }

    Mixture mix = nodeZArray[nodenum];
    if (mix == null) {
      fval = 0.0;
    }
    else {
      fval = mix.getFraction (nval);
    }

    return fval;
  }


/*-------------------------------------------------------------------*/

  public void dump() {
    System.out.println(this.toString());
    System.out.println("   nRows=" + nRows + ", nCols=" + nCols);
    System.out.println("   xMin=" + xMin + ", width=" + width);
    System.out.println("   yMin=" + yMin + ", height=" + height);
    System.out.println("   nodeZArray: ");
    for (int i = 0; i < nodeZArray.length; i++) {
      nodeZArray[i].dump();
    }
  }

  public class Mixture {
    private int[] values;
    private double[] fractions;

    Mixture(double[] fractions, int[] values) {
      this.fractions = new double [fractions.length];
      this.values = new int[values.length];
      for (int i = 0; i < values.length; i++) {
        this.values[i] = values[i];
        this.fractions[i] = fractions[i];
      }
    }

    public int getValue(int index) {
      if (index >= NUM_IN_MIXTURE)
        return -1;
      return values[index];
    }

    public double getFraction(int index) {
      if (index > NUM_IN_MIXTURE)
        return -1;
      return fractions[index];
    }

    private void dump() {
      System.out.println("      Mixture:");
      for (int i = 0; i < NUM_IN_MIXTURE; i++) {
        if (values[i] != -1)
            System.out.println("        " + values[i] + ": " + fractions[i]);
      }
    }

  }

}
