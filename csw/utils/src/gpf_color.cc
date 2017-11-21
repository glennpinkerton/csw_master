
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_color.cc

    Implement the GPFColor methods.  This is a refactgor of the
    old gpf_color.c functions.

    This file contains functions that define, read and write the
    device independent color map.  This map is kept in the hue
    lightness and saturation color scheme, with two sets of hls
    coordinates for each entry.  One set is used for hard copy
    plots to printers and plotters.  The other is used for 
    terminal graphics.  The colors will look very similar on both
    plots and the terminal this way.
*/

#include <string.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/gpf_color.h"



/*
******************************************************************

                    g p f _ s e t d e f h l s

******************************************************************

  function name:    gpf_setdefhls        (int)

  call sequence:    gpf_setdefhls ()

  purpose:          set up the default color table

  return value:     always returns zero

*/

int GPFColor::gpf_setdefhls (void)
{
    static const JTshift    jtshft[] = {
                                 {0,0}, {60,-5}, {110,-35}, {120,0},
                                 {130,25}, {140,23}, {155,16}, {165,8},
                                 {175,5}, {180,5}, {195,2}, {220,5},
                                 {305,5}, {350,-20}, {360,-10}
                                 };
    static const int        nshft = 15;
    static const int        j16[] =    {34, 35, 36, 37, 38, 44, 46, 48, 50, 51, 52, 53,
                                  54, 55, 56, 60, 65, 66, 67, 68, 70, 75, 78, 79,
                                  80, 81, 82, 86, 87, 88, 89, 93};
    static const int        hdef[] =   {0, 0, 120, 240, 0, 300, 60, 180, 150, 160},
                      ldef[] =   {100, 0, 50, 50, 50, 50, 50, 50, 50, 35},
                      sdef[] =   {0, 0, 100, 100, 100, 100, 100, 100, 100, 60},
                      numprim = 10;

    int               i, j, lgbase, itmp1, itmp2, i2;
    CSW_F             htest, pct, tmp1, tmp2, rat;

    lgbase = 70;

/*  define primary colors  */

    for (i=0; i<numprim; i++) {
        ColorTable[i].hue = hdef[i];
        ColorTable[i].light = ldef[i];
        ColorTable[i].sat = sdef[i];
        ColorTable[i].hue2 = hdef[i];
        ColorTable[i].light2 = ldef[i];
        ColorTable[i].sat2 = sdef[i];
    }

/*  grey scale colors  */

    tmp1 = (CSW_F)(35-numprim);
    for (i=numprim; i<32; i++) {
        ColorTable[i].hue = 0;
        ColorTable[i].sat = 0;
        ColorTable[i].hue2 = 0;
        ColorTable[i].sat2 = 0;
        tmp2 = (CSW_F)(i-numprim) / tmp1 + .1f;
        tmp2 = tmp2 * tmp2;
        itmp1 = (int)(90 - tmp2*90);
        ColorTable[i].light = itmp1;
        ColorTable[i].light2 = itmp1;
    }

/*  colors 32 to 63  */

    for (i=32; i<64; i++) {
        ColorTable[i].hue = 360 - (i-32) * 11;
        ColorTable[i].light = lgbase - 10;
        ColorTable[i].sat = 100;
    }
    for (i=64; i<96; i++) {
        ColorTable[i].hue = 360 - (i-64) * 11;
        ColorTable[i].light = lgbase - 30;
        ColorTable[i].sat = 50;
    }
    for (i=32; i<64; i++) {
        i2 = j16[i-32] - 1;
        ColorTable[i].hue = ColorTable[i2].hue;
        ColorTable[i].light = ColorTable[i2].light;
        ColorTable[i].sat = ColorTable[i2].sat;
    }

/*  colors 64 to 95  */

    for (i=64; i<96; i++) {
        ColorTable[i].hue = 240 - (i-64) * 4;
        itmp1 = lgbase - 15;
        if (itmp1 > 40  &&  itmp1 < 60) {
            itmp1 = 50;
        }
        ColorTable[i].light = itmp1;
        if (i <= 80) {
            itmp1 = 100 - (i-64) * 2;
        }
        else {
            itmp1 = 100 - (95-i) * 2;
        }
        ColorTable[i].sat = itmp1;
    }

/*  colors 96 to 127  */

    for (i=96; i<128; i++) {
        itmp1 = (int)(330 + (i-96) * 4.75f);
        if (itmp1 > 360) {
            itmp1 -= 360;
        }
        ColorTable[i].hue = itmp1;
        itmp1 = lgbase - 15;
        if (itmp1 > 40  &&  itmp1 < 60) {
            itmp1 = 50;
        }
        ColorTable[i].light = itmp1;
        if (i <= 111) {
            itmp1 = 100 - (i-96) * 4;
        }
        else {
            itmp1 = 100 - (127-i) * 4;
        }
        ColorTable[i].sat = itmp1;
    }

/*  colors 128 to 255  */

    tmp1 = 350.f / (MAXNCOLOR-128.f);
    for (i=128; i<MAXNCOLOR; i++) {
        itmp1 = (int)(360 - (i-128) * tmp1);
        if (itmp1 < 0) {
            itmp1 += 360;
        }
        if (itmp1 > 360) {
            itmp1 -= 360;
        }
        ColorTable[i].hue = itmp1;
        if (i<194) {
            itmp1 = lgbase - (i-128) / 4;
        }
        else {
            itmp1 = lgbase - (MAXNCOLOR-i) / 4;
        }
        ColorTable[i].light = itmp1;
        ColorTable[i].sat = 100;
    }

/*  darken green colors  */

    for (i=32; i<MAXNCOLOR; i++) {
        tmp1 = (CSW_F)ColorTable[i].hue;
        tmp1 = (tmp1 - 240.f) / 60.f;
        if (tmp1 < 0.0f) {
            tmp1 = -tmp1;
        }
        if (tmp1 < 1.0f) {
            itmp1 = ColorTable[i].light;
            tmp2 = 1.f - .25f * (1.f-tmp1);
            ColorTable[i].light = (int)(tmp2*(CSW_F)itmp1);
        }
    }
        
/*  apply empirically determined hue shifts to make a set of
    hls coordinates for screen color that will match the
    hardcopy color fairly well  */

    for (i=32; i<MAXNCOLOR; i++) {
        htest = (CSW_F)ColorTable[i].hue;
        htest = (htest - 240.f) / 60.f;
        if (htest < 0.0f) {
            htest = -htest;
        }
        if (htest < 1.0f) {
            itmp1 = ColorTable[i].light;
            pct = (1.f - .1f * (1.f-htest));
            itmp1 = (int)((CSW_F)itmp1/pct);
            ColorTable[i].light2 = itmp1;
        }
        else {
            ColorTable[i].light2 = ColorTable[i].light;
        }
        ColorTable[i].sat2 = ColorTable[i].sat;
        itmp1 = ColorTable[i].hue;
        htest = (CSW_F)itmp1;
        for (j=0; j<nshft-1; j++) {
            ColorTable[i].hue2 = ColorTable[i].hue;
            if (itmp1 >= jtshft[j].anchor  &&
                itmp1 <= jtshft[j+1].anchor) {
                tmp1 = (CSW_F)jtshft[j].hshift;
                tmp2 = (CSW_F)jtshft[j+1].hshift;
                rat = (htest - jtshft[j].anchor) /
                      (jtshft[j+1].anchor - jtshft[j].anchor);
                itmp2 = (int)(tmp1 + (tmp2-tmp1)*rat + .5f);
                ColorTable[i].hue2 = ColorTable[i].hue + itmp2;
                break;
            }
        }
    }

    return 0;
                                 
}  /*  end of function gpf_setdefhls  */






