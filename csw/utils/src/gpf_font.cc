
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_font.c

    This file has functions for creating vector and polygon fonts.  
    The fonts defined here are completely antiquated.  A goal of
    a 2017 refactor should be to get rid of these in their entirely.
*/

/*
    include files needed
*/

#include <stdio.h>

#include "csw/utils/include/csw_.h"

#include "csw/utils/private_include/gpf_spline.h"
#include "csw/utils/private_include/gpf_font.h"


/*
********    coordinate definitions for fonts    ********
*/                

#include "csw/utils/private_include/gpf_sfont.h"
#include "csw/utils/private_include/gpf_cfont.h"
#include "csw/utils/private_include/gpf_sfill.h"
#include "csw/utils/private_include/gpf_bfill.h"
#include "csw/utils/private_include/gpf_lefts.h"
#include "csw/utils/private_include/gpf_rights.h"
#include "csw/utils/private_include/gpf_leftb.h"
#include "csw/utils/private_include/gpf_rightb.h"

#include "csw/utils/private_include/gpf_hersh1.h"
#include "csw/utils/private_include/gpf_hersh2.h"
#include "csw/utils/private_include/gpf_hersh3.h"
#include "csw/utils/private_include/gpf_hersh4.h"
#include "csw/utils/private_include/gpf_hersh5.h"
#include "csw/utils/private_include/gpf_hersh6.h"
#include "csw/utils/private_include/gpf_hersh7.h"
#include "csw/utils/private_include/gpf_hersh8.h"
#include "csw/utils/private_include/gpf_hersh9.h"
#include "csw/utils/private_include/gpf_hersh10.h"
#include "csw/utils/private_include/gpf_hersh11.h"
#include "csw/utils/private_include/gpf_hersh12.h"






/*
******************************************************************

                 g p f _ s e t f o n t

******************************************************************

*/

int GPFFont::gpf_setfont (int font)
{

    if (font >= MIN_HERSHEY  &&  font <= MAX_HERSHEY) {
        HersheyFlag = 1;
    }
    else {
        HersheyFlag = 0;
    }

    if (!HersheyFlag) {
        if (font < 0  ||  font >= Numvfont)
            font = 0;
        SlantX = VFontList[font].xslant;
        SlantY = VFontList[font].yslant;
        FontCurve = VFontList[font].curveflag;
        FontNumber = VFontList[font].fontnum;
        ShadowNumber = VFontList[font].shadownum;
    }
    else {
        if (font < MIN_HERSHEY  ||  font > MAX_HERSHEY)
            font = MIN_HERSHEY;
        SlantX = HFontList[font-101].xslant;
        SlantY = HFontList[font-101].yslant;
        FontCurve = HFontList[font-101].curveflag;
        FontNumber = HFontList[font-101].fontnum;
        ShadowNumber = HFontList[font-101].shadownum;
    }

    return 0;

}  /*  end of function gpf_setfont  */



/*
  ****************************************************************

              g p f _ G e t F o n t N u m b e r

  ****************************************************************

    Return the actual number of the base font used for a given
  drawn font number.  There are only 5 base fonts used for all
  20 drawn fonts.

*/

int GPFFont::gpf_GetFontNumber (int font)
{

    if (font < 0  ||  font >= Numvfont) {
        return -1;
    }

   return VFontList[font].fontnum;

}  /*  end of function gpf_GetFontNumber  */





/*
******************************************************************

                 g p f _ g e t f o n t s t r o k e s

******************************************************************

  function name:    gpf_getfontstrokes     (int)

  call sequence:    gpf_getfontstrokes (charin, size, x, y, parts, nparts,
                                        maxxy, maxparts) 

  purpose:          Get font strokes for vector font at a certain size.
                    The font used is the last font selected with gpf_setfont.
                    The coordinates returned are scaled for size and relative
                    to the lower left of the character.

  return value:     status code

                    0 = character not supported by the current font
                        or the current font is not a vector font
                        (i.e. the current font is a polygon font)
                    1 = normal successful completion

  calling parameters:

    charin      r     char         Character to plot
    size        r     CSW_F        Size in plotter units
    x           w     CSW_F*       Array which will receive x coordinates
    y           w     CSW_F*       Array which will receive y coordinates
    parts       w     int*         Array which will receive the number of points
                                   in each continuously drawn segment of the character
    nparts      w     int*         Number of segments packed into parts array 
    maxxy       r     int          Maximum size of x, y arrays.
    maxparts    r     int          Maximum size of parts array.

*/

int GPFFont::gpf_getfontstrokes (char charin, CSW_F size, CSW_F *x, CSW_F *y, 
                        int *parts, int *nparts, int maxxy, int maxparts)
{
    int            istat;

    istat = 0;
    if (FontNumber == 1) {    
        istat = gpf_getsimplefont (charin, size, x, y, parts, nparts, 
                                   maxxy, maxparts);
    }
    else if (FontNumber == 2) {
        istat = gpf_getcomplexfont (charin, size, x, y, parts, nparts, 
                                    maxxy, maxparts);
    }

/*
    Get the hershey font strokes.
*/
    else if (FontNumber >= MIN_HERSHEY  &&
             FontNumber <= MAX_HERSHEY) {
        istat = gpf_gethersheyfont (charin, size, x, y, parts, nparts,
                                    maxxy, maxparts);
    }

    return istat;

}  /*  end of function gpf_getfontstrokes  */






/*
******************************************************************

               g p f _ g e t s i m p l e f o n t 

******************************************************************

  function name:    gpf_getsimplefont     (int)

  call sequence:    gpf_getsimplefont (charin, size, x, y, parts, nparts,
                                       maxxy, maxparts)

  purpose:          Return the vector font strokes for the simple font.
                    The vectors are returned as plot units relative to
                    the lower left corner of the character, after having
                    been scaled for size.

  return value:     status code

                    0 = the character is not supported in this font
                    1 = normal successful completion

  calling parameters:

    charin      r     char         Character to plot
    size        r     CSW_F        Size (height) in plotter units
    x           w     CSW_F*       Array which will receive x coordinates
    y           w     CSW_F*       Array which will receive y coordinates
    parts       w     int*         Array which will receive the number of points
                                   in each continuously drawn segment of the character
    nparts      w     int*         Number of segments packed into parts array 
    maxxy       r     int          Maximum size of x and y output arrays.
    maxparts    r     int          Maximum size of parts array.

*/

