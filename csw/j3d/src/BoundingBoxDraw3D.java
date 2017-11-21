
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.awt.Color;
import java.awt.Font;
import java.util.ArrayList;

import csw.jsurfaceworks.src.JSResult2D;
import csw.jsurfaceworks.src.JSurfaceWorks;

/**
 This class stores information needed to draw a trimesh in the 3d
 display.  The j3d package is the only code that needs access to this
 code so it is declared with package scope.
 */
class BoundingBoxDraw3D  implements TextPositionFilter
{
    private double    prettyXmin,
                      prettyYmin,
                      prettyZmin,
                      prettyXmax,
                      prettyYmax,
                      prettyZmax;
    private double    prettyZtick = 1.e30,
                      prettyXtick = 1.e30,
                      prettyYtick = 1.e30;
    private double    xyMpu = 1.0,
                      zMpu = 1.0;

    private Scaler    scaler;

    private Font      labelsFont;
    private Font      numbersFont;

/*--------------------------------------------------------------------------------*/

    private void pmsg (String str)
    {
        //System.out.println (str);
    }

    private void pmsg ()
    {
        //System.out.println ();
    }

    private void pmsg2 (String str)
    {
        //System.out.println (str);
    }

    private void pmsg2 ()
    {
        //System.out.println ();
    }

/*--------------------------------------------------------------------------------*/

  /**
  This constructor creates a bounding box near the specified bounds.
  The bounds are expanded slightly to make a box with "pretty" numbers.
   */
    BoundingBoxDraw3D (double xunits, double zunits,
                       double xmin, double ymin, double zmin,
                       double xmax, double ymax, double zmax,
                       Font labelsFont, Font numbersFont)
    {
        setXyMpu (xunits);
        setZMpu (zunits);
        calcPrettyLimits  (xmin, ymin, zmin,
                           xmax, ymax, zmax);
        this.labelsFont = labelsFont;
        this.numbersFont = numbersFont;
        if (labelsFont == null)
          labelsFont = new Font("SansSerif", Font.PLAIN, 18);
        if (numbersFont == null)
          numbersFont = new Font("SansSerif", Font.PLAIN, 14);
    }

/*--------------------------------------------------------------------------------*/

    void setScaler (Scaler s)
    {
        scaler = s;
    }

/*--------------------------------------------------------------------------------*/

    void setXyMpu (double d)
    {
        xyMpu = d;
    }

    double getXyMpu ()
    {
        return xyMpu;
    }

/*--------------------------------------------------------------------------------*/

    void setZMpu (double d)
    {
        zMpu = d;
    }

    double getZMpu ()
    {
        return zMpu;
    }
/*--------------------------------------------------------------------------------*/
  void setNumbersFont(Font font) {
    if (font != null)
      this.numbersFont = font;
  }

  void setLabelsFont(Font font) {
    if (font != null)
      this.labelsFont = font;
  }

/*--------------------------------------------------------------------------------*/

