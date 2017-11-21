
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/**
 *
 * This class is only used by the JDisplayListBase class.  I removed
 * the LocalDraw functionality from JDisplayListBase to make the
 * file smaller and more manageable.
 *
 * LocalDraw has default access.  Only JDisplayListBase should use it.
 *
 *  @author Glenn Pinkerton
 *
*/
package csw.jeasyx.src;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.TexturePaint;
import java.awt.geom.AffineTransform;
import java.awt.geom.Arc2D;
import java.awt.geom.GeneralPath;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.util.ArrayList;

class LocalDraw {

    private static final double DTORAD = -3.1415926 / 180.0;

    private ArrayList<NativePrim.Fill>   nativeFillArray = null;
    private ArrayList<NativePrim.Line>   nativeLineArray = null;
    private ArrayList<NativePrim.Text>   nativeTextArray = null;
    private ArrayList<NativePrim.Arc>    nativeArcArray = null;
    private ArrayList<NativePrim.FilledArc>  nativeFilledArcArray = null;
    private ArrayList<NativePrim.Image>  nativeImageArray = null;
    private ArrayList<NativePrim.Frame>  nativeFrameArray = null;

  /*
   * Variables to keep track of the device viewport.
   */
    private int screen_width = -1;
    private int screen_height = -1;

    private int background_red = -1;
    private int background_green = -1;
    private int background_blue = -1;
    private int background_alpha = -1;

    private int anti_alias_ok = 1;

    private float[] dashLine2 = new float[2];
    private float[] dashLine4 = new float[4];

  /*
   * Constant array with line dashes as integers.
   */
    private static final int[][] dashTable =
    {
        {2, 2}, // pattern number 1
        {4, 4}, // pattern number 2
        {8, 8}, // pattern number 3
        {16, 16}, // pattern number 4
        {2, 4}, // pattern number 5
        {4, 2}, // pattern number 6
        {4, 8}, // pattern number 7
        {8, 4}, // pattern number 8
        {8, 2}, // pattern number 9
        {8, 16}, // pattern number 10
        {16, 2}, // pattern number 11
        {16, 4}, // pattern number 12
        {16, 8}, // pattern number 13
        {2, 2, 2, 4}, // pattern number 14
        {2, 4, 2, 8}, // pattern number 15
        {4, 2, 4, 8}, // pattern number 16
        {4, 4, 4, 8}, // pattern number 17
        {8, 4, 8, 8}, // pattern number 18
        {8, 8, 8, 16}, // pattern number 19
        {2, 2, 4, 2}, // pattern number 20
        {2, 2, 8, 2}, // pattern number 21
        {2, 2, 16, 2}, // pattern number 22
        {4, 2, 8, 2}, // pattern number 23
        {4, 2, 16, 2}, // pattern number 24
        {8, 2, 16, 2}, // pattern number 25
        {2, 4, 4, 4}, // pattern number 26
        {2, 4, 8, 4}, // pattern number 27
        {4, 4, 8, 4}, // pattern number 28
        {4, 4, 16, 4}, // pattern number 29
        {8, 8, 16, 4}  // pattern number 30
    };

  /**
   * Set some properties needed to draw to the screen.
   */
    void setProperties (
        int        width,
        int        height,
        int        red,
        int        green,
        int        blue,
        int        alpha
    )
    {
        screen_width = width;
        screen_height = height;
        background_red = red;
        background_green = green;
        background_blue = blue;
        background_alpha = alpha;

        return;
    }

/*--------------------------------------------------------------------------*/

  /**
   * Specify the arrays of graphic primitives to draw.
   */
    void setNativeArrays (
        ArrayList<NativePrim.Line>   lines,
        ArrayList<NativePrim.Fill>   fills,
        ArrayList<NativePrim.Text>   texts,
        ArrayList<NativePrim.Arc>    arcs,
        ArrayList<NativePrim.FilledArc>   farcs,
        ArrayList<NativePrim.Image>   images,
        ArrayList<NativePrim.Frame>   frames
    )
    {
        nativeFillArray = fills;
        nativeLineArray = lines;
        nativeTextArray = texts;
        nativeArcArray = arcs;
        nativeFilledArcArray = farcs;
        nativeImageArray = images;
        nativeFrameArray = frames;

        return;
    }


/*--------------------------------------------------------------------------*/

  /**
   *  Draw the primitives to the screen.  The primitives need to be specified
   *  by calling the setNativeArrays method prior to calling this method.  This
   *  is only called from JDisplayListBase.  It has package scope and cannot be
   *  accessed from application code.
   */
    int localDraw (Graphics2D g2d_bg, Graphics2D g2d_fg) {

        int             nprim, i, ntot, j;
        int             a, r, g, b;
        int             closure, pattern, small_flag;
        int[]           rgb_array;
        Graphics2D      g2d;
        double          patScale;

        NativePrim.Image     native_image;
        NativePrim.Arc       native_arc;
        NativePrim.FilledArc native_filled_arc;
        NativePrim.Text      native_text;
        NativePrim.Line      native_line;
        NativePrim.Fill      native_fill;
        NativePrim.Frame     native_frame;

        Color           fill_color;
        Color           arc_color;

        int             fgflag = 0;

    /*
     * Do nothing if properties have not been set up.
     */
        if (screen_width < 0  ||  screen_height < 0) {
            return fgflag;
        }
        if (background_red < 0  ||
            background_green < 0  ||
            background_blue < 0  ||
            background_alpha < 0) {
            return fgflag;
        }

        AffineTransform t = g2d_bg.getTransform ();
        t.setToScale (1.0, 1.0);
        t = g2d_fg.getTransform ();
        t.setToScale (1.0, 1.0);

    /*
     * Initialize the background image with opaque background colors.
     */
        if (g2d_bg != g2d_fg) {
            fill_color = new Color (
                background_red,
                background_green,
                background_blue,
                background_alpha
            );
            g2d_bg.setColor (fill_color);
            g2d_bg.fillRect (0, 0, screen_width, screen_height);
            g2d_bg.setClip (null);
        }

    /*
     * Initialize the foreground image with transparent background colors.
     */
        fill_color = new Color (
            background_red,
            background_green,
            background_blue,
            0
        );
        g2d_fg.setColor (fill_color);
        g2d_fg.fillRect (0, 0, screen_width, screen_height);
        g2d_fg.setClip (null);

        g2d_bg.setRenderingHint (
            RenderingHints.KEY_RENDERING,
            RenderingHints.VALUE_RENDER_QUALITY
        );
        g2d_fg.setRenderingHint (
            RenderingHints.KEY_RENDERING,
            RenderingHints.VALUE_RENDER_QUALITY
        );

        g2d_fg.setRenderingHint (
            RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON
        );
        g2d_bg.setRenderingHint (
            RenderingHints.KEY_ANTIALIASING,
            RenderingHints.VALUE_ANTIALIAS_ON
        );

    /*
     * Draw the frame borders for non clipping frames first.
     * Frame borders are always drawn to the background buffer.
     */
        if (nativeFrameArray != null) {
            nprim = nativeFrameArray.size();
            for (i=0; i<nprim; i++) {
                native_frame = nativeFrameArray.get(i);
                if (native_frame.borderflag != 0  &&
                    native_frame.clip_shape != null  &&
                    native_frame.clipflag == 0) {
                    lookupAndSetLinePattern (g2d_bg, 0, 1.0f);
                    g2d_bg.setColor (Color.black);
                    g2d_bg.draw (native_frame.clip_shape);
                }
            }
        }
        lookupAndSetLinePattern (g2d_bg, 0, 0.0f);

    /*
     * Draw all the images to the appropriate buffers.  These are always
     * drawn first.  They have the lowest priority.
     */
        if (nativeImageArray != null) {
            SortImagesByID ();
            nprim = nativeImageArray.size();
            for (i=0; i<nprim; i++) {
                native_image = nativeImageArray.get(i);
                if (native_image == null) {
                    continue;
                }
                g2d = g2d_bg;
                if (native_image.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }
                BufferedImage bimage =
                new BufferedImage (
                    native_image.ncol,
                    native_image.nrow,
                    BufferedImage.TYPE_INT_ARGB
                );
                ntot = native_image.ncol * native_image.nrow;
                rgb_array = new int[ntot];

              /*
               * Java pixels using the ARGB color model pack an
               * integer as 0xAARRGGBB.  The loop below takes
               * each individual color component and packs
               * them in this manner.
               */
                for (j=0; j<ntot; j++) {
                    a = native_image.transparency[j];
                    r = native_image.red[j];
                    g = native_image.green[j];
                    b = native_image.blue[j];
                    a = a << 24;
                    r = (r << 16) & 0x00ff0000;
                    g = (g << 8) & 0x0000ff00;
                    b &= 0x000000ff;
                    rgb_array[j] = a | r | g | b;
                }

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_image.frame_num);

              /*
               * set the rgb colors for the entire image.
               */
                bimage.setRGB (0, 0,
                               native_image.ncol,
                               native_image.nrow,
                               rgb_array,
                               0, native_image.ncol);
              /*
               * Draw the image to the current g2d.  The graphics context
               * is responsible for scaling the image to fit the specified
               * rectangular area on the screen.
               */
                int iwidth = (int)(native_image.x2 - native_image.x1);
                int iheight = (int)(native_image.y2 - native_image.y1);
                int ix = (int)native_image.x1;
                int iy = (int)native_image.y1;

                g2d.drawImage (bimage,
                               (int)native_image.x1,
                               (int)native_image.y1,
                               iwidth,
                               iheight,
                               null /* no image observer */
                );

            /*
             * If this image has vector primitives bundled with it,
             * draw thoe vector primitives now.
             */
                if (native_image.id >= 0  &&  native_image.hasLines) {
                    DrawVectorsOverImage (native_image.id, g2d_fg, g2d_bg);
                }
            }

        }  /* end of image section */