int GPFFont::gpf_getsimplefont (char charin, CSW_F size, CSW_F *x, CSW_F *y, 
                       int *parts, int *nparts, int maxxy, int maxparts)
{
    int         i, j, offset, npts, n, ntotal;
    int         nout, npartmax;
    const FONTINT     *now;
    FONTINT     it;
    CSW_F       sizeratio, xt, yt, dres, yoffset;
    CSW_F       *xout, *yout, sres;

    GPFSpline   gpf_spline_obj;

/*
    if the character is 127, the degree sign is drawn.
    This is a lower case o drawn as a superscript
*/

    if ((int)charin == DEGREE_CHAR) {
        charin = 'o';
        yoffset = .75f * size;
        size *= 0.75f;
    }
    else {
        yoffset = 0.0f;
    }

/*  find the offset and number of points in the font vector array  */

    i = (int)charin - FIRSTCHAR;
    if (i < 0  ||  i > (128-FIRSTCHAR)) {
        return 0;
    }

    offset = SimpleList[i].offset;
    npts = SimpleList[i].npts;
    if (offset < 0  ||  npts <= 2) {
        return 0;
    }

    sizeratio = size / 0.1f;
    if (sizeratio < MINSIZERATIO) {
        sizeratio = MINSIZERATIO;
    }

    now = SimpleVector + offset;

/*  rescale the points and output in the x,y arrays  */

    n = 0;
    parts[0] = 0;
    npts /= 2;
    for (j=0; j<npts; j++) {
        it = *now;
        if (it < 0) {
            if (j > 0) {
                n++;
                if (n >= maxparts) {
                    return 0;
                }
                parts[n] = 0;
            }
            it = (FONTINT)(-it);
        }
        x[j] = (CSW_F)it / 1000.f * sizeratio;
        now++;
        it = *now;
        if (it < 0) {
            it = (FONTINT)(-it);
        }
        y[j] = (CSW_F)(it -100)/ 1000.f * sizeratio;
        y[j] += yoffset;

/*      slant the vectors if needed  */

        xt = x[j];
        yt = y[j];

        x[j] = SlantX * yt + xt;
        y[j] = SlantY * xt + yt;

        (parts[n])++;
        now++;
    }

    *nparts = n + 1;

/*  fit spline to make curved font if needed  */

    offset = 0;
    xout = x;
    yout = y;
    if (FontCurve  &&  size > 0.13f) {
        maxxy -= *nparts;
        maxxy -= 10;
        gpf_fontxylim (x, y, npts, &Ox1, &Oy1, &Ox2, &Oy2);
        for (j=0; j<npts; j++) {
            Xwork[j] = x[j];
            Ywork[j] = y[j];
        }
        sres = size / 20.f;
        if (sres < 0.02f) {
            sres = 0.02f;
        }
        ntotal = 0;
        for (i=0; i<*nparts; i++) {
            npartmax = maxxy * parts[i] / npts;
            dres = sres;
            if (parts[i] == 5) {
                if (gpf_CheckForFontDot (Xwork+offset, Ywork+offset, size)) {
                    dres = sres / 5.f;
                }
            }
            gpf_spline_obj.gpf_SplineFit (Xwork+offset, Ywork+offset, parts[i], 0,
                           xout, yout, npartmax, dres, 
                           &nout);
            offset += parts[i];
            parts[i] = nout;
            ntotal += nout;
            xout += nout;
            yout += nout;
        }

/*      rescale to same height as original unsmoothed character  */

        gpf_fontxylim (x, y, ntotal, &X1, &Y1, &X2, &Y2);
        gpf_fontscalexy (x, y, ntotal,
                         X1, Y1, X2, Y2,
                         Ox1, Oy1, Ox2, Oy2);

    }

/*  connect segments that have common end points  */

    gpf_ConnectFontStrokes (parts, nparts, x, y, size);

    return 1;

}  /*  end of function gpf_getsimplefont  */





/*
******************************************************************

               g p f _ g e t c o m p l e x f o n t 

******************************************************************

  function name:    gpf_getcomplexfont     (int)

  call sequence:    gpf_getcomplexfont (charin, size, x, y, parts, nparts,
                                        maxxy, maxparts)

  purpose:          Return the vector font strokes for the complex font.
                    The vectors are returned as plot units relative to
                    the lower left corner of the character, after having
                    been scaled for size.

  return value:     status code

                    0 = the character is not supported in this font
                    1 = normal successful completion

  calling parameters:

    charin      r     char         Character to plot
    size        r     CSW_F        Size (height) in plotter units
    x           w     CSW_F*       Array which will receive x coordinates
    y           w     CSW_F*       Array which will receive y coordinates
    parts       w     int*         Array which will receive the number of points
                                   in each continuously drawn segment of the character
    nparts      w     int*         Number of segments packed into parts array 
    maxxy       r     int          Maximum size of x and y output arrays.
    maxparts    r     int          Maximum size of parts array.

*/

int GPFFont::gpf_getcomplexfont (char charin, CSW_F size, CSW_F *x, CSW_F *y, 
                        int *parts, int *nparts, int maxxy, int maxparts)
{
    int         i, j, offset, npts, n, ntotal;
    int         nout, npartmax;
    const FONTINT     *now;
    FONTINT     it;
    CSW_F       sizeratio, xt, yt, dres, yoffset;
    CSW_F       *xout, *yout, sres;

    GPFSpline   gpf_spline_obj;

/*
    if the character is 127, the degree sign is drawn.
    This is a lower case o drawn as a superscript
*/

    if ((int)charin == DEGREE_CHAR) {
        charin = 'o';
        yoffset = .75f * size;
        size *= 0.75f;
    }
    else {
        yoffset = 0.0f;
    }

/*  find the offset and number of points in the font vector array  */

    i = (int)charin - FIRSTCHAR;
    if (i < 0  ||  i > (128-FIRSTCHAR)) {
        return 0;
    }

    offset = ComplexList[i].offset;
    npts = ComplexList[i].npts;
    if (offset < 0  ||  npts <= 2) {
        return 0;
    }

    sizeratio = size / 0.1f;
    if (sizeratio < MINSIZERATIO) {
        sizeratio = MINSIZERATIO;
    }

    now = ComplexVector + offset;

/*  rescale the points and output in the x,y arrays  */

    n = 0;
    parts[0] = 0;
    npts /= 2;
    for (j=0; j<npts; j++) {
        it = *now;
        if (it < 0) {
            n++;
            if (n >= maxparts) {
                return 0;
            }
            parts[n] = 0;
            it = (FONTINT)(-it);
        }
        x[j] = (CSW_F)it / 1000.f * sizeratio;
        now++;
        it = *now;
        if (it < 0) {
            it = (FONTINT)(-it);
        }
        y[j] = (CSW_F)(it -100)/ 1000.f * sizeratio;
        y[j] += yoffset;

/*      slant the vectors if needed  */

        xt = x[j];
        yt = y[j];

        x[j] = SlantX * yt + xt;
        y[j] = SlantY * xt + yt;

        (parts[n])++;
        now++;
    }

    *nparts = n + 1;

/*  fit spline to make curved font if needed  */

    offset = 0;
    xout = x;
    yout = y;
    if (FontCurve  &&  size > 0.13f) {
        maxxy -= *nparts;
        maxxy -= 10;
        gpf_fontxylim (x, y, npts, &Ox1, &Oy1, &Ox2, &Oy2);
        for (j=0; j<npts; j++) {
            Xwork[j] = x[j];
            Ywork[j] = y[j];
        }
        sres = size / 20.f;
        if (sres < 0.02f) {
            sres = 0.02f;
        }
        ntotal = 0;
        for (i=0; i<*nparts; i++) {
            npartmax = maxxy * parts[i] / npts;
            dres = sres;
            if (parts[i] == 5) {
                if (gpf_CheckForFontDot (Xwork+offset, Ywork+offset, size)) {
                    dres = sres / 5.f;
                }
            }
            gpf_spline_obj.gpf_SplineFit (Xwork+offset, Ywork+offset, parts[i], 0,
                           xout, yout, npartmax, dres, 
                           &nout);
            offset += parts[i];
            parts[i] = nout;
            ntotal += nout;
            xout += nout;
            yout += nout;
        }

/*      rescale to same height as original unsmoothed character  */

        gpf_fontxylim (x, y, ntotal, &X1, &Y1, &X2, &Y2);
        gpf_fontscalexy (x, y, ntotal,
                         X1, Y1, X2, Y2,
                         Ox1, Oy1, Ox2, Oy2);

    }

/*  connect segments that have common end points  */

    gpf_ConnectFontStrokes (parts, nparts, x, y, size);

    return 1;

}  /*  end of function gpf_getcomplexfont  */





