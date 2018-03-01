
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

import java.lang.Thread;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.util.ArrayList;

import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;

/**
 *
 *  This is the base class used only as the foundation for
 *  the JDisplayList class.  This foundation has the functions needed
 *  to communicate with the native EasyX graphics and also to draw the
 *  simple primitives returned from the EasyX graphics.
 *
 *  This class should not be instantiated by itself.
 *  The class extends the DLConst class simply so the constants in
 *  that class can be used directly by the entire JDisplayList class
 *  hierarchy.
 *
 *  @author Glenn Pinkerton
 *
*/

class JDisplayListBase extends DLConst {

    JDisplayListBase () {
        createDefaultPopupButtons ();
    }

  /*
   * Define some constants.
   */
    protected static final int SMALL_CHUNK = 20;
    protected static final int MEDIUM_CHUNK = 100;
    protected static final int BIG_CHUNK = 1000;
    protected static final int HUGE_CHUNK = 10000;

    protected static final int MAX_LIST_SIZE = 20;

    protected int nativeDlistID = -1;

    protected int       backgroundRed = 230;
    protected int       backgroundGreen = 230;
    protected int       backgroundBlue = 230;
    protected int       backgroundAlpha = 255;

    protected int       selectRed = 0;
    protected int       selectGreen = 255;
    protected int       selectBlue = 255;
    protected int       selectAlpha = 255;

    protected JDisplayListPanel dlPanel = null;

  /*
   * define some variables to keep track of the state and
   * what is needed for redraw.
   */
    int needsNativeRedraw = 0;
    int needsNativeSelectRedraw = 0;

/*--------------------------------------------------------------------------*/

    void setNativeDrawNeeded (int ival)
    {
        if (ival != 1) ival = 0;

        needsNativeRedraw = ival;

        return;
    }

/*--------------------------------------------------------------------------*/

    void setNativeSelectDrawNeeded (int ival)
    {
        if (ival != 1) ival = 0;

        needsNativeSelectRedraw = ival;

        return;
    }

/*--------------------------------------------------------------------------*/

  /*
   * Pure java wraps around the native Command function.
   */

  /**
   * Send a command to the native display list.  This version is used when only
   * the command id is needed.
   */
    protected int sendNativeCommand (
        int         command_id
    )
    {
        needsNativeRedraw = 1;

        int  threadid = Thread.currentThread().hashCode();

        int status =
        sendCommand (
            nativeDlistID,
            command_id,
            threadid,
            null,
            null,
            null,
            null,
            null,
            null,
            null,
            null,
            0
        );

        return status;
    }


  /**
   * Send a command to the native display list.  This version is used when only
   * the short list of integer values is needed for the command.  For example,
   * a command to set a color would only need 3 integer for red, green and blue
   * so it could use this version of sendNativeCommand.
   */
    protected int sendNativeCommand (
        int         command_id,
        int[]       ilist
    )
    {
        needsNativeRedraw = 1;

        int  threadid = Thread.currentThread().hashCode();

        int status =
        sendCommand (
            nativeDlistID,
            command_id,
            threadid,
            null,
            ilist,
            null,
            null,
            null,
            null,
            null,
            null,
            0
        );

        return status;
    }

    static protected int sendStaticNativeCommand (
        int         nativeid,
        int         command_id,
        int[]       ilist
    )
    {

        int  threadid = Thread.currentThread().hashCode();

        int status =
        sendStaticCommand (
            nativeid,
            command_id,
            threadid,
            null,
            ilist,
            null,
            null,
            null,
            null,
            null,
            null,
            0
        );

        return status;
    }

  /**
   * Send a command to the native display list.  This version is used when only
   * the short list of integer values and a string are needed for the command.
   */
    protected int sendNativeCommand (
        int         command_id,
        int[]       ilist,
        String      cdata
    )
    {
        needsNativeRedraw = 1;

        int  threadid = Thread.currentThread().hashCode();

        int status =
        sendCommand (
            nativeDlistID,
            command_id,
            threadid,
            null,
            ilist,
            cdata,
            null,
            null,
            null,
            null,
            null,
            0
        );

        return status;
    }

  /**
   * Send a command to the native display list.  This version is used when only
   * the short list of integer values and a list of double values
   * are needed for the command.
   */
    protected int sendNativeCommand (
        int         command_id,
        int[]       ilist,
        double[]    ddata
    )
    {
        needsNativeRedraw = 1;

        int  threadid = Thread.currentThread().hashCode();

        int status =
        sendCommand (
            nativeDlistID,
            command_id,
            threadid,
            null,
            ilist,
            null,
            null,
            null,
            null,
            null,
            ddata,
            0
        );

        return status;
    }

  /**
   * Send a command to the native display list.  This version is used when only
   * the short list of integer values, a string and a list of double values
   * are needed for the command.
   */
    protected int sendNativeCommand (
        int         command_id,
        int[]       ilist,
        String      cdata,
        double[]    ddata
    )
    {
        needsNativeRedraw = 1;

        int  threadid = Thread.currentThread().hashCode();

        int status =
        sendCommand (
            nativeDlistID,
            command_id,
            threadid,
            null,
            ilist,
            cdata,
            null,
            null,
            null,
            null,
            ddata,
            0
        );

        return status;
    }

  /**
   * Send a command to the native display list.  This version is used when only
   * the short list of integer values, the short list of long values,
   * a string and a list of double values are needed for the command.
   */
    protected int sendNativeCommand (
        int         command_id,
        int[]       ilist,
        long[]      llist,
        String      cdata,
        double[]    ddata
    )
    {
        needsNativeRedraw = 1;

        int  threadid = Thread.currentThread().hashCode();

        int status =
        sendCommand (
            nativeDlistID,
            command_id,
            threadid,
            llist,
            ilist,
            cdata,
            null,
            null,
            null,
            null,
            ddata,
            0
        );

        return status;
    }

  /**
   * Send a command to the native display list.  This version is used when any
   * of the data types may need to be defined for the command.  If a data type
   * is not used for a particular command. you should set it to null.
   */
    protected int sendNativeCommand (
        int         command_id,
        int[]       ilist,
        String      cdata,
        boolean[]   bdata,
        short[]     sdata,
        int[]       idata,
        float[]     fdata,
        double[]    ddata
    )
    {
        needsNativeRedraw = 1;

        int  threadid = Thread.currentThread().hashCode();

        int status =
        sendCommand (
            nativeDlistID,
            command_id,
            threadid,
            null,
            ilist,
            cdata,
            bdata,
            sdata,
            idata,
            fdata,
            ddata,
            0
        );

        return status;
    }


/*--------------------------------------------------------------------------*/

  /*
   * Send a command to the native easyx graphics system.
   * This is not public.  Use the public functions such
   * as addFill, addSymb, addText, addLine to draw primitives.
   * These functions will format the commands and send them
   * to the native display list.
   */
    protected native int sendCommand (
        int         nativeDlistID,
        int         command_id,
        int         threadid,
        long[]      llist,
        int[]       ilist,
        String      cdata,
        boolean[]   bdata,
        short[]     sdata,
        int[]       idata,
        float[]     fdata,
        double[]    ddata,
        int         selectable
    );

    protected native static int sendStaticCommand (
        int         nativeDlistID,
        int         command_id,
        int         threadid,
        long[]      llist,
        int[]       ilist,
        String      cdata,
        boolean[]   bdata,
        short[]     sdata,
        int[]       idata,
        float[]     fdata,
        double[]    ddata,
        int         selectable
    );


/*--------------------------------------------------------------------------*/

  /*
   * Actually draw the entire display list, using the current
   * clipping and scaling parameters.
   */
    protected native void nativeDraw (
        int         nativeDlistID,
        int         threadid
    );

/*--------------------------------------------------------------------------*/

  /*
   * Actually draw the selected items in the display list, using the current
   * clipping and scaling parameters.
   */
    protected native void nativeDrawSelected (
        int         nativeDlistID,
        int         threadid
    );


/*--------------------------------------------------------------------------*/

  /*
   * The nativeDraw method and the nativeDrawSelected method
   * will each end up having the EasyX
   * graphics system send back many different "native primitives"
   * to this Java object.  These are collected here and then
   * they are drawn to the actual Java Graphics2D after they
   * have all been collected.
   *
   * Each "native primitive" has a corresponding member class
   * defined for it and collections of each native primitve
   * type in ArrayList objects.  Multithread synchronization
   * is not needed, so ArrayList is used instead of Vector.
   */


/*--------------------------------------------------------------------------*/

  /*
   * Declare a list of
   * native fill objects, and implement the method
   * called from the native code to fill in a native
   * fill object and add it to the list.
   */
    ArrayList<NativePrim.Fill>   nativeFillArray = null;

    private void addNativeFill (
        float[]     xy,
        float       thickness,
        float       patScale,
        int         npts,
        int         red,
        int         green,
        int         blue,
        int         alpha,
        int         pattern,
        int         frame_num,
        int         selectable
    )
    {
        if (nativeFillArray == null) {
            nativeFillArray = new ArrayList<NativePrim.Fill> (BIG_CHUNK);
        }
        NativePrim.Fill native_fill = new NativePrim.Fill ();

        if (selectable == 1) {
            red = selectRed;
            green = selectGreen;
            blue = selectBlue;
            alpha = selectAlpha;
            thickness *= 1.5;
            if (thickness < 2) thickness = 2;
        }

        native_fill.xy = xy;
        native_fill.thickness = thickness;
        native_fill.patScale = patScale;
        native_fill.npts = npts;
        native_fill.red = red;
        native_fill.green = green;
        native_fill.blue = blue;
        native_fill.alpha = alpha;
        native_fill.pattern = pattern;
        native_fill.frame_num = frame_num;
        native_fill.image = -1;
        native_fill.selectable = selectable;

        nativeFillArray.add (native_fill);

        return;
    }

/*--------------------------------------------------------------------------*/