    /*
     * Draw all the fills that have their small flag set
     * to zero to the appropriate buffers.  These are fills not
     * used in symbols or text.  This includes fills with snall_flag
     * of zero (interior) or 9 (border).
     */
        if (nativeFillArray != null) {
            nprim = nativeFillArray.size ();
            for (i=0; i<nprim; i++) {
                native_fill = nativeFillArray.get(i);
                if (native_fill.image >= 0) {
                    continue;
                }
                if (native_fill == null) {
                    continue;
                }
                small_flag = native_fill.pattern / 10000;
                if (!(small_flag == 0  ||  small_flag == 9)) {
                    continue;
                }
                pattern = native_fill.pattern % 10000;
                g2d = g2d_bg;
                if (native_fill.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

                patScale = native_fill.patScale;
                if (patScale < 0.5) patScale = 1.0;
                if (patScale > 10.0) patScale = 1.0;

              /*
               * Create a general path for filling.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_fill.npts + 2);
                gpath.moveTo (
                    native_fill.xy[0],
                    native_fill.xy[1]
                );
                for (j=1; j<native_fill.npts; j++) {
                    gpath.lineTo (
                        native_fill.xy[2*j],
                        native_fill.xy[2*j+1]
                    );
                }

                if (small_flag == 0) {
                    gpath.closePath ();
                }

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_fill.frame_num);

              /*
               * If small_flag is 9, this is a fill border, so draw it instead of filling it.
               */
                if (small_flag == 9) {
                    if (native_fill.red >= 0  &&
                        native_fill.green >= 0  &&
                        native_fill.blue >= 0) {
                        fill_color =
                        new Color (native_fill.red,
                                   native_fill.green,
                                   native_fill.blue,
                                   native_fill.alpha
                                  );
                        lookupAndSetLinePattern (g2d, pattern, native_fill.thickness);
                        g2d.setColor (fill_color);
                        g2d.draw (gpath);
                    }
                }

              /*
               * draw interior of polygon.
               */
                else {
                  /*
                   * draw solid color first.
                   */
                    if (pattern <= 0) {
                        if (native_fill.red >= 0  &&
                            native_fill.green >= 0  &&
                            native_fill.blue >= 0) {
                            fill_color =
                            new Color (native_fill.red,
                                       native_fill.green,
                                       native_fill.blue,
                                       native_fill.alpha
                                      );
                            g2d.setColor (fill_color);
                            g2d.fill (gpath);
                        }
                    }

                  /*
                   * draw the fill pattern if needed.
                   */
                    else if (pattern > 0) {
                        fill_color =
                        new Color (native_fill.red,
                                   native_fill.green,
                                   native_fill.blue,
                                   native_fill.alpha
                                  );
                        lookupAndSetFillPattern (g2d, pattern, patScale, fill_color);
                        g2d.fill (gpath);
                    }
                }
            }
        }  /* end of large filled polygon section */

    /*
     * Draw all the arc fills that have their small flag set to zero.
     * These are arc fills not used in symbols or text.
     */
        if (nativeFilledArcArray != null) {
            nprim = nativeFilledArcArray.size ();
            for (i=0; i<nprim; i++) {
                native_filled_arc = nativeFilledArcArray.get(i);
                if (native_filled_arc.image >= 0) {
                    continue;
                }
                if (native_filled_arc == null) {
                    continue;
                }
                small_flag = native_filled_arc.closure / 10000;
                if (small_flag != 0) {
                    continue;
                }
                closure = native_filled_arc.closure % 10000;
                g2d = g2d_bg;
                if (native_filled_arc.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create an Arc2D object.
               */
                int type = Arc2D.OPEN;
                if (closure == 1) {
                    type = Arc2D.PIE;
                }
                else if (closure == 2) {
                    type = Arc2D.CHORD;
                }
                Arc2D.Float arc =
                new Arc2D.Float (
                    native_filled_arc.x - native_filled_arc.r1,
                    native_filled_arc.y - native_filled_arc.r2,
                    native_filled_arc.r1 * 2.0f,
                    native_filled_arc.r2 * 2.0f,
                    native_filled_arc.ang1,
                    native_filled_arc.ang2,
                    type);

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_filled_arc.frame_num);

              /*
               * Draw filled arc in solid color first.
               */
                if (native_filled_arc.red >= 0  &&
                    native_filled_arc.green >= 0  &&
                    native_filled_arc.blue >= 0) {

                 /*
                  * Set the fill color for the arc.
                  */
                    fill_color =
                    new Color (native_filled_arc.red,
                               native_filled_arc.green,
                               native_filled_arc.blue,
                               native_filled_arc.alpha
                              );
                    g2d.setColor (fill_color);

                 /*
                  * If the rotation angle is not very near zero,
                  * save the current transform.  Then modify the
                  * current transform to rotate the arc.  Then
                  * reinstate the saved transform.
                  */
                    if (native_filled_arc.angle > 0.1  ||
                        native_filled_arc.angle < -0.1) {
                        AffineTransform tsave = g2d.getTransform ();
                        g2d.rotate (
                            native_filled_arc.angle * DTORAD,
                            (native_filled_arc.x),
                            (native_filled_arc.y)
                        );
                        g2d.fill (arc);
                        g2d.setTransform (tsave);
                    }

                 /*
                  * No rotation needed.
                  */
                    else {
                        g2d.fill (arc);
                    }

                }

              /*
               * Draw in pattern fill.
               */
                if (native_filled_arc.pattern > 0) {
                    lookupAndSetFillPattern (g2d, native_filled_arc.pattern);
                    if (native_filled_arc.angle > 0.1  ||
                        native_filled_arc.angle < -0.1) {
                        AffineTransform tsave = g2d.getTransform ();
                        g2d.rotate (
                            native_filled_arc.angle * DTORAD,
                            (native_filled_arc.x),
                            (native_filled_arc.y)
                        );
                        g2d.fill (arc);
                        g2d.setTransform (tsave);
                    }

                 /*
                  * No rotation needed.
                  */
                    else {
                        g2d.fill (arc);
                    }
                }

            }

        }  /* end of large filled arc section */

    /*
     * Draw all the lines that have their small flag set
     * to zero to the appropriate buffers.  These are lines
     * not used in symbols or text.  This is done in 2 passes.
     * The solid lines are drawn first, followed by dashed lines.
     * This is done so dashed lines directly on top of solid lines
     * will be visible.
     */
        if (nativeLineArray != null) {
            nprim = nativeLineArray.size ();

            for (int ido=0; ido<2; ido++) {

                for (i=0; i<nprim; i++) {

                    native_line = nativeLineArray.get(i);
                    if (native_line.image >= 0) {
                        continue;
                    }
                    if (native_line == null) {
                        continue;
                    }
                    if (native_line.npts < 2) {
                        continue;
                    }
                    small_flag = native_line.pattern / 10000;
/*
                    if (small_flag != 0) {
                        continue;
                    }
*/
                    pattern = native_line.pattern % 10000;

                    if (ido == 0) {
                        if (pattern % 1000 != 0) {
                            continue;
                        }
                    }
                    else {
                        if (pattern % 1000 == 0) {
                            continue;
                        }
                    }

                    g2d = g2d_bg;
                    if (native_line.selectable == 1) {
                        fgflag = 1;
                        g2d = g2d_fg;
                    }

                  /*
                   * set clipping if needed.
                   */
                    lookupAndSetClip (g2d,
                                  native_line.frame_num);

                  /*
                   * Create a general path for stroking.
                   */
                    GeneralPath gpath =
                    new GeneralPath (
                        GeneralPath.WIND_EVEN_ODD,
                        native_line.npts);
                    gpath.moveTo (
                        native_line.xy[0],
                        native_line.xy[1]
                    );
                    for (j=1; j<native_line.npts; j++) {
                        gpath.lineTo (
                            native_line.xy[2*j],
                            native_line.xy[2*j+1]
                        );
                    }

                  /*
                   * Set the color for line drawing.
                   */
                    if (native_line.red >= 0  &&
                        native_line.green >= 0  &&
                        native_line.blue >= 0) {
                        fill_color =
                        new Color (native_line.red,
                                   native_line.green,
                                   native_line.blue,
                                   native_line.alpha
                                  );
                        g2d.setColor (fill_color);
                    }
                    else {
                        continue;
                    }

                  /*
                   * draw the line
                   */
                    lookupAndSetLinePattern (g2d, pattern, native_line.thickness);
                    g2d.draw (gpath);
                }
            }

        }  /* end of large line section */

    /*
     * Draw all the arcs that have their small flag set to zero.
     * These are arcs not used in symbols or text.
     */
        if (nativeArcArray != null) {
            nprim = nativeArcArray.size ();
            for (i=0; i<nprim; i++) {
                native_arc = nativeArcArray.get(i);
                if (native_arc.image >= 0) {
                    continue;
                }
                if (native_arc == null) {
                    continue;
                }
                small_flag = native_arc.closure / 10000;
                if (small_flag != 0) {
                    continue;
                }
                closure = native_arc.closure % 10000;
                g2d = g2d_bg;
                if (native_arc.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Draw the arc if it has valid colors.
               */
                if (native_arc.red >= 0  &&
                    native_arc.green >= 0  &&
                    native_arc.blue >= 0) {

                  /*
                   * Create an Arc2D object.
                   */
                    int type = Arc2D.OPEN;
                    if (closure == 1) {
                        type = Arc2D.PIE;
                    }
                    else if (closure == 2) {
                        type = Arc2D.CHORD;
                    }
                    Arc2D.Float arc =
                    new Arc2D.Float (
                        native_arc.x - native_arc.r1,
                        native_arc.y - native_arc.r2,
                        native_arc.r1 * 2.0f,
                        native_arc.r2 * 2.0f,
                        native_arc.ang1,
                        native_arc.ang2,
                        type);

                 /*
                  * Set the color for the arc.
                  */
                    arc_color =
                    new Color (native_arc.red,
                               native_arc.green,
                               native_arc.blue,
                               native_arc.alpha
                              );
                    g2d.setColor (arc_color);
                    lookupAndSetLinePattern (g2d, 0, native_arc.thickness);

                 /*
                  * set clipping if needed.
                  */
                    lookupAndSetClip (g2d,
                                      native_arc.frame_num);

                 /*
                  * If the rotation angle is not very near zero,
                  * save the current transform.  Then modify the
                  * current transform to rotate the arc.  Then
                  * reinstate the saved transform.
                  */
                    if (native_arc.angle > 0.1  ||
                        native_arc.angle < -0.1) {
                        AffineTransform tsave = g2d.getTransform ();
                        g2d.rotate (
                            native_arc.angle * DTORAD,
                            (native_arc.x),
                            (native_arc.y)
                        );
                        g2d.draw (arc);
                        g2d.setTransform (tsave);
                    }

                 /*
                  * No rotation needed.
                  */
                    else {
                        g2d.draw (arc);
                    }

                }

            }

        }  /* end of large arc section */

    /*
     * Draw all the fills that have their small flag set
     * to one to the appropriate buffers.  These are fills
     * used for symbols.
     */
        if (nativeFillArray != null) {
            nprim = nativeFillArray.size ();
            for (i=0; i<nprim; i++) {
                native_fill = nativeFillArray.get(i);
                if (native_fill.image >= 0) {
                    continue;
                }
                if (native_fill == null) {
                    continue;
                }
                if (native_fill.npts < 3) {
                    continue;
                }
                small_flag = native_fill.pattern / 10000;
                if (small_flag != 1) {
                    continue;
                }
                pattern = native_fill.pattern % 10000;
                g2d = g2d_bg;
                if (native_fill.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for filling.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_fill.npts + 2);
                gpath.moveTo (
                    native_fill.xy[0],
                    native_fill.xy[1]
                );
                for (j=1; j<native_fill.npts; j++) {
                    gpath.lineTo (
                        native_fill.xy[2*j],
                        native_fill.xy[2*j+1]
                    );
                }
                gpath.closePath ();

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_fill.frame_num);

              /*
               * draw solid color first.
               */
                if (native_fill.red >= 0  &&
                    native_fill.green >= 0  &&
                    native_fill.blue >= 0) {
                    fill_color =
                    new Color (native_fill.red,
                               native_fill.green,
                               native_fill.blue,
                               native_fill.alpha
                              );
                    g2d.setColor (fill_color);
                    g2d.fill (gpath);
                }

              /*
               * draw the fill pattern if needed.
               */
                if (pattern > 0) {
                    lookupAndSetFillPattern (g2d, pattern);
                    g2d.fill (gpath);
                }
            }
        }  /* end of small filled polygon section */

    /*
     * Draw all the arc fills that have their small flag set to one.
     * These are arc fills used for symbols.
     */
        if (nativeFilledArcArray != null) {
            nprim = nativeFilledArcArray.size ();
            for (i=0; i<nprim; i++) {
                native_filled_arc = nativeFilledArcArray.get(i);
                if (native_filled_arc.image >= 0) {
                    continue;
                }
                if (native_filled_arc == null) {
                    continue;
                }
                small_flag = native_filled_arc.closure / 10000;
                if (small_flag != 1) {
                    continue;
                }
                closure = native_filled_arc.closure % 10000;
                g2d = g2d_bg;
                if (native_filled_arc.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create an Arc2D object.
               */
                int type = Arc2D.OPEN;
                if (closure == 1) {
                    type = Arc2D.PIE;
                }
                else if (closure == 2) {
                    type = Arc2D.CHORD;
                }
                Arc2D.Float arc =
                new Arc2D.Float (
                    native_filled_arc.x - native_filled_arc.r1,
                    native_filled_arc.y - native_filled_arc.r2,
                    native_filled_arc.r1 * 2.0f,
                    native_filled_arc.r2 * 2.0f,
                    native_filled_arc.ang1,
                    native_filled_arc.ang2,
                    type);

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_filled_arc.frame_num);

              /*
               * Draw in solid color first.
               */
                if (native_filled_arc.red >= 0  &&
                    native_filled_arc.green >= 0  &&
                    native_filled_arc.blue >= 0) {

                 /*
                  * Set the fill color for the arc.
                  */
                    fill_color =
                    new Color (native_filled_arc.red,
                               native_filled_arc.green,
                               native_filled_arc.blue,
                               native_filled_arc.alpha
                              );
                    g2d.setColor (fill_color);

                 /*
                  * If the rotation angle is not very near zero,
                  * save the current transform.  Then modify the
                  * current transform to rotate the arc.  Then
                  * reinstate the saved transform.
                  */
                    if (native_filled_arc.angle > 0.1  ||
                        native_filled_arc.angle < -0.1) {
                        AffineTransform tsave = g2d.getTransform ();
                        g2d.rotate (
                            native_filled_arc.angle * DTORAD,
                            (native_filled_arc.x),
                            (native_filled_arc.y)
                        );
                        g2d.fill (arc);
                        g2d.setTransform (tsave);
                    }

                 /*
                  * No rotation needed.
                  */
                    else {
                        g2d.fill (arc);
                    }

                }

              /*
               * Draw in pattern fill.
               */
                if (native_filled_arc.pattern > 0) {
                    lookupAndSetFillPattern (g2d, native_filled_arc.pattern);
                    if (native_filled_arc.angle > 0.1  ||
                        native_filled_arc.angle < -0.1) {
                        AffineTransform tsave = g2d.getTransform ();
                        g2d.rotate (
                            native_filled_arc.angle * DTORAD,
                            (native_filled_arc.x),
                            (native_filled_arc.y)
                        );
                        g2d.fill (arc);
                        g2d.setTransform (tsave);
                    }

                 /*
                  * No rotation needed.
                  */
                    else {
                        g2d.fill (arc);
                    }
                }


            }

        }  /* end of small filled arc section */

    /*
     * Draw all the arcs that have their small flag set to one.
     * These are arcs used for symbols.
     */
        if (nativeArcArray != null) {
            nprim = nativeArcArray.size ();
            for (i=0; i<nprim; i++) {
                native_arc = nativeArcArray.get(i);
                if (native_arc.image >= 0) {
                    continue;
                }
                if (native_arc == null) {
                    continue;
                }
                small_flag = native_arc.closure / 10000;
                if (small_flag != 1) {
                    continue;
                }
                closure = native_arc.closure % 10000;
                g2d = g2d_bg;
                if (native_arc.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Draw the arc if it has valid colors.
               */
                if (native_arc.red >= 0  &&
                    native_arc.green >= 0  &&
                    native_arc.blue >= 0) {

                  /*
                   * Create an Arc2D object.
                   */
                    int type = Arc2D.OPEN;
                    if (closure == 1) {
                        type = Arc2D.PIE;
                    }
                    else if (closure == 2) {
                        type = Arc2D.CHORD;
                    }
                    Arc2D.Float arc =
                    new Arc2D.Float (
                        native_arc.x - native_arc.r1,
                        native_arc.y - native_arc.r2,
                        native_arc.r1 * 2.0f,
                        native_arc.r2 * 2.0f,
                        native_arc.ang1,
                        native_arc.ang2,
                        type);

                 /*
                  * Set the color for the arc.
                  */
                    arc_color =
                    new Color (native_arc.red,
                               native_arc.green,
                               native_arc.blue,
                               native_arc.alpha
                              );
                    g2d.setColor (arc_color);
                    lookupAndSetLinePattern (g2d, 0, native_arc.thickness);

                 /*
                  * set clipping if needed.
                  */
                    lookupAndSetClip (g2d,
                                      native_arc.frame_num);

                 /*
                  * If the rotation angle is not very near zero,
                  * save the current transform.  Then modify the
                  * current transform to rotate the arc.  Then
                  * reinstate the saved transform.
                  */
                    if (native_arc.angle > 0.1  ||
                        native_arc.angle < -0.1) {
                        AffineTransform tsave = g2d.getTransform ();
                        g2d.rotate (
                            native_arc.angle * DTORAD,
                            (native_arc.x),
                            (native_arc.y)
                        );
                        g2d.draw (arc);
                        g2d.setTransform (tsave);
                    }

                 /*
                  * No rotation needed.
                  */
                    else {
                        g2d.draw (arc);
                    }

                }

            }

        }  /* end of small arc section */

    /*
     * Draw all the lines that have their small flag set
     * to one to the appropriate buffers.  These are lines
     * used for symbols.
     */
        if (nativeLineArray != null) {
            nprim = nativeLineArray.size ();
            for (i=0; i<nprim; i++) {
                native_line = nativeLineArray.get(i);
                if (native_line.image >= 0) {
                    continue;
                }
                if (native_line == null) {
                    continue;
                }
                if (native_line.npts < 2) {
                    continue;
                }
                small_flag = native_line.pattern / 10000;
                if (small_flag != 1) {
                    continue;
                }
                pattern = native_line.pattern % 10000;
                g2d = g2d_bg;
                if (native_line.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for stroking.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_line.npts);
                gpath.moveTo (
                    native_line.xy[0],
                    native_line.xy[1]
                );
                for (j=1; j<native_line.npts; j++) {
                    gpath.lineTo (
                        native_line.xy[2*j],
                        native_line.xy[2*j+1]
                    );
                }

              /*
               * Set the color for line drawing.
               */
                if (native_line.red >= 0  &&
                    native_line.green >= 0  &&
                    native_line.blue >= 0) {
                    fill_color =
                    new Color (native_line.red,
                               native_line.green,
                               native_line.blue,
                               native_line.alpha
                              );
                    g2d.setColor (fill_color);
                }
                else {
                    continue;
                }

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_line.frame_num);

              /*
               * draw the line
               */
                lookupAndSetLinePattern (g2d, pattern, native_line.thickness);
                g2d.draw (gpath);
            }

        }  /* end of small line section */

    /*
     * Draw all the fills that have their small flag set
     * to two to the appropriate buffers.  These are polygons
     * backgrounding text.
     */
        if (nativeFillArray != null) {
            nprim = nativeFillArray.size ();
            for (i=0; i<nprim; i++) {
                native_fill = nativeFillArray.get(i);
                if (native_fill.image >= 0) {
                    continue;
                }
                if (native_fill == null) {
                    continue;
                }
                if (native_fill.npts < 3) {
                    continue;
                }
                small_flag = native_fill.pattern / 10000;
                if (small_flag != 2) {
                    continue;
                }
                pattern = native_fill.pattern % 10000;
                g2d = g2d_bg;
                if (native_fill.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for filling.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_fill.npts + 2);
                gpath.moveTo (
                    native_fill.xy[0],
                    native_fill.xy[1]
                );
                for (j=1; j<native_fill.npts; j++) {
                    gpath.lineTo (
                        native_fill.xy[2*j],
                        native_fill.xy[2*j+1]
                    );
                }
                gpath.closePath ();

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_fill.frame_num);

              /*
               * draw solid color first.
               */
                if (native_fill.red >= 0  &&
                    native_fill.green >= 0  &&
                    native_fill.blue >= 0) {
                    fill_color =
                    new Color (native_fill.red,
                               native_fill.green,
                               native_fill.blue,
                               native_fill.alpha
                              );
                    g2d.setColor (fill_color);
                    g2d.fill (gpath);
                }

              /*
               * draw the fill pattern if needed.
               */
                if (pattern > 0) {
                    lookupAndSetFillPattern (g2d, pattern);
                    g2d.fill (gpath);
                }
            }

        }  /* end of text background polygon section */

