
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

/**
 Store data used for looking up colors for attribute values.
 This is used for coloring maps and 3d views.
*/
public class AtColorLookup
{
    private Color[]      colorValues;

    private double       minValue,
                         deltaValue;
    private int[]        colorIndex;


/*-------------------------------------------------------------------*/

  /**
   Create an empty attribute color lookup table.
  */
    public AtColorLookup () {}

/*-------------------------------------------------------------------*/

  /**
  Create a color lookup table using the specified values.
  @param low Array of low values for color bands.
  @param high Array of high values for color bands.
  @param colors Array of color objects for color bands.
  @param nValues Number of color bands.
  */
    public AtColorLookup (double[] low,
                          double[] high,
                          Color[] colors,
                          int nValues)
    {
        setValues (low, high, colors, nValues);
    }

/*-------------------------------------------------------------------*/

  /**
  Set the color band data used for the attribute color lookup.
  @param low Array of low values for color bands.
  @param high Array of high values for color bands.
  @param colors Array of color objects for color bands.
  @param nValues Number of color bands.
  */
    public void setValues (double[] low,
                           double[] high,
                           Color[] colors,
                           int nValues)
    {
        int             size, i, n;
        double          atmin, atmax, atdelta;

        colorValues = null;
        colorIndex = null;
        minValue = 1.e30;
        deltaValue = -1.0;

        if (low == null  ||  high == null  ||  colors == null) {
            return;
        }
        if (nValues < 1) {
            return;
        }

        colorValues = new Color[nValues];

        atmin = 1.e30;
        atmax = -1.e30;
        for (i=0; i<nValues; i++) {
            if (low[i] < atmin) atmin = low[i];
            if (high[i] > atmax) atmax = high[i];
            try {
                colorValues[i] = new Color(
                                       colors[i].getRed(),
                                       colors[i].getGreen(),
                                       colors[i].getBlue(),
                                       colors[i].getAlpha()
                                      );
            }
            catch (Throwable e) {
                colorValues = null;
                throw (e);
            }
        }

        if (atmin >= atmax) {
            return;
        }

        atdelta = (atmax - atmin) / 2000.0;

        try {
            colorIndex = new int[2000];
        }
        catch (Throwable e) {
            colorValues = null;
            throw (e);
        }

        for (i=0; i<2000; i++) {
            colorIndex[i] = -1;
        }

        int       i1, i2, j;

        for (i=0; i<nValues; i++) {
            i1 = (int)((low[i] - atmin) / atdelta);
            i2 = (int)((high[i] - atmin) / atdelta);
            i1--;
            i2 += 2;
            if (i1 < 0) i1 = 0;
            if (i2 > 1999) i2 = 1999;
            for (j=i1; j<= i2; j++) {
                colorIndex[j] = i;
            }
        }

        minValue = atmin;
        deltaValue = atdelta;

    }

/*-------------------------------------------------------------------*/

    public Color lookupColor (double atValue)
    {
        int          i, index;

        if (deltaValue <= 1.e-20) {
            return null;
        }

        i = (int)((atValue - minValue) / deltaValue + 0.5);
        if (i < 0  ||  i > 2000) {
            return null;
        }

        if (i > 1999) i = 1999;

        index = colorIndex[i];
        if (index < 0) {
            return null;
        }

        return colorValues[index];

    }

}
