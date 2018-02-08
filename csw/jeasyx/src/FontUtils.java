
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

import java.awt.Font;
import java.awt.Toolkit;
import java.awt.font.FontRenderContext;
import java.awt.font.LineMetrics;
import java.awt.geom.AffineTransform;
import java.awt.geom.Rectangle2D;

/**
 This class provides a set of static utility functions to query various
 font attributes.  These are used in conjunction with text drawing in
 the {@link JDisplayList} class.  This class cannot be instantiated.
*/
class FontUtils
{

    private static final String DEFAULT_FONT_NAME = "Lucida Sans Regular";

    private static Font defaultFontBase = null;
    static int   dpi =
        Toolkit.getDefaultToolkit().getScreenResolution();

/*
 * Declare the constructor private so no instances can be
 * created via the new operator.
 */
    private FontUtils(){}


/*
 * The relationship between java font size and size in inches is
 * confusing.  My experience has been that it does not work as
 * documented.  I have isolated the conversion to these two methods to
 * make it easier to maintain if I figure out the problem in the
 * future.
 *
 * G. Pinkerton  11/26/03
 */
    private static int pointSize (double size)
    {
        int isize = (int)(size * (double)dpi + .5);
        if (isize < 2) isize = 2;
        return isize;
    }

    private static double inchSize (double pointSize)
    {
        double size = (pointSize / (double)dpi);
        return size;
    }

/*---------------------------------------------------------------------------*/

/**
 Return a double array with the width and height of the text bounding
 box in the first two elements respectively.  The third element has the
 text ascent.  This version will create a font from the specified font
 name, style and size and then use that font to calculate the returned
 metrics.
@param str The text string to get the metrics for.
@param fontName Name of the font the text will be drawn in.
@param size The height of the characters in inches as drawn on the screen.
@param style One of the font style constants, Font.PLAIN, Font.BOLD or
Font.ITALIC.  You can specify a logical or of Font.BOLD|Font.ITALIC for a
style that is both bold and italic.
*/
    public synchronized static double[] getTextMetrics (
        String     str,
        String     fontName,
        double     size,
        int        style)
    {

    /*
     * Find the resolution in pixels per inch of the screen.
     */
        if (dpi < 1) {
            Toolkit tk = Toolkit.getDefaultToolkit ();
            dpi = tk.getScreenResolution ();
        }

    /*
     * Find a point size for the font.
     */
        int isize = pointSize (size);

    /*
     * Create a new font to use for the metrics.
     */
        Font font = new Font (fontName,
                              style,
                              isize);

        double[] dout = getTextMetrics (str, font);

        return dout;

    }

/*---------------------------------------------------------------------------*/

/**
 Return a double array with the width and height of the text bounding
 box in the first two elements respectively.  The third element has the
 text ascent.  This version uses a previously create font to calculate
 the returned metrics.
@param str The text string to get the metrics for.
@param font The font which will be used to draw the text.
*/
    public synchronized static double[] getTextMetrics (
        String str,
        Font   font)
    {

        FontRenderContext frc =
            new FontRenderContext (new AffineTransform(),
                                   false,
                                   false);

        Rectangle2D bounds =
            font.getStringBounds (str, frc);

        double[] dout = new double[3];

        dout[0] = bounds.getWidth();
        dout[1] = bounds.getHeight();
        double ascent = (double)font.getLineMetrics(str, frc).getAscent();
        dout[2] = dout[1] - ascent;

        dout[1] = ascent;

        return dout;

    }

/*---------------------------------------------------------------------------*/

/**
 Create a new font given the font name, a size in inches and the font style.
 This is isolated here because converting from inches to font point size is
 not as straight forward as it appears.
 @param fname The name of the font.
 @param size  The height of the font characters in inches.
 @param style Either Font.PLAIN, Font.BOLD, Font.ITALIC or the logical
 or of Font.BOLD|Font.ITALIC.
*/