    /*
     * Draw all the lines that have their small flag set
     * to two to the appropriate buffers.  These are lines for
     * drawing text background rectangles.
     */
        if (nativeLineArray != null) {
            nprim = nativeLineArray.size ();
            for (i=0; i<nprim; i++) {
                native_line = nativeLineArray.get(i);
                if (native_line.image >= 0) {
                    continue;
                }
                if (native_line == null) {
                    continue;
                }
                if (native_line.npts < 2) {
                    continue;
                }
                small_flag = native_line.pattern / 10000;
                if (small_flag != 2) {
                    continue;
                }
                pattern = native_line.pattern % 10000;
                g2d = g2d_bg;
                if (native_line.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for stroking.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_line.npts);
                gpath.moveTo (
                    native_line.xy[0],
                    native_line.xy[1]
                );
                for (j=1; j<native_line.npts; j++) {
                    gpath.lineTo (
                        native_line.xy[2*j],
                        native_line.xy[2*j+1]
                    );
                }

              /*
               * Set the color for line drawing.
               */
                if (native_line.red >= 0  &&
                    native_line.green >= 0  &&
                    native_line.blue >= 0) {
                    fill_color =
                    new Color (native_line.red,
                               native_line.green,
                               native_line.blue,
                               native_line.alpha
                              );
                    g2d.setColor (fill_color);
                }
                else {
                    continue;
                }

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_line.frame_num);

              /*
               * draw the line
               */
                lookupAndSetLinePattern (g2d, pattern, native_line.thickness);
                g2d.draw (gpath);
            }

        }  /* end of text background line section */

    /*
     * Draw all the fills that have their small flag set
     * to three to the appropriate buffers.  These are polygons
     * used in the text characters.
     */
        if (nativeFillArray != null) {
            nprim = nativeFillArray.size ();
            for (i=0; i<nprim; i++) {
                native_fill = nativeFillArray.get(i);
                if (native_fill.image >= 0) {
                    continue;
                }
                if (native_fill == null) {
                    continue;
                }
                if (native_fill.npts < 3) {
                    continue;
                }
                small_flag = native_fill.pattern / 10000;
                if (small_flag != 3) {
                    continue;
                }
                pattern = native_fill.pattern % 10000;
                g2d = g2d_bg;
                if (native_fill.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for filling.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_fill.npts + 2);
                gpath.moveTo (
                    native_fill.xy[0],
                    native_fill.xy[1]
                );
                for (j=1; j<native_fill.npts; j++) {
                    gpath.lineTo (
                        native_fill.xy[2*j],
                        native_fill.xy[2*j+1]
                    );
                }
                gpath.closePath ();

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_fill.frame_num);

              /*
               * draw solid color first.
               */
                if (native_fill.red >= 0  &&
                    native_fill.green >= 0  &&
                    native_fill.blue >= 0) {
                    fill_color =
                    new Color (native_fill.red,
                               native_fill.green,
                               native_fill.blue,
                               native_fill.alpha
                              );
                    g2d.setColor (fill_color);
                    g2d.fill (gpath);
                }

              /*
               * draw the fill pattern if needed.
               */
                if (pattern > 0) {
                    lookupAndSetFillPattern (g2d, pattern);
                    g2d.fill (gpath);
                }
            }

        }  /* end of text foreground polygon section */

    /*
     * Text character vectors are the only things that might use
     * anti aliasing in drawing.
     */
        anti_alias_ok = 1;

    /*
     * Draw all the lines that have their small flag set
     * to three to the appropriate buffers.  These are lines for
     * drawing text characters
     */
        if (nativeLineArray != null) {
            nprim = nativeLineArray.size ();
            for (i=0; i<nprim; i++) {
                native_line = nativeLineArray.get(i);
                if (native_line.image >= 0) {
                    continue;
                }
                if (native_line == null) {
                    continue;
                }
                if (native_line.npts < 2) {
                    continue;
                }
                small_flag = native_line.pattern / 10000;
                if (small_flag != 3) {
                    continue;
                }
                pattern = native_line.pattern % 10000;
                g2d = g2d_bg;
                if (native_line.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for stroking.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_line.npts);
                gpath.moveTo (
                    native_line.xy[0],
                    native_line.xy[1]
                );
                for (j=1; j<native_line.npts; j++) {
                    gpath.lineTo (
                        native_line.xy[2*j],
                        native_line.xy[2*j+1]
                    );
                }

              /*
               * Set the color for line drawing.
               */
                if (native_line.red >= 0  &&
                    native_line.green >= 0  &&
                    native_line.blue >= 0) {
                    fill_color =
                    new Color (native_line.red,
                               native_line.green,
                               native_line.blue,
                               native_line.alpha
                              );
                    g2d.setColor (fill_color);
                }
                else {
                    continue;
                }

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_line.frame_num);

              /*
               * draw the line
               */
                lookupAndSetLinePattern (g2d, pattern, native_line.thickness);
                g2d.draw (gpath);

            }

        }  /* end of text character line section */

    /*
     * Draw the actual java font based text on top of the stroked text.
     * They will also be on top of any text background rectangles, which
     * have already been drawn.
     */
        if (nativeTextArray != null) {

            nprim = nativeTextArray.size ();

            Font  font = null;
            double xtext, ytext, descent, rang;
            AffineTransform tsave;
            double[] metrics = new double[3];

            for (i=0; i<nprim; i++) {

                native_text = nativeTextArray.get(i);
                if (native_text.image >= 0) {
                    continue;
                }
                if (native_text == null) {
                    continue;
                }

                if (native_text.text == null) {
                    continue;
                }

                if (native_text.alpha <= 0) {
                    continue;
                }

                g2d = g2d_bg;
                if (native_text.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Set the color for text drawing.
               */
                if (native_text.red >= 0  &&
                    native_text.green >= 0  &&
                    native_text.blue >= 0) {
                    fill_color =
                    new Color (native_text.red,
                               native_text.green,
                               native_text.blue,
                               native_text.alpha
                              );
                    g2d.setColor (fill_color);
                }
                else {
                    continue;
                }

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_text.frame_num);

              /*
               * Get a font from the base font and size.
               */
                font = FontUtils.deriveFont (
                    native_text.baseFont,
                    native_text.size);
                g2d.setFont (font);

              /*
               * The position of the native text is the lower left corner
               * of the bounding box.  To draw with the java draw methods,
               * this needs to be converted to the java baseline by offsetting
               * based on the descent of the font.   Rotation of the descent
               * distance is also needed.
               */
                metrics = FontUtils.getTextMetrics (
                    native_text.text,
                    font);
                descent = 0.0;
                xtext = native_text.x;
                ytext = native_text.y;

              /*
               * If the rotation angle is not very near zero,
               * save the current transform.  Then modify the
               * current transform to rotate the text.  Then
               * reinstate the saved transform.
               */
                if (native_text.angle > 0.1  ||
                    native_text.angle < -0.1) {
                    rang = native_text.angle * DTORAD;
                    xtext -= descent * Math.sin (rang);
                    ytext += descent * Math.cos (rang);
                    tsave = g2d.getTransform ();
                    g2d.rotate (
                        native_text.angle * DTORAD,
                        xtext,
                        ytext);
                    g2d.drawString (native_text.text,
                                    (int)(xtext + .5),
                                    (int)(ytext + .5));
                    g2d.setTransform (tsave);
                }

              /*
               * No rotation needed.
               */
                else {
                    ytext += descent;
                    g2d.drawString (native_text.text,
                                    (int)(xtext + .5),
                                    (int)(ytext + .5));
                }

            }

        }  /* end of text character line section */

    /*
     * Draw the frame borders for clipping frames after all else is drawn.
     * Frame borders are always drawn to the background buffer.
     */
        if (nativeFrameArray != null) {
            g2d_bg.setClip (null);
            nprim = nativeFrameArray.size();
            for (i=0; i<nprim; i++) {
                native_frame = nativeFrameArray.get(i);
                if (native_frame.borderflag != 0  &&
                    native_frame.clip_shape != null  &&
                    native_frame.clipflag == 1) {
                    lookupAndSetLinePattern (g2d_bg, 0, 1.0f);
                    g2d_bg.setColor (Color.black);
                    g2d_bg.draw (native_frame.clip_shape);
                }
            }
        }
        lookupAndSetLinePattern (g2d_bg, 0, 0.0f);

      /*
       * reset arrays and properties to "undefined" values
       * just in case this object is used again.
       */
        nativeFillArray = null;
        nativeLineArray = null;
        nativeTextArray = null;
        nativeArcArray = null;
        nativeFilledArcArray = null;
        nativeImageArray = null;

        screen_width = -1;
        screen_height = -1;

        background_red = -1;
        background_green = -1;
        background_blue = -1;
        background_alpha = -1;

        return fgflag;

    }