    /*
     * Calculate rounded numbers for the cube surrounding
     * the model.  If the pretty number flag is off or
     * if the model limits are not good, the "even" numbers
     * revert to simple divisors of the actual limits.
     */
    private void calcPrettyLimits (double xmin,
                                   double ymin,
                                   double zmin,
                                   double xmax,
                                   double ymax,
                                   double zmax)
    {
        int         upl;
        double            xt, yt, zt, at,
                          xlog, ylog, zlog,
                          dx, dy, dz, da;
        double            xtick, ytick, ztick;
        int               nx, ny, nz, itmp;
        int               izlog, ixlog, iylog;

        xtick = (xmax - xmin) / 8.0;
        ytick = (ymax - ymin) / 8.0;
        ztick = 1.e30;
        if (zmin <= zmax) {
            ztick = (zmax - zmin) / 8.0;
        }

    /*
     * The limits are reasonable enough to calculate pretty
     * numbers.  The attempt is to get 6 to 10 tick marks
     * at rounded number intervals per axis.
     */

    /*
     * Initialize z and attribute in case something is wrong
     * with them.  X and Y are ok if we get this far.
     */
        prettyZtick = 1.e30;
        prettyZmin = 1.e30;
        prettyZmax = -1.e30;

        zlog = 1000.0;
        izlog = 1000;

    /*
     * Temporarily convert limits to display units.
     */
        xmin /= xyMpu;
        xmax /= xyMpu;
        ymin /= xyMpu;
        ymax /= xyMpu;
        zmin /= zMpu;
        zmax /= zMpu;

    /*
     * get range of each axis.
     */
        xt = xmax - xmin;
        yt = ymax - ymin;
        zt = zmax - zmin;

    /*
     * round off range to next order of magnitude lower.
     */
        double log10 = Math.log (10.0);
        xlog = Math.log(xt) / log10;
        ylog = Math.log(yt) / log10;
        if (zt > 0.0) {
            zlog = Math.log(zt) / log10;
        }

        ixlog = (int)xlog;
        iylog = (int)ylog;
        if (zt > 0.0) {
            izlog = (int)zlog;
        }

        if (xlog < ixlog) {
            ixlog--;
        }
        if (ylog < iylog) {
            iylog--;
        }
        if (zt > 0.0) {
            if (izlog < zlog) {
                izlog--;
            }
        }

        xlog = ixlog;
        ylog = iylog;
        if (zt > 0.0) {
            zlog = izlog;
        }

    /*
     * The first tick estimate is the next lower
     * order of magnitude of the range.
     */
        dx = Math.pow (10.0, xlog);
        nx = (int)(xt / dx);
        dy = Math.pow (10.0, ylog);
        ny = (int)(yt / dy);
        if (zt > 0.0) {
            dz = Math.pow (10.0, zlog);
            nz = (int)(zt / dz);
        }
        else {
            dz = 1.e30;
            nz = -1000;
        }

    /*
     * Adjust x so 6 to 12 ticks are present.
     */
        if (nx < 2) {
            dx /= 5.0;
        }
        else if (nx < 3) {
            dx /= 4.0;
        }
        else if (nx < 4) {
            dx /= 2.5;
        }
        else if (nx < 6) {
            dx /= 2.0;
        }
        else if (nx > 50) {
            dx *= 10.0;
        }
        else if (nx > 25) {
            dx *= 5.0;
        }
        else if (nx > 20) {
            dx *= 4.0;
        }
        else if (nx > 15) {
            dx *= 2.5;
        }
        else if (nx > 11) {
            dx *= 2.0;
        }

    /*
     * Adjust y so 6 to 12 ticks are done.
     */
        if (ny < 2) {
            dy /= 5.0;
        }
        else if (ny < 3) {
            dy /= 4.0;
        }
        else if (ny < 4) {
            dy /= 2.5;
        }
        else if (ny < 6) {
            dy /= 2.0;
        }
        else if (ny > 50) {
            dy *= 10.0;
        }
        else if (ny > 25) {
            dy *= 5.0;
        }
        else if (ny > 20) {
            dy *= 4.0;
        }
        else if (ny > 15) {
            dy *= 2.5;
        }
        else if (ny > 11) {
            dy *= 2.0;
        }

    /*
     * Adjust z so 6 to 12 ticks are done.
     */
        if (nz > -999) {
            if (nz < 2) {
                dz /= 5.0;
            }
            else if (nz < 3) {
                dz /= 4.0;
            }
            else if (nz < 4) {
                dz /= 2.5;
            }
            else if (nz < 6) {
                dz /= 2.0;
            }
            else if (nz > 50) {
                dz *= 10.0;
            }
            else if (nz > 25) {
                dz *= 5.0;
            }
            else if (nz > 20) {
                dz *= 4.0;
            }
            else if (nz > 15) {
                dz *= 2.5;
            }
            else if (nz > 11) {
                dz *= 2.0;
            }
        }

    /*
     * Assign to the object's member values, making sure
     * that the pretty limits are at or outside the actual
     * data limits.
     */
        prettyXtick = dx;
        itmp = (int)(xmin / dx);
        prettyXmin = itmp * dx;
        while (prettyXmin > xmin) {
            prettyXmin -= dx;
        }
        itmp = (int)(xmax / dx);
        prettyXmax = itmp * dx;
        while (prettyXmax < xmax) {
            prettyXmax += dx;
        }

        prettyYtick = dy;
        itmp = (int)(ymin / dy);
        prettyYmin = itmp * dy;
        while (prettyYmin > ymin) {
            prettyYmin -= dy;
        }
        itmp = (int)(ymax / dy);
        prettyYmax = itmp * dy;
        while (prettyYmax < ymax) {
            prettyYmax += dy;
        }

        if (nz > -999) {
            prettyZtick = dz;
            itmp = (int)(zmin / dz);
            prettyZmin = itmp * dz;
            while (prettyZmin > zmin) {
                prettyZmin -= dz;
            }
            itmp = (int)(zmax / dz);
            prettyZmax = itmp * dz;
            while (prettyZmax < zmax) {
                prettyZmax += dz;
            }
        }

    /*
     * Convert the limits and the pretty limits back to meters.
     */
        xmin *= xyMpu;
        xmax *= xyMpu;
        ymin *= xyMpu;
        ymax *= xyMpu;
        zmin *= zMpu;
        zmax *= zMpu;

        prettyXmin *= xyMpu;
        prettyXmax *= xyMpu;
        prettyXtick *= xyMpu;

        prettyYmin *= xyMpu;
        prettyYmax *= xyMpu;
        prettyYtick *= xyMpu;

        prettyZmin *= zMpu;
        prettyZmax *= zMpu;
        prettyZtick *= zMpu;

        return;

    } /* end of CalcPrettyLimits method */

/*-----------------------------------------------------------------------------*/

    double getPrettyXmin ()
    {
        return prettyXmin;
    }

    double getPrettyYmin ()
    {
        return prettyYmin;
    }

    double getPrettyZmin ()
    {
        return prettyZmin;
    }

    double getPrettyXmax ()
    {
        return prettyXmax;
    }

    double getPrettyYmax ()
    {
        return prettyYmax;
    }

    double getPrettyZmax ()
    {
        return prettyZmax;
    }


/*-----------------------------------------------------------------------------*/

    private Color     xColor = Color.green;
    private Color     yColor = Color.red;
    private Color     zColor = Color.blue;

    void setXColor (Color c)
    {
        if (c == null) return;
        xColor = c;
    }

    void setYColor (Color c)
    {
        if (c == null) return;
        yColor = c;
    }

    void setZColor (Color c)
    {
        if (c == null) return;
        zColor = c;
    }

    Color getXColor ()
    {
        return xColor;
    }

    Color getYColor ()
    {
        return yColor;
    }

    Color getZColor ()
    {
        return zColor;
    }


/*-----------------------------------------------------------------------------*/