/*
******************************************************************

                   g p f _ g e t d e f h l s 1

******************************************************************

    return the hue, lightness and saturation for hard copy version
    of a color index .  Returns zero if index out of range, or returns
    1 if completed successfully.

*/

int GPFColor::gpf_getdefhls1 (int i, int *hout, int *lout, int *sout)
{
    if (i < 0  ||  i > MAXNCOLOR-1) {
        return 0;
    }

    *hout = ColorTable[i].hue;
    *lout = ColorTable[i].light;
    *sout = ColorTable[i].sat;

    return 1;

}  /*  end of function gpf_getdefhls1  */



/*
******************************************************************

                   g p f _ g e t d e f h l s 2

******************************************************************

    return the hue, lightness and saturation for screen version
    of a color index .  Returns zero if index out of range, or returns
    1 if completed successfully.

*/

int GPFColor::gpf_getdefhls2 (int i, int *hout, int *lout, int *sout)
{
    if (i < 0  ||  i > MAXNCOLOR-1) {
        return 0;
    }

    *hout = ColorTable[i].hue2;
    *lout = ColorTable[i].light2;
    *sout = ColorTable[i].sat2;

    return 1;

}  /*  end of function gpf_getdefhls2  */








/*
******************************************************************

                   g p f _ h l s t o r g b

******************************************************************

  function name:    gpf_hlstorgb      (int)

  call sequence:    gpf_hlstorgb (hin, lin, sin, rout, gout, bout)

  purpose:          convert hue, lightness, saturation color to 
                    red, green, blue color.  Blue is 0 hue, red is 120,
                    green = 240.

  return value:     always returns zero

  calling parameters:

    hin      r    int           hue angle   (0 to 360)
    lin      r    int           lightness   (0 to MAXHLS)
    sin      r    int           saturation  (0 to MAXHLS)
    rout     w    int*          red value   (0 to MAXRGB)
    gout     w    int*          green value (0 to MAXRGB)
    bout     w    int*          blue value  (0 to MAXRGB)

*/

