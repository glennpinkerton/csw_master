
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
 * This class stores the data for a contour primitive returned
 * from a pick or edit operation.  It is 
 * used to return the data and graphic attributes for a contour.  The
 * native display list has the data for all contours.  An instance of
 * DLContour will only be created by querying the display list.  Therefore,
 * the constructor is not public and the set methods are not public.
 */

public class DLContour {

  /*
   * The variables have package scope so the JDisplayList can
   * access them directly.
   */
    DLSelectable    selectableObject;
    double[]        xPoints,
                    yPoints;
    int             numPoints;
    double          zValue;
    double          thickness;
    boolean         isMajor;
    Color           color;
    String          labelText;
    int             labelFont;
    double          labelSize,
                    labelSpacing;
    double          tickLength,
                    tickSpacing;
    int             tickDirection;
    String          frameName,
                    layerName,
                    itemName;
    String          surfaceName;
    int             nativeIndex;

    /**
    Return the color of the contour line.
     */
    public Color getColor() {
        return color;
    }

    /**
    Return the name of the frame that holds the contour.  If the contour is
    not in a frame, null is returned.
     */
    public String getFrameName() {
        return frameName;
    }

    /**
    Return true if the contour is a major contour or false if the contour is
    not a major contour.
     */
    public boolean isMajor() {
        return isMajor;
    }

    /**
    Return the name of the item the contour is associated with.  If the
    contour is not associated with an item, return null.
     */
    public String getItemName() {
        return itemName;
    }

    /**
    Return the font number used to draw contour labels.  If no label is drawn for 
    this contour, -1 is returned.
     */
    public int getLabelFont() {
        return labelFont;
    }

    /**
    Return the size, in inches, of the contour label.  If there is no contour label,
    zero is returned.
     */
    public double getLabelSize() {
        return labelSize;
    }

    /**
    Return the spacing, in inches, between contour labels.  If there is no contour
    label, zero is returned.
     */
    public double getLabelSpacing() {
        return labelSpacing;
    }

    /**
    Return the contour label string.  If there is no contour label, null is returned.
     */
    public String getLabelText() {
        return labelText;
    }

    /**
    Return the name of the layer that this contour is associated with.  If the contour
    is not part of a layer, null is returned.
     */
    public String getLayerName() {
        return layerName;
    }

    /**
    Return the index in the native display list's contour list for this contour.
     */
    public int getNativeIndex() {
        return nativeIndex;
    }

    /**
    Return the number of points in the contour line.
     */
    public int getNumPoints() {
        return numPoints;
    }

    /**
    Return the {@link DLSelectable} that this contour belongs to.
     */
    public DLSelectable getSelectableObject() {
        return selectableObject;
    }

    /**
    Return the name of the surface (trimesh or grid) that this contour
    belongs to.
    */
    public String getSurfaceName () {
        return surfaceName;
    }

    /**
    Return the direction that tick marks are drawn for this contour.  If they
    are drawn downhill, 1 is returned.  If drawn uphill, -1 is returned.  If
    no tickmarks are drawn, zero is returned.
     */
    public int getTickDirection() {
        return tickDirection;
    }

    /**
    Return the length, in inches, of the tick marks drawn for this contour.  If
    no tick marks are drawn, zero is returned.
     */
    public double getTickLength() {
        return tickLength;
    }

    /**
    Return the spacing, in inches, between tick marks drawn for this contour.  If
    no tick marks are drawn, zero is returned.
     */
    public double getTickSpacing() {
        return tickSpacing;
    }

    /**
    Return an array of the x coordinates for the contour points.
     */
    public double[] getXPoints() {
        return xPoints;
    }

    /**
    Return an array of the y coordinats for the contour points.
     */
    public double[] getYPoints() {
        return yPoints;
    }

    /**
    Return the z level of the contour.
     */
    public double getZValue() {
        return zValue;
    }

    /**
    Return the thickness (in inches) of the contour line.  The text is .75 times
    as thick as the line, up to .02 inches.
     */
    public double getThickness() {
        return thickness;
    }


/*------------------------------------------------------------------------------*/

  /*
   * All the set methods have package scope.  Only the JDisplayList class
   * and its peers can create and fill in a DLContour object.
   */
    void setColor(Color color) {
        this.color = color;
    }

    void setFrameName(String string) {
        frameName = string;
    }

    void setMajor(boolean b) {
        isMajor = b;
    }

    void setItemName(String string) {
        itemName = string;
    }

    void setLabelFont(int i) {
        labelFont = i;
    }

    void setLabelSize(double d) {
        labelSize = d;
    }

    void setLabelSpacing(double d) {
        labelSpacing = d;
    }

    void setLabelText(String string) {
        labelText = string;
    }

    void setLayerName(String string) {
        layerName = string;
    }

    void setNativeIndex(int i) {
        nativeIndex = i;
    }

    void setNumPoints(int i) {
        numPoints = i;
    }

    void setSelectableObject(DLSelectable selectable) {
        selectableObject = selectable;
    }

    void setSurfaceName(String sname) {
        surfaceName = sname;
    }

    void setTickDirection(int i) {
        tickDirection = i;
    }

    void setTickLength(double d) {
        tickLength = d;
    }

    void setTickSpacing(double d) {
        tickSpacing = d;
    }

    void setXPoints(double[] ds) {
        xPoints = ds;
    }

    void setYPoints(double[] ds) {
        yPoints = ds;
    }

    void setZValue(double d) {
        zValue = d;
    }

}