    void scaleBox (Color bgColor,
                   JGL jgl)
    {


    /*
     * Draw the bounding box as line segments.
     */
        JPoint3D   p, pt1, pt2;

        // JNEWP
        float[] rgb = bgColor.getRGBColorComponents(null);

    /*
     * If the background is dark, draw border in white.  If it is
     * light, draw border in black.
     */
        double red;
        double green;
        double blue;
        double rgbtot = rgb[0] + rgb[1] + rgb[2];
        if (rgbtot > 2.0) {
            red = 0.0;
            green = 0.0;
            blue = 0.0;
        }
        else {
            red = 1.0;
            green = 1.0;
            blue = 1.0;
        }

        jgl.SetDrawLineWidth (1.5);
        jgl.SetDrawStyle(JGL.DG_WIREFRAME);
        jgl.SetDrawLighting(0);
        jgl.DGNodeColor(red, green, blue, 1.0);
        jgl.SetDrawCulling(0);

        jgl.SetDrawMode(JGL.DG_LINESTRIP);

    /*
     * Draw the bottom.
     */
        jgl.StartDrawMode();
        p = scalePoint (prettyXmin,
                        prettyYmin,
                        prettyZmin);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmax,
                        prettyYmin,
                        prettyZmin);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmax,
                        prettyYmax,
                        prettyZmin);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmin,
                        prettyYmax,
                        prettyZmin);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmin,
                        prettyYmin,
                        prettyZmin);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        jgl.StopDrawMode ();

    /*
     * Draw the top.
     */
        jgl.StartDrawMode();
        p = scalePoint (prettyXmin,
                        prettyYmin,
                        prettyZmax);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmax,
                        prettyYmin,
                        prettyZmax);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmax,
                        prettyYmax,
                        prettyZmax);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmin,
                        prettyYmax,
                        prettyZmax);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmin,
                        prettyYmin,
                        prettyZmax);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        jgl.StopDrawMode ();

    /*
     * Draw the vertical edges.
     */
        jgl.SetDrawMode(JGL.DG_LINES);
        jgl.StartDrawMode();
        p = scalePoint (prettyXmin,
                        prettyYmin,
                        prettyZmin);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmin,
                        prettyYmin,
                        prettyZmax);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmax,
                        prettyYmin,
                        prettyZmin);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmax,
                        prettyYmin,
                        prettyZmax);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmax,
                        prettyYmax,
                        prettyZmin);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmax,
                        prettyYmax,
                        prettyZmax);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmin,
                        prettyYmax,
                        prettyZmin);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        p = scalePoint (prettyXmin,
                        prettyYmax,
                        prettyZmax);
        jgl.DGNodeXYZ (p.x, p.y, p.z);
        jgl.StopDrawMode ();

    }


/*--------------------------------------------------------------------------------*/

    /*
     * Draw the grids on the bounding box.  These are
     * drawn as quad objects, with each quad oriented
     * clockwise when observed from a spot outside the
     * box normal to the face containing the grid quad.
     * Thus, looking in from outside always looks at a
     * clockwise polygon.  We ask the graphics system
     * to not draw clockwise polygons, so the bounding
     * box always appears to be open from the observer's
     * viewpoint.
     */
    void scaleSides (Color bgColor,
                     JGL jgl)
    {
        double       xt, yt, zt,
                     x1, y1, z1,
                     x2, y2, z2,
                     dx, dy, dz,
                     xt1, yt1, zt1,
                     xt2, yt2, zt2;
        int          i, j, nx, ny, nz;
        int          n;

        JPoint3D      p;

        x1 = prettyXmin;
        x2 = prettyXmax;
        dx = prettyXtick;
        y1 = prettyYmin;
        y2 = prettyYmax;
        dy = prettyYtick;
        z1 = prettyZmin;
        z2 = prettyZmax;
        dz = prettyZtick;

        nx = (int)((x2 - x1) / dx +.01);
        ny = (int)((y2 - y1) / dy +.01);
        nz = (int)((z2 - z1) / dz +.01);

    /*
     * Draw the side cells in gray unless the background is
     * very close to that color.
     */
        float[] rgb = bgColor.getRGBColorComponents(null);

        double red = .6;
        double green = .6;
        double blue = .6;
        double dr, dg, db;

        double rgbtot = rgb[0] + rgb[1] + rgb[2];

        dr = rgb[0] - red;
        if (dr < 0.0) dr = -dr;
        dg = rgb[1] - green;
        if (dg < 0.0) dg = -dg;
        db = rgb[2] - blue;
        if (db < 0.0) db = -db;

        if (dr + dg + db < 0.5) {
            if (rgbtot < 1.8) {
                red = .8;
                green = .8;
                blue = .8;
            }
            else {
                red = .4;
                green = .4;
                blue = .4;
            }
        }

        jgl.SetDrawLineWidth (1.0);
        jgl.SetDrawMode (JGL.DG_QUADS);
        jgl.SetDrawStyle (JGL.DG_WIREFRAME);
        jgl.SetDrawLighting(0);
        jgl.DGNodeColor (red, green, blue, 1.0);
        jgl.SetDrawCulling(1);

    /*
     * Draw base grid
     */
        zt = z1;
        for (i=0; i<ny; i++) {
            yt1 = y1 + i * dy;
            yt2 = yt1 + dy;
            jgl.StartDrawMode ();
            for (j=0; j<nx; j++) {
                xt1 = x1 + j * dx;
                xt2 = xt1 + dx;
                p = scalePoint (xt1, yt1, zt);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt2, yt1, zt);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt2, yt2, zt);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt1, yt2, zt);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
            }
            jgl.StopDrawMode ();
        }

    /*
     * Draw top grid
     */
        zt = z2;
        n = 0;
        for (i=0; i<ny; i++) {
            yt1 = y1 + i * dy;
            yt2 = yt1 + dy;
            jgl.StartDrawMode ();
            for (j=0; j<nx; j++) {
                xt1 = x1 + j * dx;
                xt2 = xt1 + dx;
                p = scalePoint (xt1, yt1, zt);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt1, yt2, zt);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt2, yt2, zt);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt2, yt1, zt);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
            }
            jgl.StopDrawMode ();
        }

    /*
     * Draw west grid
     */
        xt = x1;
        n = 0;
        for (i=0; i<nz; i++) {
            zt1 = z1 + i * dz;
            zt2 = zt1 + dz;
            jgl.StartDrawMode ();
            for (j=0; j<ny; j++) {
                yt1 = y1 + j * dy;
                yt2 = yt1 + dy;
                p = scalePoint (xt, yt1, zt1);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt, yt2, zt1);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt, yt2, zt2);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt, yt1, zt2);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
            }
            jgl.StopDrawMode ();
        }

    /*
     * Draw east grid
     */
        xt = x2;
        n = 0;
        for (i=0; i<nz; i++) {
            zt1 = z1 + i * dz;
            zt2 = zt1 + dz;
            jgl.StartDrawMode ();
            for (j=0; j<ny; j++) {
                yt1 = y1 + j * dy;
                yt2 = yt1 + dy;
                p = scalePoint (xt, yt1, zt1);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt, yt1, zt2);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt, yt2, zt2);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt, yt2, zt1);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
            }
            jgl.StopDrawMode ();
        }

    /*
     * Draw south grid
     */
        yt = y1;
        n = 0;
        for (i=0; i<nz; i++) {
            zt1 = z1 + i * dz;
            zt2 = zt1 + dz;
            jgl.StartDrawMode ();
            for (j=0; j<nx; j++) {
                xt1 = x1 + j * dx;
                xt2 = xt1 + dx;
                p = scalePoint (xt1, yt, zt1);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt1, yt, zt2);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt2, yt, zt2);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt2, yt, zt1);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
            }
            jgl.StopDrawMode ();
        }

    /*
     * Draw north grid
     */
        yt = y2;
        n = 0;
        for (i=0; i<nz; i++) {
            zt1 = z1 + i * dz;
            zt2 = zt1 + dz;
            jgl.StartDrawMode ();
            for (j=0; j<nx; j++) {
                xt1 = x1 + j * dx;
                xt2 = xt1 + dx;
                p = scalePoint (xt1, yt, zt1);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt2, yt, zt1);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt2, yt, zt2);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
                p = scalePoint (xt1, yt, zt2);
                jgl.DGNodeXYZ (p.x, p.y, p.z);
            }
            jgl.StopDrawMode ();
        }

        jgl.SetDrawCulling(0);

    }

