
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;

import java.awt.Color;
import java.util.ArrayList;

/**
 * Stores color palette info.  Use a ColorPalette for drawing color-filled
 * maps and 2-D graphs where a color range is used to represent values.
 */
public class ColorPalette {

	private static final int DEFAULT_BRIGHTNESS = 5;

  /*
   * Inner class for color control points.
   */
    private class ControlPoint {
        public Color color;
        public int   index;
    }

  /*
   * Define constants used by the color palette.
   */
    public static final int INTERP_RGB = 1;
    public static final int INTERP_HLS = 2;

    public static final int DEFAULT_NCOLORS = 256;
    public static final int MAX_OPACITY = 255;

    private static final int MIN_COLORS = 4;

  /*
   * private class variables.
   */
    private int nColors;
    private Color[] colorArray;
    private ArrayList<ControlPoint> controlList;

    private int interpFlag = INTERP_RGB;

	private int brightness = DEFAULT_BRIGHTNESS;


  /**
   * Default constructor.  Creates a default color palette with 
   * 256 colors.  The default colors use the Hue, Lightness,
   * Saturation scheme.
   */
    public ColorPalette() {
        setNColors(DEFAULT_NCOLORS);
        init();
        setDefaults();
    }

  /**
   * Create a color palette with numColors entries.  The default Hue,
   * Lightness, Saturation colors will be used to fill the palette.
   */
    public ColorPalette(int numColors) {
        setNColors(numColors);
        init();
        setDefaults();
    }

  /**
   * Create a palette with the specified number of colors.  Use the 
   * specified cols array as control points for the pallete.  The
   * cols colors will be spaced evenly across the nColors colors.
   * Colors between the control points are interpolated using the 
   * HLS interpolation.
   */
    public ColorPalette(int numColors, Color[] cols) {
        setNColors(numColors);
        init();
        clearControlPoints();
        setEvenlySpacedControlPoints(cols);
    }

  /**
   * Create a palette with the specified number of colors.  Use the 
   * specified cols array as control points for the pallete.  The
   * cols colors will be spaced evenly across the nColors colors.
   * Colors between the control points are interpolated using the 
   * HLS interpolation.
   */
    public ColorPalette(int numColors, Color[] cols, int interp) {
        setNColors(numColors);
        setInterpFlag (interp);
        init();
        clearControlPoints();
        setEvenlySpacedControlPoints(cols);
    }

    private void init() {
        colorArray = null;
        controlList = new ArrayList<ControlPoint> (10);
    }

  /**
   * Revert to default values. The default palette contains
   * DEFAULT_NCOLORS opaque colors, with five evenly-spaced control
   * points: blue to cyan to green to yellow to red.
   */
    public void setDefaults() {

        int div4 = nColors/4;

        int index = 0;
        Color color = Color.BLUE; // blue
        addControlPoint(color, index, false);

        index += div4;
        color = Color.CYAN;  // cyan
        addControlPoint(color, index, false);

        index += div4;
        color = Color.GREEN; // green
        addControlPoint(color, index, false);

        index += div4;
        color = Color.YELLOW; // yellow
        addControlPoint(color, index, false);

        index = nColors-1;
        color = Color.RED; // red
        addControlPoint(color, index, false);

		brightness = DEFAULT_BRIGHTNESS;

        calcColors();
    }

	public void setBrightness (int brval)
	{
		if (brval == brightness) return;

		if (brval < 0  ||  brval > 10) return;

		brightness = brval;
		calcColors();
	}

  /**
   * Set the color interpolation flag to either INTERP_RGB or to
   * INTERP_HLS.  This does nothing at the moment.  The interpolation
   * is always done in RGB space.
   */
    public void setInterpFlag (int interp)
    {
        if (interp == INTERP_RGB  ||  interp == INTERP_HLS) {
            interpFlag = INTERP_RGB;
        }
    }

  /**
   * Return the array of Colors
   *
   * @return an array of Colors
   */
    public Color[] getColors() {
        if (colorArray == null) calcColors();
        return(colorArray);
    }