  /*
   * Declare a list of native frame objects.  These will be used
   * for clipping of other primitives if needed.
   */
    ArrayList<NativePrim.Frame>     nativeFrameArray = null;

    private   void addNativeFrame (
        float       x1,
        float       y1,
        float       x2,
        float       y2,
        double      fx1,
        double      fy1,
        double      fx2,
        double      fy2,
        int         borderflag,
        int         clipflag,
        int         scaleable,
        int         scale_to_attach_frame,
        int         frame_num,
        String      frame_name
    )
    {
        float       xt;
        double      xscale, yscale;

        if (nativeFrameArray == null) {
            nativeFrameArray = new ArrayList<NativePrim.Frame> (SMALL_CHUNK);
        }
        NativePrim.Frame native_frame = new NativePrim.Frame ();

        if (x1 > x2) {
            xt = x1;
            x1 = x2;
            x2 = xt;
        }
        if (y1 > y2) {
            xt = y1;
            y1 = y2;
            y2 = xt;
        }

    /*
     * The fx1, fy1 frame point is the lower left and the
     * fx2, fy2 is the upper right.  These need to be converted
     * to upper left for x1, y1 screen coordinates and lower
     * right for the x2, y2 screen coordinates.
     */
        double yt;
        yt = fy1;
        fy1 = fy2;
        fy2 = yt;

        if (x2 <= x1  ||  y2 <= y1  ||
            fx2 - fx1 == 0.0  ||  fy2 - fy1 == 0.0) {
            xscale = 1.e30;
            yscale = 1.e30;
        }
        else {
            xscale = (fx2 - fx1) / (x2 - x1);
            yscale = (fy2 - fy1) / (y2 - y1);
        }

        native_frame.x1 = x1;
        native_frame.y1 = y1;
        native_frame.x2 = x2;
        native_frame.y2 = y2;
        native_frame.xscale = xscale;
        native_frame.yscale = yscale;
        native_frame.fx1 = fx1;
        native_frame.fy1 = fy1;
        native_frame.borderflag = borderflag;
        native_frame.clipflag = clipflag;
        native_frame.scaleable = scaleable;
        native_frame.scale_to_attach_frame = scale_to_attach_frame;
        native_frame.frame_num = frame_num;
        native_frame.clip_shape = new Rectangle2D.Float (x1, y1, x2-x1, y2-y1);
        native_frame.frame_name = frame_name;

        nativeFrameArray.add (native_frame);

        return;
    }

/*--------------------------------------------------------------------------*/

    NativePrim.Frame findFrameByName (String name)
    {

        if (nativeFrameArray == null) {
            return null;
        }

        if (name == null) {
            return null;
        }

        int    size = nativeFrameArray.size ();
        if (size < 1) {
            return null;
        }

        NativePrim.Frame    nf;
        String              s1, s2;

        s1 = name.trim().toLowerCase();

        int same;

        for (int i=0; i<size; i++) {
            nf = nativeFrameArray.get(i);
            if (nf == null) {
                continue;
            }
            if (nf.frame_name == null) {
                continue;
            }
            s2 = nf.frame_name.trim().toLowerCase();

            same = s1.compareTo(s2);
            if (same == 0) {
                return nf;
            }

        }

        return null;

    }

/*--------------------------------------------------------------------------*/

    int findFrameNumberByName (String name)
    {

        if (nativeFrameArray == null) {
            return -1;
        }

        if (name == null) {
            return -1;
        }

        int    size = nativeFrameArray.size ();
        if (size < 1) {
            return -1;
        }

        NativePrim.Frame    nf;
        String              s1, s2;

        s1 = name.trim().toLowerCase();

        int same;

        for (int i=0; i<size; i++) {
            nf = nativeFrameArray.get(i);
            if (nf == null) {
                continue;
            }
            if (nf.frame_name == null) {
                continue;
            }
            s2 = nf.frame_name.trim().toLowerCase();

            same = s1.compareTo(s2);
            if (same == 0) {
                return nf.frame_num;
            }

        }

        return -1;

    }


/*--------------------------------------------------------------------------*/

  /*
   * Declare a list of
   * native line objects, and implement the method
   * called from the native code to fill in a native
   * line object and add it to the list.
   */
    ArrayList<NativePrim.Line>     nativeLineArray = null;

    private   void addNativeLine (
        float[]     xy,
        int         npts,
        int         red,
        int         green,
        int         blue,
        int         alpha,
        int         pattern,
        float       thickness,
        int         frame_num,
        int         image,
        int         selectable
    )
    {

        if (nativeLineArray == null) {
            nativeLineArray = new ArrayList<NativePrim.Line> (BIG_CHUNK);
        }
        NativePrim.Line native_line = new NativePrim.Line ();

        if (selectable == 1) {
            red = selectRed;
            green = selectGreen;
            blue = selectBlue;
            alpha = selectAlpha;
            thickness *= 1.5;
            if (thickness < 2) thickness = 2;
        }

        native_line.xy = xy;
        native_line.npts = npts;
        native_line.red = red;
        native_line.green = green;
        native_line.blue = blue;
        native_line.alpha = alpha;
        native_line.pattern = pattern;
        native_line.frame_num = frame_num;
        native_line.selectable = selectable;
        native_line.image = image;
        native_line.thickness = thickness;

        nativeLineArray.add (native_line);

        return;
    }


/*--------------------------------------------------------------------------*/

  /*
   * Declare a list of
   * native text objects, and implement the method
   * called from the native code to fill in a native
   * text object and add it to the list.
   */
    ArrayList<NativePrim.Text>  nativeTextArray = null;

    private   void addNativeText (
        float       x,
        float       y,
        String      textin,
        int         red,
        int         green,
        int         blue,
        int         alpha,
        float       angle,
        float       size,
        int         fontIndex,
        int         frame_num,
        int         image,
        int         selectable
    )
    {
        if (nativeTextArray == null) {
            nativeTextArray = new ArrayList<NativePrim.Text> (BIG_CHUNK);
        }
        NativePrim.Text native_text = new NativePrim.Text ();

        String text = textin.trim();

        if (selectable == 1) {
            red = selectRed;
            green = selectGreen;
            blue = selectBlue;
            alpha = selectAlpha;
        }

        Font font = deriveFontForIndex (
            fontIndex,
            size);
        if (font == null) {
            font = FontUtils.getDefaultFont ((double)size);
        }

        native_text.x = x;
        native_text.y = y;
        native_text.text = text;
        native_text.red = red;
        native_text.green = green;
        native_text.blue = blue;
        native_text.alpha = alpha;
        native_text.angle = angle;
        native_text.size = size;
        native_text.baseFont = font;
        native_text.frame_num = frame_num;
        native_text.image = image;
        native_text.selectable = selectable;

        nativeTextArray.add (native_text);

        return;
    }


/*--------------------------------------------------------------------------*/

  /*
   * Declare a list of
   * native arc objects, and implement the method
   * called from the native code to fill in a native
   * arc object and add it to the list.
   */
    ArrayList<NativePrim.Arc>   nativeArcArray = null;

    private   void addNativeArc (
        float       x,
        float       y,
        float       r1,
        float       r2,
        float       ang1,
        float       ang2,
        int         closure,
        int         red,
        int         green,
        int         blue,
        int         alpha,
        float       thickness,
        float       angle,
        int         frame_num,
        int         selectable
    )
    {
        if (nativeArcArray == null) {
            nativeArcArray = new ArrayList<NativePrim.Arc> (BIG_CHUNK);
        }
        NativePrim.Arc native_arc = new NativePrim.Arc ();

        if (selectable == 1) {
            red = selectRed;
            green = selectGreen;
            blue = selectBlue;
            alpha = selectAlpha;
            thickness *= 1.5;
        }

    /*
     * If a partial arc is rotated, it only appears to work correctly
     * if the rotation angle is subtracted from the start angle for
     * the partial arc.  I don't know if this is the proper behavior
     * for rotated arcs in Java or if it is a bug.  If it is a bug,
     * that gets fixed in the future, then the following bit of code
     * will probably need to be modified.
     *
     * G. Pinkerton   11/26/03
     */
        if (ang2 < 360.0) {
           ang1 -= angle;
        }

        native_arc.x = x;
        native_arc.y = y;
        native_arc.r1 = r1;
        native_arc.r2 = r2;
        native_arc.ang1 = ang1;
        native_arc.ang2 = ang2;
        native_arc.closure = closure;
        native_arc.red = red;
        native_arc.green = green;
        native_arc.blue = blue;
        native_arc.alpha = alpha;
        native_arc.thickness = thickness;
        native_arc.angle = angle;
        native_arc.frame_num = frame_num;
        native_arc.image = -1;
        native_arc.selectable = selectable;

        nativeArcArray.add (native_arc);

        return;
    }


/*--------------------------------------------------------------------------*/

  /*
   * Declare a list of
   * native filled arc objects, and implement the method
   * called from the native code to fill in a native
   * filled arc object and add it to the list.
   */
    ArrayList<NativePrim.FilledArc>   nativeFilledArcArray = null;