/*---------------------------------------------------------------------*/

    BufferedImage   patImage;

  /*
   * Lith patterns and other fill patterns need to be set here.
   */
    private void lookupAndSetFillPattern (Graphics2D g2d,
                                          int patnum,
                                          double patScale,
                                          Color patColor)
    {
        int           r, g, b, a, rgb;
        int           r1, g1, b1, a1, rgb1;
        int           nr, nc, i, j;
        BufferedImage localImage;

        if (patnum < 1  ||  patnum > 8) {
            g2d.setPaint (null);
            return;
        }

        BufferedImage bi = LithFill.lithologyImages[patnum-1];

        if (patColor != null) {

            r1 = patColor.getRed();
            g1 = patColor.getGreen();
            b1 = patColor.getBlue();
            a1 = patColor.getAlpha();

            if (r1 == 0  &&  g1 == 0  &&  b1 == 0  &&  a1 == 255) {
                localImage = bi;
            }

            else {

                nr = bi.getWidth ();
                nc = bi.getHeight ();
                patImage = new BufferedImage (nc, nr, BufferedImage.TYPE_INT_ARGB);
                for (i=0; i<nr; i++) {
                    for (j=0; j<nc; j++) {
                        rgb1 = bi.getRGB (j, i);
                        a = rgb1 >> 24;
                        if (a != 0) {
                            a = a1 << 24;
                            r = (r1 << 16) & 0x00ff0000;
                            g = (g1 << 8) & 0x0000ff00;
                            b = b1 & 0x000000ff;
                            rgb = a | r | g | b;
                            patImage.setRGB (j, i, rgb);
                        }
                        else {
                            patImage.setRGB (j, i, 0);
                        }
                    }
                }
                localImage = patImage;
            }
        }
        else {
            localImage = bi;
        }

        Rectangle2D tr = new Rectangle2D.Double
        (
            0,
            0,
            localImage.getWidth() * patScale,
            localImage.getHeight() * patScale
        );

        TexturePaint tp = new TexturePaint (localImage, tr);

        g2d.setPaint (tp);

        return;

    }

