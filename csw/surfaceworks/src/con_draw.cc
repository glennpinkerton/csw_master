
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    con_draw.cc

    This file has CCWConDraw class methods to create drawable primitives
    from a calculated contour line.  The contour lines can be colored,
    various thicknesses, dashed or solid.  They can have in line
    labels and tick marks pointing uphill or downhill.

    The functions in this file take the contour line coordinates
    and calculate a set of line and text primitives that can be used
    to draw the contour.
*/

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/simulP.h"

#include "csw/utils/private_include/gpf_font.h"
#include "csw/utils/private_include/gpf_utils.h"

#include "csw/surfaceworks/private_include/con_draw.h"




/*
  *****************************************************************************

                    c o n _ s e t _ d r a w _ s c a l e

  *****************************************************************************

  Set the corners of the contour area in grid units and plot units.  These
  are used to set scaling factors and origins that will be used to convert
  contour lines into drawable primitives.

*/

int CSWConDraw::con_set_draw_scale
     (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
      CSW_F px1, CSW_F py1, CSW_F px2, CSW_F py2,
      int dflag)
{

    if (x2 <= x1  ||  y2 <= y1  ||  px2 <= px1  ||  py2 <= py1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    dflag = dflag;

/*
    If the program is in parameter error simulation mode,
    return success (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

    Xmin = x1;
    Ymin = y1;
    Pxmin = px1;
    Pymin = py1;
    Pxmax = px2;
    Pymax = py2;

    Tiny = (Pxmax - Pxmin + Pymax - Pymin) / 100000.0f;

    Scalex = (px2 - px1) / (x2 - x1);
    Scaley = (py2 - py1) / (y2 - y1);

    return 1;

}  /*  end of function con_set_draw_scale  */





/*
  *******************************************************************************

                  c o n _ s e t _ d r a w _ o p t i o n

  *******************************************************************************

    Set a parameter to be used for subsequent contour drawing or reset all
  parameters to their default values.

*/

int CSWConDraw::con_set_draw_option (int tag, int ival, CSW_F fval)
{

    if (tag == CON_DEFAULT_OPTIONS) {

        OptMajorTickSpacing = 0.0f;
        OptMajorTextSpacing = 10.0f;
        OptMajorTickLen = 0.07f;
        OptMajorTextSize = 0.1f;
        OptMinorTickSpacing = 0.0f;
        OptMinorTextSpacing = 0.0f;
        OptMinorTickLen = 0.04f;
        OptMinorTextSize = 0.07f;
        OptMinorTextFont = 2;
        OptMajorTextFont = 2;
        OptTickDirection = 1;

    }

    else if (tag == CON_MAJOR_TICK_SPACING) {
        OptMajorTickSpacing = fval;
    }

    else if (tag == CON_MAJOR_TEXT_SPACING) {
        OptMajorTextSpacing = fval;
    }

    else if (tag == CON_MAJOR_TICK_LEN) {
        OptMajorTickLen = fval;
    }

    else if (tag == CON_MAJOR_TEXT_SIZE) {
        OptMajorTextSize = fval;
    }

    else if (tag == CON_MINOR_TICK_SPACING) {
        OptMinorTickSpacing = fval;
    }

    else if (tag == CON_MINOR_TEXT_SPACING) {
        OptMinorTextSpacing = fval;
    }

    else if (tag == CON_MINOR_TICK_LEN) {
        OptMinorTickLen = fval;
    }

    else if (tag == CON_MINOR_TEXT_SIZE) {
        OptMinorTextSize = fval;
    }

    else if (tag == CON_MINOR_TEXT_FONT) {
        OptMinorTextFont = ival;
    }

    else if (tag == CON_MAJOR_TEXT_FONT) {
        OptMajorTextFont = ival;
    }

    else if (tag == CON_TICK_DIRECTION) {
        OptTickDirection = ival;
    }

    else {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    return 1;

}  /*  end of function con_set_draw_option  */




/*
  *******************************************************************************

               c o n _ s e t _ d r a w _ o p t i o n s

  *******************************************************************************

    Set all contour drawing options to the values specified in the COntourDrawOptions
  structure.

*/

int CSWConDraw::con_set_draw_options (COntourDrawOptions *options)
{

    con_set_draw_option (CON_MAJOR_TICK_SPACING, 0, options->major_tick_spacing);
    con_set_draw_option (CON_MAJOR_TEXT_SPACING, 0, options->major_text_spacing);
    con_set_draw_option (CON_MAJOR_TICK_LEN, 0, options->major_tick_len);
    con_set_draw_option (CON_MAJOR_TEXT_SIZE, 0, options->major_text_size);
    con_set_draw_option (CON_MINOR_TICK_SPACING, 0, options->minor_tick_spacing);
    con_set_draw_option (CON_MINOR_TEXT_SPACING, 0, options->minor_text_spacing);
    con_set_draw_option (CON_MINOR_TICK_LEN, 0, options->minor_tick_len);
    con_set_draw_option (CON_MINOR_TEXT_SIZE, 0, options->minor_text_size);
    con_set_draw_option (CON_MINOR_TEXT_FONT, options->minor_text_font, 0.0f);
    con_set_draw_option (CON_MAJOR_TEXT_FONT, options->major_text_font, 0.0f);
    con_set_draw_option (CON_TICK_DIRECTION, options->tick_direction, 0.0f);

    return 1;

}  /*  end of function con_set_draw_options  */





/*
  *******************************************************************************

               c o n _ d e f a u l t  _ d r a w _ o p t i o n s

  *******************************************************************************

    Set all members of a contour drawing options to their default values.

*/

int CSWConDraw::con_default_draw_options (COntourDrawOptions *options)
{

    options->major_tick_spacing = 0.0f;
    options->major_text_spacing = 10.0f;
    options->major_tick_len = 0.07f;
    options->major_text_size = 0.10f;
    options->minor_tick_spacing = 0.0f;
    options->minor_text_spacing = 0.0f;
    options->minor_tick_len = 0.04f;
    options->minor_text_size = 0.07f;
    options->major_text_font = 2;
    options->minor_text_font = 2;
    options->tick_direction = 1;

    options->text_append[0] = '\0';
    options->text_prepend[0] = '\0';

    options->error_number = 0;

    return 1;

}  /*  end of function con_default_draw_options  */







/*
  ***********************************************************************

                     c o n _ d r a w _ l i n e

  ***********************************************************************

    Calculate line and text primitives for a contour line.

*/

int CSWConDraw::con_draw_line
                  (COntourOutputRec *cptr, CSW_F *char_widths,
                   COntourLinePrim **lptr, int *nline,
                   COntourTextPrim **tptr, int *ntext,
                   COntourDrawOptions *options)
{
    int              istat, i, npts, nchar, nt;
    CSW_F            *xcon = NULL, *ycon = NULL, dx, dy, tspace, textlength, tfact,
                     delta, tspace5, dtest1, dtest2, ctext,
                     curvemax, dbest, dtot;

/*
    obvious errors
*/
    if (!cptr || !lptr || !nline || !tptr || !ntext) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    initialize the return data pointers in case errors are encountered.
*/
    *lptr = NULL;
    *nline = 0;
    *tptr = NULL;
    *ntext = 0;

/*
    error if no scale has been set yet
*/
    if (Scalex <= 0.0f  ||  Scaley <= 0.0f) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

/*
    There should never be less than 2 points in the line,
    but just in case.
*/
    if (cptr->npts < 2) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

/*
    Assign the options either from the global options or
    from the specified option structure.
*/
    if (options) {
        MajorTickSpacing = options->major_tick_spacing;
        MajorTextSpacing = options->major_text_spacing;
        MajorTickLen = options->major_tick_len;
        MajorTextSize = options->major_text_size;
        MinorTickSpacing = options->minor_tick_spacing;
        MinorTextSpacing = options->minor_text_spacing;
        MinorTickLen = options->minor_tick_len;
        MinorTextSize = options->minor_text_size;
        MinorTextFont = options->minor_text_font;
        MajorTextFont = options->major_text_font;
        TickDirection = options->tick_direction;
    }

    else {
        MajorTickSpacing = OptMajorTickSpacing;
        MajorTextSpacing = OptMajorTextSpacing;
        MajorTickLen = OptMajorTickLen;
        MajorTextSize = OptMajorTextSize;
        MinorTickSpacing = OptMinorTickSpacing;
        MinorTextSpacing = OptMinorTextSpacing;
        MinorTickLen = OptMinorTickLen;
        MinorTextSize = OptMinorTextSize;
        MinorTextFont = OptMinorTextFont;
        MajorTextFont = OptMajorTextFont;
        TickDirection = OptTickDirection;
    }

/*
    Assign the appropriate major or minor attributes.
*/
    MajorFlag = cptr->major;
    if (MajorFlag) {
        TickSpacing = MajorTickSpacing;
        TickLen = MajorTickLen;
        TextSpacing = MajorTextSpacing * 1.5f;
        TextSize = MajorTextSize;
        TextFont = MajorTextFont;
    }
    else {
        TickSpacing = MinorTickSpacing;
        TickLen = MinorTickLen;
        TextSpacing = MinorTextSpacing * 1.5f;
        TextSize = MinorTextSize;
        TextFont = MinorTextFont;
    }
    FaultFlag = 0;
    if (cptr->faultflag) {
        MajorFlag = 0;
        TickSpacing = -1.0f;
        TickLen = -1.0f;
        TextSpacing = -1.0f;
        TextSize = -1.0f;
        FaultFlag = 1;
    }

    TickLeftRight = TickDirection * cptr->downhill;
    if (TickDirection == 0) {
        TickSpacing = -1.0f;
        TickLen = -1.0f;
    }

/*
    The line primitives output will always have textflag = 0
*/
    TextFlag = 0;

/*
    assign local variables for contour structure stuff
*/
    npts = cptr->npts;
    xcon = cptr->x;
    ycon = cptr->y;

    nchar = strlen (cptr->text);
    if (nchar < MAX_CONTOUR_LABEL_SIZE - 1) {
        strcpy (TextLabel, cptr->text);
    }
    else {
        strncpy (TextLabel, cptr->text, MAX_CONTOUR_LABEL_SIZE-1);
        TextLabel[MAX_CONTOUR_LABEL_SIZE-1] = '\0';
    }

/*
    put char_widths into the CharWidths array if possible.
*/
    if (char_widths) {
        i = 0;
        NcharWidths = 0;
        while (char_widths[i] > 0.0f) {
            CharWidths[i] = char_widths[i];
            i++;
            NcharWidths++;
            if (i >= MAX_CONTOUR_LABEL_SIZE-1) break;
        }
    }

/*
    use the text, font and size to populate the character width array
*/
    else {
        CalcCharWidths (cptr->text);
    }

/*
    Sum up the text length;
*/
    textlength = 0.0f;
    for (i=0; i<nchar; i++) {
        if (i < NcharWidths) {
            textlength += CharWidths[i];
        }
        else {
            textlength += TextSize;
        }
    }
    if (nchar > 0) {
        textlength += TextSize;
    }
/*
    Initialize output stuff.
*/
    OutputLineRecs = NULL;
    NoutputLine = 0;
    MaxOutputLine = 0;
    OutputTextRecs = NULL;
    NoutputText = 0;
    MaxOutputText = 0;

/*
    Allocate memory for Xline, Yline and LineDistances
    work spaces if needed.  I allocate space for 100
    extra points in case a whole lot of very short
    contours get processed first.  This eliminates a
    large number of csw_Malloc calls for the short contour
    case.
*/
    if (npts >= MaxLine) {
        if (Xline) csw_Free (Xline);
        Xline = NULL;
    }

    if (Xline == NULL) {
MSL
        Xline = (CSW_F *)csw_Malloc ((npts+100)*5*sizeof(CSW_F));
        if (!Xline) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        Yline = Xline + npts + 100;
        LineDistance = Yline + npts + 100;
        Xwork = LineDistance + npts + 100;
        Ywork = Xwork + npts + 100;
        MaxLine = npts + 100;
    }

/*
    Scale the contour into the page units and store in
    the Xline, Yline arrays.
*/
    Nline = npts;
    for (i=0; i<Nline; i++) {
        Xline[i] = Pxmin + (xcon[i] - Xmin) * Scalex;
        Yline[i] = Pymin + (ycon[i] - Ymin) * Scaley;
    }

/*
    Calculate accumulated distance along the contour
    at each point.
*/
    SetPointDistances ();

/*
    Is the contour closed?
*/
    dx = Xline[0] - Xline[Nline-1];
    dy = Yline[0] - Yline[Nline-1];
    if (dx < 0.0f) dx = -dx;
    if (dy < 0.0f) dy = -dy;

    if (dx < Tiny  &&  dy < Tiny) {
        ClosedContour = 1;
    }
    else {
        ClosedContour = 0;
    }

/*
    If the text length is longer than 5 times the whole line
    length or shorter than 1/10000th of the total line length,
    assume the character size is not set correctly
    and adjust the text length.
*/
    dtot = LineDistance[Nline-1];
    if (nchar > 0) {
        if (ClosedContour) {
            if (textlength > 5.0 * dtot  ||  textlength < .00001 * dtot) {
                tfact = .05f * dtot / textlength;
                TextSize *= tfact;
                for (i=0; i<NcharWidths; i++) {
                    CharWidths[i] *= tfact;
                }
                textlength = .05f * dtot;
            }
        }
        else {
            if (textlength > 5.0 * dtot  ||  textlength < .00001 * dtot) {
                tfact = .05f * dtot / textlength;
                TextSize *= tfact;
                for (i=0; i<NcharWidths; i++) {
                    CharWidths[i] *= tfact;
                }
                textlength = .05f * dtot;
            }
        }
    }
    else {
        textlength = .00002f * dtot;
    }

/*
    Adjust the text spacing depending on the closure
    and total length of the contour line.
*/
    tspace = TextSpacing;
    nt = 0;
    if (tspace > 0.0f) {

        if (nchar == 0  ||  textlength >= dtot) {
            tspace = 0.0f;
        }
        else {
            if (ClosedContour == 0  &&  LineDistance[Nline-1] < TextSpacing * .2f) {
                tspace = 0.0f;
            }
            else if (ClosedContour == 1  &&  LineDistance[Nline-1] < TextSpacing * .4f) {
                tspace = 0.0f;
            }
            else {
                nt = (int) (LineDistance[Nline-1] / TextSpacing + .5f);
                if (nt < 1) nt = 1;
                if (nt > MAX_TEXT_LABELS) nt = MAX_TEXT_LABELS;
                tspace = LineDistance[Nline-1] / (CSW_F)(nt + 1);
            }
        }
    }

/*
    If no label or tick marks are needed, the
    Xline and Yline arrays represent the line
    primitive and should be output.
*/
    if (tspace <= 0.0f  &&  TickSpacing <= 0.0f) {
        istat = OutputWholeLine ();
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            FreeOutputMem ();
            return -1;
        }
        *lptr = OutputLineRecs;
        *tptr = NULL;
        *nline = 1;
        *ntext = 0;
        return 1;
    }

/*
    Calculate ticks for the whole line if no label is desired.
*/
    if (tspace <= 0.0f  &&  TickSpacing > 0.0f) {
        if (LineDistance [Nline-1] >= TickSpacing * 0.1f) {
            istat = GenerateTicks (0.0f, LineDistance[Nline-1]);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                FreeOutputMem ();
                return -1;
            }
        }
        istat = OutputWholeLine ();
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            FreeOutputMem ();
            return -1;
        }
        *lptr = OutputLineRecs;
        *tptr = NULL;
        *nline = NoutputLine;
        *ntext = 0;
        return 1;
    }

/*
    Calculate start and end distances for text label gaps.
    The algorithm tries to locate these in straight sections
    of the contour near the original spacings specified.
*/
    dbest = 1.e30f;
    tspace5 = tspace / 5.0f;
    if (nt == 1  &&  ClosedContour) tspace5 *= 2.0f;
    if (tspace5 < textlength * 2.0f) tspace5 = textlength * 2.0f;
    if (tspace5 > tspace * .9f) tspace5 = tspace * .9f;
    delta = textlength / 2.0f;
    if (delta < tspace / 20.0f) delta = tspace / 20.0f;
    for (i=0; i<nt; i++) {
        curvemax = 1.e30f;
        dtest1 = tspace * (i + 1) - tspace5;
        for (;;) {
            dtest2 = dtest1 + textlength;
            CheckCurvature (dtest1, dtest2, &ctext);
            if (ctext < curvemax) {
                curvemax = ctext;
                dbest = dtest1;
            }
            dtest1 += delta;
            if (dtest1 + textlength > tspace*(i+1)+tspace5) {
                break;
            }
        }
        TextGap1[i] = dbest;
        TextGap2[i] = dbest + textlength;
        if (TextGap2[i] > dtot) TextGap2[i] = dtot;
    }

    NtextSeg = nt;

/*
    For a labelled line, separate segments between labels
    and treat each as a separate line primitive.  If ticks
    are desired, do the ticks on each segment separately.
*/
    istat = SeparateLabelSegments ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        FreeOutputMem ();
        return -1;
    }