/*
******************************************************************

                 g p f _ r o t a t e c h a r

******************************************************************

  function name:    gpf_rotatechar        (int)

  call sequence:    gpf_rotatechar (x, y, n, cosa, sina)

  purpose:          rotate the vector strokes for a character
                    the rotated points will replace the input points

  return value:     always returns zero

  calling parameters:

    x         r/w    CSW_F*    x coordinate array
    y         r/w    CSW_F*    y coordinate array
    n         r      int       number of points
    cosa      r      CSW_F     cosine of rotation angle
    sina      r      CSW_F     sin of rotation angle

*/

int GPFFont::gpf_rotatechar (CSW_F *x, CSW_F *y, int n, 
                    CSW_F cosa, CSW_F sina)
{
    int             i;
    CSW_F           xt;

    for (i=0; i<n; i++) {
        xt = x[i];
        x[i] = x[i]*cosa - y[i]*sina;
        y[i] = y[i]*cosa + xt*sina;
    }

    return 0;

}  /*  end of function gpf_rotatechar  */




/*
******************************************************************

              g p f _ C h e c k F o r F o n t D o t

******************************************************************

    Return 1 if the font segment is a dot or zero otherwise.
    This is only used in conjunction with font stroke generation
    functions.

*/

int GPFFont::gpf_CheckForFontDot (CSW_F *x, CSW_F *y, CSW_F size)
{
    int             i;
    CSW_F           x1, y1, x2, y2;

    size /= 10.f;

    x1 = 1.e30f;
    y1 = 1.e30f;
    x2 = -1.e30f;
    y2 = -1.e30f;

    for (i=0; i<5; i++) {
        if (x[i] < x1) x1 = x[i];
        if (y[i] < y1) y1 = y[i];
        if (x[i] > x2) x2 = x[i];
        if (y[i] > y2) y2 = y[i];
    }

    if (x2-x1 < size  &&  y2-y1 < size) {
        return 1;
    }

    return 0;

}  /*  end of function gpf_CheckForFontDot  */



/*
******************************************************************

          g p f _ C o n n e c t F o n t S t r o k e s

******************************************************************

    Connect font stroke polylines that have identical endpoints.
    This function is only called from the font stroke generation
    functions.

*/

int GPFFont::gpf_ConnectFontStrokes (int *parts, int *nparts, 
                            CSW_F *x, CSW_F *y, CSW_F size)
{
    int       i, j, n1, n2, n3, npts;
    CSW_F     xt, yt, tiny;

/*  do nothing if less than one polyline  */

    if (*nparts < 2) {
        return 1;
    }

/*  set up for connecting polyline tests  */

    n1 = *nparts;
    i = 1;
    tiny = size/100.f;
    
    npts = 0;
    for (j=0; j<n1; j++) {
        npts += parts[j];
    }

/*
    Test the last endpoint of a part with the first endpoint of
    the subsequent part.  If they are very close, combine the
    two parts into one output polyline.
*/

    while (i < n1) {
        n2 = parts[i-1] - 1;
        n3 = n2 + 1;
        xt = x[n2] - x[n3];
        if (xt < 0.0f) xt = -xt;
        yt = y[n2] - y[n3];
        if (yt < 0.0f) yt = -yt;
        
/*      points are identical, push x, y, and parts arrays down by one  */

        if (xt < tiny  &&  yt < tiny) {
            for (j=n3+1; j<npts; j++) {
                x[j-1] = x[j];
                y[j-1] = y[j];
            }
            parts[i-1] = parts[i-1] + parts[i] - 1;
            for (j=i+1; j<n1; j++) {
                parts[j-1] = parts[j];
            }
            n1--;
            npts--;
        }

/*      points differ, check the next parts  */

        else {
            i++;
        }
    }

    *nparts = n1;

    return 1;

}  /*  end of function gpf_ConnectFontStrokes  */




/*
******************************************************************

                   g p f _ C h a r W i d t h

******************************************************************

    Return the width of a character in the current font.

*/

int GPFFont::gpf_CharWidth (char ichar, CSW_F size, CSW_F *width)
{
    int        index, iwide;
    CSW_F      xt, xgap;

/*
    if the character is 127, the degree symbol is drawn.
    This is the same as a lower case o in width.
*/
    if ((int)ichar == DEGREE_CHAR) {
        ichar = 'o';
    }

/*
    Constant width simple font.
*/
    if (FontNumber == 1) {
        *width = .9f * size;
    }

/*
    Variable width EasyX complex font.
*/
    else if (FontNumber == 2) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = ComplexWidth[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 100.f;
        xt += xgap;
        *width = xt;
    }

/*
    Variable width EasyX block polygon font.
*/
    else if (FontNumber == 3) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = BfillWidth[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 2200.f;
        xt += xgap;
        *width = xt;
    }

/*
    Variable width EasyX smooth polygon font.
*/
    else if (FontNumber == 4) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = SfillWidth[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 2200.f;
        xt += xgap;
        *width = xt;
    }

/*
    Hershey font number 1.
*/
    else if (FontNumber == 101) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey1Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        xt += xgap;
        *width = xt;
    }