    private   void addNativeFilledArc (
        float       x,
        float       y,
        float       r1,
        float       r2,
        float       ang1,
        float       ang2,
        int         closure,
        int         red,
        int         green,
        int         blue,
        int         alpha,
        float       thickness,
        float       angle,
        int         pattern,
        int         frame_num,
        int         selectable
    )
    {
        if (nativeFilledArcArray == null) {
            nativeFilledArcArray = new ArrayList<NativePrim.FilledArc> (BIG_CHUNK);
        }
        NativePrim.FilledArc native_filled_arc = new NativePrim.FilledArc ();

        if (selectable == 1) {
            red = selectRed;
            green = selectGreen;
            blue = selectBlue;
            alpha = selectAlpha;
            thickness *= 1.5;
        }

    /*
     * If a partial arc is rotated, it only appears to work correctly
     * if the rotation angle is subtracted from the start angle for
     * the partial arc.  I don't know if this is the proper behavior
     * for rotated arcs in Java or if it is a bug.  If it is a bug,
     * that gets fixed in the future, then the following bit of code
     * will probably need to be modified.
     *
     * G. Pinkerton   11/26/03
     */
        if (ang2 < 360.0) {
           ang1 -= angle;
        }

        native_filled_arc.x = x;
        native_filled_arc.y = y;
        native_filled_arc.r1 = r1;
        native_filled_arc.r2 = r2;
        native_filled_arc.ang1 = ang1;
        native_filled_arc.ang2 = ang2;
        native_filled_arc.closure = closure;
        native_filled_arc.red = red;
        native_filled_arc.green = green;
        native_filled_arc.blue = blue;
        native_filled_arc.alpha = alpha;
        native_filled_arc.angle = angle;
        native_filled_arc.pattern = pattern;
        native_filled_arc.frame_num = frame_num;
        native_filled_arc.image = -1;
        native_filled_arc.selectable = selectable;

        nativeFilledArcArray.add (native_filled_arc);

        return;
    }


/*--------------------------------------------------------------------------*/

  /*
   * Declare a list of
   * native image objects, and implement the method
   * called from the native code to fill in a native
   * image object and add it to the list.
   */
    ArrayList<NativePrim.Image>   nativeImageArray = null;

    private   void addNativeImage (
        float       x1,
        float       y1,
        float       x2,
        float       y2,
        int         ncol,
        int         nrow,
        byte[]      red,
        byte[]      green,
        byte[]      blue,
        byte[]      transparency,
        int         frame_num,
        int         has_lines,
        int         image_id,
        int         selectable
    )
    {
        if (nativeImageArray == null) {
            nativeImageArray = new ArrayList<NativePrim.Image> (SMALL_CHUNK);
        }
        NativePrim.Image native_image = new NativePrim.Image ();

        native_image.x1 = x1;
        native_image.y1 = y1;
        native_image.x2 = x2;
        native_image.y2 = y2;
        native_image.ncol = ncol;
        native_image.nrow = nrow;
        native_image.red = red;
        native_image.green = green;
        native_image.blue = blue;
        native_image.transparency = transparency;
        native_image.frame_num = frame_num;
        native_image.selectable = selectable;
        if (has_lines == 0) {
            native_image.hasLines = false;
        }
        else {
            native_image.hasLines = true;
        }
        native_image.id = image_id;

        nativeImageArray.add (native_image);

        return;
    }

/*--------------------------------------------------------------------------*/

  /*
   * Add to the list of shapes to draw for the zoom/pan operation.
   */
    ArrayList<NativePrim.DirectShape>  zoomPanShapeArray = null;

    void addZoomPanShape (Shape shape,
                         int red,
                         int green,
                         int blue,
                         int fill_flag
    )
    {
        if (zoomPanShapeArray == null) {
            zoomPanShapeArray = new ArrayList<NativePrim.DirectShape> (SMALL_CHUNK);
        }
        NativePrim.DirectShape direct_shape = new NativePrim.DirectShape ();

        direct_shape.shape = shape;
        direct_shape.red = red;
        direct_shape.green = green;
        direct_shape.blue = blue;
        if (fill_flag != 1) fill_flag = 0;
        direct_shape.fill_flag = fill_flag;

        zoomPanShapeArray.add (direct_shape);

        return;
    }


/*--------------------------------------------------------------------------*/

  /*
   * Declare a list of Shapes to draw directly to the screen.  This list
   * should be used for rubber band graphics.  This is not public.  All
   * the rubber banding is done via interaction with the display list panel
   * class, which is in the same package.
   */
    ArrayList<NativePrim.DirectShape>  directShapeArray = null;

    void addDirectShape (Shape shape,
                         int red,
                         int green,
                         int blue,
                         int fill_flag
    )
    {
        if (directShapeArray == null) {
            directShapeArray = new ArrayList<NativePrim.DirectShape> (SMALL_CHUNK);
        }
        NativePrim.DirectShape direct_shape = new NativePrim.DirectShape ();

        direct_shape.shape = shape;
        direct_shape.red = red;
        direct_shape.green = green;
        direct_shape.blue = blue;
        if (fill_flag != 1) fill_flag = 0;
        direct_shape.fill_flag = fill_flag;
        direct_shape.fnum = -1;

        directShapeArray.add (direct_shape);

        return;
    }

    void addDirectShape (Shape shape,
                         int red,
                         int green,
                         int blue,
                         int fill_flag,
                         int fnum
    )
    {
        if (directShapeArray == null) {
            directShapeArray = new ArrayList<NativePrim.DirectShape> (SMALL_CHUNK);
        }
        NativePrim.DirectShape direct_shape = new NativePrim.DirectShape ();

        direct_shape.shape = shape;
        direct_shape.red = red;
        direct_shape.green = green;
        direct_shape.blue = blue;
        if (fill_flag != 1) fill_flag = 0;
        direct_shape.fill_flag = fill_flag;
        direct_shape.fnum = fnum;

        directShapeArray.add (direct_shape);

        return;
    }


/*--------------------------------------------------------------------------*/

  /*
   * Remove all shapes from the direct shape array.
   */
    public void clearDirectShapes ()
    {
        if (directShapeArray != null) {
            directShapeArray.clear ();
        }
    }

    public void clearZoomPanShapes ()
    {
        if (zoomPanShapeArray != null) {
            zoomPanShapeArray.clear ();
        }
    }

/*--------------------------------------------------------------------------*/

  /*
   * Variables to keep track of the device viewport.
   */
    int screenXOrigin = -1;
    int screenYOrigin = -1;
    int screenWidth = -1;
    int screenHeight = -1;

  /**
   *
   * Set the rectangular area on the component where this display list
   * should be drawn.  If this method is not called, the entire
   * component size will be used for drawing.  The coordinates passed
   * to this function must be Java user space coordinates.  By default,
   * the user space coordinate system has its origin in the upper left
   * corner of the window, with x increasing to the right and y increasing
   * downward.   The default units are points (1/72 of an inch).  There
   * is no need to change this default Java user space setup.
   *
   * If you want to revert to using the entire component, set any of the
   * four parameters to -1.
   */
    public void setDeviceViewport (
        int     xorigin,
        int     yorigin,
        int     width,
        int     height
    )
    {
        if (xorigin < 0  ||  yorigin < 0  ||
            width < 0  ||  height < 0) {
            screenXOrigin = -1;
            screenYOrigin = -1;
            screenWidth = -1;
            screenHeight = -1;
        }
        else {
            screenXOrigin = xorigin;
            screenYOrigin = yorigin;
            screenWidth = width;
            screenHeight = height;
        }

        return;
    }

  /**
   *  Return the X origin of the viewport as assigned by SetDeviceViewport.
   *  A value of -1 means to use the entire component for drawing.
   */
    public int getDeviceViewportXorigin () {return screenXOrigin;}
  /**
   *  Return the Y origin of the viewport as assigned by SetDeviceViewport.
   *  A value of -1 means to use the entire component for drawing.
   */
    public int getDeviceViewportYorigin () {return screenYOrigin;}
  /**
   *  Return the width of the viewport as assigned by SetDeviceViewport.
   *  A value of -1 means to use the entire component for drawing.
   */
    public int getDeviceViewportWidth () {return screenWidth;}
  /**
   *  Return the height of the viewport as assigned by SetDeviceViewport.
   *  A value of -1 means to use the entire component for drawing.
   */
    public int getDeviceViewportHeight () {return screenHeight;}

/*--------------------------------------------------------------------------*/

  /*
   * Two buffered image objects are possibly used for drawing into.
   * One is for non selectable (background) primitives.  The other
   * is for selectable primitives that are not currently being animated.
   */
    BufferedImage    backgroundImage = null;
    BufferedImage    foregroundImage = null;

/*--------------------------------------------------------------------------*/

    private void clearNativePrims ()
    {
        if (nativeLineArray != null) {
            nativeLineArray.clear();
        }
        if (nativeFillArray != null) {
            nativeFillArray.clear();
        }
        if (nativeTextArray != null) {
            nativeTextArray.clear();
        }
        if (nativeArcArray != null) {
            nativeArcArray.clear();
        }
        if (nativeFilledArcArray != null) {
            nativeFilledArcArray.clear();
        }
        if (nativeImageArray != null) {
            nativeImageArray.clear();
        }
        if (nativeFrameArray != null) {
            nativeFrameArray.clear();
        }
    }

/*--------------------------------------------------------------------------*/

    private void clearNativePrimsWithoutFrames ()
    {
        if (nativeLineArray != null) {
            nativeLineArray.clear();
        }
        if (nativeFillArray != null) {
            nativeFillArray.clear();
        }
        if (nativeTextArray != null) {
            nativeTextArray.clear();
        }
        if (nativeArcArray != null) {
            nativeArcArray.clear();
        }
        if (nativeFilledArcArray != null) {
            nativeFilledArcArray.clear();
        }
        if (nativeImageArray != null) {
            nativeImageArray.clear();
        }
    }

/*--------------------------------------------------------------------------*/

    private int          foregroundFlag = 1;