/*
    Position each text character along the curve in
    each of the label gaps.
*/
    PositionLabels ();

/*
    All done.
*/
    *lptr = OutputLineRecs;
    *tptr = OutputTextRecs;
    *nline = NoutputLine;
    *ntext = NoutputText;

    OutputLineRecs = NULL;
    OutputTextRecs = NULL;
    NoutputLine = 0;
    NoutputText = 0;

    return 1;

}  /*  end of function con_draw_line  */







/*
  ****************************************************************

                 c o n _ f r e e _ d r a w i n g

  ****************************************************************

    Free the line and text data returned by calling con_draw_line
  at an earlier time.  You must use this function to csw_Free these
  data rather than simply csw_Freeing the pointers returned by con_draw_line.
  If you do not use this function, then a memory leak will occur.

*/

int CSWConDraw::con_free_drawing
                     (COntourLinePrim *lines, int nlines,
                      COntourTextPrim *text, int ntext,
                      COntourFillPrim *fills, int nfills)
{
    int                 i;
    COntourLinePrim     *lptr;
    COntourTextPrim     *tptr;

    nfills = nfills;

    if (lines) {
        for (i=0; i<nlines; i++) {
            lptr = lines+i;
            if (lptr->x) csw_Free (lptr->x);
        }
        csw_Free (lines);
    }

    if (text) {
        for (i=0; i<ntext; i++) {
            tptr = text+i;
            if (tptr->text) csw_Free (tptr->text);
            if (tptr->x) csw_Free (tptr->x);
        }
        csw_Free (text);
    }

    if (fills) {
        csw_Free (fills);
    }

    return 1;

}  /*  end of function con_free_drawing  */