/*---------------------------------------------------------------------*/

  /*
   * Lith patterns and other fill patterns need to be set here.
   */
    private void lookupAndSetFillPattern (Graphics2D g2d, int patnum)
    {
        if (patnum < 1  ||  patnum > 8) {
            g2d.setPaint (null);
            return;
        }

        BufferedImage bi = LithFill.lithologyImages[patnum-1];

        Rectangle2D tr = new Rectangle2D.Double
        (
            0,
            0,
            bi.getWidth(),
            bi.getHeight()
        );

        TexturePaint tp = new TexturePaint (bi, tr);

        g2d.setPaint (tp);

        return;

    }

/*---------------------------------------------------------------------*/

  /*
   * Line dash patterns and line thickness need to be set here.
   */
    private void lookupAndSetLinePattern (
        Graphics2D g2d,
        int        pattern,
        float      thickness
    )
    {
        int        patnum, pscale, i;
        float      fscale;
        BasicStroke   basic_stroke;

        patnum = pattern % 100;

    /*
     * solid line.
     */
        if (patnum <= 0  ||  patnum > 30) {
            basic_stroke =
            new BasicStroke (thickness);
        }

    /*
     * dashed line
     */
        else {
            patnum--;
            int[] idash = dashTable[patnum];
            pscale = pattern / 100;
            fscale = (float)pscale / 10.0f;
            if (fscale < 0.1  ||  fscale > 10.0) fscale = 1.0f;
            fscale *= 1.5f;
            if (patnum < 13) {
                for (i=0; i<2; i++) {
                    dashLine2[i] = fscale * (float)idash[i];
                }
                basic_stroke =
                new BasicStroke (
                                 thickness,
                                 BasicStroke.CAP_BUTT,
                                 BasicStroke.JOIN_BEVEL,
                                 0.0f, // miter limit
                                 dashLine2,
                                 0.0f  // dash phase
                                );
            }
            else {
                for (i=0; i<4; i++) {
                    dashLine4[i] = fscale * (float)idash[i];
                }
                basic_stroke =
                new BasicStroke (
                                 thickness,
                                 BasicStroke.CAP_BUTT,
                                 BasicStroke.JOIN_BEVEL,
                                 0.0f, // miter limit
                                 dashLine4,
                                 0.0f  // dash phase
                                );
            }
        }

        g2d.setStroke (basic_stroke);

        return;

    }