/*--------------------------------------------------------------------------------*/

    private JPoint3D scalePoint (double x, double y, double z)
    {
        JPoint3D            pt;

        pt = scaler.scalePoint (x, y, z);

        return pt;

    }


/*-----------------------------------------------------------------------------*/

    private JSurfaceWorks    jsw;

/*
 * Called by the JGL text drawing method to determine if the text should
 * be drawn at the specified raster position.
 * The method should check if the text rectangle is inside the raster outline
 * of the cube and return false if the text is inside or true if the
 * text is outside.
 */
    public boolean checkTextPosition (
        double    x1in,
        double    y1in,
        double    x2in,
        double    y2in)
    {

        if (jsw == null) {
            return false;
        }

        if (cubeOutline == null) {
            return false;
        }

        double xmid = (x1in + x2in) / 2.0;
        double ymid = (y1in + y2in) / 2.0;

        int istat = jsw.pointInPoly (
            x1in,
            y1in,
            cubeOutline.getXArray(),
            cubeOutline.getYArray(),
            cubeOutline.getNpts());

        if (istat > 0) {
            return false;
        }

        istat = jsw.pointInPoly (
            x2in,
            y1in,
            cubeOutline.getXArray(),
            cubeOutline.getYArray(),
            cubeOutline.getNpts());

        if (istat >= 0) {
            return false;
        }

        istat = jsw.pointInPoly (
            x2in,
            y2in,
            cubeOutline.getXArray(),
            cubeOutline.getYArray(),
            cubeOutline.getNpts());

        if (istat >= 0) {
            return false;
        }

        istat = jsw.pointInPoly (
            x1in,
            y2in,
            cubeOutline.getXArray(),
            cubeOutline.getYArray(),
            cubeOutline.getNpts());

        if (istat >= 0) {
            return false;
        }

pmsg ("bounding box inside: "+x1in+" "+y1in+" "+x2in+" "+y2in);
for (int i=0; i<cubeOutline.getNpts(); i++) {
pmsg ("  "+cubeOutline.getXArray()[i]+" "+cubeOutline.getYArray()[i]);
}

        return true;

    }

/*-----------------------------------------------------------------------------*/

    private boolean drawAxisColors = false;
    private boolean drawAxisNames = false;
    private boolean drawAxisNumbers = false;
    private JSResult2D cubeOutline;

    void setAxesFlags (
        boolean color,
        boolean name,
        boolean number)
    {
        drawAxisColors = color;
        drawAxisNames = name;
        drawAxisNumbers = number;
    }

/*------------------------------------------------------------------------------*/

