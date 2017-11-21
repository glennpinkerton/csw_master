
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
 * This class stores the data for a rectangle primitive returned 
 * from a pick or edit operation.  It is 
 * used to return the data and graphic attributes for a rectangle.  The
 * native display list has the data for all rectangles.  An instance of
 * DLRectangle will only be created by querying the display list.  Therefore,
 * the constructor is not public and the set methods are not public.
 */

public class DLRectangle {

  /*
   * The variables have package scope so the JDisplayList can
   * access them directly.
   */
    DLSelectable    selectableObject;
    double          xCenter,
                    yCenter,
                    width,
                    height,
                    cornerRadius,
                    rotationAngle;
    double          borderThickness,
                    patternScale,
                    dashScale;
    Color           fillColor,
                    patternColor,
                    borderColor;
    String          frameName,
                    layerName,
                    itemName;
    int             fillPattern,
                    linePattern,
                    nativeIndex;

    /**
    Return the color used for drawing the rectangle border.
     */
    public Color getBorderColor() {
        return borderColor;
    }

    /**
    Return the thickness, in inches, for the rectangle border.
     */
    public double getBorderThickness() {
        return borderThickness;
    }

    /**
    Return the radius, in frame units, for the curved corners of the rectangle.
    This is in the same units as the center, width and height of the rectangle.
     */
    public double getCornerRadius() {
        return cornerRadius;
    }

    /**
    Return the scale factor for a dashed border.
     */
    public double getDashScale() {
        return dashScale;
    }

    /**
    Return the color used for a solid fill of the rectangle.  If the rectangle has no
    solid fill, null is returned.
     */
    public Color getFillColor() {
        return fillColor;
    }

    /**
    Return the pattern used to fill the rectangle.  If the rectangle has no pattern fill,
    zero is returned.
     */
    public int getFillPattern() {
        return fillPattern;
    }

    /**
    Return the name of the frame that holds the rectangle.  If the rectangle is not ina frame,
    null is returned.
     */
    public String getFrameName() {
        return frameName;
    }

    /**
    Return the height of the rectangle.
     */
    public double getHeight() {
        return height;
    }

    /**
    Return the name of the item associated with the rectangle.  If no item is associated with 
    the rectangle, null is returned.
     */
    public String getItemName() {
        return itemName;
    }

    /**
    Return the name of the layer which contains the rectangle.  If no layer contains the
    rectangle, null is returned.
     */
    public String getLayerName() {
        return layerName;
    }

    /**
    Return the dash pattern for the rectangle border.  For a solid border, zero is returned.
     */
    public int getLinePattern() {
        return linePattern;
    }

    /**
    Return the native display list index for the rectangle.
     */
    public int getNativeIndex() {
        return nativeIndex;
    }

    /**
    Return the color used to draw the fill pattern in the rectangle.  If no fill pattern
    is drawn, this is null.
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
    Return the rotation angle, in degrees, positive counterclockwise from the positive
    x axis.
     */
    public double getRotationAngle() {
        return rotationAngle;
    }

    /**
    Return the {@link DLSelectable} object which contains the rectangle.  If no selectable
    contains the rectangle, this will be null.
     */
    public DLSelectable getSelectableObject() {
        return selectableObject;
    }

    /**
    Return the width of the rectangle.
     */
    public double getWidth() {
        return width;
    }

    /**
    Return the x coordinate of the center of the rectangle.
     */
    public double getXCenter() {
        return xCenter;
    }

    /**
    Return the y coordinate of the center of the rectangle.
     */
    public double getYCenter() {
        return yCenter;
    }


/*-----------------------------------------------------------------------------*/

  /*
   * The set methods have package scope.  Only the JDisplayList class and
   * its peers are allowed to create and populate a DLRectangle object.
   */
    void setBorderColor(Color color) {
        borderColor = color;
    }

    void setBorderThickness(double d) {
        borderThickness = d;
    }

    void setCornerRadius(double d) {
        cornerRadius = d;
    }

    void setDashScale(double d) {
        dashScale = d;
    }

    void setFillColor(Color color) {
        fillColor = color;
    }

    void setFillPattern(int i) {
        fillPattern = i;
    }

    void setFrameName(String string) {
        frameName = string;
    }

    void setHeight(double d) {
        height = d;
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

    void setRotationAngle(double d) {
        rotationAngle = d;
    }

    void setSelectableObject(DLSelectable selectable) {
        selectableObject = selectable;
    }

    void setWidth(double d) {
        width = d;
    }

    void setXCenter(double d) {
        xCenter = d;
    }

    void setYCenter(double d) {
        yCenter = d;
    }

}