/*
    Hershey font number 2.
*/
    else if (FontNumber == 102) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey2Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        xt += xgap;
        *width = xt;
    }

/*
    Hershey font number 3.
*/
    else if (FontNumber == 103) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey3Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        xt += xgap;
        *width = xt;
    }

/*
    Hershey font number 4.
*/
    else if (FontNumber == 104) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey4Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        xt += xgap;
        *width = xt;
    }

/*
    Hershey font number 5.
*/
    else if (FontNumber == 105) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey5Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        xt += xgap;
        *width = xt;
    }

/*
    Hershey font number 6.
*/
    else if (FontNumber == 106) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey6Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        xt += xgap;
        *width = xt;
    }

/*
    Hershey font number 7.
*/
    else if (FontNumber == 107) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey7Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        xt += xgap;
        *width = xt;
    }

/*
    Hershey font number 8.
*/
    else if (FontNumber == 108) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey8Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        *width = xt;
    }

/*
    Hershey font number 9.
*/
    else if (FontNumber == 109) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey9Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        *width = xt;
    }

/*
    Hershey font number 10.
*/
    else if (FontNumber == 110) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey10Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        xt += xgap;
        *width = xt;
    }

/*
    Hershey font number 11.
*/
    else if (FontNumber == 111) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey11Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        xt += xgap;
        *width = xt;
    }

/*
    Hershey font number 12.
*/
    else if (FontNumber == 112) {
        xgap = GapFraction * size;
        index = (int)ichar - FIRSTCHAR;
        if (index < 0  ||  index > 128 - FIRSTCHAR) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        iwide = Hershey12Width[index];
        if (iwide <= 0) {
            *width = 0.f;
            if (ichar == ' '  ||  ichar == '\t'  ||  ichar == '\n') {
                *width = .7f * size;
            }
            return 1;
        }
        xt = size * (CSW_F)iwide / 210.f;
        xt += xgap;
        *width = xt;
    }

/*
    If the font is shadowed, include its shadow in the width.
*/
    if (ShadowNumber > 0) {
        *width = *width * ShadowThickMult;
    }

    return 1;

}  /*  end of function gpf_CharWidth  */


/*
******************************************************************

                   g p f _ f o n t x y l i m

******************************************************************

    Calculate the x,y limits of font strokes.

*/

int GPFFont::gpf_fontxylim (CSW_F *x, CSW_F *y, int n, 
                   CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2)
{
    int         i;
    CSW_F       xx1,yy1, xx2, yy2;

    xx1 = 1.e30f;
    yy1 = 1.e30f;
    xx2 = -1.e30f;
    yy2 = -1.e30f;

    for (i=0; i<n; i++) {
        if (x[i] < xx1) xx1 = x[i];
        if (x[i] > xx2) xx2 = x[i];
        if (y[i] < yy1) yy1 = y[i];
        if (y[i] > yy2) yy2 = y[i];
    }

    *x1 = xx1;
    *y1 = yy1;
    *x2 = xx2;
    *y2 = yy2;

    return 1;

}  /*  end of function gpf_fontxylim  */



/*
******************************************************************

                 g p f _ f o n t s c a l e x y

******************************************************************

    Scale the font strokes from one bounding rectangle to another.
    This is used to adjust the smoothed coordinates back to the
    same bounding box as the original character.

*/

int GPFFont::gpf_fontscalexy (CSW_F *x, CSW_F *y, int n, 
                     CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                     CSW_F xx1, CSW_F yy1, CSW_F xx2, CSW_F yy2)
{
    int          i;
    CSW_F        w1, w2, h1, h2, xt, yt;

    w1 = x2 - x1;
    w2 = xx2 - xx1;
    h1 = y2 - y1;
    h2 = yy2 - yy1;
    if (w1 < h1 / 1000.f  ||  h1 < w1 /1000.f  ||  
        w1 == 0.f  ||  h1 == 0.f) {
        return 1;
    }

    for (i=0; i<n; i++) {
        xt = (x[i] - x1) / w1;
        x[i] = (xt * w2) + xx1;
        yt = (y[i] - y1) / h1;
        y[i] = (yt * h2) + yy1;
    }

    return 1;

}  /*  end of function gpf_fontscalexy  */



/*
  ****************************************************************

              g p f _ g e t p o l y f o n t p t s

  ****************************************************************

  function name:    gpf_getpolyfontpts        (int)

  call sequence:    gpf_getpolyfontpts (charin, size,
                                        x, y, ncomp, icomp, ihole)

  purpose:          Retrieve the polygon data for a polygon font
                    character.   The most recently defined font
                    from gpf_setfont is used.

  return value:     status code

                    0 = error, either an illegal character or
                        the currently set font is not a polygon
                        font.
                    1 = normal successful completion

  calling parameters:

    charin     r    char     Character to retrieve.
    size       r    CSW_F    Character height in plotting units.
    x          w    CSW_F*   Array for x coordinates of polygons.
    y          w    CSW_F*   Array for y coordinates of polygons.
                             If x and y are dimensioned at least
                             for 1000 points each, all characters 
                             can be accommidated.
    ncomp      w    int*     Returned number of separate polygons
                             needed to draw the character.
    icomp      w    int*     Array to receive the number of holes
                             for each separate polygon.
    ihole      w    int*     Array to receive the number of points
                             for each hole.
                             Dimension icomp and ihole to at least 20 
                             to accommidate all characters.

*/

int GPFFont::gpf_getpolyfontpts (char charin, CSW_F size,
                        CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole)
{
    int         istat;

    istat = 0;
    
    if (FontNumber == 3) {
        istat = gpf_getbfillfont (charin, size, 
                                  x, y, ncomp, icomp, ihole);
    }
    
    else if (FontNumber == 4) {
        istat = gpf_getsfillfont (charin, size, 
                                  x, y, ncomp, icomp, ihole);
    }

    return istat;

}  /*  end of function gpf_getpolyfontpts  */



/*
  ****************************************************************

               g p f _ g e t s f i l l f o n t

  ****************************************************************

    Retrieve the polygon data for the smooth filled fonts.  The
    points are scaled and slanted but not rotated or translated
    to the text origin.  This is only called from gpf_getpolyfontpts.
    See this function for details on parameters.

    Returns zero if the specified character is not supported or 
    returns 1 on success.

*/