/*
 * Scale and draw axis colors, names and numbers according to the
 * state desired.
 */
    void scaleAxes (JGL jgl)
    {

        pmsg ();
        pmsg ();
        pmsg ("******************************************");
        pmsg ();


    /*
     * Do nothing if no axis stuff is to be drawn.
     */
        if (drawAxisNames == false  &&
            drawAxisNumbers == false  &&
            drawAxisColors == false) {
            return;
        }

    /*
     * Get a jsurfaceworks calculation object and
     * set the grazing value for point in polygon
     * operations to 2 pixels.
     */
        jsw = new JSurfaceWorks ();
        jsw.setGrazingValue (2.0);
        jsw.pauseLogging ();

    /*
     * Set the text position filter in the jgl object to this
     * if axis numbers are wanted.
     */
        if (drawAxisNumbers  ||  drawAxisNames) {
            createCubeOutline2 (jgl);
            jgl.setTextPositionFilter (this);
        }

    /*
     * Draw the x, y and z axes.
     */
        JPoint3D   p;

        jgl.SetDrawLineWidth (2.0);
        jgl.SetDrawStyle(JGL.DG_WIREFRAME);
        jgl.SetDrawLighting(0);
        jgl.SetDrawCulling(0);
        jgl.SetDrawDepthTest (0);


    /*
     * Draw green X axis on the top.
     */
        jgl.DGNodeColor(0.0, 0.7, 0.0, 1.0);
        if (drawAxisColors == true) {
            jgl.SetDrawMode(JGL.DG_LINES);
            jgl.StartDrawMode();
            p = scalePoint (prettyXmin,
                            prettyYmin,
                            prettyZmax);
            jgl.DGNodeXYZ (p.x, p.y, p.z);
            p = scalePoint (prettyXmax,
                            prettyYmin,
                            prettyZmax);
            jgl.DGNodeXYZ (p.x, p.y, p.z);
            jgl.StopDrawMode ();
        }
        else {
            p = scalePoint (prettyXmax,
                            prettyYmin,
                            prettyZmax);
        }

        if (drawAxisNames) {
            positionAxisName (jgl, p, "East");
        }
        if (drawAxisNumbers) {
            jgl.SetMoveAdjust (true);
            drawXNumbers (jgl);
            jgl.SetMoveAdjust (false);
        }

    /*
     * Draw red Y axis on top
     */
        jgl.DGNodeColor(0.7, 0.0, 0.0, 1.0);
        if (drawAxisColors == true) {
            jgl.SetDrawMode(JGL.DG_LINES);
            jgl.StartDrawMode();
            p = scalePoint (prettyXmin,
                            prettyYmin,
                            prettyZmax);
            jgl.DGNodeXYZ (p.x, p.y, p.z);
            p = scalePoint (prettyXmin,
                            prettyYmax,
                            prettyZmax);
            jgl.DGNodeXYZ (p.x, p.y, p.z);
            jgl.StopDrawMode ();
        }
        else {
            p = scalePoint (prettyXmin,
                            prettyYmax,
                            prettyZmax);
        }

        if (drawAxisNames) {
            positionAxisName (jgl, p, "North");
        }
        if (drawAxisNumbers) {
            jgl.SetMoveAdjust (true);
            drawYNumbers (jgl);
            jgl.SetMoveAdjust (false);
        }

    /*
     * Draw a blue z axis from the origin downward.
     */
        jgl.DGNodeColor(0.0, 0.5, 0.9, 1.0);
        if (drawAxisColors == true) {
            jgl.SetDrawMode(JGL.DG_LINES);
            jgl.StartDrawMode();
            p = scalePoint (prettyXmin,
                            prettyYmin,
                            prettyZmin);
            jgl.DGNodeXYZ (p.x, p.y, p.z);
            p = scalePoint (prettyXmin,
                            prettyYmin,
                            prettyZmax);
            jgl.DGNodeXYZ (p.x, p.y, p.z);
            jgl.StopDrawMode ();
        }

        if (drawAxisNames) {
            p = scalePoint (prettyXmin,
                            prettyYmin,
                            prettyZmin);
            positionBottomAxisName (jgl, p, "Elev");
        }
        if (drawAxisNumbers) {
            jgl.SetMoveAdjust (true);
            drawZNumbers (jgl);
            jgl.SetMoveAdjust (false);
        }

        jgl.SetDrawDepthTest (1);
        jgl.setTextPositionFilter (null);

        jsw.restartLogging ();
        jsw = null;

    }

/*---------------------------------------------------------------*/

    private void positionAxisName (
        JGL jgl,
        JPoint3D p,
        String text)
    {
       int              istat;

    /*
     * Try to draw the name using all but the center anchor
     * position.  If the text is completely outside of the
     * cube polygon, the text is drawn.
     */
        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 2, 0, 15);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 4, 15, 0);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 6, -15, 0);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 8, 0, -15);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 1, 15, 15);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 3, -15, 15);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 7, 15, -15);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 9, -15, -15);
        if (istat == 1) {
            return;
        }

    /*
     * If no text is outside the polygon, draw using the lower left anchor.
     */
        jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont);
    }

/*---------------------------------------------------------------*/

    private void positionBottomAxisName (
        JGL jgl,
        JPoint3D p,
        String text)
    {
       int              istat;

    /*
     * Try to draw the name using all but the center anchor
     * position.  If the text is completely outside of the
     * cube polygon, the text is drawn.
     */
        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 8, 0, -15);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 4, 15, 0);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 6, -15, 0);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 2, 0, 15);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 7, 15, -15);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 9, -15, -15);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 1, 15, 15);
        if (istat == 1) {
            return;
        }

        istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont,
                                 3, -15, 15);
        if (istat == 1) {
            return;
        }

    /*
     * If no text is outside the polygon, draw using the lower left anchor.
     */
        jgl.DrawTextXYZ (p.x, p.y, p.z, text, labelsFont);
    }

/*---------------------------------------------------------------*/

    private static final double _HORIZONTAL_SLOPE_CUTOFF_ = 0.2;

/*---------------------------------------------------------------*/

    ArrayList<LastTextData>     xLastText = new ArrayList<LastTextData> ();
    ArrayList<LastTextData>     yLastText = new ArrayList<LastTextData> ();
    ArrayList<LastTextData>     zLastText = new ArrayList<LastTextData> ();

/*---------------------------------------------------------------*/

    private void drawXNumbers (JGL jgl)
    {
        double          xt, yt, zt;
        double          xfirst, xlast;
        double          rx1, ry1, rx2, ry2, dx, dy, slope;
        String          text;
        int             ival, istat;
        int             horizontalFlag;
        double          range;
        int             ndec;
        double          tval;
        double          xmid, ymid;

        JPoint3D        p3;
        JPoint3D        p;
        JPoint2D        p2;

    /*
     * If the model extents are not set, do nothing.
     */
        if (prettyXmax <= prettyXmin  ||
            prettyYmax <= prettyYmin  ||
            prettyZmax <= prettyZmin) {
            return;
        }

        range = (prettyXmax - prettyXmin) / xyMpu;
        ndec = 0;
        if (range < 40.0) {
            ndec = 1;
        }
        if (range < 4.0) {
            ndec = 2;
        }
        if (range < .4) {
            ndec = 3;
        }

    /*
     * For now, do not label the end points of the axes.
     */
        xfirst = prettyXmin + prettyXtick;
        xlast = prettyXmax - prettyXtick / 2.0;

    /*
     * Do the axis at minimum y and minimum z
     */

pmsg ();
pmsg ("drawing x at min y min z");

        yt = prettyYmin;
        zt = prettyZmin;
        xt = xfirst;

        int n2d;

    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (prettyXmin, yt, zt);
        j3dArray[0] = p3;
        p3 = scalePoint (prettyXmax, yt, zt);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                xt = xlast + prettyXtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            xt = xlast + prettyXtick;
        }

    /*
     * Draw the numbers.
     */
        int       ndo = 0;
        int       size = xLastText.size();
        LastTextData    lt;

        while (xt < xlast) {
            tval = xt / xyMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);

            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                xLastText.add (lt);
            }
            else {
                lt = xLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    xt += prettyXtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    xt += prettyXtick;
                    continue;
                }
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 3, 3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