/*
  ************************************************************************

                      S e t P o i n t D i s t a n c e s

  ************************************************************************

    Calculate distance along the line at every point.

*/

int CSWConDraw::SetPointDistances (void)
{
    int           i;
    CSW_F     dx, dy, dist, dbase;

    dbase = 0.0f;
    LineDistance[0] = 0.0f;

    for (i=1; i<Nline; i++) {

        dx = Xline[i] - Xline[i-1];
        dy = Yline[i] - Yline[i-1];
        dist = dx * dx + dy * dy;
        dist = (CSW_F)sqrt ((double)dist);
        dbase += dist;
        LineDistance[i] = dbase;

    }

    return 1;

}  /*  end of private SetPointDistances function  */





/*
  *****************************************************************************

                        P o i n t A t D i s t a n c e

  *****************************************************************************

    Return the x,y location, slope and angle of a point the specified distance
  along the current LineDistance line.  The index of the line point just prior
  to the distance is returned in nextibase.

*/

int CSWConDraw::PointAtDistance
                           (int ibase, CSW_F dist,
                            CSW_F *x, CSW_F *y, CSW_F *angle, CSW_F *slope,
                            int *nextibase)
{
    int           i, i1, i2;
    CSW_F     dx, dy, d1, d2, pct;

    i1 = Nline - 2;
    i2 = i1 + 1;
    for (i=ibase; i<Nline; i++) {
        if (dist < LineDistance[i]) {
            i1 = i - 1;
            i2 = i;
            break;
        }
    }

    d1 = LineDistance[i1];
    d2 = LineDistance[i2];

    if (d2 - d1 == 0.0f)
        pct = 0.0f;
    else
        pct = (dist - d1) / (d2 - d1);

    dx = Xline[i2] - Xline[i1];
    dy = Yline[i2] - Yline[i1];

    *angle = (CSW_F)atan2 ((double)dy, (double)dx);
    if (dx == 0.0f) {
        *slope = 1.e30f;
    }
    else {
        *slope = dy / dx;
    }

    dx *= pct;
    dy *= pct;

    *x = Xline[i1] + dx;
    *y = Yline[i1] + dy;

    *nextibase = i1;

    return 1;

}  /*  end of private PointAtDistance function  */




