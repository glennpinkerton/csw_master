
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
 * This class stores the data for a symb primitive
 * returned from a pick or edit operation.  It is
 * used to return the data and graphic attributes for a symb.  The
 * native display list has the data for all symbs.  An instance of
 * DLSymb will only be created by querying the display list.  Therefore,
 * the constructor is not public and the set methods are not public.
 */

public class DLSymb {

  public static final int SYMBOL_MIN = 1;
  public static final int SYMBOL_MAX = 140;
  /*
   * The variables have package scope so the JDisplayList can
   * access them directly.
   */
    DLSelectable    selectableObject;
    double          xLocation,
                    yLocation;
    int             number;
    double          thickness,
                    angle,
                    size;
    Color           color;
    String          frameName,
                    layerName,
                    itemName,
                    surfaceName;
    int             nativeIndex;

    /**
    Return the rotation angle, in degrees measured counterclockwise from
    the positive x axis.
     */
    public double getAngle() {
        return angle;
    }

    /**
    Return the symbol color.
     */
    public Color getColor() {
        return color;
    }

    /**
    Return the name of the frame that holds the symbol.  If the symbol is not
    in a frame, return null.
     */
    public String getFrameName() {
        return frameName;
    }

    /**
    Return the name of the item that is associated with the symbol.  If the symbol
    is not associated with an item, return null.
     */
    public String getItemName() {
        return itemName;
    }

    /**
    Return the name of the layer that contains the symbol.  If the symbol is not in a layer,
    return null.
     */
    public String getLayerName() {
        return layerName;
    }

    /**
    Return the name of the surface (trimesh or grid) that contains the symbol.
    If the symbol is not in a surface, return null.
     */
    public String getSurfaceName() {
        return surfaceName;
    }

    /**
    Return the index number of the symbol in the native display list.
     */
    public int getNativeIndex() {
        return nativeIndex;
    }

    /**
    Return the number of the symbol drawn.
     */
    public int getNumber() {
        return number;
    }

    /**
    Return the {@link DLSelectable} object that contains the symbol.  If no selecatable
    has this symbol, null is returned.
     */
    public DLSelectable getSelectableObject() {
        return selectableObject;
    }

    /**
    Return the symbol size in inches.
     */
    public double getSize() {
        return size;
    }

    /**
    Return the thickness of lines used to draw the symbol, in inches.
     */
    public double getThickness() {
        return thickness;
    }

    /**
    Return the x coordinate of the symbol location.
     */
    public double getXLocation() {
        return xLocation;
    }

    /**
    Return the y coordinate of the symbol location.
     */
    public double getYLocation() {
        return yLocation;
    }


/*------------------------------------------------------------------------------*/

  /*
   * All of the set methods have package scope.  Only the JDisplayList class and
   * its peers can create and populate a DLSymb object.
   */
    void setAngle(double d) {
        angle = d;
    }

    void setColor(Color color) {
        this.color = color;
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

    void setSurfaceName(String string) {
        surfaceName = string;
    }

    void setNativeIndex(int i) {
        nativeIndex = i;
    }

    void setNumber(int i) {
        number = i;
    }

    void setSelectableObject(DLSelectable selectable) {
        selectableObject = selectable;
    }

    void setSize(double d) {
        size = d;
    }

    void setThickness(double d) {
        thickness = d;
    }

    void setXLocation(double d) {
        xLocation = d;
    }

    void setYLocation(double d) {
        yLocation = d;
    }

}
