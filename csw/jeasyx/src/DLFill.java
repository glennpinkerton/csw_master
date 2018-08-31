
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

import java.lang.Math;
import java.awt.Color;
import java.io.Serializable;

/**
 */

public class DLFill implements Serializable {

    private static final long serialVersionUID = 1357952L;

  /*
   * The variables have package scope so the JDisplayList can
   * access them directly.
   */
    DLSelectable    selectableObject;
    double[]        xPoints,
                    yPoints;
    int[]           numPoints;
    int             numComponents;
    double          borderThickness,
                    patternScale,
                    dashScale;
    Color           color,
                    patternColor,
                    borderColor;
    String          frameName,
                    layerName,
                    itemName;
    int             pattern,
                    linePattern,
                    nativeIndex;

    public void outlineComponents (JDisplayList  dl) {
    }

    public void printForDebug () {
      int k=0;
      System.out.println ();
      System.out.println ("DLFill geometry dump");
      System.out.println ("  numComponents = " + numComponents);
      for (int i=0; i<numComponents; i++) {
        System.out.println ("    component # " + i + "  numpoints = "
          + numPoints[i]);
        for (int j=0; j<numPoints[i]; j++) {
          System.out.println ("      " + xPoints[k] + "    " + yPoints[k]);
          k++;
        }
      }
      System.out.println ();
    }



    public DLFill () {
    }



public void dumpPoints (int findex)
{

    System.out.println ();
    System.out.println ("In DLFill dumpPoints");
    System.out.println ("numComponents = " + numComponents + 
           "  in polygon # " + findex);
    System.out.println ();
    System.out.flush ();

    int  ixy = 0;

    for (int i=0; i<numComponents; i++) {
        System.out.println ();
        System.out.println (numPoints[i] + " points for component " + i);
        for (int j=0; j<numPoints[i]; j++) {
            System.out.println ("  ixy = " + ixy + "   " +
                  xPoints[ixy] + "   " + yPoints[ixy]);
            ixy++;
            System.out.flush ();
        }
    }

}


public void testForLongLine (int findex, double dtest) {

    dtest = 10.0;

    int  ixy = 0;

    double  dx, dy, dd;

    System.out.println ();
    System.out.println ("In DLFill testForLongLine");
    System.out.println ("numComponents = " + numComponents + 
           "  in polygon # " + findex);
    System.out.println ();
    System.out.flush ();

    for (int i=0; i<numComponents; i++) {
        System.out.println ();
        System.out.println (numPoints[i] + " points for component " + i);
        for (int j=0; j<numPoints[i] - 1; j++) {
            dx = xPoints[ixy] - xPoints[ixy+1];
            dy = yPoints[ixy] - yPoints[ixy+1];
            dd = dx * dx + dy * dy;
            dd = Math.sqrt (dd);
            if (dd > dtest) {
                System.out.println (
                    "  long vector at component " + i +
                    "  point " + j);
                System.out.println ("  ixy1 = " + ixy + "   " +
                  xPoints[ixy] + "   " + yPoints[ixy]);
                int  ixy2 = ixy + 1;
                System.out.println ("  ixy2 = " + ixy2 + "   " +
                  xPoints[ixy2] + "   " + yPoints[ixy2]);
                System.out.flush ();
            }
            ixy++;
        }
        ixy++;
    }

}



    /**
    Return the color of the polygon border.
     */
    public Color getBorderColor() {
        return borderColor;
    }

    /**
    Return the thickness (in inches) of the polygon border.
     */
    public double getBorderThickness() {
        return borderThickness;
    }

    /**
    Return the dash scale of the polygon border.
     */
    public double getDashScale() {
        return dashScale;
    }

    /**
    Return the color used for the solid fill of the polygon.  If the polygon is
    not solid filled, this is null.
     */
    public Color getColor() {
        return color;
    }

    /**
    Return the fill pattern number.  If the polygon has no fill pattern, zero is returned.
     */
    public int getPattern() {
        return pattern;
    }

    /**
    Return the name of the frame containing the polygon.  If the polygon is not in a
    frame, null is returned.
     */
    public String getFrameName() {
        return frameName;
    }

    /**
    Return the item name associated with the polygon.  If the polygon has no item
    name associated with it, null is returned.
     */
    public String getItemName() {
        return itemName;
    }

    /**
    Return the layer name associated with the polygon.  If the polygon has no
    layer associated with it, null is returned.
     */
    public String getLayerName() {
        return layerName;
    }

    /**
    Return the line pattern used by the polygon border.  If the border is a solid
    line, zero is returned.
     */
    public int getLinePattern() {
        return linePattern;
    }

    /**
    Return the index number in the native display list for this polygon.  This number
    uniquely identifies the polygon in the native display list.
     */
    public int getNativeIndex() {
        return nativeIndex;
    }

    /**
    Return the color used for the fill pattern.  If there is no fill pattern, null
    is returned.
     */
    public Color getPatternColor() {
        return patternColor;
    }

    /**
    Return the scale factor for the fill pattern.
     */
    public double getPatternScale() {
        return patternScale;
    }

    /**
    Return the {@link DLSelectable} object that this polygon belongs to.  If the
    polygon is not part of a selectable, this will be null.
     */
    public DLSelectable getSelectableObject() {
        return selectableObject;
    }

    /**
    Return the array of x coordinates for this polygon.  All of the components of the
    polygon are packed into this array.
     */
    public double[] getXPoints() {
        return xPoints;
    }

    /**
    Return the array of y coordinates for this polygon.  All of the components of the
    polygon are packed into this array.
     */
    public double[] getYPoints() {
        return yPoints;
    }

    /**
    Return the array of points per component for this polygon.
    */
    public int[] getNumPoints() {
        return numPoints;
    }

    /**
    Return the number of components in the polygon.
    */
    public int getNumComponents () {
        return numComponents;
    }


/*--------------------------------------------------------------------------------*/

    public void setBorderColor(Color color) {
        borderColor = color;
    }

    public void setBorderThickness(double d) {
        borderThickness = d;
    }

    public void setDashScale(double d) {
        dashScale = d;
    }

    public void setColor(Color color) {
        this.color = color;
    }

    public void setPattern(int i) {
        pattern = i;
    }

    public void setFrameName(String string) {
        frameName = string;
    }

    public void setItemName(String string) {
        itemName = string;
    }

    public void setLayerName(String string) {
        layerName = string;
    }

    public void setLinePattern(int i) {
        linePattern = i;
    }

    public void setNativeIndex(int i) {
        nativeIndex = i;
    }

    public void setPatternColor(Color color) {
        patternColor = color;
    }

    public void setPatternScale(double d) {
        patternScale = d;
    }

    public void setSelectableObject(DLSelectable selectable) {
        selectableObject = selectable;
    }

    public void setXPoints(double[] ds) {
        xPoints = ds;
    }

    public void setYPoints(double[] ds) {
        yPoints = ds;
    }

    public void setNumPoints (int[] np) {
        numPoints = np;
    }

    public void setNumComponents (int nc) {
        numComponents = nc;
    }

}