/*
  ******************************************************************

          P e r p e n d i c u l a r I n t e r s e c t

  ******************************************************************

    Calculate the point that lies on the line segment perpendicular
  from the specified point (off the line segment).

*/

int CSWConDraw::PerpendicularIntersect
                                  (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                                   CSW_F x, CSW_F y, CSW_F *xp, CSW_F *yp)
{
    CSW_F  dx, dy, s1, s2, tmp1, tmp2, divide, b1, b2;

    divide = 10000.f;

/*  adjust so neither line is exactly horizontal or vertical  */

    dx = x2 - x1;
    dy = y2 - y1;
    tmp1 = dx;
    if (dx < 0.0f) {
        tmp1 = -tmp1;
    }
    tmp2 = dy;
    if (dy < 0.0f) {
        tmp2 = -tmp2;
    }

    if (tmp1 == 0.0f  &&  tmp2 == 0.0f) {
        *xp = x1;
        *yp = y1;
        return 1;
    }

    if (tmp1 < tmp2/divide) {
        if (dx < 0.0f) {
            dx = -tmp2/divide;
        }
        else {
            dx = tmp2/divide;
        }
    }

    if (tmp2 < tmp1/divide) {
        if (dy < 0.0f) {
            dy = -tmp1/divide;
        }
        else {
            dy = tmp1/divide;
        }
    }

/*  intersect a perpendicular from the target with the segment  */

    s1 = dy / dx;
    s2 = -1.f / s1;
    b1 = y1 - s1 * x1;
    b2 = y - s2 * x;

    *xp = (b1 - b2) / (s2 - s1);
    *yp = s1 * *xp + b1;

/*  check if the intersection point is inside the segment endpoints  */

    dx = *xp;
    dy = *yp;

    if ( (x1-dx)*(dx-x2) >= 0.0f  ||  (y1-dy)*(dy-y2) >= 0.0f) {
        return 1;
    }
    else {
        return 0;
    }

}  /*  end of private PerpendicularIntersect function  */





/*
  *********************************************************************

                P e r p e n d i c u l a r P o i n t

  *********************************************************************

    Calculate a point perpendicular to the line at the point xanc, yanc
  (which lies on the line).  The point will be dist away from the line.
  For direction = 1, the point is to the right.  For -1, to the left.

*/

int CSWConDraw::PerpendicularPoint
                              (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                               CSW_F xanc, CSW_F yanc, int direction,
                               CSW_F dist, CSW_F *xp, CSW_F *yp)
{
    CSW_F        dx, dy, angle;

    dx = x2 - x1;
    dy = y2 - y1;
    angle = (CSW_F) atan2 ((double)dy, (double)dx);
    angle += 1.57078f * direction;

    *xp = xanc + (CSW_F) cos((double)angle) * dist;
    *yp = yanc + (CSW_F) sin((double)angle) * dist;

    return 1;

}  /*  end of private PerpendicularPoint function  */




/*
  ************************************************************************

                        C h e c k C u r v a t u r e

  ************************************************************************

    Calculate the maximum distance between a line segment connecting
  the two distance points and then divide it by the distance between
  the points.  This gives an indication of curvature over the distance
  interval.

    If any of the curve vectors intersect the base line, then the
  curvature is multiplied by the number of intersections to simulate
  a more curved segment.

*/