/*---------------------------------------------------------------------*/

  /*
   * Set the clip area if needed for the frame.
   */
    private void lookupAndSetClip (
        Graphics2D g2d,
        int        frame_num
    )
    {
        int                i;
        NativePrim.Frame   f;

        g2d.setClip (null);
        if (nativeFrameArray == null) {
            return;
        }

        if (frame_num < 0) {
            return;
        }

        int n = nativeFrameArray.size ();

        for (i=0; i<n; i++) {

            f = nativeFrameArray.get (i);
            if (f == null) continue;
            if (f.frame_num != frame_num) continue;

            if (f.clipflag == 0  ||  f.clip_shape == null) {
                continue;
            }

            g2d.setClip (f.clip_shape);

        }

        return;

    }

    private void printClip (Graphics2D g2d)
    {
        System.out.println ("clip rectangle");
        System.out.println (g2d.getClipBounds());
    }



/*---------------------------------------------------------------------*/

  /*
   * This method draws any vector images that have their image
   * member agree with the specified image_id.
   */
    private void DrawVectorsOverImage (
        int image_id,
        Graphics2D g2d_fg,
        Graphics2D g2d_bg)
    {
        int             nprim, i, ntot, j;
        int             a, r, g, b;
        int             closure, pattern, small_flag;
        Graphics2D      g2d;
        double          patScale;

        NativePrim.Arc       native_arc;
        NativePrim.FilledArc native_filled_arc;
        NativePrim.Text      native_text;
        NativePrim.Line      native_line;
        NativePrim.Fill      native_fill;

        Color           fill_color;
        Color           arc_color;

        int             fgflag = 0;

    /*
     * Draw all the lines that have their small flag set
     * to zero to the appropriate buffers.  These are lines
     * not used in symbols or text.  This is done is 2 passes.
     * The solid lines are drawn first, followed by dashed lines.
     * This is done so dashed lines directly on top of solid lines
     * will be visible.
     */
        if (nativeLineArray != null) {
            nprim = nativeLineArray.size ();

            for (int ido=0; ido<2; ido++) {

                for (i=0; i<nprim; i++) {

                    native_line = nativeLineArray.get(i);
                    if (native_line.image != image_id) {
                        continue;
                    }
                    if (native_line == null) {
                        continue;
                    }
                    if (native_line.npts < 2) {
                        continue;
                    }
                    small_flag = native_line.pattern / 10000;
                    if (small_flag != 0) {
                        continue;
                    }
                    pattern = native_line.pattern % 10000;

                    if (ido == 0) {
                        if (pattern % 1000 != 0) continue;
                    }
                    else {
                        if (pattern % 1000 == 0) continue;
                    }

                    g2d = g2d_bg;
                    if (native_line.selectable == 1) {
                        fgflag = 1;
                        g2d = g2d_fg;
                    }

                  /*
                   * Create a general path for stroking.
                   */
                    GeneralPath gpath =
                    new GeneralPath (
                        GeneralPath.WIND_EVEN_ODD,
                        native_line.npts);
                    gpath.moveTo (
                        native_line.xy[0],
                        native_line.xy[1]
                    );
                    for (j=1; j<native_line.npts; j++) {
                        gpath.lineTo (
                            native_line.xy[2*j],
                            native_line.xy[2*j+1]
                        );
                    }

                  /*
                   * Set the color for line drawing.
                   */
                    if (native_line.red >= 0  &&
                        native_line.green >= 0  &&
                        native_line.blue >= 0) {
                        fill_color =
                        new Color (native_line.red,
                                   native_line.green,
                                   native_line.blue,
                                   native_line.alpha
                                  );
                        g2d.setColor (fill_color);
                    }
                    else {
                        continue;
                    }

                  /*
                   * set clipping if needed.
                   */
                    lookupAndSetClip (g2d,
                                  native_line.frame_num);

                  /*
                   * draw the line
                   */
                    lookupAndSetLinePattern (g2d, pattern, native_line.thickness);
                    g2d.draw (gpath);
                }
            }

        }  /* end of large line section */

    /*
     * Draw all the fills that have their small flag set
     * to one to the appropriate buffers.  These are fills
     * used for symbols.
     */
        if (nativeFillArray != null) {
            nprim = nativeFillArray.size ();
            for (i=0; i<nprim; i++) {
                native_fill = nativeFillArray.get(i);
                if (native_fill.image != image_id) {
                    continue;
                }
                if (native_fill == null) {
                    continue;
                }
                if (native_fill.npts < 3) {
                    continue;
                }
                small_flag = native_fill.pattern / 10000;
                if (small_flag != 1) {
                    continue;
                }
                pattern = native_fill.pattern % 10000;
                g2d = g2d_bg;
                if (native_fill.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for filling.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_fill.npts + 2);
                gpath.moveTo (
                    native_fill.xy[0],
                    native_fill.xy[1]
                );
                for (j=1; j<native_fill.npts; j++) {
                    gpath.lineTo (
                        native_fill.xy[2*j],
                        native_fill.xy[2*j+1]
                    );
                }
                gpath.closePath ();

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_fill.frame_num);

              /*
               * draw solid color first.
               */
                if (native_fill.red >= 0  &&
                    native_fill.green >= 0  &&
                    native_fill.blue >= 0) {
                    fill_color =
                    new Color (native_fill.red,
                               native_fill.green,
                               native_fill.blue,
                               native_fill.alpha
                              );
                    g2d.setColor (fill_color);
                    g2d.fill (gpath);
                }

              /*
               * draw the fill pattern if needed.
               */
                if (pattern > 0) {
                    lookupAndSetFillPattern (g2d, pattern);
                    g2d.fill (gpath);
                }
            }
        }  /* end of small filled polygon section */

    /*
     * Draw all the arc fills that have their small flag set to one.
     * These are arc fills used for symbols.
     */
        if (nativeFilledArcArray != null) {
            nprim = nativeFilledArcArray.size ();
            for (i=0; i<nprim; i++) {
                native_filled_arc = nativeFilledArcArray.get(i);
                if (native_filled_arc.image != image_id) {
                    continue;
                }
                if (native_filled_arc == null) {
                    continue;
                }
                small_flag = native_filled_arc.closure / 10000;
                if (small_flag != 1) {
                    continue;
                }
                closure = native_filled_arc.closure % 10000;
                g2d = g2d_bg;
                if (native_filled_arc.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create an Arc2D object.
               */
                int type = Arc2D.OPEN;
                if (closure == 1) {
                    type = Arc2D.PIE;
                }
                else if (closure == 2) {
                    type = Arc2D.CHORD;
                }
                Arc2D.Float arc =
                new Arc2D.Float (
                    native_filled_arc.x - native_filled_arc.r1,
                    native_filled_arc.y - native_filled_arc.r2,
                    native_filled_arc.r1 * 2.0f,
                    native_filled_arc.r2 * 2.0f,
                    native_filled_arc.ang1,
                    native_filled_arc.ang2,
                    type);

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_filled_arc.frame_num);

              /*
               * Draw in solid color first.
               */
                if (native_filled_arc.red >= 0  &&
                    native_filled_arc.green >= 0  &&
                    native_filled_arc.blue >= 0) {

                 /*
                  * Set the fill color for the arc.
                  */
                    fill_color =
                    new Color (native_filled_arc.red,
                               native_filled_arc.green,
                               native_filled_arc.blue,
                               native_filled_arc.alpha
                              );
                    g2d.setColor (fill_color);

                 /*
                  * If the rotation angle is not very near zero,
                  * save the current transform.  Then modify the
                  * current transform to rotate the arc.  Then
                  * reinstate the saved transform.
                  */
                    if (native_filled_arc.angle > 0.1  ||
                        native_filled_arc.angle < -0.1) {
                        AffineTransform tsave = g2d.getTransform ();
                        g2d.rotate (
                            native_filled_arc.angle * DTORAD,
                            (native_filled_arc.x),
                            (native_filled_arc.y)
                        );
                        g2d.fill (arc);
                        g2d.setTransform (tsave);
                    }

                 /*
                  * No rotation needed.
                  */
                    else {
                        g2d.fill (arc);
                    }

                }

              /*
               * Draw in pattern fill.
               */
                if (native_filled_arc.pattern > 0) {
                    lookupAndSetFillPattern (g2d, native_filled_arc.pattern);
                    if (native_filled_arc.angle > 0.1  ||
                        native_filled_arc.angle < -0.1) {
                        AffineTransform tsave = g2d.getTransform ();
                        g2d.rotate (
                            native_filled_arc.angle * DTORAD,
                            (native_filled_arc.x),
                            (native_filled_arc.y)
                        );
                        g2d.fill (arc);
                        g2d.setTransform (tsave);
                    }

                 /*
                  * No rotation needed.
                  */
                    else {
                        g2d.fill (arc);
                    }
                }


            }

        }  /* end of small filled arc section */

    /*
     * Draw all the arcs that have their small flag set to one.
     * These are arcs used for symbols.
     */
        if (nativeArcArray != null) {
            nprim = nativeArcArray.size ();
            for (i=0; i<nprim; i++) {
                native_arc = nativeArcArray.get(i);
                if (native_arc.image != image_id) {
                    continue;
                }
                if (native_arc == null) {
                    continue;
                }
                small_flag = native_arc.closure / 10000;
                if (small_flag != 1) {
                    continue;
                }
                closure = native_arc.closure % 10000;
                g2d = g2d_bg;
                if (native_arc.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Draw the arc if it has valid colors.
               */
                if (native_arc.red >= 0  &&
                    native_arc.green >= 0  &&
                    native_arc.blue >= 0) {

                  /*
                   * Create an Arc2D object.
                   */
                    int type = Arc2D.OPEN;
                    if (closure == 1) {
                        type = Arc2D.PIE;
                    }
                    else if (closure == 2) {
                        type = Arc2D.CHORD;
                    }
                    Arc2D.Float arc =
                    new Arc2D.Float (
                        native_arc.x - native_arc.r1,
                        native_arc.y - native_arc.r2,
                        native_arc.r1 * 2.0f,
                        native_arc.r2 * 2.0f,
                        native_arc.ang1,
                        native_arc.ang2,
                        type);

                 /*
                  * Set the color for the arc.
                  */
                    arc_color =
                    new Color (native_arc.red,
                               native_arc.green,
                               native_arc.blue,
                               native_arc.alpha
                              );
                    g2d.setColor (arc_color);
                    lookupAndSetLinePattern (g2d, 0, native_arc.thickness);

                 /*
                  * set clipping if needed.
                  */
                    lookupAndSetClip (g2d,
                                      native_arc.frame_num);

                 /*
                  * If the rotation angle is not very near zero,
                  * save the current transform.  Then modify the
                  * current transform to rotate the arc.  Then
                  * reinstate the saved transform.
                  */
                    if (native_arc.angle > 0.1  ||
                        native_arc.angle < -0.1) {
                        AffineTransform tsave = g2d.getTransform ();
                        g2d.rotate (
                            native_arc.angle * DTORAD,
                            (native_arc.x),
                            (native_arc.y)
                        );
                        g2d.draw (arc);
                        g2d.setTransform (tsave);
                    }

                 /*
                  * No rotation needed.
                  */
                    else {
                        g2d.draw (arc);
                    }

                }

            }

        }  /* end of small arc section */

    /*
     * Draw all the lines that have their small flag set
     * to one to the appropriate buffers.  These are lines
     * used for symbols.
     */
        if (nativeLineArray != null) {
            nprim = nativeLineArray.size ();
            for (i=0; i<nprim; i++) {
                native_line = nativeLineArray.get(i);
                if (native_line.image != image_id) {
                    continue;
                }
                if (native_line == null) {
                    continue;
                }
                if (native_line.npts < 2) {
                    continue;
                }
                small_flag = native_line.pattern / 10000;
                if (small_flag != 1) {
                    continue;
                }
                pattern = native_line.pattern % 10000;
                g2d = g2d_bg;
                if (native_line.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for stroking.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_line.npts);
                gpath.moveTo (
                    native_line.xy[0],
                    native_line.xy[1]
                );
                for (j=1; j<native_line.npts; j++) {
                    gpath.lineTo (
                        native_line.xy[2*j],
                        native_line.xy[2*j+1]
                    );
                }

              /*
               * Set the color for line drawing.
               */
                if (native_line.red >= 0  &&
                    native_line.green >= 0  &&
                    native_line.blue >= 0) {
                    fill_color =
                    new Color (native_line.red,
                               native_line.green,
                               native_line.blue,
                               native_line.alpha
                              );
                    g2d.setColor (fill_color);
                }
                else {
                    continue;
                }

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_line.frame_num);

              /*
               * draw the line
               */
                lookupAndSetLinePattern (g2d, pattern, native_line.thickness);
                g2d.draw (gpath);
            }

        }  /* end of small line section */

    /*
     * Draw all the fills that have their small flag set
     * to two to the appropriate buffers.  These are polygons
     * backgrounding text.
     */
        if (nativeFillArray != null) {
            nprim = nativeFillArray.size ();
            for (i=0; i<nprim; i++) {
                native_fill = nativeFillArray.get(i);
                if (native_fill.image != image_id) {
                    continue;
                }
                if (native_fill == null) {
                    continue;
                }
                if (native_fill.npts < 3) {
                    continue;
                }
                small_flag = native_fill.pattern / 10000;
                if (small_flag != 2) {
                    continue;
                }
                pattern = native_fill.pattern % 10000;
                g2d = g2d_bg;
                if (native_fill.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for filling.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_fill.npts + 2);
                gpath.moveTo (
                    native_fill.xy[0],
                    native_fill.xy[1]
                );
                for (j=1; j<native_fill.npts; j++) {
                    gpath.lineTo (
                        native_fill.xy[2*j],
                        native_fill.xy[2*j+1]
                    );
                }
                gpath.closePath ();

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_fill.frame_num);

              /*
               * draw solid color first.
               */
                if (native_fill.red >= 0  &&
                    native_fill.green >= 0  &&
                    native_fill.blue >= 0) {
                    fill_color =
                    new Color (native_fill.red,
                               native_fill.green,
                               native_fill.blue,
                               native_fill.alpha
                              );
                    g2d.setColor (fill_color);
                    g2d.fill (gpath);
                }

              /*
               * draw the fill pattern if needed.
               */
                if (pattern > 0) {
                    lookupAndSetFillPattern (g2d, pattern);
                    g2d.fill (gpath);
                }
            }

        }  /* end of text background polygon section */

    /*
     * Draw all the lines that have their small flag set
     * to two to the appropriate buffers.  These are lines for
     * drawing text background rectangles.
     */
        if (nativeLineArray != null) {
            nprim = nativeLineArray.size ();
            for (i=0; i<nprim; i++) {
                native_line = nativeLineArray.get(i);
                if (native_line.image != image_id) {
                    continue;
                }
                if (native_line == null) {
                    continue;
                }
                if (native_line.npts < 2) {
                    continue;
                }
                small_flag = native_line.pattern / 10000;
                if (small_flag != 2) {
                    continue;
                }
                pattern = native_line.pattern % 10000;
                g2d = g2d_bg;
                if (native_line.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for stroking.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_line.npts);
                gpath.moveTo (
                    native_line.xy[0],
                    native_line.xy[1]
                );
                for (j=1; j<native_line.npts; j++) {
                    gpath.lineTo (
                        native_line.xy[2*j],
                        native_line.xy[2*j+1]
                    );
                }

              /*
               * Set the color for line drawing.
               */
                if (native_line.red >= 0  &&
                    native_line.green >= 0  &&
                    native_line.blue >= 0) {
                    fill_color =
                    new Color (native_line.red,
                               native_line.green,
                               native_line.blue,
                               native_line.alpha
                              );
                    g2d.setColor (fill_color);
                }
                else {
                    continue;
                }

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_line.frame_num);

              /*
               * draw the line
               */
                lookupAndSetLinePattern (g2d, pattern, native_line.thickness);
                g2d.draw (gpath);
            }

        }  /* end of text background line section */

    /*
     * Draw all the fills that have their small flag set
     * to three to the appropriate buffers.  These are polygons
     * used in the text characters.
     */
        if (nativeFillArray != null) {
            nprim = nativeFillArray.size ();
            for (i=0; i<nprim; i++) {
                native_fill = nativeFillArray.get(i);
                if (native_fill.image != image_id) {
                    continue;
                }
                if (native_fill == null) {
                    continue;
                }
                if (native_fill.npts < 3) {
                    continue;
                }
                small_flag = native_fill.pattern / 10000;
                if (small_flag != 3) {
                    continue;
                }
                pattern = native_fill.pattern % 10000;
                g2d = g2d_bg;
                if (native_fill.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for filling.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_fill.npts + 2);
                gpath.moveTo (
                    native_fill.xy[0],
                    native_fill.xy[1]
                );
                for (j=1; j<native_fill.npts; j++) {
                    gpath.lineTo (
                        native_fill.xy[2*j],
                        native_fill.xy[2*j+1]
                    );
                }
                gpath.closePath ();

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_fill.frame_num);

              /*
               * draw solid color first.
               */
                if (native_fill.red >= 0  &&
                    native_fill.green >= 0  &&
                    native_fill.blue >= 0) {
                    fill_color =
                    new Color (native_fill.red,
                               native_fill.green,
                               native_fill.blue,
                               native_fill.alpha
                              );
                    g2d.setColor (fill_color);
                    g2d.fill (gpath);
                }

              /*
               * draw the fill pattern if needed.
               */
                if (pattern > 0) {
                    lookupAndSetFillPattern (g2d, pattern);
                    g2d.fill (gpath);
                }
            }

        }  /* end of text foreground polygon section */

    /*
     * Text character vectors are the only things that might use
     * anti aliasing in drawing.
     */
        anti_alias_ok = 1;

    /*
     * Draw all the lines that have their small flag set
     * to three to the appropriate buffers.  These are lines for
     * drawing text characters
     */
        if (nativeLineArray != null) {
            nprim = nativeLineArray.size ();
            for (i=0; i<nprim; i++) {
                native_line = nativeLineArray.get(i);
                if (native_line.image != image_id) {
                    continue;
                }
                if (native_line == null) {
                    continue;
                }
                if (native_line.npts < 2) {
                    continue;
                }
                small_flag = native_line.pattern / 10000;
                if (small_flag != 3) {
                    continue;
                }
                pattern = native_line.pattern % 10000;
                g2d = g2d_bg;
                if (native_line.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Create a general path for stroking.
               */
                GeneralPath gpath =
                new GeneralPath (
                    GeneralPath.WIND_EVEN_ODD,
                    native_line.npts);
                gpath.moveTo (
                    native_line.xy[0],
                    native_line.xy[1]
                );
                for (j=1; j<native_line.npts; j++) {
                    gpath.lineTo (
                        native_line.xy[2*j],
                        native_line.xy[2*j+1]
                    );
                }

              /*
               * Set the color for line drawing.
               */
                if (native_line.red >= 0  &&
                    native_line.green >= 0  &&
                    native_line.blue >= 0) {
                    fill_color =
                    new Color (native_line.red,
                               native_line.green,
                               native_line.blue,
                               native_line.alpha
                              );
                    g2d.setColor (fill_color);
                }
                else {
                    continue;
                }

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_line.frame_num);

              /*
               * draw the line
               */
                lookupAndSetLinePattern (g2d, pattern, native_line.thickness);
                g2d.draw (gpath);

            }

        }  /* end of text character line section */

    /*
     * Draw the actual java font based text on top of the stroked text.
     * They will also be on top of any text background rectangles, which
     * have already been drawn.
     */
        if (nativeTextArray != null) {

            nprim = nativeTextArray.size ();

            Font  font = null;
            double xtext, ytext, descent, rang;
            AffineTransform tsave;
            double[] metrics = new double[3];

            for (i=0; i<nprim; i++) {

                native_text = nativeTextArray.get(i);
                if (native_text.image != image_id) {
                    continue;
                }
                if (native_text == null) {
                    continue;
                }

                if (native_text.text == null) {
                    continue;
                }

                if (native_text.alpha <= 0) {
                    continue;
                }

                g2d = g2d_bg;
                if (native_text.selectable == 1) {
                    fgflag = 1;
                    g2d = g2d_fg;
                }

              /*
               * Set the color for text drawing.
               */
                if (native_text.red >= 0  &&
                    native_text.green >= 0  &&
                    native_text.blue >= 0) {
                    fill_color =
                    new Color (native_text.red,
                               native_text.green,
                               native_text.blue,
                               native_text.alpha
                              );
                    g2d.setColor (fill_color);
                }
                else {
                    continue;
                }

              /*
               * set clipping if needed.
               */
                lookupAndSetClip (g2d,
                                  native_text.frame_num);

              /*
               * Get a font from the base font and size.
               */
                font = FontUtils.deriveFont (
                    native_text.baseFont,
                    native_text.size);
                g2d.setFont (font);

              /*
               * The position of the native text is the lower left corner
               * of the bounding box.  To draw with the java draw methods,
               * this needs to be converted to the java baseline by offsetting
               * based on the descent of the font.   Rotation of the descent
               * distance is also needed.
               */
                metrics = FontUtils.getTextMetrics (
                    native_text.text,
                    font);
                descent = 0.0;
                xtext = native_text.x;
                ytext = native_text.y;

              /*
               * If the rotation angle is not very near zero,
               * save the current transform.  Then modify the
               * current transform to rotate the text.  Then
               * reinstate the saved transform.
               */
                if (native_text.angle > 0.1  ||
                    native_text.angle < -0.1) {
                    rang = native_text.angle * DTORAD;
                    xtext -= descent * Math.sin (rang);
                    ytext += descent * Math.cos (rang);
                    tsave = g2d.getTransform ();
                    g2d.rotate (
                        native_text.angle * DTORAD,
                        xtext,
                        ytext);
                    g2d.drawString (native_text.text,
                                    (int)(xtext + .5),
                                    (int)(ytext + .5));
                    g2d.setTransform (tsave);
                }

              /*
               * No rotation needed.
               */
                else {
                    ytext += descent;
                    g2d.drawString (native_text.text,
                                    (int)(xtext + .5),
                                    (int)(ytext + .5));
                }

            }

        }  /* end of text character line section */

    }  // end of DrawVectorsOverImage method


