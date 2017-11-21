package csw.j3d.src;

import java.awt.Color;

class JColor4D {
    public double  r, g, b, a;
    JColor4D (double rr,
              double gg,
              double bb,
              double aa)
    {
        r = rr;
        g = gg;
        b = bb;
        a = aa;
    }

    JColor4D (float rr,
              float gg,
              float bb,
              float aa)
    {
        r = (double)rr;
        g = (double)gg;
        b = (double)bb;
        a = (double)aa;
    }

    JColor4D (Color color)
    {
        r = 1.0;
        g = 1.0;
        b = 1.0;
        a = 1.0;

        r = (double)color.getRed ();
        g = (double)color.getGreen ();
        b = (double)color.getBlue ();
        a = (double)color.getAlpha ();
        r /= 255.0;
        g /= 255.0;
        b /= 255.0;
        a /= 255.0;
        if (r < 0.0) r = 0.0;
        if (g < 0.0) g = 0.0;
        if (b < 0.0) b = 0.0;
        if (a < 0.0) a = 0.0;
        if (r > 1.0) r = 1.0;
        if (g > 1.0) g = 1.0;
        if (b > 1.0) b = 1.0;
        if (a > 0.99) a = 1.0;
    }
}
