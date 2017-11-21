
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/


package csw.jeasyx.src;

import java.awt.Color;

/**
 * This class stores the data for a line primitive returned
 * from a pick or edit operation.  It is 
 * used to return the data and graphic attributes for a line.  The
 * native display list has the data for all lines.  An instance of
 * DLLine will only be created by querying the display list.  Therefore,
 * the constructor is not public and the set methods are not public.
 */

public class DLLine {

  /*
   * The variables have package scope so the JDisplayList can
   * access them directly.
   */
    DLSelectable    selectableObject;
    double[]        xPoints,
                    yPoints;
    int             numPoints;
    double          thickness,
                    dashScale;
    Color           color;
    String          frameName,
                    layerName,
                    itemName;
    int             pattern,
                    symbol,
                    arrowStyle,
                    nativeIndex;

    /**
    Return the arrow style used for the last point of the line.  The possible return values
    are:
    <ul>
    <li> {@link DLConst#NO_ARROW}
    <li> {@link DLConst#SIMPLE_ARROW}
    <li> {@link DLConst#TRIANGLE_ARROW}
    <li> {@link DLConst#FILLED_TRIANGLE_ARROW}
    <li> {@link DLConst#OFFSET_TRIANGLE_ARROW}
    <li> {@link DLConst#FILLED_OFFSET_TRIANGLE_ARROW}
    </ul>
     */
    public int getArrowStyle() {
        return arrowStyle;
    }

    /**
    Return the scale factor for a dashed line.
     */
    public double getDashScale() {
        return dashScale;
    }

    /**
    Return the name of the frame that contains the line.  If the line is not inside a frame,
    null is returned.
     */
    public String getFrameName() {
        return frameName;
    }

    /**
    Return the name of the item associated with the line.  If no item is associated with the
    line, return null.
     */
    public String getItemName() {
        return itemName;
    }

    /**
    Return the name of the layer that the line belongs to.  If no layer holds the
    line, return null.
     */
    public String getLayerName() {
        return layerName;
    }

    /**
    Return the color of the line.
     */
    public Color getColor() {
        return color;
    }

    /**
    Return the index in the native display list for the line.
     */
    public int getNativeIndex() {
        return nativeIndex;
    }

    /**
    Return the number of points in the line.  
     */
    public int getNumPoints() {
        return numPoints;
    }

    /**
    Return the dash pattern used to draw the line.  If the line is solid, zero is returned.
     */
    public int getPattern() {
        return pattern;
    }

    /**
    Return the {@link DLSelectable} that the line belongs to.
     */
    public DLSelectable getSelectableObject() {
        return selectableObject;
    }

    /**
    Return the symbol number drawn at each line point.  If no symbols are drawn,
    this is zero.
     */
    public int getSymbol() {
        return symbol;
    }

    /**
    Return the thickness, in inches, of the line.
     */
    public double getThickness() {
        return thickness;
    }

    /**
    Return an array of x coordinates for the line.
     */
    public double[] getXPoints() {
        return xPoints;
    }

    /**
    Return an array of y coordinates for the line.
     */
    public double[] getYPoints() {
        return yPoints;
    }

/*--------------------------------------------------------------------*/

  /*
   * The set methods all have package scope.  Only JDisplayList and its
   * peers are allowed to create and populate DLLine objects.
   */
    void setArrowStyle(int i) {
        arrowStyle = i;
    }

    void setDashScale(double d) {
        dashScale = d;
    }

    void setFrameName(String string) {
        frameName = string;
    }

    void setItemName(String string) {
        itemName = string;
    }

    void setLayerName(String string) {
        layerName = string;
    }

    void setLineColor(Color c) {
        color = c;
    }

    void setNativeIndex(int i) {
        nativeIndex = i;
    }

    void setNumPoints(int i) {
        numPoints = i;
    }

    void setPattern(int i) {
        pattern = i;
    }

    void setSelectableObject(DLSelectable selectable) {
        selectableObject = selectable;
    }

    void setSymbol(int i) {
        symbol = i;
    }

    void setThickness(double d) {
        thickness = d;
    }

    void setXPoints(double[] ds) {
        xPoints = ds;
    }

    void setYPoints(double[] ds) {
        yPoints = ds;
    }

}