/*---------------------------------------------------------------------*/

    private void SortImagesByID ()
    {
        int       i, j, nprim, id, minid;
        NativePrim.Image native_image;
        int[]     done;

        nprim = nativeImageArray.size();
        ArrayList<NativePrim.Image>  sortedList = 
			new ArrayList<NativePrim.Image> (nprim);
        done = new int[nprim];

    /*
     * Put in the images without an id first.
     */
        for (i=0; i<nprim; i++) {
            native_image = nativeImageArray.get(i);
            if (native_image == null) {
                done[i] = 1;
                continue;
            }
            id = native_image.id;
            if (id == -1) {
                sortedList.add (native_image);
                done[i] = 1;
            }
        }

        int minindex;

        for (i=0; i<nprim; i++) {
            minid = 10000000;
            minindex = -1;
            for (j=0; j<nprim; j++) {
                if (done[j] == 1) continue;
                native_image = nativeImageArray.get(j);
                if (native_image == null) {
                    done[j] = 1;
                    continue;
                }
                if (native_image.id < minid) {
                    minid = native_image.id;
                    minindex = j;
                }
            }
            if (minindex < 0) {
                break;
            }
            native_image = nativeImageArray.get(minindex);
            done[minindex] = 1;
            sortedList.add (native_image);
        }

        nativeImageArray = sortedList;

        return;

    }




};  // end of LocalDraw class