int CSWConDraw::CheckCurvature (CSW_F dist1, CSW_F dist2, CSW_F *curvemax)
{
    CSW_F    x1, y1, x2, y2, dum1, dum2, ldist,
                 xp, yp, dist, maxdist, dx, dy;
    CSW_F    xt1, yt1, xt2, yt2, tiny, a, b;
    int          i, i1, i2, nint;

    *curvemax = 1.e30f;

    if (dist1 <= 0.0f  ||  dist2 <= 0.0f) {
        return 1;
    }

    tiny = dist1 / 2000.f;
    if (dist2 > dist1) {
        tiny = dist2 / 2000.0f;
    }

/*
    Find end points of the distance range.
*/
    PointAtDistance (0, dist1,
                     &x1, &y1, &dum1, &dum2,
                     &i1);
    PointAtDistance (i1, dist2,
                     &x2, &y2, &dum1, &dum2,
                     &i2);

    dx = x2 - x1;
    dy = y2 - y1;
    ldist = dx*dx + dy*dy;
    ldist = (CSW_F)sqrt ((double)ldist);

    if (ldist <= tiny) {
        return 1;
    }

/*
    Find the maximum perpendicular distance to the line
    defined by the end points of the distance range.
*/
    maxdist = 0.0f;
    nint = 0;
    for (i=i1+1; i<=i2; i++) {

        xt1 = Xline[i];
        yt1 = Yline[i];
        PerpendicularIntersect (x1, y1, x2, y2, xt1, yt1,
                                &xp, &yp);
        dx = Xline[i] - xp;
        dy = Yline[i] - yp;
        dist = dx*dx + dy*dy;
        if (dist > maxdist) maxdist = dist;

        if (i == i2) continue;

    /*
        check for intersection
    */
        xt2 = Xline[i+1];
        yt2 = Yline[i+1];
        if (dx <= tiny  &&  dx >= -tiny) {
            if (xt1 < x1-tiny  &&  xt2 > x1+tiny) {
                nint++;
            }
            else if (xt1 > x1+tiny  &&  xt2 < x1-tiny) {
                nint++;
            }
        }
        else {
            a = dy / dx;
            b = y1 - a * x1;
            if (yt1 > a*xt1+b+tiny  &&  yt2 < a*xt2+b-tiny) {
                nint++;
            }
            else if (yt1 < a*xt1+b-tiny  &&  yt2 > a*xt2+b+tiny) {
                nint++;
            }
        }
    }

    maxdist = (CSW_F)sqrt ((double)maxdist);

    if (nint > 0) {
        xt1 = (CSW_F) (nint + 1);
        xt1 *= xt1;
        maxdist *= xt1;
    }

/*
    curvature is defined as the ratio between maximum distance
    from the line and the distance between the endpoints.
*/
    *curvemax = maxdist / ldist;

    return 1;

}  /*  end of private CheckCurvature function  */





/*
  ****************************************************************************

                          G e n e r a t e T i c k s

  ****************************************************************************

    Generate line primitives for tick marks between two distances along the
  contour line.  The ticks are drawn TickLen long in the TickLeftRight
  direction.

*/

int CSWConDraw::GenerateTicks (CSW_F dist1, CSW_F dist2)
{
    CSW_F     x1, y1, x2, y2, tspace, dum1, dum2;
    int           istat, i1, i2, n, i;

/*
    Calculate an exact tick spacing for this segment
    that is close to the global tick spacing and also
    positions a tick at each end of the segment.
*/
    n = (int) ((dist2 - dist1) / TickSpacing + 1.5f);
    if (n < 2) n = 2;
    tspace = (dist2 - dist1) / ((CSW_F)(n-1));
    if (dist2 - dist1 < TickSpacing) {
        tspace = dist2 - dist1;
        n = 2;
    }

/*
    Calculate ticks at tspace intervals along the
    line segment.
*/
    TickFlag = 1;
    i1 = 0;
    if (ClosedContour) n--;
    for (i=0; i<n; i++) {
        PointAtDistance (i1, dist1+i*tspace,
                         &x1, &y1, &dum1, &dum2,
                         &i2);
        i1 = i2;
        if (i1 >= Nline-1) i1--;
        PerpendicularPoint (Xline[i1], Yline[i1],
                            Xline[i1+1], Yline[i1+1],
                            x1, y1, TickLeftRight, TickLen,
                            &x2, &y2);
        istat = AppendVecToOutput (x1, y1, x2, y2);
        if (istat == -1) {
            return -1;
        }
    }
    TickFlag = 0;

    return 1;

}  /*  end of private GenerateTicks function  */





/*
  ****************************************************************

            S e p a r a t e L a b e l S e g m e n t s

  ****************************************************************

    Draw the ticks and main line for the contour points between
  contour labels.

*/

int CSWConDraw::SeparateLabelSegments (void)
{
    CSW_F       d1, d2;
    CSW_F       *x, *y, dum1, dum2;
    int         i, j, n, idum, istat, csav;

/*
    First draw ticks between label gaps.
*/
    if (TickSpacing > 0.0f) {
        d1 = 0.0f;
        for (i=0; i<NtextSeg; i++) {
            d2 = TextGap1[i];
            csav = ClosedContour;
            ClosedContour = 0;
            istat = GenerateTicks (d1, d2);
            ClosedContour = csav;
            if (istat == -1) {
                return -1;
            }
            d1 = TextGap2[i];
        }

        istat = GenerateTicks (d1, LineDistance[Nline-1]);
        if (istat == -1) {
            return -1;
        }
    }

/*
    Draw the solid lines between label gaps.
*/
    n = 0;
    d1 = TextGap1[n];
    i = 0;
    j = 0;

    for (;;) {
        if (LineDistance[i] < d1) {
            Xwork[j] = Xline[i];
            Ywork[j] = Yline[i];
            i++;
            j++;
            continue;
        }
        else {
            if (i < 1) i = 1;
            PointAtDistance (i-1, d1,
                             Xwork+j, Ywork+j, &dum1, &dum2,
                             &idum);
            j++;
            if (j > 1) {

            /*
                Note that the memory allocated below is put into the
                COntourLinePrim structure.  The application must csw_Free
                this by calling con_FreeDrawing.
            */
MSL
                x = (CSW_F *)csw_Malloc (j * 2 * sizeof(CSW_F));
                if (!x) {
                    return -1;
                }
                y = x + j;
                memcpy (x, Xwork, j * sizeof(CSW_F));
                memcpy (y, Ywork, j * sizeof(CSW_F));
                istat = AppendLineToOutput (x, y, j);
                if (istat == -1) {
                    return -1;
                }
            }
            if (d1 >= LineDistance[Nline-1]) {
                break;
            }
            d1 = TextGap2[n];
            PointAtDistance (i-1, d1,
                             Xwork, Ywork, &dum1, &dum2,
                             &idum);
            j = 1;
            i = idum + 1;
            n++;
            if (n < NtextSeg) {
                d1 = TextGap1[n];
            }
            else {
                d1 = LineDistance[Nline-1];
            }

        }

    }

    return 1;

}  /*  end of private SeparateLabelSegments function  */






