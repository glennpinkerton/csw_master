
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import csw.jutils.src.ColorPalette;

/**
 * This class defines an attribute that may be used in the 3D display.  The
 * actual attribute values are not defined here.
 */
public class J3DAttribute
{
    ColorPalette         colorPalette;
    String               name;
    double               hardMin,
                         hardMax;
    double               modelMin,
                         modelMax;
    boolean              logFlag;

/*-------------------------------------------------------------------*/

  /**
   Create an empty 3D attribute definition.
  */
    public J3DAttribute () {}

    /**
     */
    public ColorPalette getColorPalette() {
        return colorPalette;
    }

    /**
     */
    public double getHardMax() {
        return hardMax;
    }

    /**
     */
    public double getHardMin() {
        return hardMin;
    }

    /**
     */
    public boolean isLogFlag() {
        return logFlag;
    }

    /**
     */
    public double getModelMax() {
        return modelMax;
    }

    /**
     */
    public double getModelMin() {
        return modelMin;
    }

    /**
     */
    public String getName() {
        return name;
    }

    /**
     * @param palette
     */
    public void setColorPalette(ColorPalette palette) {
        colorPalette = palette;
    }

    /**
     * @param d
     */
    public void setHardMax(double d) {
        hardMax = d;
    }

    /**
     * @param d
     */
    public void setHardMin(double d) {
        hardMin = d;
    }

    /**
     * @param b
     */
    public void setLogFlag(boolean b) {
        logFlag = b;
    }

    /**
     * @param d
     */
    public void setModelMax(double d) {
        modelMax = d;
    }

    /**
     * @param d
     */
    public void setModelMin(double d) {
        modelMin = d;
    }

    /**
     * @param string
     */
    public void setName(String string) {
        name = string;
    }

}