pmsg ("can't find spot for "+xt);
            xt += prettyXtick;

        }

    /*
     * Do the axis at minimum y and maximum z
     */

pmsg ("drawing x at min y max z");

        yt = prettyYmin;
        zt = prettyZmax;
        xt = xfirst;

    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (prettyXmin, yt, zt);
        j3dArray[0] = p3;
        p3 = scalePoint (prettyXmax, yt, zt);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                xt = xlast + prettyXtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            xt = xlast + prettyXtick;
        }

    /*
     * Draw the numbers.
     */
        ndo = 0;
        size = xLastText.size();

        while (xt < xlast) {
            tval = xt / xyMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);
            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                xLastText.add (lt);
            }
            else {
                lt = xLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    xt += prettyXtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    xt += prettyXtick;
                    continue;
                }
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 3, 3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

pmsg ("can't find spot for "+xt);
            xt += prettyXtick;
        }

    /*
     * Do the axis at maximum y and minimum z.
     */

pmsg ("drawing x at max y min z");

        yt = prettyYmax;
        zt = prettyZmin;
        xt = xfirst;

    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (prettyXmin, yt, zt);
        j3dArray[0] = p3;
        p3 = scalePoint (prettyXmax, yt, zt);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                xt = xlast + prettyXtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            xt = xlast + prettyXtick;
        }

    /*
     * Draw the numbers.
     */
        ndo = 0;
        size = xLastText.size();

        while (xt < xlast) {
            tval = xt / xyMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);
            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                xLastText.add (lt);
            }
            else {
                lt = xLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    xt += prettyXtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    xt += prettyXtick;
                    continue;
                }
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 3, 3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

pmsg ("can't find spot for "+xt);
            xt += prettyXtick;
        }

    /*
     * Do the axis at maximum y and maximum z.
     */

pmsg ("drawing x at max y max z");

        yt = prettyYmax;
        zt = prettyZmax;
        xt = xfirst;

    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (prettyXmin, yt, zt);
        j3dArray[0] = p3;
        p3 = scalePoint (prettyXmax, yt, zt);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                xt = xlast + prettyXtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            xt = xlast + prettyXtick;
        }

    /*
     * Draw the numbers.
     */
        ndo = 0;
        size = xLastText.size();

        while (xt < xlast) {
            tval = xt / xyMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);

            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                xLastText.add (lt);
            }
            else {
                lt = xLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    xt += prettyXtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    xt += prettyXtick;
                    continue;
                }
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 3, 3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                xt += prettyXtick;
                continue;
            }

pmsg ("can't find spot for "+xt);
            xt += prettyXtick;
        }

        return;

    }


/*---------------------------------------------------------------*/

    private void drawYNumbers (JGL jgl)
    {
        double          xt, yt, zt;
        double          yfirst, ylast;
        double          rx1, ry1, rx2, ry2, dx, dy, slope;
        String          text;
        int             ival, istat;
        int             horizontalFlag;
        double          range;
        int             ndec;
        double          tval;
        double          xmid, ymid;

        JPoint3D        p3;
        JPoint3D        p;
        JPoint2D        p2;

    /*
     * If the model extents are not set, do nothing.
     */
        if (prettyXmax <= prettyXmin  ||
            prettyYmax <= prettyYmin  ||
            prettyZmax <= prettyZmin) {
            return;
        }

        range = (prettyYmax - prettyYmin) / xyMpu;
        ndec = 0;
        if (range < 40.0) {
            ndec = 1;
        }
        if (range < 4.0) {
            ndec = 2;
        }
        if (range < .4) {
            ndec = 3;
        }

    /*
     * For now, do not label the end points of the axes.
     */
        yfirst = prettyYmin + prettyYtick;
        ylast = prettyYmax - prettyYtick / 2.0;

    /*
     * Do the axis at minimum x and minimum z
     */

pmsg ();
pmsg ("drawing y at min x min z");

        xt = prettyXmin;
        zt = prettyZmin;
        yt = yfirst;

        int n2d;

    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (xt, prettyYmin, zt);
        j3dArray[0] = p3;
        p3 = scalePoint (xt, prettyYmax, zt);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                yt = ylast + prettyYtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            yt = ylast + prettyYtick;
        }

    /*
     * Draw the numbers.
     */
        int ndo = 0;
        int size = yLastText.size();
        LastTextData lt;

        while (yt < ylast) {
            tval = yt / xyMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);

            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                yLastText.add (lt);
            }
            else {
                lt = yLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    yt += prettyYtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    yt += prettyYtick;
                    continue;
                }
            }
            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 5, 3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

pmsg ("can't find spot for "+yt);
            yt += prettyYtick;
        }

    /*
     * Do the axis at minimum x and maximum z
     */

pmsg ("drawing y at min x max z");

        xt = prettyXmin;
        zt = prettyZmax;
        yt = yfirst;

    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (xt, prettyYmin, zt);
        j3dArray[0] = p3;
        p3 = scalePoint (xt, prettyYmax, zt);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                yt = ylast + prettyYtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            yt = ylast + prettyYtick;
        }

    /*
     * Draw the numbers.
     */
        ndo = 0;
        size = yLastText.size();

        while (yt < ylast) {
            tval = yt / xyMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);

            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                yLastText.add (lt);
            }
            else {
                lt = yLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    yt += prettyYtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    yt += prettyYtick;
                    continue;
                }
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 3, 3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