  /**
   *
   * Draw the current display list to the specifed graphics2d object.
   * This is called from the JDisplayListPanel class only.
   */
    int drawToDevice (Graphics2D g2d,
                      int xorigin, int yorigin,
                      int width, int height) {

      /*
       * Do not attempt null g2d or very small area drawing.
       */
        if (g2d == null  ||  height * width < 100) {
            return 0;
        }

        if (needsNativeRedraw == 0) {
            if (height != screenHeight  ||  width != screenWidth) {
                needsNativeRedraw = 1;
            }
        }

        screenWidth = width;
        screenHeight = height;

        int threadid = Thread.currentThread().hashCode();

        if (needsNativeRedraw == 1) {

            clearNativePrims ();
            setViewportForDrawing (width, height);
            nativeDraw (nativeDlistID, threadid);
            backgroundImage =
              new BufferedImage (width, height, BufferedImage.TYPE_INT_ARGB);
            foregroundImage =
              new BufferedImage (width, height, BufferedImage.TYPE_INT_ARGB);
            foregroundFlag = localDrawFunc ();
            needsNativeRedraw = 0;
        }

        else if (needsNativeSelectRedraw == 1) {
            clearNativePrimsWithoutFrames ();
            setViewportForDrawing (width, height);
            nativeDrawSelected (nativeDlistID, threadid);
            needsNativeRedraw = 0;
            foregroundImage =
              new BufferedImage (width, height, BufferedImage.TYPE_INT_ARGB);
            foregroundFlag = localDrawFunc ();
            needsNativeSelectRedraw = 0;
        }

        if (jdlEditor != null) {
            jdlEditor.draw ();
        }

        visibleDraw (g2d, xorigin, yorigin);


        return 0;

    }

/*--------------------------------------------------------------------------*/

    boolean               selectionAvailable;

    void clearSelectedPrims ()
    {
        DLSelectable         dls;

        if (selectableList != null) {
            int n = selectableList.size ();
            for (int i=0; i<n; i++) {
                dls = selectableList.get (i);
                dls.isSelected = false;
                dls.lineList = null;
                dls.fillList = null;
                dls.textList = null;
                dls.symbList = null;
                dls.arcList = null;
                dls.rectangleList = null;
                dls.contourList = null;
            }
        }
        selectionAvailable = false;
    }

/*--------------------------------------------------------------------------*/

    private int checkSelectedPrims ()
    {
        DLSelectable         dls;

        int n = 0;

        if (selectableList != null) {
            int size = selectableList.size ();
            for (int i=0; i<size; i++) {
                dls = selectableList.get (i);
                if (dls.isSelected) {
                    n++;
                }
            }
        }
        return n;
    }

/*--------------------------------------------------------------------------*/

    boolean pickFrameObject (int ix, int iy)
    {
        NativePrim.Frame    frame;

        clearSelectedPrims ();

        frame = findEditFrame (ix, iy);
        if (frame == null) {
            callSelectListeners();
            return false;
        }

        int threadid = Thread.currentThread().hashCode();

        int istat =
        nativePick (nativeDlistID,
                    threadid,
                    frame.frame_num,
                    ix, iy);

        if (istat == -1) {
            selectionAvailable = false;
            return false;
        }

        convertToFrame (nativeDlistID,
                        threadid,
                        frame.frame_num,
                        ix, iy);

        setNativeSelectDrawNeeded (1);

        screenX = ix;
        screenY = iy;
        callSelectListeners ();

        return selectionAvailable;
    }

/*--------------------------------------------------------------------------*/
    int getSelectableIndex (int ix, int iy)
    {
        NativePrim.Frame    frame;

        frame = findEditFrame (ix, iy);
        if (frame == null) {
            return -1;
        }

        int[] ilist = new int[3];
        ilist[0] = frame.frame_num;
        ilist[1] = ix;
        ilist[2] = iy;

        int isel = sendNativeCommand (DLConst.GTX_GET_PRIM_NUM, ilist);

        return isel;
    }
/*--------------------------------------------------------------------------*/

    protected void setViewportForDrawing (int width, int height) {

        int[]    ilist;

        ilist = new int[5];

        ilist[0] = width;
        ilist[1] = height;

        sendNativeCommand (
            DLConst.GTX_SETSCREENSIZE,
            ilist
        );

    }

/*--------------------------------------------------------------------------*/

    NativePrim.Frame findEditFrame (int fnum)
    {
        NativePrim.Frame   native_frame;
        int                i, n;

        if (nativeFrameArray == null  ||  fnum < 0) {
            return null;
        }

        n = nativeFrameArray.size();
        for (i=0; i<n; i++) {
            native_frame = nativeFrameArray.get(i);
            if (native_frame == null) {
                continue;
            }
            if (native_frame.frame_num == fnum) {
                return native_frame;
            }
        }

        return null;

    }

/*--------------------------------------------------------------------------*/

    NativePrim.Frame findEditFrame (int ix, int iy)
    {
        NativePrim.Frame   native_frame;
        int                i, n;

        if (nativeFrameArray == null) {
            return null;
        }

        n = nativeFrameArray.size();
        for (i=0; i<n; i++) {
            native_frame = nativeFrameArray.get(i);
            if (native_frame == null) {
                continue;
            }
            if (native_frame.clip_shape.contains ((double)ix, (double)iy)) {
                return native_frame;
            }
        }

        return null;

    }

/*--------------------------------------------------------------------------*/

    int findRescaleFrame (int ix, int iy)
    {
        NativePrim.Frame   native_frame;
        int                i, n;

        if (nativeFrameArray == null) {
            return -1;
        }

        n = nativeFrameArray.size();
        for (i=0; i<n; i++) {
            native_frame = nativeFrameArray.get(i);
            if (native_frame == null) {
                continue;
            }
            if (native_frame.scaleable == 0) {
                continue;
            }
            if (native_frame.clip_shape.contains ((double)ix, (double)iy)) {
                return native_frame.frame_num;
            }
        }

        return -1;

    }

/*--------------------------------------------------------------------------*/

    int getSingleRescaleFrame ()
    {
        int                nscale, iscale;

        NativePrim.Frame   native_frame;
        int                i, n;

        if (nativeFrameArray == null) {
            return -1;
        }

        nscale = 0;
        iscale = -1;
        n = nativeFrameArray.size();
        for (i=0; i<n; i++) {
            native_frame = nativeFrameArray.get(i);
            if (native_frame == null) {
                continue;
            }
            if (native_frame.scaleable == 1  &&
                native_frame.scale_to_attach_frame == 0) {
                iscale = i;
                nscale++;
                continue;
            }
        }

        if (nscale != 1) {
            return -1;
        }

        return iscale;

    }

/*--------------------------------------------------------------------------*/

    void zoomFrame (int frame_num,
                    int resize_border_flag,
                    int ix1,
                    int iy1,
                    int ix2,
                    int iy2)
    {
        int[] ilist = new int[10];

        ilist[0] = frame_num;
        ilist[1] = resize_border_flag;
        ilist[2] = ix1;
        ilist[3] = iy1;
        ilist[4] = ix2;
        ilist[5] = iy2;

        sendNativeCommand (
            DLConst.GTX_ZOOMFRAME,
            ilist
        );

    }

/*--------------------------------------------------------------------------*/

    void panFrame (int frame_num,
                   int ix1,
                   int iy1,
                   int ix2,
                   int iy2)
    {
        int[] ilist = new int[10];

        ilist[0] = frame_num;
        ilist[1] = ix1;
        ilist[2] = iy1;
        ilist[3] = ix2;
        ilist[4] = iy2;

        sendNativeCommand (
            DLConst.GTX_PANFRAME,
            ilist
        );

    }

/*--------------------------------------------------------------------------*/

    void zoomToExtents (int frame_num)
    {
        int[] ilist = new int[2];

        ilist[0] = frame_num;

        sendNativeCommand (
            DLConst.GTX_ZOOMEXTENTS,
            ilist
        );

    }

/*--------------------------------------------------------------------------*/

    void zoomOut (int frame_num)
    {
        int[] ilist = new int[2];

        ilist[0] = frame_num;

        sendNativeCommand (
            DLConst.GTX_ZOOMOUT,
            ilist
        );

    }

/*--------------------------------------------------------------------------*/

    void zoomIn (int frame_num,
                 int ix,
                 int iy)
    {
        int                iw, ih, i, nframes;
        NativePrim.Frame   nf;

        if (nativeFrameArray == null) {
            return;
        }

        nframes = nativeFrameArray.size();

        for (i=0; i<nframes; i++) {
            nf = nativeFrameArray.get(i);
            if (nf.frame_num == frame_num) {
                iw = (int)(nf.x2 - nf.x1);
                ih = (int)(nf.y2 - nf.y1);
                if (iw < 0) iw = -iw;
                if (ih < 0) ih = -ih;
                iw /= 4;
                ih /= 4;
                if (iw < 2) iw = 2;
                if (ih < 2) ih = 2;

                int[] ilist = new int[9];

                ilist[0] = frame_num;
                ilist[1] = 0;
                ilist[2] = ix - iw;
                ilist[3] = iy - ih;
                ilist[4] = ix + iw;
                ilist[5] = iy + ih;

                sendNativeCommand (
                    DLConst.GTX_ZOOMFRAME,
                    ilist
                );
            }
        }

        return;

    }

/*--------------------------------------------------------------------------*/