  /**
   * Create a color palette's control points by passing in an
   * array of colors.
   *
   * @param colorsToUse the array of colors to use as control points
   */
    public void setEvenlySpacedControlPoints(Color[] colorsToUse) {
        int nCol = colorsToUse.length;
        if (nCol < 2) return;
        clearControlPoints();
        int div = nColors/(nCol-1);
        int index = 0;
        Color coltmp;
        for (int i = 0; i < nCol-1; i++) {
            coltmp = new Color( colorsToUse[i].getRed(),
                                colorsToUse[i].getGreen(),
                                colorsToUse[i].getBlue(),
                                colorsToUse[i].getAlpha());
            addControlPoint(coltmp, index, false);
            index += div;
        }
        index = nColors-1;
        coltmp = new Color(colorsToUse[nCol-1].getRed(),
                           colorsToUse[nCol-1].getGreen(),
                           colorsToUse[nCol-1].getBlue(),
                           colorsToUse[nCol-1].getAlpha());
        addControlPoint(coltmp, index, false);
        calcColors();
  }

  /**
   * Add a control point to the color palette.  A control point consists
   * of a Color and an index into the array of colors and is used to
   * interpolate colors.
   *
   * @param c the Color to use
   * @param i the index at which to use this Color as a control point
   */
    public void addControlPoint(Color c, int i) {
        addControlPoint(c, i, true);
    }

  /**
   * Add a control point to the color palette.  A control point consists
   * of a Color and an index into the array of colors and is used to
   * interpolate colors.
   *
   * @param c the Color to use
   * @param index the index at which to use this Color as a control point
   * @param calcColors if true, this method will recalculate the color range
   * after the control point has been added
   */
    private void addControlPoint(Color c, int index, boolean calcColorsFlag) {

        Color cnew = new Color(c.getRed(),
                               c.getGreen(),
                               c.getBlue(),
                               c.getAlpha());
        ControlPoint cpt = new ControlPoint ();
        cpt.color = cnew;
        cpt.index = index;

        controlList.add (cpt);

        if (calcColorsFlag) {
            calcColors();
        }

    }

  /**
   * Remove a control point.
   *
   * @param index the index of the control point to remove
   */
    public void removeControlPoint(int index) {

        int           i, size;
        ControlPoint  contmp;

        size = controlList.size();
        for (i=size-1; i>=0; i--) {
            contmp = controlList.get(i);
            if (contmp.index == index) {
                controlList.remove (i);
            }
        }

        calcColors();

    }

  /*
   * Clear the list of control points
   */
    public void clearControlPoints() {
        controlList.clear ();
    }

  /**
   * Recalculate colors after adding new control points.  Call this
   * after adding control points.
   */
    public void updateColors() {
        calcColors();
    }

  /**
   * Fill the array of Colors, interpolating colors between control points.
   */
    private void calcColors() {

        colorArray = new Color[nColors];

    /*
     * iterate through control points
     */
        ControlPoint cpt1, cpt2;
        int ncontrol = controlList.size();
        for (int i=0; i<ncontrol-1; i++) {
            cpt1 = controlList.get(i);
            cpt2 = controlList.get(i+1);
            Color cfirst = adjustForBrightness (cpt1.color);
            int indexfirst = cpt1.index;
            Color clast = adjustForBrightness (cpt2.color);
            int indexlast = cpt2.index;
            fillColorRange (cfirst, indexfirst, clast, indexlast);
        }

        fillUnassignedColors ();

    }

	private Color adjustForBrightness (Color cin)
	{
		int        red, green, blue, alpha;

		red = cin.getRed();
		green = cin.getGreen();
		blue = cin.getBlue ();
		alpha = cin.getAlpha ();

		int ibr = 5 - brightness;
		ibr *= 20;

		red += ibr;
		if (red > 255) red = 255;
		if (red < 0) red = 0;

		green += ibr;
		if (green > 255) green = 255;
		if (green < 0) green = 0;

		blue += ibr;
		if (blue > 255) blue = 255;
		if (blue < 0) blue = 0;

		Color c = new Color (red, green, blue, alpha);

		return c;

	}

