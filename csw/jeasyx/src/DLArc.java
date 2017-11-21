
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
 * This class stores the data for an arc primitive returned
 * from a pick or edit operation.  It is 
 * used to return the data and graphic attributes for a arc.  The
 * native display list has the data for all arcs.  An instance of
 * DLArc will only be created by querying the display list.  Therefore,
 * the constructor is not public and the set methods are not public.
 */

public class DLArc {

  /*
   * The variables have package scope so the JDisplayList can
   * access them directly.
   */
    DLSelectable    selectableObject;
    double          xCenter,
                    yCenter,
                    xRadius,
                    yRadius,
                    startAngle,
                    angleLength,
                    rotationAngle;
    int             closure;
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
    Return the angular length, in degrees, of a partial arc.  If the whole arc
    is drawn, this is 360 degrees.
     */
    public double getAngleLength() {
        return angleLength;
    }

    /**
    Return the color used to draw the border of the arc.  If no border is drawn
    for the arc, this is returned as null.
     */
    public Color getBorderColor() {
        return borderColor;
    }

    /**
    Return the thickness of the arc's border, in inches.
     */
    public double getBorderThickness() {
        return borderThickness;
    }

    /**
    Return the closure flag for the arc.  
    @return The closure type for the arc:
    <ul>
    <li> {@link DLConst#OPEN}
    <li> {@link DLConst#PIE}
    <li> {@link DLConst#CHORD}
    </ul>
     */
    public int getClosure() {
        return closure;
    }

    /**
    Return the dashed line scaling factor used for a dashed arc border.
     */
    public double getDashScale() {
        return dashScale;
    }

    /**
    Return the solid fill color for the arc.  If the arc is not filled
    with a solid color, this is returned as null.
     */
    public Color getFillColor() {
        return fillColor;
    }

    /**
    Return the fill pattern number for the arc.  If the arc has no pattern
    fill, this is returned as zero.
     */
    public int getFillPattern() {
        return fillPattern;
    }

    /**
    Return the name of the frame that holds the arc.  If the arc is not in
    a frame, this is null.
     */
    public String getFrameName() {
        return frameName;
    }

    /**
    Return the name of the item associated with the arc.  If the arc is not
    associated with an item, this is null.
     */
    public String getItemName() {
        return itemName;
    }

    /**
    Return the name of the layer associated with the arc.  If the arc is not
    in a layer, this is null.
     */
    public String getLayerName() {
        return layerName;
    }

    /**
    Return the line pattern used for the arc border.  If the border is solid,
    this is zero.
     */
    public int getLinePattern() {
        return linePattern;
    }

    /**
    Return the index number of the arc in the native display list.
     */
    public int getNativeIndex() {
        return nativeIndex;
    }

    /**
    Return the color used for the arc fill pattern.  If there is no arc fill
    pattern, this is zero.
     */
    public Color getPatternColor() {
        return patternColor;
    }

    /**
    Return the scale factor for the arc fill pattern.
     */
    public double getPatternScale() {
        return patternScale;
    }

    /**
    Return the rotation angle, in degrees counterclockwise from the positive
    x axis, for the arc.
     */
    public double getRotationAngle() {
        return rotationAngle;
    }

    /**
    Return the DLSelectable object that the arc belongs to.  If the arc is not
    part of a selectable, this will be null.
     */
    public DLSelectable getSelectableObject() {
        return selectableObject;
    }

    /**
    Return the starting angle, in degrees counterclockwise from the positive x axis,
    for a partial arc.
     */
    public double getStartAngle() {
        return startAngle;
    }

    /**
    Return the x coordinate of the center of the arc.
     */
    public double getXCenter() {
        return xCenter;
    }

    /**
    Return the radius along the x axis of the unrotated arc.
     */
    public double getXRadius() {
        return xRadius;
    }

    /**
    Return the y coordinate of the center of the arc.
     */
    public double getYCenter() {
        return yCenter;
    }

    /**
    Return the radius along the y axis of the unrotated arc.
     */
    public double getYRadius() {
        return yRadius;
    }

/*----------------------------------------------------------------------------------*/

  /*
   * All of the set methods are package scope.  The application objects 
   * cannot and should not explicitly set data in any DLArc object.
   */
    void setAngleLength(double d) {
        angleLength = d;
    }

    void setBorderColor(Color color) {
        borderColor = color;
    }

    void setBorderThickness(double d) {
        borderThickness = d;
    }

    void setClosure(int i) {
        closure = i;
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

    void setStartAngle(double d) {
        startAngle = d;
    }

    void setXCenter(double d) {
        xCenter = d;
    }

    void setXRadius(double d) {
        xRadius = d;
    }

    void setYCenter(double d) {
        yCenter = d;
    }

    void setYRadius(double d) {
        yRadius = d;
    }

}