    protected int localDrawFunc () {

        int             status;
        Graphics2D      g2d_fg, g2d_bg;

        if (foregroundImage == null  ||  backgroundImage == null) {
            return 0;
        }

    /*
     * Create graphic contexts for each image.
     */
        g2d_fg = foregroundImage.createGraphics ();
        if (needsNativeRedraw == 1) {
            g2d_bg = backgroundImage.createGraphics ();
        }
        else {
            g2d_bg = g2d_fg;
        }

    /*
     * Create a LocalDraw object.  This is only used
     * from this single spot.  I put it in a file of
     * its own just to keep this file down to a
     * manageable size.
     */
        LocalDraw local_draw = new LocalDraw ();
        local_draw.setNativeArrays (
            nativeLineArray,
            nativeFillArray,
            nativeTextArray,
            nativeArcArray,
            nativeFilledArcArray,
            nativeImageArray,
            nativeFrameArray
        );

        local_draw.setProperties (
            screenWidth,
            screenHeight,
            backgroundRed,
            backgroundGreen,
            backgroundBlue,
            backgroundAlpha
        );

     /*
      * Do the drawing via the local draw object.
      */
        status = local_draw.localDraw (g2d_bg, g2d_fg);

        return status;

    }


/*---------------------------------------------------------------------*/

    void visibleDraw (
        Graphics2D g2d,
        int xorigin,
        int yorigin
    )
    {
        int                      i, nshape;
        NativePrim.DirectShape   direct_shape;
        Color                    color;

        g2d.setColor (Color.red);
        g2d.fill3DRect (0, 0, 50, 50, true);

    /*
     * Draw the opaque background image.
     */
        if (backgroundImage != null) {
            g2d.drawImage (
                backgroundImage,
                xorigin,
                yorigin,
                null
            );
        }

    /*
     * If needed, the foreground image is drawn.  It has transparent
     * background pixels.
     */
        if (foregroundFlag == 1) {
            if (foregroundImage != null) {
                g2d.drawImage (
                    foregroundImage,
                    xorigin,
                    yorigin,
                    null
                );
            }
        }

    /*
     * Draw any rubber band graphics on top of everything else.
     */
        BasicStroke basic_stroke =
        new BasicStroke (1.0f);

        g2d.setStroke (basic_stroke);
        if (directShapeArray != null) {
            nshape = directShapeArray.size ();
            for (i=0; i<nshape; i++) {
                 direct_shape = directShapeArray.get(i);
                 color = new Color (direct_shape.red,
                                    direct_shape.green,
                                    direct_shape.blue,
                                    255);
                 lookupAndSetClip (g2d,
                                   direct_shape.fnum);
                 g2d.setColor (color);
                 if (direct_shape.fill_flag == 0) {
                     g2d.draw (direct_shape.shape);
                 }
                 else {
                     g2d.fill (direct_shape.shape);
                 }
            }
        }

        if (zoomPanShapeArray != null) {
            nshape = zoomPanShapeArray.size ();
            for (i=0; i<nshape; i++) {
                 direct_shape = zoomPanShapeArray.get(i);
                 color = new Color (direct_shape.red,
                                    direct_shape.green,
                                    direct_shape.blue,
                                    255);
                 lookupAndSetClip (g2d,
                                   direct_shape.fnum);
                 g2d.setColor (color);
                 if (direct_shape.fill_flag == 0) {
                     g2d.draw (direct_shape.shape);
                 }
                 else {
                     g2d.fill (direct_shape.shape);
                 }
            }
        }

        return;
    }


/*-----------------------------------------------------------------------*/

 /*
  * Methods and members for managing the DLSelectable objects set in the
  * display list.
  */
    ArrayList<DLSelectable>  selectableList = null;

    void addSelectable (DLSelectable s)
    {
        int       i, size;

        if (s == null) {
            return;
        }

        if (selectableList == null) {
            selectableList = new ArrayList<DLSelectable> (BIG_CHUNK);
        }

        if (selectableList == null) {
            return;
        }

        i = s.getNativeIndex ();
        if (i == -1) {
            size = selectableList.size ();
            selectableList.add (s);
            s.setNativeIndex (size);
        }

        return;
    }

    DLSelectable getSelectable (int i)
    {
        int        size;

        if (i < 0) {
            return null;
        }

        if (selectableList == null) {
            return null;
        }

        size = selectableList.size ();

        if (i >= size) {
            return null;
        }

        DLSelectable s = selectableList.get (i);

        return s;
    }

/*--------------------------------------------------------------------------*/

    boolean getEditPrimitives (int dlNum)
    {

        clearSelectedPrims ();

        int threadid = Thread.currentThread().hashCode();

        int istat =
        nativeEdit (nativeDlistID,
                    threadid,
                    dlNum);

        if (istat == -1) {
            selectionAvailable = false;
        }

        setNativeSelectDrawNeeded (1);

        return selectionAvailable;
    }

/*--------------------------------------------------------------------------*/

  /*
   * Select or unselect an item in the native display list.  The
   * screen coordinates of the mouse click along with the frame
   * number are specified.
   */
    native int nativePick (
        int        nativeDlistID,
        int        threadid,
        int        frameNum,
        int        x,
        int        y);

    native int convertToFrame (
        int        nativeDlistID,
        int        threadid,
        int        frameNum,
        int        x,
        int        y);


/*--------------------------------------------------------------------------*/

  /*
   * Return the primitives associated with the the specified DLSelectable
   * object number.
   */
    native int nativeEdit (
        int        nativeDlistID,
        int        threadid,
        int        dlNum);

/*--------------------------------------------------------------------------*/

  /*
   * The nativePick and the nativeEdit function can have the easyx display list
   * send back the collection of primitives currently selected.  The next
   * several methods are called from the native code to populate the
   * java list of selected primitives.
   */

/*--------------------------------------------------------------------------*/

    private void addSelectedFill (
        int             selectable_index,
        double[]        xp,
        double[]        yp,
        int[]           npts,
        int             ncomp,
        double          thick,
        double          patscale,
        double          dashscale,
        int             red_fill,
        int             green_fill,
        int             blue_fill,
        int             alpha_fill,
        int             red_pat,
        int             green_pat,
        int             blue_pat,
        int             alpha_pat,
        int             red_border,
        int             green_border,
        int             blue_border,
        int             alpha_border,
        String          fname,
        String          lname,
        String          iname,
        int             fillpat,
        int             linepat,
        int             native_index
    )
    {
        DLFill          fill;
        DLSelectable    dls;

        fill = new DLFill ();

        dls = getSelectable (selectable_index);
        if (dls == null) {
            return;
        }

        fill.selectableObject = dls;
        dls.isSelected = true;

        fill.xPoints = xp;
        fill.yPoints = yp;
        fill.numPoints = npts;
        fill.numComponents = ncomp;

        fill.borderThickness = thick;
        fill.patternScale = patscale;
        fill.dashScale = dashscale;

        if (alpha_fill > 0) {
            fill.color =
            new Color (red_fill, green_fill, blue_fill, alpha_fill);
        }
        if (alpha_pat > 0) {
            fill.patternColor =
            new Color (red_pat, green_pat, blue_pat, alpha_pat);
        }
        if (alpha_border > 0) {
            fill.borderColor =
            new Color (red_border, green_border, blue_border, alpha_border);
        }

        fill.frameName = fname;
        fill.layerName = lname;
        fill.itemName = iname;
        fill.pattern = fillpat;
        fill.linePattern = linepat;
        fill.nativeIndex = native_index;

        if (dls.fillList == null) {
            dls.fillList = new ArrayList<DLFill> (10);
        }
        dls.fillList.add (fill);

        selectionAvailable = true;

        return;

    }



/*--------------------------------------------------------------------------*/

    private void addSelectedArc (
        int             selectable_index,
        double          xc,
        double          yc,
        double          xr,
        double          yr,
        double          ang1,
        double          anglen,
        double          rang,
        int             closure,
        double          thick,
        double          patscale,
        double          dashscale,
        int             red_fill,
        int             green_fill,
        int             blue_fill,
        int             alpha_fill,
        int             red_pat,
        int             green_pat,
        int             blue_pat,
        int             alpha_pat,
        int             red_border,
        int             green_border,
        int             blue_border,
        int             alpha_border,
        String          fname,
        String          lname,
        String          iname,
        int             fillpat,
        int             linepat,
        int             native_index
    )
    {
        DLArc           arc;
        DLSelectable    dls;

        arc = new DLArc ();

        dls = getSelectable (selectable_index);
        if (dls == null) {
            return;
        }
        arc.selectableObject = dls;
        dls.isSelected = true;

        arc.xCenter = xc;
        arc.yCenter = yc;
        arc.xRadius = xr;
        arc.yRadius = yr;
        arc.startAngle = ang1;
        arc.angleLength = anglen;
        arc.rotationAngle = rang;
        arc.closure = closure;
        arc.borderThickness = thick;
        arc.patternScale = patscale;
        arc.dashScale = dashscale;

        if (alpha_fill > 0) {
            arc.fillColor =
            new Color (red_fill, green_fill, blue_fill, alpha_fill);
        }
        if (alpha_pat > 0) {
            arc.patternColor =
            new Color (red_pat, green_pat, blue_pat, alpha_pat);
        }
        if (alpha_border > 0) {
            arc.borderColor =
            new Color (red_border, green_border, blue_border, alpha_border);
        }

        arc.frameName = fname;
        arc.layerName = lname;
        arc.itemName = iname;
        arc.fillPattern = fillpat;
        arc.linePattern = linepat;
        arc.nativeIndex = native_index;

        if (dls.arcList == null) {
            dls.arcList = new ArrayList<DLArc> (10);
        }
        dls.arcList.add (arc);

        selectionAvailable = true;

        return;

    }

/*--------------------------------------------------------------------------*/