pmsg ("can't find spot for "+yt);
            yt += prettyYtick;
        }

    /*
     * Do the axis at maximum x and minimum z.
     */

pmsg ("drawing y at max x min z");

        xt = prettyXmax;
        zt = prettyZmin;
        yt = yfirst;

    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (xt, prettyYmin, zt);
        j3dArray[0] = p3;
        p3 = scalePoint (xt, prettyYmax, zt);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                yt = ylast + prettyYtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            yt = ylast + prettyYtick;
        }

    /*
     * Draw the numbers.
     */
        ndo = 0;
        size = yLastText.size();

        while (yt < ylast) {
            tval = yt / xyMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);

            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                yLastText.add (lt);
            }
            else {
                lt = yLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    yt += prettyYtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    yt += prettyYtick;
                    continue;
                }
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 3, 3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

pmsg ("can't find spot for "+yt);
            yt += prettyYtick;
        }

    /*
     * Do the axis at maximum x and maximum z.
     */

pmsg ("drawing y at max x max z");

        xt = prettyXmax;
        zt = prettyZmax;
        yt = yfirst;

    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (xt, prettyYmin, zt);
        j3dArray[0] = p3;
        p3 = scalePoint (xt, prettyYmax, zt);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                yt = ylast + prettyYtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            yt = ylast + prettyYtick;
        }

    /*
     * Draw the numbers.
     */
        ndo = 0;
        size = yLastText.size();

        while (yt < ylast) {
            tval = yt / xyMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);

            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                yLastText.add (lt);
            }
            else {
                lt = yLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    yt += prettyYtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    yt += prettyYtick;
                    continue;
                }
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 3, 3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                yt += prettyYtick;
                continue;
            }

pmsg ("can't find spot for "+yt);
            yt += prettyYtick;
        }

        return;

    }


/*---------------------------------------------------------------*/

    private void drawZNumbers (JGL jgl)
    {
        double          xt, yt, zt;
        double          zfirst, zlast;
        double          rx1, ry1, rx2, ry2, dx, dy, slope;
        String          text;
        int             ival, istat;
        int             horizontalFlag;
        double          range;
        int             ndec;
        double          tval;
        double          xmid, ymid;

        JPoint3D        p3;
        JPoint3D        p;
        JPoint2D        p2;

    /*
     * If the model extents are not set, do nothing.
     */
        if (prettyXmax <= prettyXmin  ||
            prettyYmax <= prettyYmin  ||
            prettyZmax <= prettyZmin) {
            return;
        }

        range = (prettyZmax - prettyZmin) / zMpu;
        ndec = 0;
        if (range < 40.0) {
            ndec = 1;
        }
        if (range < 4.0) {
            ndec = 2;
        }
        if (range < .4) {
            ndec = 3;
        }

    /*
     * For now, do not label the end points of the axes.
     */
        zfirst = prettyZmin + prettyZtick;
        zlast = prettyZmax - prettyZtick / 2.0;

        int n2d;

    /*
     * Do the axis at minimum y and minimum x
     */

pmsg ();
pmsg ("drawing z at min x min y");

        yt = prettyYmin;
        xt = prettyXmin;
        zt = zfirst;

    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (xt, yt, prettyZmin);
        j3dArray[0] = p3;
        p3 = scalePoint (xt, yt, prettyZmax);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                zt = zlast + prettyZtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            zt = zlast + prettyZtick;
        }

    /*
     * Draw the numbers.
     */
        int ndo = 0;
        int size = zLastText.size();
        LastTextData lt;

        while (zt < zlast) {
            tval = zt / zMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);

            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                zLastText.add (lt);
            }
            else {
                lt = zLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    zt += prettyZtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    zt += prettyZtick;
                    continue;
                }
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 3, 3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

pmsg ("can't find spot for "+zt);
            zt += prettyZtick;
        }

    /*
     * Do the axis at minimum y and maximum x
     */

pmsg ("drawing z at max x min y");

        yt = prettyYmin;
        xt = prettyXmax;
        zt = zfirst;


    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (xt, yt, prettyZmin);
        j3dArray[0] = p3;
        p3 = scalePoint (xt, yt, prettyZmax);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                zt = zlast + prettyZtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            zt = zlast + prettyZtick;
        }

    /*
     * Draw the numbers.
     */
        ndo = 0;
        size = zLastText.size();

        while (zt < zlast) {
            tval = zt / zMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);

            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                zLastText.add (lt);
            }
            else {
                lt = zLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    zt += prettyZtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    zt += prettyZtick;
                    continue;
                }
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 3, 3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

pmsg ("can't find spot for "+zt);
            zt += prettyZtick;
        }

    /*
     * Do the axis at maximum y and minimum x.
     */

pmsg ("drawing z at min x max y");

        yt = prettyYmax;
        xt = prettyXmin;
        zt = zfirst;

    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (xt, yt, prettyZmin);
        j3dArray[0] = p3;
        p3 = scalePoint (xt, yt, prettyZmax);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                zt = zlast + prettyZtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            zt = zlast + prettyZtick;
        }

    /*
     * Draw the numbers.
     */
        ndo = 0;
        size = zLastText.size();

        while (zt < zlast) {
            tval = zt / zMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);

            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                zLastText.add (lt);
            }
            else {
                lt = zLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    zt += prettyZtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    zt += prettyZtick;
                    continue;
                }
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 3, 3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

pmsg ("can't find spot for "+zt);
            zt += prettyZtick;
        }

    /*
     * Do the axis at maximum y and maximum x.
     */