/*
  ****************************************************************

                   P o s i t i o n L a b e l s

  ****************************************************************

    Position each character of each label along the contour in the
  gaps provided.

*/

int CSWConDraw::PositionLabels (void)
{
    int             istat, j, ido, i2, jj, nsmooth;
    CSW_F           d1, d2, dt, x0, y0, x1, y1, x2, y2;
    CSW_F           dum1, dum2, sign, angle, slope;
    CSW_F           *xt, *yt, *at, tsiz2;
    char            *text;
    CSW_F           xt2[100], yt2[100];
    CSW_F           xtlast, ytlast;
    double          tsiz3, ang90;

    tsiz2 = TextSize / 20.0f;
    tsiz3 = TextSize / 3.0;

    if (NcharWidths < 1) return 1;

/*
    Draw an in line label in each of the gaps previously
    defined by the TextGap1 and TextGap2 arrays.
*/
    if (NcharWidths > 100) NcharWidths = 100;
    nsmooth = NcharWidths / 2;
    nsmooth++;
    if (nsmooth < 2) nsmooth = 2;

    for (ido=0; ido<NtextSeg; ido++) {

    /*
        Allocate space for elements of the COntourTextPrim
        structure for each label drawn.  This memory must be
        csw_Freed by the application via a call to con_FreeDrawing.
    */
MSL
        xt = (CSW_F *)csw_Malloc (3 * NcharWidths * sizeof(CSW_F));
        if (!xt) {
            return -1;
        }
        yt = xt + NcharWidths;
        at = yt + NcharWidths;
MSL
        text = (char *)csw_Malloc ((strlen(TextLabel) + 1) * sizeof(char));
        if (!text) {
            csw_Free (xt);
            return -1;
        }
        strcpy (text, TextLabel);

        d1 = TextGap1[ido] + TextSize / 2.0f;
        d2 = TextGap2[ido] - TextSize / 2.0f;

    /*
        Draw from d1 to d2 or d2 to d1, depending on which
        is the most left to right (or bottom to top if vertical).
    */
        PointAtDistance (0, d1,
                         &x1, &y1, &dum1, &dum2,
                         &i2);
        PointAtDistance (0, d2,
                         &x2, &y2, &dum1, &dum2,
                         &i2);
        if (x1 - x2 != 0.0) {
            if (x1 > x2) {
                dt = d1;
                d1 = d2;
                d2 = dt;
            }
        }
        else {
            if (y1 > y2) {
                dt = d1;
                d1 = d2;
                d2 = dt;
            }
        }

        sign = 1.0f;
        if (d1 > d2) sign = -1.0f;

    /*
        Each character will have an x, y and angle
        value in the xt, yt and at arrays.
    */
        dt = 0.0f;
        for (j=0; j<NcharWidths; j++) {
            PointAtDistance (0, d1 + sign * dt + tsiz2,
                             &x0, &y0, &angle, &slope,
                             &i2);
            dt += CharWidths[j];

            xt[j] = x0;
            yt[j] = y0;
            at[j] = angle * 180.f / PI;
        }
        PointAtDistance (0, d1 + sign * dt + tsiz2,
                         &xtlast, &ytlast, &angle, &slope,
                         &i2);

    /*
     * Smooth the xt, yt positions with a 3 point moving average.
     * Recalculate the angles based on the smoothed points.
     */
        if (NcharWidths > 2) {
          for (jj=0; jj<nsmooth; jj++) {
            for (j=1; j<NcharWidths-1; j++) {
                xt2[j] = (xt[j-1] + xt[j] + xt[j+1]) / 3.0f;
                yt2[j] = (yt[j-1] + yt[j] + yt[j+1]) / 3.0f;
            }
            j = NcharWidths - 1;
            xt2[j] = (xt[j-1] + xt[j] + xtlast) / 3.0f;
            yt2[j] = (yt[j-1] + yt[j] + ytlast) / 3.0f;
            for (j=1; j<NcharWidths; j++) {
                xt[j] = xt2[j];
                yt[j] = yt2[j];
            }
          }
        }

        for (j=0; j<NcharWidths-1; j++) {
            angle = (CSW_F)atan2 (yt[j+1]-yt[j], xt[j+1]-xt[j]);
            ang90 = angle - HALFPI;
            xt[j] += (CSW_F)(tsiz3 * cos (ang90));
            yt[j] += (CSW_F)(tsiz3 * sin (ang90));
            at[j] = angle * 180.f / PI;
        }
        angle = (CSW_F)atan2 (ytlast - yt[NcharWidths-1],
                              xtlast - xt[NcharWidths-1]);
        ang90 = angle - HALFPI;
        xt[NcharWidths-1] += (CSW_F)(tsiz3 * cos (ang90));
        yt[NcharWidths-1] += (CSW_F)(tsiz3 * sin (ang90));
        j = NcharWidths - 1;
        at[NcharWidths-1] = angle * 180.f / PI;

    /*
        Add this label structure to the output COntourTextPrim array.
    */
        istat = AppendTextToOutput (text, xt, yt, at, NcharWidths);
        if (istat == -1) {
            return -1;
        }

    }  /*  loop to next in line label  */

    return 1;

}  /*  end of private PositionLabels function  */





