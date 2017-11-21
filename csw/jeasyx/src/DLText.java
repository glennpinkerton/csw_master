
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
 * This class stores the data for a text primitive returned from a pick or
 * edit operation.  It is 
 * used to return the data and graphic attributes for a text.  The
 * native display list has the data for all texts.  An instance of
 * DLText will only be created by querying the display list.  Therefore,
 * the constructor is not public and the set methods are not public.
 */

public class DLText {

  /*
   * The variables have package scope so the JDisplayList can
   * access them directly.
   */
    DLSelectable    selectableObject;
    double          xLocation,
                    yLocation;
    int             anchor;
    double          thickness,
                    backgroundBorderThickness,
                    angle,
                    size,
                    xOffset,
                    yOffset;
    Color           color,
                    fillColor,
                    backgroundFillColor,
                    backgroundBorderColor;
    int             backgroundFlag,
                    fontNumber;
    String          textData;
    String          frameName,
                    layerName,
                    itemName;
    int             nativeIndex;

    /**
    Return the text anchor position.  One of the following will be returned.
<ui>
   <li>{@link DLConst#TEXT_BOTTOM_LEFT}
   <li>{@link DLConst#TEXT_BOTTOM_CENTER}
   <li>{@link DLConst#TEXT_BOTTOM_RIGHT}
   <li>{@link DLConst#TEXT_CENTER_LEFT}
   <li>{@link DLConst#TEXT_CENTER_CENTER}
   <li>{@link DLConst#TEXT_CENTER_RIGHT}
   <li>{@link DLConst#TEXT_TOP_LEFT}
   <li>{@link DLConst#TEXT_TOP_CENTER}
   <li>{@link DLConst#TEXT_TOP_RIGHT}
</ui>
     */
    public int getAnchor() {
        return anchor;
    }

    /**
    Return the rotation angle for the text.  The text is rotated around its anchor.
    The angle is in degrees, measured counterclockwise from the positive x axis.
     */
    public double getAngle() {
        return angle;
    }

    /**
    Return the background border color.  If no background border is drawn, null is returned.
     */
    public Color getBackgroundBorderColor() {
        return backgroundBorderColor;
    }

    /**
    Return the background border thickness, in inches.
     */
    public double getBackgroundBorderThickness() {
        return backgroundBorderThickness;
    }

    /**
    Return the color of the background rectangle fill.  If there is no background rectangle,
    this is null.
     */
    public Color getBackgroundFillColor() {
        return backgroundFillColor;
    }

    /**
    Return the background rectangle flag for the text.  The flag will be one of:
   <ui>
   <li> {@link DLConst#TEXT_BG_NONE}
   <li> {@link DLConst#TEXT_BG_FILLED}
   <li> {@link DLConst#TEXT_BG_FILLED_BORDER}
   <li> {@link DLConst#TEXT_BG_CURVE_FILLED}
   <li> {@link DLConst#TEXT_BG_CURVE_FILLED_BORDER}
   </ui>
     */
    public int getBackgroundFlag() {
        return backgroundFlag;
    }

    /**
    Return the color for lines used to draw text characters.
     */
    public Color getColor() {
        return color;
    }

    /**
    Return the color for polygon fills used to draw text characters.  If the polygons
    in the text are not filled, null is returned.
     */
    public Color getFillColor() {
        return fillColor;
    }

    /**
    Return the font number for the text.
     */
    public int getFontNumber() {
        return fontNumber;
    }

    /**
    Return the name of the frame that holds the text.  If the text is not in a frame, 
    null is returned.
     */
    public String getFrameName() {
        return frameName;
    }

    /**
    Return the name of the item associated with the text.  If the text is not associated
    with an item, null is returned.
     */
    public String getItemName() {
        return itemName;
    }

    /**
    Return the name of the layer that contains the text.  If the text is not in a layer,
    null is returned.
     */
    public String getLayerName() {
        return layerName;
    }

    /**
    Return the index number of the text in the native display list.
     */
    public int getNativeIndex() {
        return nativeIndex;
    }

    /**
    Return the {@link DLSelectable} object that contains the text.
     */
    public DLSelectable getSelectableObject() {
        return selectableObject;
    }

    /**
    Return the height of the text characters, in inches.
     */
    public double getSize() {
        return size;
    }

    /**
    Return the actual text string.
     */
    public String getTextData() {
        return textData;
    }

    /**
    Return the thickness of lines and polygon borders used to draw the text, in inches.
     */
    public double getThickness() {
        return thickness;
    }

    /**
    Return the x coordinate of the text anchor position.  This is in "world" coordinates.
     */
    public double getXLocation() {
        return xLocation;
    }

    /**
    Return the horizontal offset, in inches, from the x location.
     */
    public double getXOffset() {
        return xOffset;
    }

    /**
    Return the y coordinate of the text anchor position.  This is in "world" coordinates.
     */
    public double getYLocation() {
        return yLocation;
    }

    /**
    Return the vertical offset, in inches, from the y location.
     */
    public double getYOffset() {
        return yOffset;
    }

/*-------------------------------------------------------------------------------------------*/

  /*
   * The set methods have package scope.  Only the JDisplayList class and its peers are
   * allowed to create and populate a DLText object.
   */
    public void setAnchor(int i) {
        anchor = i;
    }

    public void setAngle(double d) {
        angle = d;
    }

    public void setBackgroundBorderColor(Color color) {
        backgroundBorderColor = color;
    }

    public void setBackgroundBorderThickness(double d) {
        backgroundBorderThickness = d;
    }

    public void setBackgroundFillColor(Color color) {
        backgroundFillColor = color;
    }

    public void setBackgroundFlag(int i) {
        backgroundFlag = i;
    }

    public void setColor(Color color) {
        this.color = color;
    }

    public void setFillColor(Color color) {
        fillColor = color;
    }

    public void setFontNumber(int i) {
        fontNumber = i;
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

    public void setNativeIndex(int i) {
        nativeIndex = i;
    }

    public void setSelectableObject(DLSelectable selectable) {
        selectableObject = selectable;
    }

    public void setSize(double d) {
        size = d;
    }

    public void setString(String string) {
        textData = string;
    }

    public void setThickness(double d) {
        thickness = d;
    }

    public void setXLocation(double d) {
        xLocation = d;
    }

    public void setXOffset(double d) {
        xOffset = d;
    }

    public void setYLocation(double d) {
        yLocation = d;
    }

    public void setYOffset(double d) {
        yOffset = d;
    }

}