pmsg ("drawing z at max x max y");

        yt = prettyYmax;
        xt = prettyXmax;
        zt = zfirst;

    /*
     * Get the raster coordinates of the edge, clipped
     * to the viewport.  If no raster points are returned,
     * the edge is completely out of the viewport.
     */
        p3 = scalePoint (xt, yt, prettyZmin);
        j3dArray[0] = p3;
        p3 = scalePoint (xt, yt, prettyZmax);
        j3dArray[1] = p3;
        n2d = jgl.GetRasterPoints (
            j3dArray, 2,
            j2dArray, 100);

      /*
       * At least partly in the viewport.
       */
        if (n2d == 2  &&  cubeOutline != null) {
            p2 = j2dArray[0];;
            rx1 = p2.x;
            ry1 = p2.y;
            p2 = j2dArray[1];
            rx2 = p2.x;
            ry2 = p2.y;
            dx = rx2 - rx1;
            dy = ry2 - ry1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            horizontalFlag = 0;
            if (dx > dy) {
                slope = dy / dx;
                if (slope < _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 1;
                }
            }
            else {
                if (dx < dy * _HORIZONTAL_SLOPE_CUTOFF_) {
                    horizontalFlag = 2;
                }
            }

            xmid = (rx1 + rx2) / 2.0;
            ymid = (ry1 + ry2) / 2.0;
            istat = jsw.pointInPoly (
                xmid,
                ymid,
                cubeOutline.getXArray(),
                cubeOutline.getYArray(),
                cubeOutline.getNpts());
            if (istat > 0) {
                zt = zlast + prettyZtick;
            }
        }

      /*
       * Completely out of the viewport.
       */
        else {
            horizontalFlag = 0;
            zt = zlast + prettyZtick;
        }

    /*
     * Draw the numbers.
     */
        ndo = 0;
        size = zLastText.size();

        while (zt < zlast) {
            tval = zt / zMpu;
            ival = (int)(tval + 0.1);
            if (tval < 0.0) ival = (int)(tval - 0.1);
            if (ndec == 0) {
                text = formatInteger (ival);
            }
            else {
                text = formatNumber (tval, ndec);
            }

            p = scalePoint (xt, yt, zt);

            if (ndo >= size) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         -2, 0, 3);
                lt = jgl.getLastTextData ();
                zLastText.add (lt);
            }
            else {
                lt = zLastText.get(ndo);
                jgl.setLastTextData (lt);
            }

            ndo++;

            jgl.setLastTextData (lt);

            if (horizontalFlag == 1) {
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         2, 0, 5);
                if (istat == 1) {
                    zt += prettyZtick;
                    continue;
                }
                istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                         8, 0, -5);
                if (istat == 1) {
                    zt += prettyZtick;
                    continue;
                }
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     4, 5, 0);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     6, -5, 0);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     1, 3, 3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     7, 3, -3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     3, -3, 3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

            istat = jgl.DrawTextXYZ (p.x, p.y, p.z, text, numbersFont,
                                     9, -3, -3);
            if (istat == 1) {
                zt += prettyZtick;
                continue;
            }

pmsg ("can't find spot for "+zt);
            zt += prettyZtick;
        }

        return;

    }

/*------------------------------------------------------------------*/

    private String formatInteger (int ival)
    {
        String istr = "" + ival;
        String str = istr.trim();
        return str;
    }

/*------------------------------------------------------------------*/

    private String formatNumber (double val, int ndec)
    {
        int      i, ival, ival2;
        double   pval, left, right, mult;

        pval = val;
        if (val < 0.0) {
            pval = -pval;
        }
        ival = (int)pval;
        left = (double)ival;
        right = pval - left;

        for (i=0; i<ndec; i++) {
            right *= 10.0;
        }
        ival2 = (int)(right + 0.5);

        String s1 = "" + ival;
        String s2 = ".";
        String s3 = "" + ival2;

        String str;

        if (val < 0.0) {
            str = "-"+s1.trim()+s2+s3.trim();
        }
        else {
            str = s1.trim()+s2+s3.trim();
        }

        return str;
    }

/*----------------------------------------------------------------------*/


    private JPoint3D[]    j3dArray = new JPoint3D[50];
    private JPoint2D[]    j2dArray = new JPoint2D[100];

    private void createCubeOutline2 (JGL jgl)
    {

        cubeOutline = null;

        if (jsw == null) {
            return;
        }

        JPoint3D    p, pa;
        JPoint2D    p2, p22;

    /*
     * Convert each corner of the cube into a point in the opengl 3d
     * coordinates.  Then get the raqster position of each point.
     * Use the raster positions to calculate a convex hull.
     */
        int          n, i;
        n = 0;

      /*
       * Cube corner points
       */
        p = scalePoint (prettyXmin, prettyYmin, prettyZmin);
        j3dArray[n] = p;
        n++;
        p = scalePoint (prettyXmax, prettyYmin, prettyZmin);
        j3dArray[n] = p;
        n++;
        p = scalePoint (prettyXmin, prettyYmax, prettyZmin);
        j3dArray[n] = p;
        n++;
        p = scalePoint (prettyXmax, prettyYmax, prettyZmin);
        j3dArray[n] = p;
        n++;
        p = scalePoint (prettyXmin, prettyYmax, prettyZmax);
        j3dArray[n] = p;
        n++;
        p = scalePoint (prettyXmax, prettyYmax, prettyZmax);
        j3dArray[n] = p;
        n++;
        p = scalePoint (prettyXmin, prettyYmin, prettyZmax);
        j3dArray[n] = p;
        n++;
        p = scalePoint (prettyXmax, prettyYmin, prettyZmax);
        j3dArray[n] = p;
        n++;

        int n2d = jgl.GetRasterPoints (j3dArray, n,
                                       j2dArray, 100);

        double[] xpts = new double[n2d];
        double[] ypts = new double[n2d];

        for (i=0; i<n2d; i++) {
            xpts[i] = j2dArray[i].x;
            ypts[i] = j2dArray[i].y;
        }

        cubeOutline = jsw.outlinePoints (xpts, ypts, n2d);

        return;
    }

}  // end of main class