int GPFColor::gpf_hlstorgb (int hin, int lin, int sin, 
                  int *rout, int *gout, int *bout)
{
    CSW_F        h, l, s, r, g, b, htmp, n1, n2, val;
    int          i;

    r = 0.0f;
    g = 0.0f;
    b = 0.0f;

/*  convert hls to CSW_F  */

    h = (CSW_F)hin - 120.f;
    l = (CSW_F)lin / MAXHLS;
    s = (CSW_F)sin / MAXHLS;

    if (h < 0.0f) {
        h += 360.f;
    }
    if (h > 360.f) {
        h -= 360.f;
    }
    if (l < 0.0f) {
        l = 0.0f;
    }
    if (l > 1.0f) {
        l = 1.0f;
    }
    if (s < 0.0f) {
        s = 0.0f;
    }
    if (s > 1.0f) {
        s = 1.0f;
    }

/*  calculate n1 and n2 based on lightness and saturation  */

    if (l <= 0.5f) {
        n2 = l * (1 + s);
        n1 = 0.0f;
    }
    else {
        n2 = l + s - l*s;
        n1 = 2 * l - n2;
    }

/*  grey scale only  */

    if (s < 0.001f) {
        r = l;
        g = l;
        b = l;
    }

/*  color  */

    else {
        
        for (i=0; i<3; i++) {

            if (i == 0) {
                htmp = h+120.f;
            }
            else if (i == 1) {
                htmp = h;
            }
            else {
                htmp = h-120.f;
            }
            if (htmp > 360.f) {
                htmp -= 360.f;
            }
            if (htmp < 0.0f) {
                htmp += 360.f;
            }

            if (htmp < 60.f) {
                val = n1 + (n2-n1) * htmp/60.f;
            }
            else if (htmp <180.f) {
                val = n2;
            }
            else if (htmp < 240.f) {
                val = n1 + (n2-n1) * (240-htmp) / 60.f;
            }
            else {
                val = n1;
            }

            if (i == 0) {
                r = val;
            }
            else if (i == 1) {
                g = val;
            }
            else {
                b = val;
            }
        }
    }

/*  convert to integer values  */

    *rout = (int)(r * MAXRGB);
    *gout = (int)(g * MAXRGB);
    *bout = (int)(b * MAXRGB);

    return 0;

}  /*  end of function gpf_hlstorgb  */








/*
******************************************************************

                    g p f _ r g b t o h l s

******************************************************************

  function name:    gpf_rgbtohls        (int)

  call sequence:    gpf_rgbtohls (rin, gin, bin, hout, lout, sout)

  purpose:          convert red, green, blue color to 
                    hue, lightness, saturation color.

  return value:     always returns zero

  calling parameters:

    rin        r    int     input red value (0 to MAXRGB)
    gin        r    int     input green value (0 to MAXRGB)
    bin        r    int     input blue value (0 to MAXRGB)
    hout       w    int*    output hue (0 to 360)
    lout       w    int*    output lightness (0 to MAXHLS)
    sout       w    int*    output saturation (0 to MAXHLS)

*/

