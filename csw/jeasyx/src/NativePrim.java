
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
 *  This file defines classes used for storage of native graphic
 *  primitives on the java side of the jni interface.  All of the
 *  variables in the classes are default access, which means any
 *  other class in the package can directly access them.  The
 *  classes themselves have the same accessibility.  There is no
 *  reason for anything outside of this package to access any
 *  of these classes directly.
 *
 *  @author Glenn Pinkerton
 *
*/
package csw.jeasyx.src;

import java.awt.Font;
import java.awt.Shape;
import java.awt.geom.Rectangle2D;

/*--------------------------------------------------------------------------*/

  /**
   * This class is only a container for various types of entites that
   * need to be drawn.  It cannot be instantiated and it is only accessible
   * in the package that owns it.
   */
    abstract class NativePrim {

/*--------------------------------------------------------------------------*/

      /*
       * Define the native frame class.
       */
        static class Frame {
            float      x1,
                       y1,
                       x2,
                       y2;
            int        borderflag,
                       clipflag,
                       scaleable,
                       scale_to_attach_frame,
                       frame_num;
            double     xscale,
                       yscale,
                       fx1,
                       fy1;
            Rectangle2D.Float  clip_shape;
            String     frame_name;
        }

/*--------------------------------------------------------------------------*/

      /*
       * Define the native fill class.
       */
        static class Fill {
            float[]    xy;
            int        npts,
                       red,
                       green,
                       blue,
                       alpha,
                       pattern,
                       frame_num,
                       image,
                       selectable;
            float      thickness;
            float      patScale;
        }

/*--------------------------------------------------------------------------*/

      /*
       * Define the native line class.
       */
        static class Line {
            float[]    xy;
            int        npts,
                       red,
                       green,
                       blue,
                       alpha,
                       pattern,
                       frame_num,
                       image,
                       selectable;
            float      thickness;
        }

/*--------------------------------------------------------------------------*/

      /*
       * Define the native text class.
       */
        static class Text {
            float      x,
                       y;
            float      angle,
                       size;
            String     text;
            int        red,
                       green,
                       blue,
                       alpha,
                       frame_num,
                       image,
                       selectable;
            Font       baseFont;
        }

/*--------------------------------------------------------------------------*/

      /*
       * Define the native arc class.
       */
        static class Arc {
            float      x,
                       y,
                       r1,
                       r2,
                       ang1,
                       ang2,
                       thickness,
                       angle;
            int        red,
                       green,
                       blue,
                       alpha,
                       closure,
                       frame_num,
                       image,
                       selectable;
        }

/*--------------------------------------------------------------------------*/

      /*
       * Define the native filled arc class.
       */
        static class FilledArc {
            float      x,
                       y,
                       r1,
                       r2,
                       ang1,
                       ang2,
                       angle;
            int        red,
                       green,
                       blue,
                       alpha,
                       closure,
                       pattern,
                       frame_num,
                       image,
                       selectable;
        }

/*--------------------------------------------------------------------------*/

      /*
       * Define the native image class.
       */
        static class Image {
            float      x1,
                       y1,
                       x2,
                       y2;
            int        ncol,
                       nrow,
                       frame_num,
                       id,
                       selectable;
            byte[]     red,
                       green,
                       blue,
                       transparency;
            boolean    hasLines;
        }

/*--------------------------------------------------------------------------*/

      /*
       * Define direct java shape class.
       */
        static class DirectShape {
            Shape      shape;
            int        red,
                       green,
                       blue;
            int        fill_flag;
            int        fnum = -1;
        }


    }  // end of enclosing NativePrim class.