    private void addSelectedRectangle (
        int             selectable_index,
        double          xc,
        double          yc,
        double          xr,
        double          yr,
        double          crad,
        double          rang,
        double          thick,
        double          patscale,
        double          dashscale,
        int             red_fill,
        int             green_fill,
        int             blue_fill,
        int             alpha_fill,
        int             red_pat,
        int             green_pat,
        int             blue_pat,
        int             alpha_pat,
        int             red_border,
        int             green_border,
        int             blue_border,
        int             alpha_border,
        String          fname,
        String          lname,
        String          iname,
        int             fillpat,
        int             linepat,
        int             native_index
    )
    {
        DLRectangle           rect;
        DLSelectable    dls;

        rect = new DLRectangle ();

        dls = getSelectable (selectable_index);
        if (dls == null) {
            return;
        }
        rect.selectableObject = dls;
        dls.isSelected = true;

        rect.xCenter = xc;
        rect.yCenter = yc;
        rect.width = xr;
        rect.height = yr;
        rect.cornerRadius = crad;
        rect.rotationAngle = rang;
        rect.borderThickness = thick;
        rect.patternScale = patscale;
        rect.dashScale = dashscale;

        if (alpha_fill > 0) {
            rect.fillColor =
            new Color (red_fill, green_fill, blue_fill, alpha_fill);
        }
        if (alpha_pat > 0) {
            rect.patternColor =
            new Color (red_pat, green_pat, blue_pat, alpha_pat);
        }
        if (alpha_border > 0) {
            rect.borderColor =
            new Color (red_border, green_border, blue_border, alpha_border);
        }

        rect.frameName = fname;
        rect.layerName = lname;
        rect.itemName = iname;
        rect.fillPattern = fillpat;
        rect.linePattern = linepat;
        rect.nativeIndex = native_index;

        if (dls.rectangleList == null) {
            dls.rectangleList = new ArrayList<DLRectangle> (10);
        }
        dls.rectangleList.add (rect);

        selectionAvailable = true;

        return;

    }


/*--------------------------------------------------------------------------*/

    private void addSelectedLine (
        int             selectable_index,
        double[]        xp,
        double[]        yp,
        int             npts,
        double          thick,
        double          dashscale,
        int             red_line,
        int             green_line,
        int             blue_line,
        int             alpha_line,
        String          fname,
        String          lname,
        String          iname,
        int             linepat,
        int             symbol,
        int             arrow_style,
        int             native_index
    )
    {
        DLLine           line;
        DLSelectable    dls;

        line = new DLLine ();

        dls = getSelectable (selectable_index);
        if (dls == null) {
            return;
        }

        line.selectableObject = dls;
        dls.isSelected = true;

        line.xPoints = xp;
        line.yPoints = yp;
        line.numPoints = npts;

        line.thickness = thick;
        line.dashScale = dashscale;

        line.color =
            new Color (red_line, green_line, blue_line, alpha_line);

        line.frameName = fname;
        line.layerName = lname;
        line.itemName = iname;

        line.pattern = linepat;
        line.symbol = symbol;
        line.arrowStyle = arrow_style;
        line.nativeIndex = native_index;

        if (dls.lineList == null) {
            dls.lineList = new ArrayList<DLLine> (10);
        }
        dls.lineList.add (line);

        selectionAvailable = true;

        return;

    }


/*--------------------------------------------------------------------------*/

    private void addSelectedContour (
        int             selectable_index,
        double[]        xp,
        double[]        yp,
        int             npts,
        double          zval,
        double          thick,
        boolean         major,
        int             red_cont,
        int             green_cont,
        int             blue_cont,
        int             alpha_cont,
        String          label,
        int             label_font,
        double          label_size,
        double          label_space,
        double          tick_len,
        double          tick_space,
        int             tick_dir,
        String          fname,
        String          lname,
        String          iname,
        String          sname,
        int             native_index
    )
    {
        DLContour       cont;
        DLSelectable    dls;

        cont = new DLContour ();

        dls = getSelectable (selectable_index);
        if (dls == null) {
            return;
        }
        cont.selectableObject = dls;
        dls.isSelected = true;

        cont.xPoints = xp;
        cont.yPoints = yp;
        cont.numPoints = npts;

        cont.zValue = zval;
        cont.thickness = thick;
        cont.isMajor = major;

        cont.color =
            new Color (red_cont, green_cont, blue_cont, alpha_cont);

        cont.labelText = label;
        cont.labelFont = label_font;
        cont.labelSize = label_size;
        cont.labelSpacing = label_space;
        cont.tickLength = tick_len;
        cont.tickSpacing = tick_space;
        cont.tickDirection = tick_dir;

        cont.frameName = fname;
        cont.layerName = lname;
        cont.itemName = iname;
        cont.surfaceName = sname;

        cont.nativeIndex = native_index;

        if (dls.contourList == null) {
            dls.contourList = new ArrayList<DLContour> (10);
        }
        dls.contourList.add (cont);

        selectionAvailable = true;

        return;

    }


/*--------------------------------------------------------------------------*/

    private void addSelectedText (
        int             selectable_index,
        double          xp,
        double          yp,
        int             anchor,
        double          thick,
        double          bgthick,
        double          angle,
        double          size,
        double          xoff,
        double          yoff,
        int             red_text,
        int             green_text,
        int             blue_text,
        int             alpha_text,
        int             red_fill,
        int             green_fill,
        int             blue_fill,
        int             alpha_fill,
        int             red_bgfill,
        int             green_bgfill,
        int             blue_bgfill,
        int             alpha_bgfill,
        int             red_bgborder,
        int             green_bgborder,
        int             blue_bgborder,
        int             alpha_bgborder,
        int             bgflag,
        int             font,
        String          textData,
        String          fname,
        String          lname,
        String          iname,
        int             native_index
    )
    {
        DLText          text;
        DLSelectable    dls;

        text = new DLText ();

        dls = getSelectable (selectable_index);
        if (dls == null) {
            return;
        }
        text.selectableObject = dls;
        dls.isSelected = true;

        text.xLocation = xp;
        text.yLocation = yp;
        text.anchor = anchor;

        text.thickness = thick;
        text.backgroundBorderThickness = bgthick;
        text.angle = angle;
        text.size = size;
        text.xOffset = xoff;
        text.yOffset = yoff;

        text.color =
            new Color (red_text, green_text, blue_text, alpha_text);
        if (alpha_fill > 0) {
            text.fillColor =
            new Color (red_fill, green_fill, blue_fill, alpha_fill);
        }
        if (alpha_bgfill > 0) {
            text.backgroundFillColor =
            new Color (red_bgfill, green_bgfill, blue_bgfill, alpha_bgfill);
        }
        if (alpha_bgborder > 0) {
            text.backgroundBorderColor =
            new Color (red_bgborder, green_bgborder, blue_bgborder, alpha_bgborder);
        }

        text.backgroundFlag = bgflag;
        text.fontNumber = font;

        text.textData = textData;

        text.frameName = fname;
        text.layerName = lname;
        text.itemName = iname;

        text.nativeIndex = native_index;

        if (dls.textList == null) {
            dls.textList = new ArrayList<DLText> (10);
        }
        dls.textList.add (text);

        selectionAvailable = true;

        return;

    }



/*--------------------------------------------------------------------------*/

    private void addSelectedSymb (
        int             selectable_index,
        double          xp,
        double          yp,
        int             number,
        double          thick,
        double          angle,
        double          size,
        int             red_symb,
        int             green_symb,
        int             blue_symb,
        int             alpha_symb,
        String          fname,
        String          lname,
        String          iname,
        String          sname,
        int             native_index
    )
    {
        DLSymb          symb;
        DLSelectable    dls;

        symb = new DLSymb ();

        dls = getSelectable (selectable_index);
        if (dls == null) {
            return;
        }
        symb.selectableObject = dls;
        dls.isSelected = true;

        symb.xLocation = xp;
        symb.yLocation = yp;
        symb.number = number;

        symb.thickness = thick;
        symb.angle = angle;
        symb.size = size;

        symb.color =
            new Color (red_symb, green_symb, blue_symb, alpha_symb);

        symb.frameName = fname;
        symb.layerName = lname;
        symb.itemName = iname;
        symb.surfaceName = sname;

        symb.nativeIndex = native_index;

        if (dls.symbList == null) {
            dls.symbList = new ArrayList<DLSymb> (10);
        }
        dls.symbList.add (symb);

        selectionAvailable = true;

        return;

    }

/*--------------------------------------------------------------------------*/

    private void addSelectedAxis (
        int         labelFlag,
        int         tickFlag,
        int         tickDirection,
        String      caption,
        double      majorInterval,
        int         lineRed,
        int         lineGreen,
        int         lineBlue,
        int         textRed,
        int         textGreen,
        int         textBlue,
        double      lineThickness,
        double      textSize,
        double      textThickness,
        int         textFont,
        double      x1,
        double      y1,
        double      x2,
        double      y2,
        int         labelDir,
        int         selectableIndex,
        double      firstAxisValue,
        double      lastAxisValue,
        String      fname,
        String      lname,
        String      iname,
        int         nativeIndex
    )
    {
        DLAxis          axis;
        DLSelectable    dls;

        axis = new DLAxis ();

        dls = getSelectable (selectableIndex);
        if (dls == null) {
            return;
        }
        axis.selectableObject = dls;
        dls.isSelected = true;

        axis.labelFlag = labelFlag;
        axis.tickFlag = tickFlag;
        axis.tickDirection = tickDirection;
        axis.caption = caption;
        axis.majorInterval = majorInterval;
        axis.lineColor =
            new Color (lineRed, lineGreen, lineBlue);
        axis.textColor =
            new Color (textRed, textGreen, textBlue);
        axis.lineThickness = lineThickness;
        axis.textSize = textSize;
        axis.textThickness = textThickness;
        axis.textFont = textFont;
        axis.x1 = x1;
        axis.y1 = y1;
        axis.x2 = x2;
        axis.y2 = y2;
        axis.labelDirection = labelDir;
        axis.firstAxisValue = firstAxisValue;
        axis.lastAxisValue = lastAxisValue;

        axis.frameName = fname;
        axis.layerName = lname;
        axis.itemName = iname;

        axis.nativeIndex = nativeIndex;

        if (dls.axisList == null) {
            dls.axisList = new ArrayList<DLAxis> (10);
        }
        dls.axisList.add (axis);

        selectionAvailable = true;

        return;

    }

