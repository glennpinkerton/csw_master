
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
 * This class stores the data for an axis returned 
 * from a pick or edit operation.  It returns all the information
 * used to create and draw the axis.
 * An instance of
 * DLAxis will only be created by querying the display list.  Therefore,
 * the constructor is not public and the set methods are not public.
 */

public class DLAxis {

  /*
   * The variables have package scope so the JDisplayList can
   * access them directly.
   */
    int             labelFlag;
    int             tickFlag;
    int             tickDirection;
    String          caption;
    double          majorInterval;
    Color           lineColor;
    Color           textColor;
    double          lineThickness;
    double          textSize;
    double          textThickness;
    int             textFont;
    double          x1;
    double          y1;
    double          x2;
    double          y2;
    int             labelDirection;
    DLSelectable    selectableObject;
    double          firstAxisValue;
    double          lastAxisValue;
    String          frameName,
                    layerName,
                    itemName;
    int             nativeIndex;

  /*
   * The constructor is package scope.  Only the JDisplayList and its peers
   * can create and populate a DLAxis object.
   */
    DLAxis () 
    {
        nativeIndex = -1;
    };

    /**
     Return the caption drawn parallel to the axis.  If no caption is drawn, null is returned.
     */
    public String getCaption() {
        return caption;
    }

    /**
    Return the axis value for the x1, y1 point.
     */
    public double getFirstAxisValue() {
        return firstAxisValue;
    }

    /**
    Return the name of the frame that holds the axis.  If the axis is not inside a frame,
    null is returned.
     */
    public String getFrameName() {
        return frameName;
    }

    /**
    Return the name of the item that is associated with the axis.  If no item is associated
    with the axis, null is returned.
     */
    public String getItemName() {
        return itemName;
    }

    /**
    Return the label direction for the axis.  This is only used for axes that are drawn
    inside of frames.  For axes on the frame borders, this is not relevant, and it will
    be zero.  If it is 1, then the caption is on the same side as the tick marks.
    If this is -1, the caption is on the opposite side from the tick marks.
     */
    public int getLabelDirection() {
        return labelDirection;
    }

    /**
    Return the label drawing flag.  If this is 1, labels are drawn at major intervals.
    If this is zero, no labels are drawn.
     */
    public int getLabelFlag() {
        return labelFlag;
    }

    /**
    Return the axis value at the x2, y2 point.
     */
    public double getLastAxisValue() {
        return lastAxisValue;
    }

    /**
    Return the name of the layer that holds this axis.  If the axis is not part of a 
    layer, null is returned.
     */
    public String getLayerName() {
        return layerName;
    }

    /**
    Return the color used for axis lines.
     */
    public Color getLineColor() {
        return lineColor;
    }

    /**
    Return the thickness (in inches) for axis lines.
     */
    public double getLineThickness() {
        return lineThickness;
    }

    /**
    
     */
    public double getMajorInterval() {
        return majorInterval;
    }

    /**
     */
    public int getNativeIndex() {
        return nativeIndex;
    }

    /**
     */
    public DLSelectable getSelectableObject() {
        return selectableObject;
    }

    /**
     */
    public Color getTextColor() {
        return textColor;
    }

    /**
     */
    public int getTextFont() {
        return textFont;
    }

    /**
     */
    public double getTextSize() {
        return textSize;
    }

    /**
     */
    public double getTextThickness() {
        return textThickness;
    }

    /**
     */
    public int getTickDirection() {
        return tickDirection;
    }

    /**
     */
    public int getTickFlag() {
        return tickFlag;
    }

    /**
     */
    public double getX1() {
        return x1;
    }

    /**
     */
    public double getX2() {
        return x2;
    }

    /**
     */
    public double getY1() {
        return y1;
    }

    /**
     */
    public double getY2() {
        return y2;
    }


  /*
   * Instances of this class are only created and populated
   * by the JDisplayList and its peers.
   * Thus, all the set functions are package scope.
   */
    void setCaption(String string) {
        caption = string;
    }

    void setFirstAxisValue(double d) {
        firstAxisValue = d;
    }

    void setFrameName(String string) {
        frameName = string;
    }

    void setItemName(String string) {
        itemName = string;
    }

    void setLabelDirection(int i) {
        labelDirection = i;
    }

    void setLabelFlag(int i) {
        labelFlag = i;
    }

    void setLastAxisValue(double d) {
        lastAxisValue = d;
    }

    void setLayerName(String string) {
        layerName = string;
    }

    void setLineColor(Color color) {
        lineColor = color;
    }

    void setLineThickness(double d) {
        lineThickness = d;
    }

    void setMajorInterval(double d) {
        majorInterval = d;
    }

    void setNativeIndex(int i) {
        nativeIndex = i;
    }

    void setSelectableObject(DLSelectable selectable) {
        selectableObject = selectable;
    }

    void setTextColor(Color color) {
        textColor = color;
    }

    void setTextFont(int i) {
        textFont = i;
    }

    void setTextSize(double d) {
        textSize = d;
    }

    void setTextThickness(double d) {
        textThickness = d;
    }

    void setTickDirection(int i) {
        tickDirection = i;
    }

    void setTickFlag(int i) {
        tickFlag = i;
    }

    void setX1(double d) {
        x1 = d;
    }

    void setX2(double d) {
        x2 = d;
    }

    void setY1(double d) {
        y1 = d;
    }

    void setY2(double d) {
        y2 = d;
    }

}