int GPFFont::gpf_getsfillfont (char charin, CSW_F size,
                      CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole)
{
    int         i, j, index, comp1, nc, hole1, nhole,
                nt, pt1, pt2, npt, nptot;
    CSW_F       sizeratio, xt, yt, yoffset;

/*
    if the character is 127, the degree sign is drawn.
    This is a lower case o drawn as a superscript
*/

    if ((int)charin == DEGREE_CHAR) {
        charin = 'o';
        yoffset = .75f * size;
        size *= 0.75f;
    }
    else {
        yoffset = 0.0f;
    }

/*  retrieve points from array  */

    nptot = 0;
    index = (int)charin - FIRSTCHAR;
    comp1 = SfillChar[index].firstcomp;
    nc = SfillChar[index].ncomp;
    nt = 0;

    for (i=0; i<nc; i++) {
        nhole = SfillComp[comp1+i].nholes;
        hole1 = SfillComp[comp1+i].firsthole;
        icomp[i] = nhole;

        for (j=0; j<nhole; j++) {
            npt = SfillHole[hole1+j].npts;
            pt1 = SfillHole[hole1+j].firstpt;
            pt1 *= 2;
            ihole[nt] = npt;
            nt++;
            pt2 = pt1 + npt * 2;

            while (pt1 < pt2) {
                x[nptot] = (CSW_F)SfillPoints[pt1];
                pt1++;
                y[nptot] = (CSW_F)SfillPoints[pt1] - 1000.f;
                pt1++;
                nptot++;
            }
        }
    }

    *ncomp = nc;

/*  scale and slant points  */

    sizeratio = size / 0.22f;
    if (sizeratio < MINSIZERATIO) {
        sizeratio = MINSIZERATIO;
    }
    sizeratio /= 10000.f;

    for (i=0; i<nptot; i++) {
        x[i] = x[i] * sizeratio;
        y[i] = y[i] * sizeratio + yoffset;
        xt = x[i];
        yt = y[i];
        x[i] = SlantX * yt + xt;;
        y[i] = SlantY * xt + yt;;
    }

    return 1;

}  /*  end of function gpf_getsfillfont  */



/*
  ****************************************************************

              g p f _ g e t f o n t s h a d o w

  ****************************************************************

  function name:    gpf_getfontshadow         (int)

  call sequence:    gpf_getfontshadow  (charin, size,
                                        x, y, ncomp, icomp, ihole)

  purpose:          Retrieve the shadow for a raised character if the 
                    current font is raised.

  return value:     status code

                    0 = error, either an illegal character or
                        the currently set font is not a raised
                        font.
                    1 = normal successful completion

  calling parameters:

    charin     r    char     Character to retrieve.
    size       r    CSW_F    Character height in plotting units.
    x          w    CSW_F*   Array for x coordinates of polygons.
    y          w    CSW_F*   Array for y coordinates of polygons.
                             If x and y are dimensioned at least
                             for 1000 points each, all characters 
                             can be accommadated.
    ncomp      w    int*     Returned number of separate polygons
                             needed to draw the character.
    icomp      w    int*     Array to receive the number of holes
                             for each separate polygon.
    ihole      w    int*     Array to receive the number of points
                             for each hole.
                             Dimension icomp and ihole to at least 20 
                             to accommidate all characters.

*/

int GPFFont::gpf_getfontshadow  (char charin, CSW_F size,
                        CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole)
{
    int         istat;

    istat = 0;

/*  no shadow available  */

    if (ShadowNumber == 0) {
        return 0;
    }

/*  left block shadow  */
    
    if (ShadowNumber == 1) {
        istat = gpf_getleftbfont (charin, size, 
                                  x, y, ncomp, icomp, ihole);
    }

/*  right block shadow  */
    
    else if (ShadowNumber == 2) {
        istat = gpf_getrightbfont (charin, size, 
                                  x, y, ncomp, icomp, ihole);
    }
   
/*  left smooth shadow  */
 
    else if (ShadowNumber == 3) {
        istat = gpf_getleftsfont (charin, size, 
                                  x, y, ncomp, icomp, ihole);
    }

/*  right smooth shadow  */
    
    else if (ShadowNumber == 4) {
        istat = gpf_getrightsfont (charin, size, 
                                  x, y, ncomp, icomp, ihole);
    }

    return istat;

}  /*  end of function gpf_getfontshadow  */



/*
  ****************************************************************

               g p f _ g e t l e f t s f o n t

  ****************************************************************

    Retrieve the polygon data for the left shadow on the smooth filled 
    fonts.  The points are scaled and slanted but not rotated or translated
    to the text origin.  This is only called from gpf_getfontshadow.
    See this function for details on parameters.

    Returns zero if the specified character is not supported or 
    returns 1 on success.

*/

int GPFFont::gpf_getleftsfont (char charin, CSW_F size,
                      CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole)
{
    int         i, j, index, comp1, nc, hole1, nhole,
                nt, pt1, pt2, npt, nptot;
    CSW_F       sizeratio, xt, yt, yoffset;;

/*
    if the character is 127, the degree sign is drawn.
    This is a lower case o drawn as a superscript
*/

    if ((int)charin == DEGREE_CHAR) {
        charin = 'o';
        yoffset = .75f * size;
        size *= 0.75f;
    }
    else {
        yoffset = 0.0f;
    }

/*  retrieve points from array  */

    nptot = 0;
    index = (int)charin - FIRSTCHAR;
    comp1 = LeftsChar[index].firstcomp;
    nc = LeftsChar[index].ncomp;
    nt = 0;

    for (i=0; i<nc; i++) {
        nhole = LeftsComp[comp1+i].nholes;
        hole1 = LeftsComp[comp1+i].firsthole;
        icomp[i] = nhole;

        for (j=0; j<nhole; j++) {
            npt = LeftsHole[hole1+j].npts;
            pt1 = LeftsHole[hole1+j].firstpt;
            pt1 *= 2;
            ihole[nt] = npt;
            nt++;
            pt2 = pt1 + npt * 2;

            while (pt1 < pt2) {
                x[nptot] = (CSW_F)LeftsPoints[pt1];
                pt1++;
                y[nptot] = (CSW_F)LeftsPoints[pt1] - 1000.f;
                pt1++;
                nptot++;
            }
        }
    }

    *ncomp = nc;

/*  scale and slant points  */

    sizeratio = size / 0.22f;
    if (sizeratio < MINSIZERATIO) {
        sizeratio = MINSIZERATIO;
    }
    sizeratio /= 10000.f;

    for (i=0; i<nptot; i++) {
        x[i] = x[i] * sizeratio;
        y[i] = y[i] * sizeratio + yoffset;
        xt = x[i];
        yt = y[i];
        x[i] = SlantX * yt + xt;;
        y[i] = SlantY * xt + yt;;
    }

    return 1;

}  /*  end of function gpf_getleftsfont  */



/*
  ****************************************************************

               g p f _ g e t r i g h t s f o n t

  ****************************************************************

    Retrieve the polygon data for the right shadow on the smooth filled 
    fonts.  The points are scaled and slanted but not rotated or translated
    to the text origin.  This is only called from gpf_getfontshadow.
    See this function for details on parameters.

    Returns zero if the specified character is not supported or 
    returns 1 on success.

*/