    private void printSelectedItems ()
    {
        DLSelectable    dls;
        if (selectableList != null) {
            int n = selectableList.size ();
            System.out.println (" ");
            for (int i=0; i<n; i++) {
                dls = selectableList.get (i);
                if (dls.isSelected) {
                    System.out.println ("native index = " + dls.nativeIndex);
                    System.out.flush ();
                }
            }
        }
    }


/*-----------------------------------------------------------------------------*/

    private JDLEditListener  jdlEditor = null;

    public void setEditor (JDLEditListener editor)
    {
        jdlEditor = editor;

    /*
     * Disable the selection of other objects while an editor
     * is being used.
     */
        if (jdlEditor == null) {
            dlPanel.setSelectionAllowed (true);
        }
        else {
            dlPanel.setSelectionAllowed (false);
        }
    }

    /**
     * Set the Display List to selection mode.
     */
    public void setSelectionMode() {
      this.setEditor(null);
      dlPanel.setSelectCursor();
    }

    boolean isEditorActive() {
      return (jdlEditor != null);
    }




    public boolean
    processMouseEvent (MouseEvent e,
                       int type,
                       int btn1,
                       int btn2,
                       int btn3)
    {

        if (jdlEditor == null) {
            return false;
        }

        if (type == MouseEvent.MOUSE_CLICKED) {
            jdlEditor.mouseClicked (e);
            return true;
        }

        if (type == MouseEvent.MOUSE_MOVED) {
            jdlEditor.mouseMoved (e);
            return true;
        }

        if (type == MouseEvent.MOUSE_DRAGGED) {
            jdlEditor.mouseDragged (e);
            return true;
        }

        if (type == MouseEvent.MOUSE_PRESSED) {
            jdlEditor.mousePressed (e);
            return true;
        }

        if (type == MouseEvent.MOUSE_RELEASED) {
            jdlEditor.mouseReleased (e);
            return true;
        }

        return false;

    }

/*-----------------------------------------------------------*/

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


/*---------------------------------------------------------------*/

/*
 * Group of methods to handle right mouse button popup menu.
 */

    int            defaultPopupButtonMask = 0xffffffff;
    JPopupMenu     defPopup;

    JMenuItem      unselectPopupItem,
                   deletePopupItem,
                   hidePopupItem,
                   unhidePopupItem,
                   propertiesPopupItem;

    boolean        hiddenFlag = false;

    protected void unselectAll ()
    {
        sendNativeCommand (
            DLConst.GTX_UNSELECT_ALL
        );
        needsNativeSelectRedraw = 1;
        needsNativeRedraw = 0;
        dlPanel.repaint ();
    }

    protected void deleteSelected ()
    {
        sendNativeCommand (
            DLConst.GTX_DELETE_SELECTED
        );
        needsNativeSelectRedraw = 1;
        needsNativeRedraw = 1;
        dlPanel.repaint ();
    }

    protected void hideSelected ()
    {
        sendNativeCommand (
            DLConst.GTX_HIDE_SELECTED
        );
        needsNativeSelectRedraw = 1;
        needsNativeRedraw = 1;
        dlPanel.repaint ();
        hiddenFlag = true;
    }

    protected void unhideAll ()
    {
        sendNativeCommand (
            DLConst.GTX_UNHIDE_ALL
        );
        needsNativeSelectRedraw = 1;
        needsNativeRedraw = 1;
        dlPanel.repaint ();
        hiddenFlag = false;
    }

    private void createDefaultPopupButtons ()
    {
        unselectPopupItem = new JMenuItem ("Unselect");
        unselectPopupItem.addActionListener (
            new ActionListener() {
                public void actionPerformed(ActionEvent ae){
                    JDisplayListBase.this.unselectAll ();
                }
            }
        );

        deletePopupItem = new JMenuItem ("Delete");
        deletePopupItem.addActionListener (
            new ActionListener() {
                public void actionPerformed(ActionEvent ae){
                    JDisplayListBase.this.deleteSelected ();
                }
            }
        );

        hidePopupItem = new JMenuItem ("Hide");
        hidePopupItem.addActionListener (
            new ActionListener() {
                public void actionPerformed(ActionEvent ae){
                    JDisplayListBase.this.hideSelected ();
                }
            }
        );

        unhidePopupItem = new JMenuItem ("Show");
        unhidePopupItem.addActionListener (
            new ActionListener() {
                public void actionPerformed(ActionEvent ae){
                    JDisplayListBase.this.unhideAll ();
                }
            }
        );

        propertiesPopupItem = new JMenuItem ("Properties...");
        propertiesPopupItem.addActionListener (
            new ActionListener() {
                public void actionPerformed(ActionEvent ae){
                }
            }
        );

    }


/*-------------------------------------------------------------------------*/

    ArrayList<DLSelectListener>  selectListenerList = 
		new ArrayList<DLSelectListener> ();

/**
 Add a listener to monitor selection changes in the display.  The display
 will change the color and redraw the selected objects. The application can
 change its state in response to selection changes via this listener.
 If the specified listener is already in the list, no duplicate will be added.
 @param sl Object which implements the {@link DLSelectListener} interface.
*/
    public void addSelectListener (DLSelectListener sl)
    {
        int index = selectListenerList.indexOf (sl);
        if (index >= 0) {
            return;
        }

        selectListenerList.add (sl);

        return;
    }

/**
 Remove the specified select listener from the list.  If the specified listener is not
 in the list, nothing is done.
 @param sl Object which implements the {@link DLSelectListener} interface.
*/
/*
    public void removeSelectListener (DLSelectListener sl)
    {
        int index = selectListenerList.indexOf (sl);
        if (index < 0) {
            return;
        }

        selectListenerList.remove (index);

        return;
    }
*/

/**
 Remove all the select listeners from the display list.
*/
    public void removeAllSelectListeners ()
    {
        selectListenerList.clear ();
    }

    void callSelectListeners ()
    {

        if (selectableList == null) {
            return;
        }

        if (selectableList.size () < 1) {
            return;
        }

    /*
     * Create and populate a select info object.
     */
        DLSelectionInfo info =
            new DLSelectionInfo ();
        info.xPick = convertedX;
        info.yPick = convertedY;
        info.zPick = convertedZ;
        info.xScreen = screenX;
        info.yScreen = screenY;
        info.selectableList = selectableList;

        info.dump ();

        int size = selectListenerList.size();
        if (size < 1) {
            return;
        }

        DLSelectListener sl;

        for (int i=0; i<size; i++) {
            sl = selectListenerList.get(i);
            if (sl != null) {
                sl.selectionChanged (info);
            }
        }

        return;

    }

/*-------------------------------------------------------------------------*/

    ArrayList<DLRightClickListener> rightClickListenerList = 
		new ArrayList<DLRightClickListener> ();

/**
 Add a listener to process a right mouse button click in the display.  The
 customary usage id to show a popup menu when the right mouse button is clicked.
 If the specified listener is already in the list, no duplicate will be added.
 @param sl Object which implements the {@link DLRightClickListener} interface.
*/
    public void addRightClickListener (DLRightClickListener sl)
    {
        if (sl == null)
          return;
        int index = rightClickListenerList.indexOf (sl);
        if (index >= 0) {
            return;
        }

        rightClickListenerList.add (sl);

        return;
    }

/**
 Remove the specified right click listener from the list.  If the specified listener is not
 in the list, nothing is done.
 @param sl Object which implements the {@link DLRightClickListener} interface.
*/
    public void removeRightClickListener (DLRightClickListener sl)
    {
        int index = rightClickListenerList.indexOf (sl);
        if (index < 0) {
            return;
        }

        rightClickListenerList.remove (index);

        return;
    }

/**
 Remove all the right click listeners from the display list.
*/
    public void removeAllRightClickListeners ()
    {
        rightClickListenerList.clear ();
    }

    private boolean callRightClickListeners ()
    {

        int size = rightClickListenerList.size();
        if (size < 1) {
            return false;
        }

    /*
     * Create and populate a right click info object.
     */
        DLRightClickInfo info =
            new DLRightClickInfo ();
        info.xPick = convertedX;
        info.yPick = convertedY;
        info.zPick = convertedZ;
        info.xScreen = screenX;
        info.yScreen = screenY;
        info.selectableList = selectableList;

        DLRightClickListener sl;

        boolean bval = true;
        for (int i=0; i<size; i++) {
            sl = rightClickListenerList.get(i);
            if (sl != null) {
                boolean b = sl.processRightClick (info);
                if (b == false) {
                    bval = false;
                }
            }
        }

        return bval;

    }

/*-------------------------------------------------------------------------*/

    double convertedX = -1.e30;
    double convertedY = -1.e30;
    double convertedZ = -1.e30;

    int    screenX = 0;
    int    screenY = 0;

    void processRightClick (MouseEvent e)
    {
        NativePrim.Frame    frame;

        if (selectableList == null) {
            return;
        }

        int size = selectableList.size ();
        if (size < 1) {
            return;
        }

        int ix = e.getX();
        int iy = e.getY();

        screenX = ix;
        screenY = iy;

        frame = findEditFrame (ix, iy);
        if (frame == null) {
            return;
        }

        convertedX = 1.e30;
        convertedY = 1.e30;
        convertedZ = 1.e30;

        int threadid = Thread.currentThread().hashCode();

        convertToFrame (nativeDlistID,
                        threadid,
                        frame.frame_num,
                        ix, iy);

        if (convertedX > 1.e20  ||
            convertedY > 1.e20) {
            return;
        }

        boolean doPopup;
        doPopup = callRightClickListeners ();

        if (doPopup) {
            createDefaultPopup (e.getX(), e.getY());
        }

        return;

    }

/*-------------------------------------------------------------------------*/