/*
  ****************************************************************

                A p p e n d V e c T o O u t p u t

  ****************************************************************

    Add a COntourLinePrim record to the output reflecting a single
  vector to draw.  This is used most often for tick marks.

*/

int CSWConDraw::AppendVecToOutput
     (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2)
{
    CSW_F        *x, *y;
    int              istat;

/*
    Note that the memory allocated below is put into the
    COntourLinePrim structure.  The application must csw_Free
    this by calling con_FreeDrawing.
*/
MSL
    x = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
    if (!x) return -1;
    y = x + 2;

    x[0] = x1;
    y[0] = y1;
    x[1] = x2;
    y[1] = y2;

    istat = AppendLineToOutput (x, y, 2);

    return istat;

}  /*  end of private AppendVecToOutput function  */







/*
  ****************************************************************

                O u t p u t W h o l e L i n e

  ****************************************************************

    Allocate space for copies of Xline and Yline and append them
  to the line output.

*/

int CSWConDraw::OutputWholeLine (void)
{
    CSW_F          *x, *y;


/*
    Note that the memory allocated below is put into the
    COntourLinePrim structure.  The application must csw_Free
    this by calling con_FreeDrawing.
*/
MSL
    x = (CSW_F *)csw_Malloc (Nline * 2 * sizeof(CSW_F));
    if (!x) {
        return -1;
    }
    y = x + Nline;

    memcpy ((char *)x, (char *)Xline, Nline*sizeof(CSW_F));
    memcpy ((char *)y, (char *)Yline, Nline*sizeof(CSW_F));

    AppendLineToOutput (x, y, Nline);

    return 1;

}  /*  end of private OutputWholeLine function  */










/*
  ****************************************************************

               A p p e n d L i n e T o O u t p u t

  ****************************************************************

    Add another COntourLinePrim to the list of output contour
  lines, enlarging the space as needed.

*/

int CSWConDraw::AppendLineToOutput (CSW_F *x, CSW_F *y, int npts)
{

    if (NoutputLine >= MaxOutputLine) {
        MaxOutputLine += LINE_CHUNK;
MSL
        OutputLineRecs = (COntourLinePrim *)csw_Realloc (OutputLineRecs,
                                                         MaxOutputLine * sizeof(COntourLinePrim));
        if (!OutputLineRecs) {
            csw_Free (x);
            return -1;
        }
    }

    OutputLineRecs[NoutputLine].npts = npts;
    OutputLineRecs[NoutputLine].x = x;
    OutputLineRecs[NoutputLine].y = y;
    OutputLineRecs[NoutputLine].tickflag = TickFlag;
    OutputLineRecs[NoutputLine].textflag = TextFlag;
    OutputLineRecs[NoutputLine].majorflag = MajorFlag;
    OutputLineRecs[NoutputLine].faultflag = FaultFlag;

    NoutputLine++;

    return 1;

}  /*  end of private AppendLineToOutput function  */






/*
  ****************************************************************

              A p p e n d T e x t T o O u t p u t

  ****************************************************************

    Add a COntourTextPrim structure to the list of output text
  primitive structures, enlarging the list if needed.

*/

int CSWConDraw::AppendTextToOutput
                  (char *text,
                   CSW_F *xt, CSW_F *yt, CSW_F *at, int nt)
{

    if (NoutputText >= MaxOutputText) {
        MaxOutputText += TEXT_CHUNK;
MSL
        OutputTextRecs =
          (COntourTextPrim *)csw_Realloc
                            (OutputTextRecs,
                             MaxOutputText * sizeof(COntourTextPrim));
        if (!OutputTextRecs) {
            csw_Free (text);
            csw_Free (xt);
            return -1;
        }
    }

    OutputTextRecs[NoutputText].nchar = nt;
    OutputTextRecs[NoutputText].text = text;
    OutputTextRecs[NoutputText].x = xt;
    OutputTextRecs[NoutputText].y = yt;
    OutputTextRecs[NoutputText].angles = at;
    OutputTextRecs[NoutputText].majorflag = MajorFlag;
    OutputTextRecs[NoutputText].font = TextFont;
    OutputTextRecs[NoutputText].size = TextSize * .9f;

    NoutputText++;

    return 1;

}  /*  end of private AppendTextToOutput function  */





/*
  ****************************************************************

                   F r e e O u t p u t M e m

  ****************************************************************

  Called if an error occurs when the output line and text buffers
  may be partially filled.  If they are non mull they are csw_Freed and
  then the private variables are reset as empty.

*/

int CSWConDraw::FreeOutputMem (void)
{
    int                 i;
    COntourLinePrim     *lptr;
    COntourTextPrim     *tptr;

    if (OutputLineRecs) {

        for (i=0; i<NoutputLine; i++) {
            lptr = OutputLineRecs+i;
            if (lptr->x) csw_Free (lptr->x);
        }
        csw_Free (OutputLineRecs);
    }

    if (OutputTextRecs) {

        for (i=0; i<NoutputText; i++) {
            tptr = OutputTextRecs+i;
            if (tptr->text) csw_Free (tptr->text);
            if (tptr->x) csw_Free (tptr->x);
        }
        csw_Free (OutputTextRecs);
    }

    OutputTextRecs = NULL;
    OutputLineRecs = NULL;
    NoutputLine = 0;
    NoutputText = 0;

    return 1;

}  /*  end of private FreeOutputMem function  */





/*
  ***********************************************************************

                     c o n _ d r a w _ f i l l

  ***********************************************************************

    Calculate line primitives for a contour color fill.  The polygon points
  in the COntourFillRec structure are converted to page coordinates
  according to the scale set up by con_SetDrawScale.

*/