int GPFFont::gpf_getrightsfont (char charin, CSW_F size,
                       CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole)
{
    int         i, j, index, comp1, nc, hole1, nhole,
                nt, pt1, pt2, npt, nptot;
    CSW_F       sizeratio, xt, yt, yoffset;

/*
    if the character is 127, the degree sign is drawn.
    This is a lower case o drawn as a superscript
*/

    if ((int)charin == DEGREE_CHAR) {
        charin = 'o';
        yoffset = .75f * size;
        size *= 0.75f;
    }
    else {
        yoffset = 0.0f;
    }

/*  retrieve points from array  */

    nptot = 0;
    index = (int)charin - FIRSTCHAR;
    comp1 = RightsChar[index].firstcomp;
    nc = RightsChar[index].ncomp;
    nt = 0;

    for (i=0; i<nc; i++) {
        nhole = RightsComp[comp1+i].nholes;
        hole1 = RightsComp[comp1+i].firsthole;
        icomp[i] = nhole;

        for (j=0; j<nhole; j++) {
            npt = RightsHole[hole1+j].npts;
            pt1 = RightsHole[hole1+j].firstpt;
            pt1 *= 2;
            ihole[nt] = npt;
            nt++;
            pt2 = pt1 + npt * 2;

            while (pt1 < pt2) {
                x[nptot] = (CSW_F)RightsPoints[pt1];
                pt1++;
                y[nptot] = (CSW_F)RightsPoints[pt1] - 1000.f;
                pt1++;
                nptot++;
            }
        }
    }

    *ncomp = nc;

/*  scale and slant points  */

    sizeratio = size / 0.22f;
    if (sizeratio < MINSIZERATIO) {
        sizeratio = MINSIZERATIO;
    }
    sizeratio /= 10000.f;

    for (i=0; i<nptot; i++) {
        x[i] = x[i] * sizeratio;
        y[i] = y[i] * sizeratio + yoffset;
        xt = x[i];
        yt = y[i];
        x[i] = SlantX * yt + xt;;
        y[i] = SlantY * xt + yt;;
    }

    return 1;

}  /*  end of function gpf_getrightsfont  */



/*
  ****************************************************************

               g p f _ g e t b f i l l f o n t

  ****************************************************************

    Retrieve the polygon data for the block filled fonts.  The
    points are scaled and slanted but not rotated or translated
    to the text origin.  This is only called from gpf_getpolyfontpts.
    See this function for details on parameters.

    Returns zero if the specified character is not supported or 
    returns 1 on success.

*/

int GPFFont::gpf_getbfillfont (char charin, CSW_F size,
                      CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole)
{
    int         i, j, index, comp1, nc, hole1, nhole,
                nt, pt1, pt2, npt, nptot;
    CSW_F       sizeratio, xt, yt, yoffset;

/*
    if the character is 127, the degree sign is drawn.
    This is a lower case o drawn as a superscript
*/

    if ((int)charin == DEGREE_CHAR) {
        charin = 'o';
        yoffset = .75f * size;
        size *= 0.75f;
    }
    else {
        yoffset = 0.0f;
    }

/*  retrieve points from array  */

    nptot = 0;
    index = (int)charin - FIRSTCHAR;
    comp1 = BfillChar[index].firstcomp;
    nc = BfillChar[index].ncomp;
    nt = 0;

    for (i=0; i<nc; i++) {
        nhole = BfillComp[comp1+i].nholes;
        hole1 = BfillComp[comp1+i].firsthole;
        icomp[i] = nhole;

        for (j=0; j<nhole; j++) {
            npt = BfillHole[hole1+j].npts;
            pt1 = BfillHole[hole1+j].firstpt;
            pt1 *= 2;
            ihole[nt] = npt;
            nt++;
            pt2 = pt1 + npt * 2;

            while (pt1 < pt2) {
                x[nptot] = (CSW_F)BfillPoints[pt1];
                pt1++;
                y[nptot] = (CSW_F)BfillPoints[pt1] - 1000.f;
                pt1++;
                nptot++;
            }
        }
    }

    *ncomp = nc;

/*  scale and slant points  */

    sizeratio = size / 0.22f;
    if (sizeratio < MINSIZERATIO) {
        sizeratio = MINSIZERATIO;
    }
    sizeratio /= 10000.f;

    for (i=0; i<nptot; i++) {
        x[i] = x[i] * sizeratio;
        y[i] = y[i] * sizeratio + yoffset;
        xt = x[i];
        yt = y[i];
        x[i] = SlantX * yt + xt;;
        y[i] = SlantY * xt + yt;;
    }

    return 1;

}  /*  end of function gpf_getbfillfont  */



/*
  ****************************************************************

               g p f _ g e t l e f t b f o n t

  ****************************************************************

    Retrieve the polygon data for the left shadow on the block filled 
    fonts.  The points are scaled and slanted but not rotated or translated
    to the text origin.  This is only called from gpf_getfontshadow.
    See this function for details on parameters.

    Returns zero if the specified character is not supported or 
    returns 1 on success.

*/

int GPFFont::gpf_getleftbfont (char charin, CSW_F size,
                      CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole)
{
    int         i, j, index, comp1, nc, hole1, nhole,
                nt, pt1, pt2, npt, nptot;
    CSW_F       sizeratio, xt, yt, yoffset;

/*
    if the character is 127, the degree sign is drawn.
    This is a lower case o drawn as a superscript
*/

    if ((int)charin == DEGREE_CHAR) {
        charin = 'o';
        yoffset = .75f * size;
        size *= 0.75f;
    }
    else {
        yoffset = 0.0f;
    }

/*  retrieve points from array  */

    nptot = 0;
    index = (int)charin - FIRSTCHAR;
    comp1 = LeftbChar[index].firstcomp;
    nc = LeftbChar[index].ncomp;
    nt = 0;

    for (i=0; i<nc; i++) {
        nhole = LeftbComp[comp1+i].nholes;
        hole1 = LeftbComp[comp1+i].firsthole;
        icomp[i] = nhole;

        for (j=0; j<nhole; j++) {
            npt = LeftbHole[hole1+j].npts;
            pt1 = LeftbHole[hole1+j].firstpt;
            pt1 *= 2;
            ihole[nt] = npt;
            nt++;
            pt2 = pt1 + npt * 2;

            while (pt1 < pt2) {
                x[nptot] = (CSW_F)LeftbPoints[pt1];
                pt1++;
                y[nptot] = (CSW_F)LeftbPoints[pt1] - 1000.f;
                pt1++;
                nptot++;
            }
        }
    }

    *ncomp = nc;

/*  scale and slant points  */

    sizeratio = size / 0.22f;
    if (sizeratio < MINSIZERATIO) {
        sizeratio = MINSIZERATIO;
    }
    sizeratio /= 10000.f;

    for (i=0; i<nptot; i++) {
        x[i] = x[i] * sizeratio;
        y[i] = y[i] * sizeratio + yoffset;
        xt = x[i];
        yt = y[i];
        x[i] = SlantX * yt + xt;;
        y[i] = SlantY * xt + yt;;
    }

    return 1;

}  /*  end of function gpf_getleftbfont  */



