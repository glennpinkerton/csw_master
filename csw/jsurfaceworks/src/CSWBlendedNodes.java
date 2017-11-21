
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
 * class_definition
 */
public class CSWBlendedNodes {
  private static int NUM_IN_MIXTURE = 4;
  private int[][] valueIDs;
  private double[][] fractions;

  protected CSWBlendedNodes(int numGridNodes) {
    valueIDs = new int[numGridNodes][NUM_IN_MIXTURE];
    fractions = new double[numGridNodes][NUM_IN_MIXTURE];
  }

  protected CSWBlendedNodes(
      int[] values1,
      int[] values2,
      int[] values3,
      int[] values4,
      double[] fractions1,
      double[] fractions2,
      double[] fractions3,
      double[] fractions4
  ) {
    int numGridNodes = values1.length;
    valueIDs = new int[numGridNodes][NUM_IN_MIXTURE];
    fractions = new double[numGridNodes][NUM_IN_MIXTURE];
    for (int i = 0; i < numGridNodes; i++) {
      valueIDs[i][0] = values1[i];
      valueIDs[i][1] = values2[i];
      valueIDs[i][2] = values3[i];
      valueIDs[i][3] = values4[i];
      fractions[i][0] = fractions1[i];
      fractions[i][1] = fractions2[i];
      fractions[i][2] = fractions3[i];
      fractions[i][3] = fractions4[i];
    }
  }

  protected void setValueIDsForNode(
    int index,
    int[] values
  ) {
    for (int i = 0; i < NUM_IN_MIXTURE; i++) {
      valueIDs[index][i] = values[i];
    }
  }

  protected void setFractionsForNode(
    int index,
    double[] values
  ) {
    for (int i = 0; i < NUM_IN_MIXTURE; i++) {
      this.fractions[index][i] = values[i];
    }
  }

  public int[] getValueIDsForNode(int index) {
    return valueIDs[index];
  }

  public double[] getFractionsForNode(int index) {
    return fractions[index];
  }

  public int size() {
    return fractions.length;
  }

}