    public synchronized static Font createFont (
        String     fname,
        double     size,
        int        style)
    {
        int        isize;

        isize = pointSize (size);

        Font font = new Font (
            fname,
            style,
            isize);

        return font;

    }

/*----------------------------------------------------------------------------*/

/**
 This method should be used if all hope is lost for getting the font you want.
 The default font is Lucida Sans Normal, which according to the Java documentation
 is guaranteed to be available on every Java Run Time system.  If you specify a
 size smaller than .02 inches, a default size of .10 inches is used.
@param size The font height in inches.
*/
    public synchronized static Font getDefaultFont (double size)
    {
        int     isize;

        if (size < .02) size = .10;

        if (defaultFontBase == null) {
            isize = pointSize (.10);
            defaultFontBase = new Font (
                DEFAULT_FONT_NAME,
                Font.PLAIN,
                isize);
        }

        float fsize = (float)pointSize (size);

        Font font = defaultFontBase.deriveFont (fsize);

        return font;
    }

/*---------------------------------------------------------------------------*/

/**
 Derive a font of the specified size from the specified input font.
 If the input font is null or if the size is less than .02, null is returned.
@param fontIn  The base font to derive from.
@param size    The size, in screen inches, for the new font.
 */
    public synchronized static Font deriveFont (
        Font fontIn,
        double size)
    {
        if (fontIn == null) {
            return null;
        }

        int isize = pointSize (size);

        Font fout = fontIn.deriveFont ((float)isize);

        return fout;
    }

/*---------------------------------------------------------------------------*/

/**
 Return a double array with the width and height of the text bounding
 box in the first two elements respectively.  The third element has the
 text ascent.  This version will create a font from the specified font
 name, style and size and then use that font to calculate the returned
 metrics.  The returned bounds from this method are in inches on the screen,
 not in Java coordinates.  To get the bounds in Java point cordinates,
 use the {@link #getTextMetrics} methods.
@param str The text string to get the metrics for.
@param fontName Name of the font the text will be drawn in.
@param size The height of the characters in inches as drawn on the screen.
@param style One of the font style constants, Font.PLAIN, Font.BOLD or
Font.ITALIC.  You can specify a logical or of Font.BOLD|Font.ITALIC for a
style that is both bold and italic.
*/
    public synchronized static double[] getTextBounds (
        String     str,
        String     fontName,
        double     size,
        int        style)
    {

    /*
     * Find the resolution in pixels per inch of the screen.
     */

    /*
     * Find a point size for the font.
     */
        int isize = pointSize (size);

    /*
     * Create a new font to use for the metrics.
     */
        Font font = new Font (fontName,
                              style,
                              isize);

        double[] dout = getTextBounds (str, font);

        return dout;

    }

/*---------------------------------------------------------------------------*/

/**
 Return a double array with the width and height of the text bounding
 box in the first two elements respectively.  The third element has the
 text ascent.  This version uses a previously create font to calculate
 the returned metrics.  The bounds are returned in screen inches, not in points.
 To get bounds in points, use the {@link #getTextMetrics} methods.
@param str The text string to get the metrics for.
@param font The font which will be used to draw the text.
*/
    public synchronized static double[] getTextBounds (
        String str,
        Font   font)
    {

        FontRenderContext frc =
            new FontRenderContext (new AffineTransform(),
                                   false,
                                   false);

        Rectangle2D bounds =
            font.getStringBounds (str, frc);

        double[] dout = new double[3];

        LineMetrics metrics = font.getLineMetrics (str, frc);


        dout[0] = bounds.getWidth();
        dout[1] = bounds.getHeight();

        double ascent = (double)metrics.getAscent();
        dout[2] = dout[1] - ascent;

        dout[1] = ascent;

        dout[0] = inchSize(dout[0]);
        dout[1] = inchSize(dout[1]);
        dout[2] = inchSize(dout[2]);

        return dout;

    }


}  // end of main FontUtils class