/*
  ****************************************************************

               g p f _ g e t r i g h t b f o n t

  ****************************************************************

    Retrieve the polygon data for the right shadow on the block filled 
    fonts.  The points are scaled and slanted but not rotated or translated
    to the text origin.  This is only called from gpf_getfontshadow.
    See this function for details on parameters.

    Returns zero if the specified character is not supported or 
    returns 1 on success.

*/

int GPFFont::gpf_getrightbfont (char charin, CSW_F size,
                       CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole)
{
    int         i, j, index, comp1, nc, hole1, nhole,
                nt, pt1, pt2, npt, nptot;
    CSW_F       sizeratio, xt, yt, yoffset;

/*
    if the character is 127, the degree sign is drawn.
    This is a lower case o drawn as a superscript
*/

    if ((int)charin == DEGREE_CHAR) {
        charin = 'o';
        yoffset = .75f * size;
        size *= 0.75f;
    }
    else {
        yoffset = 0.0f;
    }

/*  retrieve points from array  */

    nptot = 0;
    index = (int)charin - FIRSTCHAR;
    comp1 = RightbChar[index].firstcomp;
    nc = RightbChar[index].ncomp;
    nt = 0;

    for (i=0; i<nc; i++) {
        nhole = RightbComp[comp1+i].nholes;
        hole1 = RightbComp[comp1+i].firsthole;
        icomp[i] = nhole;

        for (j=0; j<nhole; j++) {
            npt = RightbHole[hole1+j].npts;
            pt1 = RightbHole[hole1+j].firstpt;
            pt1 *= 2;
            ihole[nt] = npt;
            nt++;
            pt2 = pt1 + npt * 2;

            while (pt1 < pt2) {
                x[nptot] = (CSW_F)RightbPoints[pt1];
                pt1++;
                y[nptot] = (CSW_F)RightbPoints[pt1] - 1000.f;
                pt1++;
                nptot++;
            }
        }
    }

    *ncomp = nc;

/*  scale and slant points  */

    sizeratio = size / 0.22f;
    if (sizeratio < MINSIZERATIO) {
        sizeratio = MINSIZERATIO;
    }
    sizeratio /= 10000.f;

    for (i=0; i<nptot; i++) {
        x[i] = x[i] * sizeratio;
        y[i] = y[i] * sizeratio + yoffset;
        xt = x[i];
        yt = y[i];
        x[i] = SlantX * yt + xt;;
        y[i] = SlantY * xt + yt;;
    }

    return 1;

}  /*  end of function gpf_getrightbfont  */




/*
******************************************************************

               g p f _ g e t h e r s h e y f o n t 

******************************************************************

  function name:    gpf_gethersheyfont     (int)

  call sequence:    gpf_gethersheyfont (charin, size, x, y, parts, nparts,
                                        maxxy, maxparts)

  purpose:          Return the vector font strokes for the hershey font.
                    The vectors are returned as plot units relative to
                    the lower left corner of the character, after having
                    been scaled for size.

  return value:     status code

                    0 = the character is not supported in this font
                    1 = normal successful completion

  calling parameters:

    charin      r     char         Character to plot
    size        r     CSW_F        Size (height) in plotter units
    x           w     CSW_F*       Array which will receive x coordinates
    y           w     CSW_F*       Array which will receive y coordinates
    parts       w     int*         Array which will receive the number of points
                                   in each continuously drawn segment of the character
    nparts      w     int*         Number of segments packed into parts array 
    maxxy       r     int          Maximum size of x and y output arrays.
    maxparts    r     int          Maximum size of parts array.

*/

