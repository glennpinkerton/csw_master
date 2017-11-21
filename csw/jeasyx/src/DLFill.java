
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
 * This class stores the data for a polygon fill primitive returned
 * from a pick or edit operation.  It is
 * used to return the data and graphic attributes for a fill.  The
 * native display list has the data for all fills.  An instance of
 * DLFill will only be created by querying the display list.  Therefore,
 * the constructor is not public.  The various set methods are also of
 * package scope.
 */

public class DLFill {

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

/*
 * Constructor has package scope also.
 */
    DLFill () {
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

    /*
     * The set functions have package scope.  Only the JDisplayList class
     * and its associated utilities can set the data.
     */
    void setBorderColor(Color color) {
        borderColor = color;
    }

    void setBorderThickness(double d) {
        borderThickness = d;
    }

    void setDashScale(double d) {
        dashScale = d;
    }

    void setColor(Color color) {
        this.color = color;
    }

    void setPattern(int i) {
        pattern = i;
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

    void setLinePattern(int i) {
        linePattern = i;
    }

    void setNativeIndex(int i) {
        nativeIndex = i;
    }

    void setPatternColor(Color color) {
        patternColor = color;
    }

    void setPatternScale(double d) {
        patternScale = d;
    }

    void setSelectableObject(DLSelectable selectable) {
        selectableObject = selectable;
    }

    void setXPoints(double[] ds) {
        xPoints = ds;
    }

    void setYPoints(double[] ds) {
        yPoints = ds;
    }

    void setNumPoints (int[] np) {
        numPoints = np;
    }

    void setNumComponents (int nc) {
        numComponents = nc;
    }

}