    private void fillColorRange (Color colorfirst, int indexfirst,
                                 Color colorlast, int indexlast)
    {
        Color        ctmp;
        int          nrange, i;
        int          red, green, blue, alpha;
        int          redfirst, greenfirst, bluefirst, alphafirst;
        int          redlast, greenlast, bluelast, alphalast;
        double       dred, dgreen, dblue, dalpha;

      /*
       * Make the colors at the color array control point indices
       * exactly the same as the control points.
       */
        colorArray[indexfirst] = new Color (colorfirst.getRed(),
                                            colorfirst.getGreen(),
                                            colorfirst.getBlue(),
                                            colorfirst.getAlpha());
        colorArray[indexlast] = new Color (colorlast.getRed(),
                                           colorlast.getGreen(),
                                           colorlast.getBlue(),
                                           colorlast.getAlpha());

      /*
       * Make sure the first index is numerically less
       * than the last index.
       */
        if (indexfirst > indexlast) {
            int itmp = indexfirst;
            indexfirst = indexlast;
            indexlast = itmp;
            ctmp = colorfirst;
            colorfirst = colorlast;
            colorlast = ctmp;
        }

        nrange = indexlast - indexfirst;
        if (nrange < 2) return;

        redfirst = colorfirst.getRed();
        greenfirst = colorfirst.getGreen();
        bluefirst = colorfirst.getBlue();
        alphafirst = colorfirst.getAlpha();

        redlast = colorlast.getRed();
        greenlast = colorlast.getGreen();
        bluelast = colorlast.getBlue();
        alphalast = colorlast.getAlpha();

        if (interpFlag == INTERP_HLS) {
            float[] fhsb = new float[3];
            Color.RGBtoHSB (redfirst, greenfirst, bluefirst, fhsb);
            redfirst = (int)(fhsb[0] * 360.0 + .5);
            greenfirst = (int)(fhsb[2] * 255.0 + .5);
            bluefirst = (int)(fhsb[1] * 255.0 + .5);
            Color.RGBtoHSB (redlast, greenlast, bluelast, fhsb);
            redlast = (int)(fhsb[0] * 360.0 + .5);
            greenlast = (int)(fhsb[2] * 255.0 + .5);
            bluelast = (int)(fhsb[1] * 255.0 + .5);
        }

        dred = (double)(redlast - redfirst) / nrange;
        dgreen = (double)(greenlast - greenfirst) / nrange;
        dblue = (double)(bluelast - bluefirst) / nrange;
        dalpha = (double)(alphalast - alphafirst) / nrange;

        for (i=1; i<nrange; i++) {
            red = redfirst + (int)(dred * i);
            green = greenfirst + (int)(dgreen * i);
            blue = bluefirst + (int)(dblue * i);
            alpha = alphafirst + (int)(dalpha * i);
            if (interpFlag == INTERP_HLS) {
                float fhue = (float)red / 360.0f;
                float fsat = (float)blue / 255.0f;
                float flight = (float)green / 255.0f;
                int iclr = Color.HSBtoRGB (fhue, fsat, flight);
                iclr |= alpha;
                ctmp = new Color (iclr, true);
            }
            else {
                ctmp = new Color (red, green, blue, alpha);
            }
            colorArray[indexfirst + i] = ctmp;
        }

    }

  /*
   * If any colors in the colorArray are null, set them to transparent
   * white.
   */
    private void fillUnassignedColors ()
    {
        int      size, i;

        size = colorArray.length;

        Color    ctmp;
        
        for (i=0; i<size; i++) {
            if (colorArray[i] == null) {
                ctmp = new Color (255, 255, 255, 0);
                colorArray[i] = ctmp;
            }
        }

    }


  /*
   * Accessors (get and set methods)
   */
    public int getNColors() { return nColors; }

    public void setNColors(int newNColors) {
        if (nColors == newNColors) return;

        nColors = newNColors;

        if (nColors < MIN_COLORS) nColors = MIN_COLORS;
    }

}