int GPFFont::gpf_gethersheyfont (char charin, CSW_F size, 
                        CSW_F *x, CSW_F *y, int *parts, int *nparts, 
                        int maxxy, int maxparts)
{
    int         i, j, offset, npts, n, ntotal;
    int         nout, npartmax;
    FONTINT     it;
    const FONTINT  *now, *localvector;
    CSW_F       sizeratio, xt, yt, dres, yoffset;
    CSW_F       *xout, *yout, sres;

    GPFSpline   gpf_spline_obj;

/*
    if the character is 127, the degree sign is drawn.
    This is a lower case o drawn as a superscript
*/
    if ((int)charin == DEGREE_CHAR) {
        charin = 'o';
        yoffset = .75f * size;
        size *= 0.75f;
    }
    else {
        yoffset = 0.0f;
    }

/*
    find the offset and number of points in the font vector array
*/
    i = (int)charin - FIRSTCHAR;
    if (i < 0  ||  i > (128-FIRSTCHAR)) {
        return 0;
    }

    offset = -1;
    npts = -1;
    localvector = NULL;

    if (FontNumber == 101) {
        offset = Hershey1List[i].offset;
        npts = Hershey1List[i].npts;
        localvector = Hershey1Vector;
    }
    if (FontNumber == 102) {
        offset = Hershey2List[i].offset;
        npts = Hershey2List[i].npts;
        localvector = Hershey2Vector;
    }
    if (FontNumber == 103) {
        offset = Hershey3List[i].offset;
        npts = Hershey3List[i].npts;
        localvector = Hershey3Vector;
    }
    if (FontNumber == 104) {
        offset = Hershey4List[i].offset;
        npts = Hershey4List[i].npts;
        localvector = Hershey4Vector;
    }
    if (FontNumber == 105) {
        offset = Hershey5List[i].offset;
        npts = Hershey5List[i].npts;
        localvector = Hershey5Vector;
    }
    if (FontNumber == 106) {
        offset = Hershey6List[i].offset;
        npts = Hershey6List[i].npts;
        localvector = Hershey6Vector;
    }
    if (FontNumber == 107) {
        offset = Hershey7List[i].offset;
        npts = Hershey7List[i].npts;
        localvector = Hershey7Vector;
    }
    if (FontNumber == 108) {
        offset = Hershey8List[i].offset;
        npts = Hershey8List[i].npts;
        localvector = Hershey8Vector;
    }
    if (FontNumber == 109) {
        offset = Hershey9List[i].offset;
        npts = Hershey9List[i].npts;
        localvector = Hershey9Vector;
    }
    if (FontNumber == 110) {
        offset = Hershey10List[i].offset;
        npts = Hershey10List[i].npts;
        localvector = Hershey10Vector;
    }
    if (FontNumber == 111) {
        offset = Hershey11List[i].offset;
        npts = Hershey11List[i].npts;
        localvector = Hershey11Vector;
    }
    if (FontNumber == 112) {
        offset = Hershey12List[i].offset;
        npts = Hershey12List[i].npts;
        localvector = Hershey12Vector;
    }

    if (localvector == NULL  ||  offset < 0  ||  npts <= 1) {
        return 0;
    }

    sizeratio = size / 0.1f;
    if (sizeratio < MINSIZERATIO) {
        sizeratio = MINSIZERATIO;
    }

    now = localvector + offset;

/*
    rescale the points and output in the x,y arrays
*/
    n = 0;
    parts[0] = 0;
    npts /= 2;
    for (j=0; j<npts; j++) {
        it = *now;
        if (it < 0) {
            if (j > 0) {
                n++;
                if (n >= maxparts) {
                    return 0;
                }
                parts[n] = 0;
            }
            it = (FONTINT)-it;
        }
        x[j] = (CSW_F)it / 210.f * size;
        now++;
        it = *now;
        if (it < 0) {
            it = (FONTINT)-it;
        }
        y[j] = (CSW_F)(it -100)/ 210.f * size;
        y[j] += yoffset;

    /*
        slant the vectors if needed
    */
        xt = x[j];
        yt = y[j];

        x[j] = SlantX * yt + xt;
        y[j] = SlantY * xt + yt;

        (parts[n])++;
        now++;
    }

    *nparts = n + 1;

/*
    fit spline to make curved font if needed
*/
    offset = 0;
    xout = x;
    yout = y;
    if (FontCurve  &&  size > 0.13f) {
        maxxy -= *nparts;
        maxxy -= 10;
        gpf_fontxylim (x, y, npts, &Ox1, &Oy1, &Ox2, &Oy2);
        for (j=0; j<npts; j++) {
            Xwork[j] = x[j];
            Ywork[j] = y[j];
        }
        sres = size / 20.f;
        if (sres < 0.02f) {
            sres = 0.02f;
        }
        ntotal = 0;
        for (i=0; i<*nparts; i++) {
            npartmax = maxxy * parts[i] / npts;
            dres = sres;
            if (parts[i] == 5) {
                if (gpf_CheckForFontDot (Xwork+offset, Ywork+offset, size)) {
                    dres = sres / 5.f;
                }
            }
            gpf_spline_obj.gpf_SplineFit (Xwork+offset, Ywork+offset, parts[i], 0,
                           xout, yout, npartmax, dres, 
                           &nout);
            offset += parts[i];
            parts[i] = nout;
            ntotal += nout;
            xout += nout;
            yout += nout;
        }

    /*
        rescale to same height as original unsmoothed character
    */
        gpf_fontxylim (x, y, ntotal, &X1, &Y1, &X2, &Y2);
        gpf_fontscalexy (x, y, ntotal,
                         X1, Y1, X2, Y2,
                         Ox1, Oy1, Ox2, Oy2);

    }

/*
    connect segments that have common end points
*/
    gpf_ConnectFontStrokes (parts, nparts, x, y, size);

    return 1;

}  /*  end of function gpf_gethersheyfont  */





int gpf_cctlen2 (const char *textin, int nc, int font,
                 CSW_F size, CSW_F *len)
{
    GPFFont    fobj;

    int   istat;
    istat = fobj.gpf_TextLength2 (textin, nc, font, size, len);
    return istat;
}





/*
******************************************************************

                 g p f _ T e x t L e n g t h 2

******************************************************************

    Return the length in size units (usually inches) of a text string.
  This function returns the same information as gpf_textLength, found
  in gpf_font.c.  However, only the character width information is
  included in this file so that the application programs that call this
  function will not be bloated with all of the unneeded font vectors.

*/

int GPFFont::gpf_TextLength2 (const char *textin, int nc, int font,
                     CSW_F size, CSW_F *len)
{
    int        i, ilen, index, iwide, fnum, nc2;
    CSW_F      xt, xt2, xgap;
    char       c1, cwork[500], *text;

    if (font >= 0  &&  font <= Numvfont) {
        fnum = VFontList[font].fontnum;
    }
    else if (font >= MIN_HERSHEY  &&  font <= MAX_HERSHEY) {
        fnum = HFontList[font-101].fontnum;
    }
    else {
        fnum = -1;
    }

    if (fnum == -1) {
        return -1;
    }

/*
    strip white space from right side if
    the input string is less than 500 characters.
*/
    if (nc < 499) {
        csw_StrTruncate (cwork, textin, 500);
        csw_StrRightStrip (cwork);
        nc2 = strlen (cwork);
        if (nc2 < nc) nc = nc2;
        text = cwork;
    }
    else {
        text = (char *)textin;
    }

/*
    Use the simple easyx constant width font.
*/
    if (fnum == 1) {
        *len = nc * size * .9f;
    }

/*
    Use the variable width complex stroke font.
*/
    else if (fnum == 2) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = ComplexWidth[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 100.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the variable width block polygon font.
*/
    else if (fnum == 3) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = BfillWidth[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 2200.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the variable width smooth polygon font.
*/
    else if (fnum == 4) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = SfillWidth[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 2200.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 1.
*/
    else if (fnum == 101) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey1Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 2.
*/
    else if (fnum == 102) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey2Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 3.
*/
    else if (fnum == 103) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey3Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 4.
*/
    else if (fnum == 104) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey4Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 5.
*/
    else if (fnum == 105) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey5Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 6.
*/
    else if (fnum == 106) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey6Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 7.
*/
    else if (fnum == 107) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey7Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 8.
*/
    else if (fnum == 108) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey8Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 9.
*/
    else if (fnum == 109) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey9Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 10.
*/
    else if (fnum == 110) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey10Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 11.
*/
    else if (fnum == 111) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey11Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

/*
    Use the widths for hershey font 12.
*/
    else if (fnum == 112) {
        ilen = strlen (text);
        if (ilen < nc) nc = ilen;
        xt = 0.0f;
        xgap = GapFraction * size;
        for (i=0; i<nc; i++) {
            c1 = text[i];
            index = (int)c1 - FIRSTCHAR;
            if (index < 0  ||  index > 128 - FIRSTCHAR) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            iwide = Hershey12Width[index];
            if (iwide <= 0) {
                if (c1 == ' '  ||  c1 == '\t'  ||  c1 == '\n') {
                    xt2 = size * .7f;
                    xt += xt2;
                    xt += xgap;
                }
                continue;
            }
            xt2 = size * (CSW_F)iwide / 210.f;
            xt += xt2;
            xt += xgap;
        }
        *len = xt - xgap;
    }

    return 1;

}  /*  end of function gpf_TextLength2  */