int GPFColor::gpf_rgbtohls (int rin, int gin, int bin, 
                  int *hout, int *lout, int *sout)
{
    CSW_F    r, g, b, h, l, s, delta, max, min;

/*  convert rgb to CSW_F from 0 to 1  */

    r = (CSW_F)rin / MAXRGB;
    g = (CSW_F)gin / MAXRGB;
    b = (CSW_F)bin / MAXRGB;

/*  lightness from range of rgb values  */

    max = r;
    if (g > max) {
        max = g;
    }
    if (b > max) {
        max = b;
    }
    min = r;
    if (g < min) {
        min = g;
    }
    if (b < min) {
        min = b;
    }

    l = (min+max) / 2.0f;

/*  grey scale  */

    if (max-min < 0.001f) {
        s = 0.0f;
        h = 0.0f;
    }

/*  color  */

    else {

/*      calculate saturation  */

        delta = max - min;
        if (l <= 0.5f) {
            s = delta / (max+min);
        }
        else {
            s = delta / (2 - max - min);
        }

/*      calculate hue  */

        if (r - max  ==  0.0) {
            h = (g-b) / delta;
        }
        else if (g - max  ==  0.0) {
            h = 2 + (b-r) / delta;
        }
        else {
            h = 4 + (r-g) / delta;
        }
        h = h*60.f + 120.f;
        if (h > 360.f) {
            h -= 360.f;
        }
        if (h < 0.f) {
            h += 360.f;
        }
    }

/*  convert back to integer  */

    *hout = (int)h;
    *lout = (int)(l * MAXHLS);
    *sout = (int)(s * MAXHLS);

    return 0;

}  /*  end of function gpf_rgbtohls  */



/*
  ****************************************************************

           g p f _ C o n v e r t T o I n k C o l o r

  ****************************************************************

  function name:   gpf_ConvertToInkColor        (int)

  call sequence:   gpf_ConvertToInkColor (inr, ing, inb)

  purpose:         Use an empirically determined filter to convert
                   the red, green and blue values corresponding to
                   a terminal color (based on light) to a plotter 
                   color (based on ink) that looks about the same 
                   as the light based color.

  return value:    always returns 1

  calling parameters:

    inr       r/w     int*       pointer to red value (0-65k)
    ing       r/w     int*       pointer to green value (0-65k)
    inb       r/w     int*       pointer to blue value (0-65k)

*/

int GPFColor::gpf_ConvertToInkColor (int *inr, int *ing, int *inb)
{
    int         hue, light, sat, ih;
    CSW_F       pct, dh1, dh2;
    int         i, h1, h2;

    static const int  hlevel[] = {0, 50, 70, 180, 215, 230, 260, 275, 305, 315, 330, 360},
                hshift[] = {-30, -10, 0, 0, -7, -10, -28, -25, 0, -10, -15, -30};
    static const int  nshift = 12;

/*
    convert to hue, lightness, saturation color
*/

    gpf_rgbtohls (*inr, *ing, *inb, &hue, &light, &sat);

/*
    shift the hue component of the color based on the
    empirically determined hshift array
*/

    for (i=0; i<nshift-1; i++) {
        h1 = hlevel[i];
        h2 = hlevel[i+1];
        if (hue >= h1  &&  hue <= h2) {
            dh1 = (CSW_F)(hue - h1);
            dh2 = (CSW_F)(h2 - h1);
            pct = dh1 / dh2;
            dh1 = (CSW_F)(hshift[i+1] - hshift[i]);
            dh1 = dh1 * pct + hshift[i];
            ih = (int)(dh1 + .5f);
            hue += ih;
            if (hue < 0) hue += 360;
            if (hue > 360) hue -= 360;
            break;
        }
    }

/*
    lighten greenish hues
*/

    if (hue > 200  &&  hue < 280) {
        pct = (hue - 240) / 40.f;
        if (pct < 0.0f) pct = -pct;
        pct = 1.0f - pct;
        light = light + (int)(pct * 20);
    }

/*
    lighten cyanish hues
*/

    if (hue > 260  &&  hue < 340) {
        pct = (hue - 300) / 40.f;
        if (pct < 0.0f) pct = -pct;
        pct = 1.0f - pct;
        light = light + (int)(pct * 20);
    }
    
/*
    convert back to rgb and return
*/

    gpf_hlstorgb (hue, light, sat,
                  inr, ing, inb);

    return 1;    

}  /*  end of function gpf_ConvertToInkColor  */