    private void setConvertedXYZ (
        double x,
        double y,
        double z
    )
    {
        convertedX = x;
        convertedY = y;
        convertedZ = z;
    }


/*-------------------------------------------------------------------------*/

    void createDefaultPopup (int ix, int iy)
    {

        int nsel = checkSelectedPrims ();

        boolean selected = false;

        if (nsel > 0) selected = true;

        if (selected == false  &&  hiddenFlag == false) {
            return;
        }

        defPopup = new JPopupMenu ();
        JPopupMenu pop = defPopup;

        int bm = defaultPopupButtonMask;

        unselectPopupItem.setEnabled (false);
        deletePopupItem.setEnabled (false);
        hidePopupItem.setEnabled (false);
        unhidePopupItem.setEnabled (false);
        propertiesPopupItem.setEnabled (false);

        if (selected) {
            unselectPopupItem.setEnabled (true);
            deletePopupItem.setEnabled (true);
            hidePopupItem.setEnabled (true);
            if (nsel == 1) {
                propertiesPopupItem.setEnabled (true);
            }
        }

        if (hiddenFlag) {
            unhidePopupItem.setEnabled (true);
        }

        if ((bm & POPUP_UNSELECT_BUTTON_MASK) != 0) {
            pop.add (unselectPopupItem);
        }

        if ((bm & POPUP_DELETE_BUTTON_MASK) != 0) {
            pop.add (deletePopupItem);
        }

        if ((bm & POPUP_HIDE_BUTTON_MASK) != 0) {
            pop.add (hidePopupItem);
        }

        if ((bm & POPUP_UNHIDE_BUTTON_MASK) != 0) {
            pop.add (unhidePopupItem);
        }

        if ((bm & POPUP_PROPERTIES_BUTTON_MASK) != 0) {
            pop.addSeparator ();
            pop.add (propertiesPopupItem);
        }

        pop.show (dlPanel, ix, iy);

        return;
    }

/*-----------------------------------------------------------------------------*/

 /*
  * Keep track of font names, sizes and styles in a list of
  * objects.  The font number sent to the native side for
  * using a java font will be 1000 plus the index in this
  * list of the font to use.
  */
    private static class FontDef
    {
        String    fontName;
        int       size72;
        int       style;
        Font      font;
    }

    private ArrayList<FontDef> fontList = new ArrayList<FontDef> (10);

    protected void clearFontList ()
    {
        fontList.clear ();
    }

/*----------------------------------------------------------------------------*/

 /**
  * Return the index in the font list for an entry that
  * matches the specified name and style parameters.
  * If no entry matches, -1 is returned.
  */
    protected int getExistingFontNumber (
        String fname,
        int    style
    )
    {
        FontDef  fdef;
        int      i, nfont, istat;

        nfont = fontList.size();

        for (i=0; i<nfont; i++) {
            fdef = fontList.get(i);
            if (fdef.style != style) continue;
            istat = fname.compareTo (fdef.fontName);
            if (istat == 0) {
                return i;
            }
        }

        return -1;

    }

/*----------------------------------------------------------------------------*/

 /**
  * Return the index in the font list for an entry that
  * matches the specified name, size and style parameters.
  * If no entry matches, -1 is returned.  The size is in
  * inches.
  */
    protected int getExistingFontNumber (
        String fname,
        double size,
        int    style
    )
    {
        FontDef  fdef;
        int      i, nfont, istat, isize;

        isize = (int)(size * 72.0 + .5);

        nfont = fontList.size();

        for (i=0; i<nfont; i++) {
            fdef = fontList.get(i);
            if (fdef.size72 != isize) continue;
            if (fdef.style != style) continue;
            istat = fname.compareTo (fdef.fontName);
            if (istat == 0) {
                return i;
            }
        }

        return -1;

    }

/*----------------------------------------------------------------------------*/

    protected int addNewFont (
        String fname,
        int    style)
    {
        Font font = FontUtils.createFont (fname, .1, style);

        int size = fontList.size();
        FontDef fdef = new FontDef ();
        fdef.fontName = fname;
        fdef.style = style;
        fdef.size72 = 7;
        fdef.font = font;
        fontList.add (fdef);
        return size;
    }

/*----------------------------------------------------------------------------*/

  /*
   * Return an existing font index if a match exists.  If
   * no match exists, create a new entry in the font list.
   * If the new font cannot be created, -1 is returned.
   */
    protected int getFontNumber (
        String fname,
        double size,
        int    style
    )
    {
        FontDef  fdef;
        int      i, nfont, istat, isize;

        isize = (int)(size * 72.0 + .5);

        nfont = fontList.size();

        for (i=0; i<nfont; i++) {
            fdef = fontList.get(i);
            if (fdef.size72 != isize) continue;
            if (fdef.style != style) continue;
            istat = fname.compareTo (fdef.fontName);
            if (istat == 0) {
                return i;
            }
        }

        Font font = FontUtils.createFont (
            fname,
            size,
            style);

        if (font == null) {
            return -1;
        }

        fdef = new FontDef ();

        fdef.fontName = fname;
        fdef.size72 = isize;
        fdef.style = style;
        fdef.font = font;

        fontList.add (fdef);

        return nfont;

    }

/*-----------------------------------------------------------------------*/

  /*
   * Return a font for the specified index in the font list.
   * If the specified index doesn't exist in the list, a null
   * font object is returned.
   */
    protected Font getFontForIndex (int index)
    {
        int       nfont;

        if (index >= 1000) index -= 1000;

        nfont = fontList.size();

        if (index < 0  ||  index >= nfont) {
            return null;
        }

        FontDef fdef = fontList.get(index);

        if (fdef == null) {
            return null;
        }

        return fdef.font;
    }

/*-------------------------------------------------------------------------*/

  /*
   * If the size is the same as the size fo the indexed font, return
   * the indexed font.  If the size is different, return a derived
   * font.  The derived font is not put into the font list.  If the
   * index is illegal, null is returned.
   */
    protected Font deriveFontForIndex (int index, double size)
    {
        int       nfont;


        if (index >= 1000) index -= 1000;

        nfont = fontList.size();

        if (index < 0  ||  index >= nfont) {
            return null;
        }

        FontDef fdef = fontList.get(index);

        if (fdef == null) {
            return null;
        }

        Font  font1 = fdef.font;

        int isize = (int)(size * 72.0 + .5);

        if (isize == fdef.size72) {
            return font1;
        }

        Font f2 = FontUtils.deriveFont (
            font1,
            size);

        return f2;

    }

/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/

  /*
   * If all else fails, you can get the default font at the specified
   * size using this function.  If the size is less than or equal to
   * .02 inches, which would be unreadable, a default size of .10 is
   * used to create the font.
   */
    protected Font getDefaultFont (double size)
    {
        return FontUtils.getDefaultFont (size);
    }


/*-------------------------------------------------------------------------*/

  /**
  This method is only called from the native code to give it dimensions
  of text strings.  The boundsOut array is allocated on the native side
  and is filled in here.
  */
    private void getTextBounds (
        String       text,
        int          fontNumber,
        double       fontSize,
        double[]     boundsOut)
    {

        boundsOut[0] = 0.0;
        boundsOut[1] = 0.0;
        boundsOut[2] = 0.0;

        Font font = deriveFontForIndex (fontNumber, fontSize);
        if (font == null) {
            return;
        }

    /*
     * An intermediate double array must be used since the output
     * array is allocated on the native side.  The output array
     * can only be populated.  The actual output array pointer
     * cannot be changed.
     */
        double[] db = FontUtils.getTextBounds (text, font);

        boundsOut[0] = db[0];
        boundsOut[1] = db[1];
        boundsOut[2] = db[2];

        return;

    }


/*-----------------------------------------------------------------------*/

/*
 * Zoom/Pan listener support.
 */
    ArrayList<DLZoomPanListener>  zpList = 
		new ArrayList<DLZoomPanListener> (2);
    String        zoomFrameName;
    double        oldX1, oldY1, oldX2, oldY2;
    double        newX1, newY1, newX2, newY2;


  /*
   * This method is called from the native c code.
   */
    private void setZoomPanData (
        String      fname,
        double      oX1,
        double      oY1,
        double      oX2,
        double      oY2,
        double      nX1,
        double      nY1,
        double      nX2,
        double      nY2
    )
    {
        oldX1 = oX1;
        oldY1 = oY1;
        oldX2 = oX2;
        oldY2 = oY2;
        newX1 = nX1;
        newY1 = nY1;
        newX2 = nX2;
        newY2 = nY2;

        zoomFrameName = fname;

        callZPList ();

        return;

    }

    private void callZPList ()
    {
        int       size, i;
        DLZoomPanListener     zpl;

        size = zpList.size ();
        for (i=0; i<size; i++) {
            zpl = zpList.get (i);
            if (zpl == null) {
                continue;
            }
            zpl.zoomPanChanged
                (zoomFrameName,
                 oldX1, oldY1, oldX2, oldY2,
                 newX1, newY1, newX2, newY2);
        }

        return;
    }

/*-----------------------------------------------------------------------------*/

/**
 Add a single line comment to the graphics log file.  This is useful to see
 what actually happens in the log file between spots in the java code.
 This is intended for debugging only.
*/
    public void addLogComment (String msg)
    {

        if (msg == null) {
            return;
        }

        int[] ilist = new int[10];
        double[] ddata = new double[10];

        sendNativeCommand (
            GTX_LOG_COMMENT,
            ilist,
            msg,
            ddata);

        return;

    }

};  /* end of class JDisplayListBase */