int CSWConDraw::con_draw_fill
                  (COntourFillRec *cptr,
                   COntourFillPrim **fptr, int *nfill)
{
    int              i, npts;
    CSW_F            *xcon, *ycon;
    COntourFillPrim  *fprim;

/*
    obvious errors
*/
    if (!cptr || !fptr || !nfill) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    initialize the return data pointers in case errors are encountered.
*/
    *fptr = NULL;
    *nfill = 0;

/*
    error if no scale has been set yet
*/
    if (Scalex <= 0.0f  ||  Scaley <= 0.0f) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

/*
    assign local variables for contour fill stuff
*/
    npts = cptr->npts;
    xcon = cptr->x;
    ycon = cptr->y;
    fprim = (COntourFillPrim *)csw_Malloc (sizeof(COntourFillPrim));
    if (!fprim) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Scale the polygon into the page units and store in
    the Xline, Yline arrays.
*/
    fprim->npts = npts;
    for (i=0; i<npts; i++) {
        fprim->x[i] = Pxmin + (xcon[i] - Xmin) * Scalex;
        fprim->y[i] = Pymin + (ycon[i] - Ymin) * Scaley;
    }

    if (npts == 2) {
        fprim->x[2] = fprim->x[1] - fprim->x[0];
        fprim->y[2] = fprim->y[1] - fprim->y[0];
    }

    *fptr = fprim;
    *nfill = 1;

    return 1;

}  /*  end of function con_draw_fill  */






/*
  ****************************************************************

                    C a l c C h a r W i d t h s

  ****************************************************************

  Given the contour label text, and the size and font of the text,
  fill in the private CharWidths array.  This is only called from
  con_draw_line when the application has not specified its own
  character width array.

*/

int CSWConDraw::CalcCharWidths (char *text)
{
    int               i, len, font;
    CSW_F         sum, total, size;

    GPFFont       gpf_font_obj;

    len = strlen (text);
    if (len >= MAX_CONTOUR_LABEL_SIZE) len = MAX_CONTOUR_LABEL_SIZE - 1;
    NcharWidths = len;

    if (MajorFlag) {
        size = MajorTextSize;
        font = MajorTextFont-1;
    }
    else {
        size = MinorTextSize;
        font = MinorTextFont-1;
    }

    if (font < 0  ||  font > 4) font = 1;

    font = FontLookup[font];

    sum = 0.0f;
    for (i=0; i<len; i++) {

        gpf_font_obj.gpf_TextLength2 (text+i, 1, font, size, CharWidths+i);
        sum += CharWidths[i];

    }

    CharWidths[len] = 0.0f;

    if (len > 1) {
        gpf_font_obj.gpf_TextLength2 (text, len, font, size, &total);
        sum = (total - sum) / (CSW_F)(len-1);
        for (i=0; i<len-1; i++) {
            CharWidths[i] += sum;
        }
    }

    return 1;

}  /*  end of private CalcCharWidths function  */






/*
  ****************************************************************

                    c o n _ d r a w _ t e x t

  ****************************************************************

    Return line primitives for drawing the contour label specified
  by the textptr structure.

*/

int CSWConDraw::con_draw_text
                  (COntourTextPrim *textptr,
                   COntourLinePrim **lineptr,
                   int *nlines)
{
    char             *text;
    int              i, j, n, istat, ntot, nchar, font,
                     parts[MAX_TEXT_PARTS], nparts;
    CSW_F            size, sinang, cosang, xchar, ychar, angle,
                     *xt, *yt;
    CSW_F            xtext[MAX_TEXT_VEC], ytext[MAX_TEXT_VEC];

    GPFFont       gpf_font_obj;

    if (textptr == NULL  ||  lineptr == NULL  ||  nlines == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    assign local variables to text prim members for
    code neatness sake.
*/
    text = textptr->text;
    nchar = textptr->nchar;
    size = textptr->size;

    font = textptr->font - 1;
    if (font < 0  ||  font > 4) font = 1;
    font = FontLookup[font];

/*
    tell the font utilities which font we want
*/
    gpf_font_obj.gpf_setfont (font);

/*
    Calculate vectors for each character in the text prim structure.
*/
    NoutputLine = 0;
    MaxOutputLine = 0;
    OutputLineRecs = NULL;

    for (i=0; i<nchar; i++) {

    /*
        retrieve vectors adjusted for size.
    */
        istat = gpf_font_obj.gpf_getfontstrokes
                                   (text[i], size, xtext, ytext,
                                    parts, &nparts,
                                    MAX_TEXT_VEC, MAX_TEXT_PARTS);
        if (!istat) {
            continue;
        }

        ntot = 0;
        for (j=0; j<nparts; j++) {
            ntot += parts[j];
        }

    /*
        rotate the vectors if needed
    */
        angle = textptr->angles[i];
        angle *= DEGTORAD;
        if (angle < -0.005  ||  angle > 0.005) {
            sinang = (CSW_F)sin ((double)angle);
            cosang = (CSW_F)cos ((double)angle);
            gpf_font_obj.gpf_rotatechar (xtext, ytext, ntot, cosang, sinang);
        }

    /*
        translate to location
    */
        xchar = textptr->x[i];
        ychar = textptr->y[i];
        for (j=0; j<ntot; j++) {
            xtext[j] += xchar;
            ytext[j] += ychar;
        }

    /*
        output line primitives
    */
        n = 0;
        for (j=0; j<nparts; j++) {

        /*
            Note that the memory allocated below is put into the
            COntourLinePrim structure.  The application must csw_Free
            this by calling con_FreeDrawing.
        */
            xt = (CSW_F *)csw_Malloc (parts[j] * 2 * sizeof(CSW_F));
            if (!xt) {
                grd_utils_ptr->grd_set_err (1);
                FreeOutputMem ();
                return -1;
            }
            yt = xt + parts[j];
            memcpy (xt, xtext+n, parts[j] * sizeof(CSW_F));
            memcpy (yt, ytext+n, parts[j] * sizeof(CSW_F));
            istat = AppendLineToOutput (xt, yt, parts[j]);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                FreeOutputMem ();
                return -1;
            }
            n += parts[j];
        }

    }  /*  end of loop through text characters  */

    *lineptr = OutputLineRecs;
    *nlines = NoutputLine;

    return 1;

}  /*  end of function con_draw_text  */
