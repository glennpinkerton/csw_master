
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_utils.c

    This file contains utilities for various geometric
    problems.

*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/include/csw_trig_consts.h"

#include "csw/utils/private_include/gpf_trigtab.h"
#include "csw/utils/private_include/ply_utils.h"

#include "csw/utils/private_include/gpf_font.h"
#include "csw/utils/private_include/TextBounds.h"

#include "csw/utils/private_include/gpf_utils.h"

#define DATA_MAX         1.e18f



static int BadOffsetPoint (double xc, double yc,
                           double xm1, double ym1,
                           double xp1, double yp1,
                           double xt, double yt,
                           double xpout, double ypout);

static int checkGraze (double x1,
                       double y1,
                       double x2,
                       double y2,
                       double tiny);

static int PointAtDistance (double *LineDistance, int Nline,
                            double *Xline, double *Yline, double *Zline,
                            int ibase, double dist,
                            double *x, double *y, double *z,
                            int *nextibase);

static int SeparateCrossingLines (
    double      *x,
    double      *y,
    double      *z,
    int         *np,
    int         *nf,
    int         nline,
    double      *xout,
    double      *yout,
    double      *zout,
    int         *npout,
    int         *nfout,
    int         *nloutio,
    int         *noutio,
    int         maxout,
    int         maxlout,
    double      *xcross,
    double      *ycross,
    double      *zcross,
    int         *npcross,
    int         *nfcross,
    int         *nlcrossio,
    int         *ncrossio,
    double      tiny);

static int SplitAgainstLongest (
    double    *xlong,
    double    *ylong,
    double    *zlong,
    int       *nlongio,
    double    *xlines,
    double    *ylines,
    double    *zlines,
    int       *nplines,
    int       *nflines,
    int       *nlinesio,
    double    tiny,
    int       npmax,
    int       nlmax);


/*
******************************************************************

                    g p f _ x y l i m i t s

******************************************************************

  function name:    gpf_xylimits      (int)

  call sequence:    gpf_xylimits (xy, npt, x1, y1, x2, y2)

  purpose:          Calculate the extrema of a set of x,y points

  return value:     always returns zero

  calling parameters:

    xy       r    CSW_F*      array of x,y x,y points
    npt      r    int         number of CSW_Fing point numbers in xy
                              (note that npt is 2 * number of points)
    x1       w    CSW_F*      minimum x coordinate
    y1       w    CSW_F*      minimum y coordinate
    x2       w    CSW_F*      maximum x coordinate
    y2       w    CSW_F*      maximum y coordinate

*/

int gpf_xylimits (CSW_F *xy, int npt,
                  CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2)
{
    int          i, j;

    *x1 = 1.e30f;
    *y1 = 1.e30f;
    *x2 = -1.e30f;
    *y2 = -1.e30f;

    i = 0;
    while (i < npt) {
        j = i + 1;
        if (xy[i] > DATA_MAX  ||  xy[j] > DATA_MAX  ||
            xy[i] < -DATA_MAX  ||  xy[j] < -DATA_MAX) {
            i += 2;
            continue;
        }
        if (xy[i] < *x1) {
            *x1 = xy[i];
        }
        if (xy[i] > *x2) {
            *x2 = xy[i];
        }
        if (xy[j] < *y1) {
            *y1 = xy[j];
        }
        if (xy[j] > *y2) {
            *y2 = xy[j];
        }

        i += 2;
    }

    return 0;

}  /*  end of function gpf_xylimits  */


/*
******************************************************************

                    g p f _ x y l i m i t s 2

******************************************************************

  function name:    gpf_xylimits2      (int)

  call sequence:    gpf_xylimits2 (xy, npt, x1, y1, x2, y2)

  purpose:          Calculate the extrema of a set of x,y points
                    ignoring null points

  return value:     always returns zero

  calling parameters:

    xy       r    CSW_F*      array of x,y x,y points
    npt      r    int         number of CSW_Fing point numbers in xy
                              (note that npt is 2 * number of points)
    x1       w    CSW_F*      minimum x coordinate
    y1       w    CSW_F*      minimum y coordinate
    x2       w    CSW_F*      maximum x coordinate
    y2       w    CSW_F*      maximum y coordinate

*/

int gpf_xylimits2 (CSW_F *xy, int npt,
                   CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2)
{
    int          i, j;

    *x1 = 1.e30f;
    *y1 = 1.e30f;
    *x2 = -1.e30f;
    *y2 = -1.e30f;

    i = 0;
    while (i < npt) {
        j = i + 1;
        if (xy[i] > DATA_MAX  ||  xy[j] > DATA_MAX  ||
            xy[i] < -DATA_MAX  ||  xy[j] < -DATA_MAX) {
            i += 2;
            continue;
        }
        if (xy[i] < *x1  &&  xy[i] < 1.e15f  &&  xy[i] > -1.e15f) {
            *x1 = xy[i];
        }
        if (xy[i] > *x2  &&  xy[i] < 1.e15f  &&  xy[i] > -1.e15f) {
            *x2 = xy[i];
        }
        if (xy[j] < *y1  &&  xy[j] < 1.e15f  &&  xy[j] > -1.e15f) {
            *y1 = xy[j];
        }
        if (xy[j] > *y2  &&  xy[j] < 1.e15f  &&  xy[j] > -1.e15f) {
            *y2 = xy[j];
        }

        i += 2;
    }

    return 0;

}  /*  end of function gpf_xylimits  */



/*
******************************************************************

                 g p f _ c a l c l i n e d i s t 1

******************************************************************

  function name:    gpf_calclinedist1     (int)

  call sequence:    gpf_calclinedist1 (xy, npts, xin, yin, dist)

  purpose:          calculate the closest distance between a polyline
                    and a point.  This is the single precision version.

  return value:     always returns zero

  calling parameters:

    xy        r    CSW_F*    array of xy polyline points
    npts      r    int       number of points in polyline
    xin       r    CSW_F     x coordinate of target point
    yin       r    CSW_F     y coordinate of target point
    dist      w    CSW_F*    distance from point to line

*/

int gpf_calclinedist1 (CSW_F *xy, int npts,
                       CSW_F xin, CSW_F yin, CSW_F *dist)
{
    CSW_F     x1, y1, x2, y2, *tmp, xp, yp, dx, dy;
    int       i, istat;
    double    dtmp, dmax;

/*  find the closest perpendicular distance to a polyline segment  */

    dmax = 1.e31f;
    tmp = xy;
    x1 = *tmp;
    tmp++;
    y1 = *tmp;
    tmp++;

    for (i=1; i<npts; i++) {
        x2 = *tmp;
        tmp++;
        y2 = *tmp;
        tmp++;
        istat = gpf_perpintpoint1 (x1, y1, x2, y2, xin, yin, &xp, &yp);
        if (istat == 1) {
            dx = xin - xp;
            dy = yin - yp;
            dtmp = dx*dx + dy*dy;
            dtmp = sqrt (dtmp);
            if (dtmp < dmax) {
                dmax = dtmp;
            }
        }
        x1 = x2;
        y1 = y2;
    }

    if (dmax < 1.e30f) {
        *dist = (CSW_F)dmax;
        return 0;
    }

/*  use the closest polyline vertex if no perpendicular distance
    can be calculated  */

    tmp = xy;

    for (i=0; i<npts; i++) {
        xp = *tmp;
        tmp++;
        yp = *tmp;
        tmp++;
        dx = xin - xp;
        dy = yin - yp;
        dtmp = dx*dx + dy*dy;
        dtmp = sqrt (dtmp);
        if (dtmp < dmax) {
            dmax = dtmp;
        }
    }

    *dist = (CSW_F)dmax;
    return 1;

}  /*  end of function gpf_calclinedist1  */



/*
******************************************************************

              g p f _ p e r p i n t p o i n t 1

******************************************************************

  function name:    gpf_perpintpoint1      (int)

  call sequence:    gpf_perpintpoint1 (x1, y1, x2, y2, x, y, xp, yp)

  purpose:          calculate a point on a line which is perpendicular to
                    a target point.  The target point can be on or off the
                    line.  This is the single precision version.

  return value:     status code

                    0 = perpendicular point is outside the line segment
                    1 = perpendicular point is inside the line segment

  calling parameters:

    x1     r    CSW_F        x coordinate of first segment endpoint
    y1     r    CSW_F        y coordinate of first segment endpoint
    x2     r    CSW_F        x coordinate of second endpoint
    y2     r    CSW_F        y coordinate of second endpoint
    x      r    CSW_F        x coordinate of target point
    y      r    CSW_F        y coordinate of target point
    xp     w    CSW_F*       x coordinate of intersection point
    yp     w    CSW_F*       y coordinate of intersection point

*/

int gpf_perpintpoint1 (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                       CSW_F x, CSW_F y, CSW_F *xp, CSW_F *yp)
{
    CSW_F      dx, dy, s1, s2, tmp1, tmp2, divide, b1, b2;
    double     tiny, dx2, dy2;
    int        istat;

    divide = 1000.f;

/*  special case for nearly vertical or nearly horizontal  */

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
        return 0;
    }

    if (tmp1 < tmp2/divide) {
        *xp = (x1 + x2) / 2.0f;
        *yp = y;
        if ((y1-y)*(y-y2) >= 0.0) {
            return 1;
        }
        return 0;
    }

    if (tmp2 < tmp1/divide) {
        *xp = x;
        *yp = (y1 + y2) / 2.0f;
        if ((x1-x)*(x-x2) >= 0.0) {
            return 1;
        }
        return 0;
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
        dx2 = x2 - x1;
        dy2 = y2 - y1;
        tiny = dx2 * dx2 + dy2 * dy2;
        tiny = sqrt (tiny);
        tiny /= 1000.0;
        istat = checkGraze (
            (double)x1,
            (double)y1,
            (double)dx,
            (double)dy,
            tiny);
        if (istat == 1) {
            return 1;
        }
        istat = checkGraze (
            (double)x2,
            (double)y2,
            (double)dx,
            (double)dy,
            tiny);
        if (istat == 1) {
            return 1;
        }
        return 0;
    }

}  /*  end of function gpf_perpintpoint1  */



/*
******************************************************************

                 g p f _ c a l c l i n e d i s t 2

******************************************************************

  function name:    gpf_calclinedist2     (int)

  call sequence:    gpf_calclinedist2 (xy, npts, xin, yin, dist)

  purpose:          calculate the closest distance between a polyline
                    and a point.  This is the double precision version.

  return value:     always returns zero

  calling parameters:

    xy        r    double*   array of xy polyline points
    npts      r    int       number of points in polyline
    xin       r    double    x coordinate of target point
    yin       r    double    y coordinate of target point
    dist      w    double*   distance from point to line

*/

int gpf_calclinedist2 (double *xy, int npts, double xin, double yin, double *dist)
{
    double    x1, y1, x2, y2, *tmp, xp, yp, dx, dy;
    int       i, istat;
    double    dtmp, dmax;

/*  find the closest perpendicular distance to a polyline segment  */

    dmax = 1.e31f;
    tmp = xy;
    x1 = *tmp;
    tmp++;
    y1 = *tmp;
    tmp++;
    x2 = x1;
    y2 = y1;

    for (i=1; i<npts; i++) {
        x2 = *tmp;
        tmp++;
        y2 = *tmp;
        tmp++;
        istat = gpf_perpintpoint2 (x1, y1, x2, y2, xin, yin, &xp, &yp);
        if (istat == 1) {
            dx = xin - xp;
            dy = yin - yp;
            dtmp = dx*dx + dy*dy;
            dtmp = sqrt (dtmp);
            if (dtmp < dmax) {
                dmax = dtmp;
            }
        }
        x1 = x2;
        y1 = y2;
    }

    if (dmax < 1.e30f) {
        *dist = dmax;
        return 0;
    }

/*  use the closest polyline endpoint if no perpendicular distance
    can be calculated  */

    dx = x2 - xin;
    dy = y2 - yin;
    dtmp = dx*dx + dy*dy;
    dtmp = sqrt (dtmp);
    if (dtmp < dmax) {
        dmax = dtmp;
    }

    tmp = xy;
    dx = *tmp - xin;
    tmp++;
    dy = *tmp - yin;
    dtmp = dx*dx + dy*dy;
    dtmp = sqrt (dtmp);
    if (dtmp < dmax) {
        dmax = dtmp;
    }

    *dist = dmax;
    return 1;

}  /*  end of function gpf_calclinedist2  */


/*
 ****************************************************************

          g p f _ l i n e a r i n t e r p 2

 ****************************************************************

 Return a z value based on linear interpolation between the
 two specified endpoints.  If the specified xpt, ypt is not on
 the line segment, it is projected to the line segment prior
 to the interpolation.  

 If the projected point is inside the line segment or on an
 endpoint of the line segment, the return status is 1.  If the
 projected point is outside the line segment, the return status
 is zero.

*/

int gpf_linearinterp2 (double x1, double y1, double z1,
		               double x2, double y2, double z2,
					   double xpt, double ypt, double *zret)
{
	int          istat;
	double       xt, yt, pct, zt;

	istat = gpf_perpintpoint2 (x1, y1, x2, y2, xpt, ypt, &xt, &yt);

	double dx = x2 - x1;
	if (dx < 0.0) dx = -dx;
	double dy = y2 - y1;
	if (dy < 0.0) dy = -dy;

	if (dx > dy) {
		pct = (xt - x1) / (x2 - x1);
	}
	else {
		pct = (yt - y1) / (y2 - y1);
	}

	zt = z1 + (z2 - z1) * pct;

	*zret = zt;

	return istat;

}



/*
******************************************************************

              g p f _ p e r p i n t p o i n t 2

******************************************************************

  function name:    gpf_perpintpoint2      (int)

  call sequence:    gpf_perpintpoint2 (x1, y1, x2, y2, x, y, xp, yp)

  purpose:          calculate a point on a line which is perpendicular to
                    a target point.  The target point can be on or off the
                    line.  This is the double precision version.

  return value:     status code

                    0 = perpendicular point is outside the line segment
                    1 = perpendicular point is inside the line segment

  calling parameters:

    x1     r    double       x coordinate of first segment endpoint
    y1     r    double       y coordinate of first segment endpoint
    x2     r    double       x coordinate of second endpoint
    y2     r    double       y coordinate of second endpoint
    x      r    double       x coordinate of target point
    y      r    double       y coordinate of target point
    xp     w    double*      x coordinate of intersection point
    yp     w    double*      y coordinate of intersection point

*/

int gpf_perpintpoint2 (double x1, double y1, double x2, double y2,
                       double x, double y, double *xp, double *yp)
{
    double     dx, dy, s1, s2, tmp1, tmp2, divide, b1, b2;
    double     tiny, dx2, dy2;
    int        istat;

    divide = 1000000.f;

/*
    Special cases for nearly vertical or nearly horizontal lines.
*/
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
        return 0;
    }

    if (tmp1 < tmp2/divide) {
        *xp = (x1 + x2) / 2.0f;
        *yp = y;
        if ((y1-y)*(y-y2) >= 0.0) {
            return 1;
        }
        return 0;
    }

    if (tmp2 < tmp1/divide) {
        *xp = x;
        *yp = (y1 + y2) / 2.0f;
        if ((x1-x)*(x-x2) >= 0.0) {
            return 1;
        }
        return 0;
    }

/*  intersect a perpendicular from the target with the segment  */

    s1 = dy / dx;
    s2 = -1.f / s1;
    b1 = y1 - s1 * x1;
    b2 = y - s2 * x;

    *xp = (b1 - b2) / (s2 - s1);
    *yp = s1 * *xp + b1;

/*  check if the intersection point is inside the segment endpoints  */

    if ( (x1-*xp)*(*xp-x2) >= 0.0f  ||  (y1-*yp)*(*yp-y2) >= 0.0f) {
        return 1;
    }
    else {
        dx2 = x2 - x1;
        dy2 = y2 - y1;
        tiny = dx2 * dx2 + dy2 * dy2;
        tiny = sqrt (tiny);
        tiny /= 1000.0;
        istat = checkGraze (
            x1,
            y1,
            *xp,
            *yp,
            tiny);
        if (istat == 1) {
            return 1;
        }
        istat = checkGraze (
            x2,
            y2,
            *xp,
            *yp,
            tiny);
        if (istat == 1) {
            return 1;
        }
        return 0;
    }

}  /*  end of function gpf_perpintpoint2  */


static int checkGraze (
    double      x1,
    double      y1,
    double      x2,
    double      y2,
    double      tiny)
{
    double      dx, dy, dist;

    if (tiny <= 0.0) {
        tiny = 1.e-14;
    }

    dx = x2 - x1;
    dy = y2 - y1;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);

    if (dist <= tiny) {
        return 1;
    }

    return 0;
}





/*
******************************************************************

                 g p f _ c a l c t e x t d i s t 1

******************************************************************

  function name:    gpf_calctextdist1      (int)

  call sequence:    gpf_calctextdist1 (xt, yt, text, nc, size, font, ang,
                                       x, y, dist)

  purpose:          calculate distance from a target point to
                    the edge of a text item.  single precision version.

  return value:     always returns zero

  calling parameters:

    xt      r    CSW_F    x coordinate of lower left text corner
    yt      r    CSW_F    y coordinate of lower left text corner
    text    r    char*    text string to calc distance to
    nc      r    int      number of characters
    size    r    CSW_F    character height
    font    r    int      text font the string is drawn in
    ang     r    CSW_F    text angle in radians
    x       r    CSW_F    x coordinate of target point
    y       r    CSW_F    y coordinate of target point
    dist    w    CSW_F*   distance from point to closest text edge

*/
int gpf_calctextdist1 (CSW_F xt, CSW_F yt, const char *text, int nc,
                       CSW_F size, int font, CSW_F ang,
                       CSW_F x, CSW_F y, CSW_F *dist)
{
    CSW_F      xy[12], w;
    CSW_F      sd, cd;

    GPFFont    font_obj;

    if (!text) {
        *dist = 1.e30f;
        return 1;
    }

    sd = (CSW_F)sin ((double)ang);
    cd = (CSW_F)cos ((double)ang);

    font_obj.gpf_TextLength2 (text, nc, font, size, &w);  /* not used with java fonts */

    xy[0] = xt;
    xy[1] = yt;
    xy[8] = xt;
    xy[9] = yt;
    xy[2] = xt + w * cd;
    xy[3] = yt + w * sd;
    xy[4] = xt + w * cd - size * sd;
    xy[5] = yt + w * sd + size * cd;
    xy[6] = xt - size * sd;
    xy[7] = yt + size * cd;

    gpf_calclinedist1 (xy, 5, x, y, dist);

    return 0;

}  /*  end of function gpf_calctextdist1  */



/*
******************************************************************

               g p f _ c a l c d i s t a n c e 1

******************************************************************

  function name:    gpf_calcdistance1     (int)

  call sequence:    gpf_calcdistance1 (x1, y1, x2, y2, dist)

  purpose:          calculate distance between two points
                    single precision version

  return value:     always returns zero

  calling parameters:

    x1      r   CSW_F     x coordinate of first point
    y1      r   CSW_F     y coordinate of first point
    x2      r   CSW_F     x coordinate of second point
    y2      r   CSW_F     y coordinate of second point
    dist    w   CSW_F*    distance between points

*/

int gpf_calcdistance1 (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2, CSW_F *dist)
{
    CSW_F       dx, dy;
    CSW_F       dtmp;

    dx = x1 - x2;
    dy = y1 - y2;

    dtmp = dx*dx + dy*dy;
    dtmp = (CSW_F)sqrt ((double)dtmp);

    *dist = dtmp;

    return 0;

}  /*  end of function gpf_calcdistance1  */



/*
******************************************************************

                 g p f _ c a l c t e x t d i s t 2

******************************************************************

  function name:    gpf_calctextdist2      (int)

  call sequence:    gpf_calctextdist2 (xt, yt, text, nc, size, font, ang,
                                       x, y, dist)

  purpose:          calculate distance from a target point to
                    the edge of a text item.  double precision version.

  return value:     always returns zero

  calling parameters:

    xt      r    double   x coordinate of lower left text corner
    yt      r    double   y coordinate of lower left text corner
    text    r    char*    character string drawn
    nc      r    int      number of characters
    size    r    double   character height and width
    font    r    int      font number the string is drawn in
    ang     r    double   text angle in radians
    x       r    double   x coordinate of target point
    y       r    double   y coordinate of target point
    dist    w    double*  distance from point to closest text edge

*/

int gpf_calctextdist2 (double xt, double yt, const char *text, int nc,
                       double size, int font, double ang,
                       double x, double y, double *dist)
{
    double     xy[12], w;
    double     sd, cd;
    CSW_F      fw, fs;

    GPFFont    font_obj;

    fs = (CSW_F)size;

    font_obj.gpf_TextLength2 (text, nc, font, fs, &fw);

    sd = sin (ang);
    cd = cos (ang);
    w = (double)fw;

    xy[0] = xt;
    xy[1] = yt;
    xy[8] = xt;
    xy[9] = yt;
    xy[2] = xt + w * cd;
    xy[3] = yt + w * sd;
    xy[4] = xt + w * cd - size * sd;
    xy[5] = yt + w * sd + size * cd;
    xy[6] = xt - size * sd;
    xy[7] = yt + size * cd;

    gpf_calclinedist2 (xy, 5, x, y, dist);

    return 0;

}  /*  end of function gpf_calctextdist2  */



/*
******************************************************************

               g p f _ c a l c d i s t a n c e 2

******************************************************************

  function name:    gpf_calcdistance2     (int)

  call sequence:    gpf_calcdistance2 (x1, y1, x2, y2, dist)

  purpose:          calculate distance between two points
                    double precision version

  return value:     always returns zero

  calling parameters:

    x1      r   double    x coordinate of first point
    y1      r   double    y coordinate of first point
    x2      r   double    x coordinate of second point
    y2      r   double    y coordinate of second point
    dist    w   double*   distance between points

*/

int gpf_calcdistance2 (double x1, double y1, double x2, double y2,
                       double *dist)
{
    double      dx, dy;
    double      dtmp;

    dx = x1 - x2;
    dy = y1 - y2;

    dtmp = dx*dx + dy*dy;
    dtmp = sqrt (dtmp);

    *dist = dtmp;

    return 0;

}  /*  end of function gpf_calcdistance2  */


/*
  ****************************************************************

             g p f _ C a l c C i r c l e P o i n t s

  ****************************************************************

  function name:    gpf_CalcCirclePoints       (int)

  call sequence:    gpf_CalcCirclePoints (x, y, r,
                                          xout, yout, npts)

  purpose:          Calculate x,y points for a circle and output to
                    the xout, yout arrays.

  return value:     Always returns 1.

  calling parameters:

    x         r     CSW_F         x coordinate of center
    y         r     CSW_F         y coordinate of center
    r         r     CSW_F         circle radius
    xout      w     CSW_F*        array of x coordinates along arc
    yout      w     CSW_F*        array of y coordinates along arc
    npts      r     int           number of points along arc to sample
                                  xout and yout must be large enough to hold
                                  this many points.

*/

int gpf_CalcCirclePoints (CSW_F x, CSW_F y, CSW_F r,
                          CSW_F *xout, CSW_F *yout, int npts)
{
    int          i, iang, n;
    CSW_F        dang, ang, rat;

    rat = 1000.f/360.f;
    n = npts - 1;
    dang = 360.f/(CSW_F)n;

    for (i=0; i<n; i++) {
        ang = i * dang;
        iang = (int) (ang * rat);
        xout[i] = (CSW_F)(x + r * CosTab[iang]);
        yout[i] = (CSW_F)(y + r * SinTab[iang]);
    }

    xout[n] = xout[0];
    yout[n] = yout[0];

    return 1;

}  /*  end of function gpf_CalcCirclePoints  */




/*
******************************************************************

                    g p f _ x a n d y l i m i t s

******************************************************************

  function name:    gpf_xandylimits      (int)

  call sequence:    gpf_xandylimits (x, y, npt, x1, y1, x2, y2)

  purpose:          Calculate the extrema of a set of x,y points

  return value:     always returns zero

  calling parameters:

    x        r    CSW_F*      array of x points
    y        r    CSW_F*      array of y points
    npt      r    int         number of CSW_Fing point numbers in x and y
    x1       w    CSW_F*      minimum x coordinate
    y1       w    CSW_F*      minimum y coordinate
    x2       w    CSW_F*      maximum x coordinate
    y2       w    CSW_F*      maximum y coordinate

*/

int gpf_xandylimits (CSW_F *x, CSW_F *y, int npt,
                     CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2)
{
    int          i;

    *x1 = 1.e30f;
    *y1 = 1.e30f;
    *x2 = -1.e30f;
    *y2 = -1.e30f;

    i = 0;
    while (i < npt) {
        if (x[i] > DATA_MAX  ||  y[i] > DATA_MAX  ||
            x[i] < -DATA_MAX  ||  y[i] < -DATA_MAX) {
            i++;
            continue;
        }
        if (x[i] < *x1) {
            *x1 = x[i];
        }
        if (x[i] > *x2) {
            *x2 = x[i];
        }
        if (y[i] < *y1) {
            *y1 = y[i];
        }
        if (y[i] > *y2) {
            *y2 = y[i];
        }
        i++;
    }

    return 0;

}  /*  end of function gpf_xandylimits  */



/*
******************************************************************

               g p f _ x a n d y l i m i t s 3 d

******************************************************************

  function name:    gpf_xandylimitsdouble      (int)

  call sequence:    gpf_xandylimitsdouble (x, y, npt, x1, y1, x2, y2)

  purpose:          Calculate the extrema of a set of x,y points

  return value:     always returns zero

  calling parameters:

    x        r    double*     array of x points
    y        r    double*     array of y points
    npt      r    int         number of CSW_Fing point numbers in x and y
    x1       w    double*     minimum x coordinate
    y1       w    double*     minimum y coordinate
    x2       w    double*     maximum x coordinate
    y2       w    double*     maximum y coordinate

*/

int gpf_xandylimits3d ( double *x,  double *y, double *z, int npt,
                        double *x1,  double *y1,
                        double *x2,  double *y2,
                        double *z1,  double *z2)
{
    int          i;

    *x1 = 1.e30;
    *y1 = 1.e30;
    *x2 = -1.e30;
    *y2 = -1.e30;
    *z1 = 1.e30;
    *z2 = -1.e30;

    i = 0;
    while (i < npt) {
        if (x[i] > DATA_MAX  ||  y[i] > DATA_MAX  ||
            x[i] < -DATA_MAX  ||  y[i] < -DATA_MAX) {
            i++;
            continue;
        }
        if (z[i] > 1.e20  ||  z[i] < -1.e20) {
            i++;
            continue;
        }
        if (x[i] < *x1) {
            *x1 = x[i];
        }
        if (x[i] > *x2) {
            *x2 = x[i];
        }
        if (y[i] < *y1) {
            *y1 = y[i];
        }
        if (y[i] > *y2) {
            *y2 = y[i];
        }
        if (z[i] < *z1) {
            *z1 = z[i];
        }
        if (z[i] > *z2) {
            *z2 = z[i];
        }
        i++;
    }

    return 0;

}  /*  end of function gpf_xandylimitsdouble  */


/*
******************************************************************

                    g p f _ x a n d y l i m i t s 2

******************************************************************

  function name:    gpf_xandylimits2      (int)

  call sequence:    gpf_xandylimits2 (x, y, npt, x1, y1, x2, y2)

  purpose:          Calculate the extrema of a set of x,y points
                    ignoring null points

  return value:     always returns zero

  calling parameters:

    x        r    CSW_F*      array of x points
    y        r    CSW_F*      array of y points
    npt      r    int         number of CSW_Fing point numbers in x and y
    x1       w    CSW_F*      minimum x coordinate
    y1       w    CSW_F*      minimum y coordinate
    x2       w    CSW_F*      maximum x coordinate
    y2       w    CSW_F*      maximum y coordinate

*/

int gpf_xandylimits2 (CSW_F *x, CSW_F *y, int npt,
                      CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2)
{
    int          i;

    *x1 = 1.e30f;
    *y1 = 1.e30f;
    *x2 = -1.e30f;
    *y2 = -1.e30f;

    i = 0;
    while (i < npt) {
        if (x[i] > DATA_MAX  ||  y[i] > DATA_MAX  ||
            x[i] < -DATA_MAX  ||  y[i] < -DATA_MAX) {
            i++;
            continue;
        }
        if (x[i] < *x1  &&  x[i] < 1.e15f  &&  x[i] > -1.e15f) {
            *x1 = x[i];
        }
        if (x[i] > *x2  &&  x[i] < 1.e15f  &&  x[i] > -1.e15f) {
            *x2 = x[i];
        }
        if (y[i] < *y1  &&  y[i] < 1.e15f  &&  y[i] > -1.e15f) {
            *y1 = y[i];
        }
        if (y[i] > *y2  &&  y[i] < 1.e15f  &&  y[i] > -1.e15f) {
            *y2 = y[i];
        }
        i++;
    }

    return 0;

}  /*  end of function gpf_xandylimits2  */



/*
  ****************************************************************

             g p f _ c h e c k b o x i n t e r s e c t

  ****************************************************************

    Check if the second rectangle intersects or is completely inside
  the first rectangle.  If the rectangles are entirely outside of each
  other, -1 is returned.  If the second is entirely inside the
  first, -2 is returned.  If the first clips the second, 1 is
  returned.

*/

int gpf_checkboxintersect (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                           CSW_F x11, CSW_F y11, CSW_F x22, CSW_F y22)
{

    if (x1 >= x22  ||  y1 >= y22  ||
        x11 >= x2  ||  y11 >= y2) {
        return -2;
    }

    if (x11 >= x1  &&  x22 <= x2  &&
        y11 >= y1  &&  y22 <= y2) {
        return -1;
    }

    return 1;

}  /*  end of function gpf_checkboxintersect  */



/*
  ****************************************************************

               g p f _ p e r p d i s t a n c e 1

  ****************************************************************

    Calculate the distance from a point to a line.

*/

int gpf_perpdistance1 (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                       CSW_F x, CSW_F y, CSW_F *dist)
{
    int              istat;
    CSW_F            xp, yp;

    istat = gpf_perpintpoint1 (x1, y1, x2, y2, x, y, &xp, &yp);
    gpf_calcdistance1 (x, y, xp, yp, dist);

    return istat;

}  /*  end of function gpf_perpdistance1  */





/*
  ****************************************************************

               g p f _ p e r p d i s t a n c e 2

  ****************************************************************

    Calculate the distance from a point to a line.

*/

int gpf_perpdistance2 (double x1, double y1, double x2, double y2,
                       double x, double y, double *dist)
{
    int              istat;
    double           xp, yp;

    istat = gpf_perpintpoint2 (x1, y1, x2, y2, x, y, &xp, &yp);
    gpf_calcdistance2 (x, y, xp, yp, dist);

    return istat;

}  /*  end of function gpf_perpdistance2  */


/*
 ********************************************************************

               g p f _ p e r p d i s t a n c e x y z

 ********************************************************************

  Calculate the 3d distance between an xyz point and an xyz line.
  The algorithm used does not calculate the actual intersection
  point on the line which is perpendiculat to the target point.
  Thus, no determination of the location of that point relative
  to the input segment is available.

*/
int gpf_perpdistancexyz (
    double  x1,
    double  y1,
    double  z1,
    double  x2,
    double  y2,
    double  z2,
    double  x,
    double  y,
    double  z,
    double  *pdist)
{
    double x3, y3, z3, x4, y4, z4;
    double cp1, cp2, cp3, cp, dist12;

/*
 * Find the squared cross product between the specified segment
 * and the vector connecting an arbitrary segment endpoint to the
 * target point.
 */
    x3 = x2 - x1;
    y3 = y2 - y1;
    z3 = z2 - z1;
    x4 = x - x1;
    y4 = y - y1;
    z4 = z - z1;

    cp1 = y3 * z4 - z3 * y4;
    cp2 = x3 * z4 - z3 * x4;
    cp3 = x3 * y4 - x4 * y3;
    cp = cp1 * cp1 + cp2 * cp2 + cp3 * cp3;
    cp = sqrt (cp);

/*
 * If the cross product length is very near zero, return zero
 * as the perpendicular distance.
 */
    if (cp <= 1.e-12) {
        *pdist = 0.0;
        return 1;
    }

/*
 * The lenght of the cross product is the area of a parallelogram
 * formed from the two vectors, using the vectors as the non
 * parallel sides of the parallelogram.  The area of a parallelogram
 * is its base multiplied by its height, then the height is
 * simply the area (squared cross product) divided by the base.
 * When we choose the base to be the length of the specified input
 * segment, the height will be the perpendicular distance
 * from the specified input segment.
 */
    dist12 = x3 * x3 + y3 * y3 + z3 * z3;
    dist12 = sqrt (dist12);

    if (dist12 <= 1.e-12) {
        *pdist = 0.0;
    }
    else {
        *pdist = cp / dist12;
    }

    return 1;

}


/*
  ****************************************************************

               g p f _ p e r p d s q u a r e

  ****************************************************************

    Calculate the squared distance from a point to a line.

*/

int gpf_perpdsquare (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                     CSW_F x, CSW_F y, CSW_F *dist)
{
    int              istat;
    CSW_F            xp, yp, dx, dy;

    istat = gpf_perpintpoint1 (x1, y1, x2, y2, x, y, &xp, &yp);
    dx = xp - x;
    dy = yp - y;
    *dist = dx * dx + dy * dy;

    return istat;

}  /*  end of function gpf_perpdsquare  */




/*
  ****************************************************************

              g p f _ H o r T e x t B o x S i z e

  ****************************************************************

    Calculate the width and height of the bounding box to muliple
  lines of horizontal text.

*/

int gpf_HorTextBoxSize (const char *text, int font, CSW_F size, CSW_F gap,
                        CSW_F *width, CSW_F *height, int *nlinesout,
                        CSW_F page_units_per_inch)
{
    int         i, len, nlines;
    char        *cwork, **cptrs, *clast;
    CSW_F       wt, ht, tlen;
    CSW_F       bounds[10], tsize;

    *width = 0.0f;
    *height = 0.0f;
    *nlinesout = 0;

    if (!text) return 0;
    if (text[0] == '\0') return 0;

    len = strlen (text);

/*
    Count the lines in the text.
*/
    nlines = 1;
    for (i=0; i<len; i++) {
        if (text[i] == '\n') {
            nlines++;
        }
    }

    *nlinesout = nlines;

/*
    Allocate work space memory.
*/
    cptrs = (char **)csw_Malloc ((nlines + 1) * sizeof(char *));
    if (!cptrs) {
        return -1;
    }

    cwork = (char *)csw_Malloc ((len + 1) * sizeof(char));
    if (!cwork) {
        csw_Free (cptrs);
        return -1;
    }

/*
    Copy text into work space and separate the lines.
*/
    strcpy (cwork, text);
    csw_SeparateLines (cwork, cptrs, &nlines, nlines+1);

/*
    Determine the widest line.
*/
    wt = 0.0f;
    tsize = size / page_units_per_inch;
    for (i=0; i<nlines; i++) {
        gtx_GetTextBounds (
            cptrs[i],
            font,
            tsize,
            bounds);
        tlen = bounds[0] * page_units_per_inch;
        size = bounds[1] * page_units_per_inch;
        if (tlen > wt) {
            wt = tlen;
        }
    }

/*
    Calculate the height of all the lines.
*/
    ht = nlines * (size + gap) - gap;

    if (font < 1000) {
        clast = cptrs[nlines-1];
        len = strlen (clast);
        for (i=0; i<len; i++) {
            if (clast[i] == 'g'  ||  clast[i] == 'j'  ||  clast[i] == 'p'  ||
                clast[i] == 'q'  ||  clast[i] == 'y') {
                ht += size * .4f;
                break;
            }
        }
    }

/*
    Return the width and height values.
*/
    *width = wt;
    *height = ht;

    csw_Free (cwork);
    csw_Free (cptrs);

    return 1;

}  /*  end of function gpf_TextBoxSize  */



/*
  ****************************************************************

                g p f _ F i n d C l o s e s t

  ****************************************************************

    Given a point and an array of points, find the sequence number
  in the array that is closest spatially to the point.  On success
  the sequence number is returned.  On failure, -1 is returned.

*/

int gpf_FindClosest (CSW_F x, CSW_F y, CSW_F *xa, CSW_F *ya, int npts)
{
    int          i, imin;
    CSW_F        dx, dy, dmin;

    dmin = 1.e30f;
    imin = -1;

    for (i=0; i<npts; i++) {
        dx = xa[i] - x;
        dx *= dx;
        if (dx > dmin) continue;
        dy = ya[i] - y;
        dy *= dy;
        if (dx + dy < dmin) {
            imin = i;
            dmin = dx + dy;
        }
    }

    return imin;

}  /*  end of function gpf_FindClosest  */




/*
******************************************************************

                    g p f _ x l i m i t s

******************************************************************

  function name:    gpf_xlimits      (int)

  call sequence:    gpf_xlimits (x, npt, x1, x2)

  purpose:          Calculate the extrema of a set of CSW_F values

  return value:     always returns zero

  calling parameters:

    x        r    CSW_F*      array of x points
    npt      r    int         number of CSW_Fing point numbers in x and y
    x1       w    CSW_F*      minimum x coordinate
    x2       w    CSW_F*      maximum x coordinate

*/

int gpf_xlimits (CSW_F *x, int npt, CSW_F *x1, CSW_F *x2)
{
    int          i;

    *x1 = 1.e30f;
    *x2 = -1.e30f;

    i = 0;
    while (i < npt) {
        if (x[i] > DATA_MAX || x[i] < -DATA_MAX) {
            i++;
            continue;
        }
        if (x[i] < *x1) {
            *x1 = x[i];
        }
        if (x[i] > *x2) {
            *x2 = x[i];
        }
        i++;
    }

    return 0;

}  /*  end of function gpf_xlimits  */


/*
******************************************************************

                    g p f _ x l i m i t s 2

******************************************************************

  function name:    gpf_xlimits2      (int)

  call sequence:    gpf_xlimits2 (x, npt, x1, x2)

  purpose:          Calculate the extrema of a set of CSW_F values
                    ignoring null values

  return value:     always returns zero

  calling parameters:

    x        r    CSW_F*      array of x points
    npt      r    int         number of CSW_Fing point numbers in x and y
    x1       w    CSW_F*      minimum x coordinate
    x2       w    CSW_F*      maximum x coordinate

*/

int gpf_xlimits2 (CSW_F *x, int npt, CSW_F *x1, CSW_F *x2)
{
    int          i;

    *x1 = 1.e30f;
    *x2 = -1.e30f;

    i = 0;
    while (i < npt) {
        if (x[i] > DATA_MAX || x[i] < -DATA_MAX) {
            i++;
            continue;
        }
        if (x[i] < *x1  &&  x[i] < 1.e15f  &&  x[i] > -1.e15f) {
            *x1 = x[i];
        }
        if (x[i] > *x2  &&  x[i] < 1.e15f  &&  x[i] > -1.e15f) {
            *x2 = x[i];
        }
        i++;
    }

    return 0;

}  /*  end of function gpf_xlimits2  */




/*
  ****************************************************************

                  g p f _ F o r m a t N u m b e r

  ****************************************************************

    Encode a linear or logarithmic number into a text buffer for
  subsequent use in a graph text structure.  This is used for
  axis labels.

*/

int gpf_FormatNumber (CSW_F value, CSW_F logbase, int ndecin,
                      char *buf)
{
    char         fmt[20], fmt2[10], *ctmp;
    int          ndec, ilog, i, len, dpos, zflag;
    CSW_F        fval;

    fval = value;
    if (fval < 0.0f) fval = -fval;

    if (fval == 0.0f) {
        ilog = 1;
    }
    else {
        ilog = (int)log10 ((double)fval);
    }
    ilog--;
    if (fval < 0.5f) {
        ndec = -ilog + 2;
    }
    else if (fval < 5.0f) {
        ndec = 3;
    }
    else if (fval < 50.0f) {
        ndec = 2;
    }
    else if (fval < 500.0f) {
        ndec = 1;
    }
    else {
        ndec = 0;
    }
    if (ndec > 7) ndec = 7;

    if (ndecin > 0) ndec = ndecin;

    zflag = 0;
    if (logbase > 1.01f) {
        value = (CSW_F)pow ((double)logbase, (double)value);
        strcpy (fmt, "%8.6g");
    }
    else {
        strcpy (fmt, "%10.");
        sprintf (fmt2, "%d", ndec);
        strcat (fmt, fmt2);
        if (ndec >= 6) {
            strcat (fmt, "g");
        }
        else {
            zflag = 1;
            strcat (fmt, "f");
        }
    }

    sprintf (buf, fmt, value);

    csw_StrLeftJust (buf);

    if (buf[0] == '0') {
        if (buf[1] != '\0') {
            buf[0] = ' ';
            csw_StrLeftJust (buf);
        }
    }

/*
    Remove trailing zeros if there is a decimal point
*/
    if (zflag) {
        len = strlen (buf);
        ctmp = strchr (buf, '.');
        if (ctmp) {
            dpos = ctmp - buf;
            for (i=len-1; i>dpos; i--) {
                if (buf[i] == '0') {
                    buf[i] = '\0';
                }
                else {
                    break;
                }
            }
            len = strlen (buf);
            if (buf[len-1] == '.') {
                buf[len-1] = '\0';
            }
        }
    }

    if (!strcmp (buf, "-0")) {
        strcpy (buf, "0");
    }

    return 1;

}  /*  end of function gpf_FormatNumber  */



/*
 ******************************************************************************

                g p f _ p o l y g o n a l i z e _ l i n e

 ******************************************************************************

  This function turns an unclosed polyline into a polygon by adding points
  offset from each line vertex.  The resulting polygon will start at the
  first point in the polyline and the first half of its points will be
  generated by offsetting the original points to the left.  The remainder
  of the output polygon points will be the original points in their
  reverse order.

  The calling function must provide enough space for the output in the
  xout and yout arrays.  An error is returned if that is not the case.
  There will be 2 * nptsin + 1 output points.

*/

int gpf_polygonalize_line (double *xin, double *yin, void **tagsin,
                           int nptsin, double tiny,
                           double *xout, double *yout, void **tagsout,
                           int *nout, int maxout)
{
    int              i, n, npts;
    double           *x, *y;
    void             **tags;
    double           ang1, ang2, ang3,
                     dx1, dy1, dx2, dy2, xt, yt;

    *nout = 0;
    if (nptsin < 2) {
        return -1;
    }

    if (maxout < nptsin * 2 + 1) return -1;
    x = xin;
    y = yin;
    tags = tagsin;
    npts = nptsin;

/*
    The first 2 output points are the first input point
    and a perpendicular offset to its left.
*/
    xout[0] = x[0];
    yout[0] = y[0];
    dx1 = x[1] - x[0];
    dy1 = y[1] - y[0];
    ang1 = atan2 (dy1, dx1) + HALFPI;
    xt = tiny * cos(ang1);
    yt = tiny * sin(ang1);
    xout[1] = x[0] + xt;
    yout[1] = y[0] + yt;

    if (tags && tagsout) {
        tagsout[0] = tags[0];
        tagsout[1] = NULL;
    }
    n = 2;

/*
    Create perpendicular offset points for the interior points of the line.
*/
    for (i=1; i<npts-1; i++) {
        dx1 = x[i] - x[i-1];
        dy1 = y[i] - y[i-1];
        dx2 = x[i+1] - x[i];
        dy2 = y[i+1] - y[i];
        ang1 = atan2 (dy1, dx1) + HALFPI;
        ang2 = atan2 (dy2, dx2) + HALFPI;
        ang3 = (ang1 + ang2) / 2.0;
        xt = tiny * cos(ang3);
        yt = tiny * sin(ang3);
        xt += x[i];
        yt += y[i];
        if (BadOffsetPoint (x[i], y[i], x[i-1], y[i-1], x[i+1], y[i+1],
                            xt, yt, xout[n-1], yout[n-1])) {
            ang3 += PI;
            xt = tiny * cos(ang3);
            yt = tiny * sin(ang3);
            xt += x[i];
            yt += y[i];
        }
        xout[n] = xt;
        yout[n] = yt;
        if (tags && tagsout) {
            tagsout[n] = NULL;
        }
        n++;
    }

/*
    Perpendicular offset for the last point of the line.
*/
    dx1 = x[npts-1] - x[npts-2];
    dy1 = y[npts-1] - y[npts-2];
    ang1 = atan2 (dy1, dx1) + HALFPI;
    xt = tiny * cos(ang1);
    yt = tiny * sin(ang1);
    xout[n] = x[npts-1] + xt;
    yout[n] = y[npts-1] + yt;
    if (tags && tagsout) {
        tagsout[n] = NULL;
    }
    n++;

/*
    Complete polygon by reversing the direction of
    the original points.
*/
    for (i=npts-1; i>=0; i--) {
        xout[n] = x[i];
        yout[n] = y[i];
        if (tags && tagsout) {
            tagsout[n] = tags[i];
        }
        n++;
    }

    *nout = n;

    return 1;

}  /* end of function gpf_polygonalize_line */



/*
 ************************************************************************

      g p f _ c h e c k _ f o r _ s e l f _ i n t e r s e c t i o n

 ************************************************************************

  Return 1 if any segments in the line intersect each other.  Return
  zero if they do not.  Any intersecting segments are returned by
  the number of the point starting the segment.  For example, if the
  returned seglist has a single entry with a value of 2, then the
  segment from point 2 to point 3 intersects some other segment in
  the line.

*/

int gpf_check_for_self_intersection (double *x, double *y, int npts,
                                     int *seglist, int *nseg, double tiny,
                                     int nsegmax)

{
    int        i, j, n, istat, closed;
    double     x1, y1, x2, y2, xt, yt;
    double     dist, pct, dx, dy;

    CSWPolyUtils  ply_utils_obj;

    *nseg = 0;
    if (npts < 3) return 0;

    closed = 0;
    if (x[0] - x[npts-1] == 0.0  &&  y[0] - y[npts-1] == 0.0) {
        closed = 1;
    }

    n = 0;
    for (i=0; i<npts-1; i++) {
        x1 = x[i];
        y1 = y[i];
        x2 = x[i+1];
        y2 = y[i+1];
        dx = x2 - x1;
        dy = y2 - y1;
        dist = dx * dx + dy * dy;
        dist = sqrt(dist);
        if (dist <= tiny) continue;
        pct = tiny / dist;
        x1 += dx * pct;
        x2 -= dx * pct;
        y1 += dy * pct;
        y2 -= dy * pct;
        for (j=i+1; j<npts-1; j++) {
            istat = ply_utils_obj.ply_segint (x1, y1, x2, y2,
                                x[j], y[j], x[j+1], y[j+1],
                                &xt, &yt);
            if (j == i+1) {
                if (istat == 3  ||
                    istat == 4) {
                    seglist[n] = i;
                    n++;
                    break;
                }
            }
            else if (closed  &&  j == npts-2  &&  i == 0) {
                if (istat == 3  ||
                    istat == 4) {
                    seglist[n] = i;
                    n++;
                    break;
                }
            }
            else {
                if (istat == 0  ||
                    istat == 3  ||
                    istat == 4) {
                    seglist[n] = i;
                    n++;
                    break;
                }
            }
        }
        if (n >= nsegmax) {
            printf ("Too many self intersecting points in a line\n");
            printf ("The application specified a maximum of %d\n", nsegmax);
            assert (n >= nsegmax);
        }
    }

    *nseg = n;

    if (n > 0) return 1;

    return 0;

}  /* end of function gpf_check_for_self_intersection */




/*
 ************************************************************************************

                          B a d O f f s e t P o i n t

 ************************************************************************************

  Check the xt,yt point to see if it is on the correct side of the line for
  polygonalization.  If the segment between the resultant offset point and
  the previous output point intersects either of the line vectors, then the
  resultant point is on the wrong side of the line.

  The function returns 1 if the resultant is on the wrong side of the line
  or it returns zero if the resultant is on the correct side of the line.

*/

static int BadOffsetPoint (double xc, double yc,
                           double xm1, double ym1,
                           double xp1, double yp1,
                           double xt, double yt,
                           double xpout, double ypout)
{
    int                 istat;
    double              xdum, ydum;

    CSWPolyUtils     ply_utils_obj;

    istat = ply_utils_obj.ply_segint (xc, yc, xm1, ym1,
                        xt, yt, xpout, ypout,
                        &xdum, &ydum);
    if (istat == 0) {
        return 1;
    }

    istat = ply_utils_obj.ply_segint (xc, yc, xp1, yp1,
                        xt, yt, xpout, ypout,
                        &xdum, &ydum);
    if (istat == 0) {
        return 1;
    }

    return 0;

}  /* end of static BadOffsetPoint function */



/*
******************************************************************

             g p f _ c a l c l i n e d i s t 2 x y

******************************************************************

  function name:    gpf_calclinedist2xy     (int)

  call sequence:    gpf_calclinedist2xy (x, y, npts, xin, yin, dist)

  purpose:          calculate the closest distance between a polyline
                    and a point.  This is the double precision version.

  return value:     Returns zero if the closest point lies inside the polyline
                    or returns 1 if the closest point is one of the polyline
                    end points.

  calling parameters:

    x         r    double*   array of line x coordinates
    y         r    double*   array of line y coordinates
    npts      r    int       number of points in polyline
    xin       r    double    x coordinate of target point
    yin       r    double    y coordinate of target point
    dist      w    double*   distance from point to line

*/

int gpf_calclinedist2xy (double *x, double *y, int npts,
                         double xin, double yin, double *dist)
{
    double    x1, y1, x2, y2, x3, y3, xp, yp, dx, dy;
    int       i, istat;
    double    dtmp, dmax;

/*  find the closest perpendicular distance to a polyline segment  */

    dmax = 1.e31f;
    x1 = x[0];
    y1 = y[0];
    x2 = x1;
    y2 = y1;

    for (i=1; i<npts; i++) {
        x2 = x[i];
        y2 = y[i];
        if (i == npts - 1) {
            istat = gpf_perpintpoint2 (x1, y1, x2, y2, xin, yin, &xp, &yp);
        }
        else {
            x3 = x[i+1];
            y3 = y[i+1];
            istat = gpf_perpintpoint3 (x1, y1, x2, y2, x3, y3,
                                       xin, yin, &xp, &yp);
        }
        if (istat == 1) {
            dx = xin - xp;
            dy = yin - yp;
            dtmp = dx*dx + dy*dy;
            dtmp = sqrt (dtmp);
            if (dtmp < dmax) {
                dmax = dtmp;
            }
        }
        x1 = x2;
        y1 = y2;
    }

    if (dmax < 1.e30f) {
        *dist = dmax;
        return 0;
    }

/*  use the closest polyline endpoint if no perpendicular distance
    can be calculated  */

    dx = x2 - xin;
    dy = y2 - yin;
    dtmp = dx*dx + dy*dy;
    dtmp = sqrt (dtmp);
    if (dtmp < dmax) {
        dmax = dtmp;
    }

    dx = x[0] - xin;
    dy = y[0] - yin;
    dtmp = dx*dx + dy*dy;
    dtmp = sqrt (dtmp);
    if (dtmp < dmax) {
        dmax = dtmp;
    }

    *dist = dmax;

    return 1;

}  /*  end of function gpf_calclinedist2xy  */


#define MAX_FONT    1000
#define MAX_PARTS   100

/*
 * Return the vectors for the strokes of a font.
 *
 * The output arrays are allocated by the calling function.
 * You should have 1000 points and 100 components per character to
 * be completely safe.
 */

int gpf_get_lines_for_text (char *text,
                            int nc,
                            double anglein,
                            double sizein,
                            int anchor,
                            int ifontin,
                            double *xline,
                            double *yline,
                            int *icline,
                            int *nline,
                            int maxpts,
                            int maxcomp)

{
    CSW_F          angle, textlen, charwidth, cosang, sinang, xchar, ychar;
    CSW_F          x, y;
    int            istat, i, j, k, kk, n, nparts;
    int            npout, ncout, itext;
    CSW_F          tlen, fsize;
    int            ifont;

    CSW_F          Fontx[MAX_FONT], Fonty[MAX_FONT];
    int            parts[MAX_PARTS];

    GPFFont        font_obj;

    if (nline == NULL) {
        return -1;
    }

    *nline = 0;

    if (xline == NULL  ||  yline == NULL  ||  icline == NULL) {
        return -1;
    }

    if (maxpts < 10  ||  maxcomp < 1) {
        return -1;
    }

    if (text == NULL) {
        return -1;
    }

    if (nc < 1) {
        return -1;
    }

    if (ifontin == 0) {
        ifont = 102;
    }
    else {
        ifont = 105;
    }
    font_obj.gpf_setfont (ifont);

    x = 0.0;
    y = 0.0;

    angle = (CSW_F)anglein * 3.1415926f / 180.0f;

    sinang = (CSW_F)(sin((double)angle));
    cosang = (CSW_F)(cos((double)angle));

    fsize = (CSW_F)sizein;
    tlen = 0.0f;

    if (anchor != 0) {
        font_obj.gpf_TextLength2 (text, nc, ifont, fsize, &tlen);
        if (anchor == 1) {
            tlen /= 2.0f;
        }
    }

    x = -tlen * cosang;
    y = -tlen * sinang;

/*  loop through text, getting strokes for each character  */

    xchar = x;
    ychar = y;
    textlen = 0.0;
    npout = 0;
    ncout = 0;

    for (i=0; i<nc; i++) {

        itext = (int)text[i];
        if (isspace(itext)) {
            font_obj.gpf_CharWidth (text[i], fsize, &charwidth);
            textlen += charwidth;
            xchar = textlen * cosang + x;
            ychar = textlen * sinang + y;
            continue;
        }

/*      get strokes for a character  */

        istat = font_obj.gpf_getfontstrokes (text[i], fsize, Fontx, Fonty, parts, &nparts,
                                    MAX_FONT, MAX_PARTS);
        if (!istat) {
            continue;
        }

        n = 0;
        for (j=0; j<nparts; j++) {
            n += parts[j];
        }

/*      rotate the character if not at a zero angle  */

        if (angle < -0.005  ||  angle > 0.005) {
            font_obj.gpf_rotatechar (Fontx, Fonty, n, cosang, sinang);
        }

/*      translate to plot coordinates  */

        for (j=0; j<n; j++) {
            Fontx[j] += xchar;
            Fonty[j] += ychar;
        }

/*      output each piece of the character  */

        kk = 0;
        for (j=0; j<nparts; j++) {
            n = parts[j];
            for (k=0; k<n; k++) {
                xline[npout] = (double)Fontx[kk];
                yline[npout] = (double)Fonty[kk];
                kk++;
                npout++;
                if (npout >= maxpts) {
                    return -2;
                }
            }
            icline[ncout] = n;
            ncout++;
            *nline = ncout;
            if (ncout >= maxcomp) {
                return -2;
            }
        }

/*      get position of next character  */

        font_obj.gpf_CharWidth (text[i], fsize, &charwidth);
        textlen += charwidth;
        xchar = textlen * cosang + x;
        ychar = textlen * sinang + y;

    }

    *nline = ncout;

    return 1;

}  /*  end of function gpf_get_lines_for_text  */

#undef MAX_FONT
#undef MAX_PARTS




/*
******************************************************************

             g p f _ c a l c l i n e d i s t 1 x y

******************************************************************

  function name:    gpf_calclinedist1xy     (int)

  call sequence:    gpf_calclinedist1xy (x, y, npts, xin, yin, dist)

  purpose:          calculate the closest distance between a polyline
                    and a point.  This is the double precision version.

  return value:     always returns zero

  calling parameters:

    x         r    CSW_F *   array of line x coordinates
    y         r    CSW_F *   array of line y coordinates
    npts      r    int       number of points in polyline
    xin       r    CSW_F     x coordinate of target point
    yin       r    CSW_F     y coordinate of target point
    dist      w    CSW_F *   distance from point to line

*/

int gpf_calclinedist1xy (CSW_F  *x, CSW_F  *y, int npts,
                         CSW_F  xin, CSW_F  yin, CSW_F  *dist)
{
    CSW_F     x1, y1, x2, y2, xp, yp, dx, dy;
    int       i, istat;
    CSW_F     dtmp, dmax;

/*  find the closest perpendicular distance to a polyline segment  */

    dmax = 1.e31f;
    x1 = x[0];
    y1 = y[0];
    x2 = x1;
    y2 = y1;

    for (i=1; i<npts; i++) {
        x2 = x[i];
        y2 = y[i];
        istat = gpf_perpintpoint1 (x1, y1, x2, y2, xin, yin, &xp, &yp);
        if (istat == 1) {
            dx = xin - xp;
            dy = yin - yp;
            dtmp = dx*dx + dy*dy;
            dtmp = (CSW_F)sqrt ((double)dtmp);
            if (dtmp < dmax) {
                dmax = dtmp;
            }
        }
        x1 = x2;
        y1 = y2;
    }

    if (dmax < 1.e30f) {
        *dist = dmax;
        return 0;
    }

/*  use the closest polyline endpoint if no perpendicular distance
    can be calculated  */

    dx = x2 - xin;
    dy = y2 - yin;
    dtmp = dx*dx + dy*dy;
    dtmp = (CSW_F)sqrt ((double)dtmp);
    if (dtmp < dmax) {
        dmax = dtmp;
    }

    dx = x[0] - xin;
    dy = y[0] - yin;
    dtmp = dx*dx + dy*dy;
    dtmp = (CSW_F)sqrt ((double)dtmp);
    if (dtmp < dmax) {
        dmax = dtmp;
    }

    *dist = dmax;

    return 1;

}  /*  end of function gpf_calclinedist1xy  */


/*-----------------------------------------------------------------------------*/

/*
 * Resample a line at equally spaced points as close as possible to the specified
 * average spacing.  The first and last points of the line are not changed.
 */

int gpf_resample_line (
    double *xin, double *yin, double *zin, int npts,
    double avspace,
    double **xout, double **yout, double **zout, int *n_out)
{
    double     *line_dist;
    double     xt, yt, zt, dx, dy, dz, dist, dtot;
    double     *x, *y, *z;
    int        istat, i, nout, nmax, ibase, nextbase;

    *xout = NULL;
    *yout = NULL;
    *zout = NULL;
    *n_out = 0;

/*
 * Obvious errors.
 */
    if (npts < 2) {
        return -1;
    }
    if (xin == NULL  ||  yin == NULL  ||  zin == NULL) {
        return -1;
    }
    if (avspace <= 0.0) {
        return -1;
    }

/*
 * Allocate space for distance to point arrays.
 */
    line_dist = (double *)csw_Malloc (npts * sizeof(double));
    if (line_dist == NULL) {
        return -1;
    }

/*
 * Fill in distance along line at each point values.
 */
    line_dist[0] = 0.0;
    dtot = 0.0;
    for (i=1; i<npts; i++) {
        dx = xin[i-1] - xin[i];
        dy = yin[i-1] - yin[i];
        if (zin[i] < -1.e19  ||  zin[i] > 1.e19  ||
            zin[i-1] < -1.e19  ||  zin[i-1] > 1.e19) {
            dz = 0.0;
        }
        else {
            dz = zin[i-1] - zin[i];
        }
        dist = dx * dx + dy * dy + dz * dz;
        dist = sqrt (dist);
        dtot += dist;
        line_dist[i] = dtot;
    }

/*
 * Allocate space for output arrays.
 */
    nmax = (int)(line_dist[npts-1] / avspace + .5);
    if (nmax > 1000 * npts) {
        csw_Free (line_dist);
        return -1;
    }
    nmax += 3;

    x = (double *)csw_Malloc (3 * nmax * sizeof(double));
    if (x == NULL) {
        csw_Free (line_dist);
        return -1;
    }
    y = x + nmax;
    z = y + nmax;

    nout = nmax - 1;

    avspace = line_dist[npts-1] / (nout - 1);
    ibase = 0;
    x[0] = xin[0];
    y[0] = yin[0];
    z[0] = zin[0];

/*
 * Find points at avspace distances along the line and put them into
 * the output arrays.
 */
    for (i=1; i<nout-1; i++) {

        dtot = i * avspace;
        istat = PointAtDistance (line_dist, npts,
                                 xin, yin, zin,
                                 ibase, dtot,
                                 &xt, &yt, &zt,
                                 &nextbase);
        if (istat != 1) {
            csw_Free (x);
            return -1;
        }

        ibase = nextbase;

        x[i] = xt;
        y[i] = yt;
        z[i] = zt;

    }

    x[nout-1] = xin[npts-1];
    y[nout-1] = yin[npts-1];
    z[nout-1] = zin[npts-1];

    *xout = x;
    *yout = y;
    *zout = z;
    *n_out = nout;

    csw_Free (line_dist);

    return 1;

}

static int PointAtDistance (double *LineDistance, int Nline,
                            double *Xline, double *Yline, double *Zline,
                            int ibase, double dist,
                            double *x, double *y, double *z,
                            int *nextibase)

{
    int           i, i1, i2;
    double        dx, dy, dz, d1, d2, pct;

    i1 = Nline - 2;
    i2 = i1 + 1;
    for (i=ibase; i<Nline; i++) {
        if (dist < LineDistance[i]) {
            i1 = i - 1;
            i2 = i;
            break;
        }
    }

    if (i1 < 0) i1 = 0;

    d1 = LineDistance[i1];
    d2 = LineDistance[i2];

    if (d2 - d1 == 0.0f)
        pct = 0.0f;
    else
        pct = (dist - d1) / (d2 - d1);

    dx = Xline[i2] - Xline[i1];
    dy = Yline[i2] - Yline[i1];
    dz = Zline[i2] - Zline[i1];

    dx *= pct;
    dy *= pct;
    dz *= pct;

    *x = Xline[i1] + dx;
    *y = Yline[i1] + dy;
    *z = Zline[i1] + dz;

    *nextibase = i1;

    return 1;

}  /*  end of static PointAtDistance function  */



/*
 ************************************************************************

      g p f _ c h e c k _ f o r _ i n t e r s e c t i o n

 ************************************************************************

  Return the number of intersections if any segments in the lines
  intersect each other.  Return zero if they do not.  Return -1
  on an error.

*/

int gpf_check_for_intersection (double *x, double *y, int npts,
                                double *xx, double *yy, int npts2,
                                double tiny)
{
    int        i, j, n, istat;
    double     x1, y1, x2, y2, xt, yt;
    double     gsav;

    CSWPolyUtils  ply_utils_obj;

    ply_utils_obj.ply_getgraze (&gsav);
    ply_utils_obj.ply_setgraze (tiny);
    n = 0;
    for (i=0; i<npts-1; i++) {
        x1 = x[i];
        y1 = y[i];
        x2 = x[i+1];
        y2 = y[i+1];
        for (j=0; j<npts2-1; j++) {
            istat = ply_utils_obj.ply_segint (x1, y1, x2, y2,
                                xx[j], yy[j], xx[j+1], yy[j+1],
                                &xt, &yt);
            if (istat == 0  ||
                istat == 3  ||
                istat == 4) {
                n++;
            }
        }
    }
    ply_utils_obj.ply_setgraze (gsav);

    return n;

}  /* end of function gpf_check_for_intersection */


/*-----------------------------------------------------------------------------*/

/*
 * Resample a line at equally spaced points as close as possible to the specified
 * average spacing.  The first and last points of the line are not changed.
 */

int gpf_resample_line_segments (
    double *xin, double *yin, double *zin, int npts,
    double avspace,
    double **xout, double **yout, double **zout, int *n_out)
{
    double     xt, yt, zt, dx, dy, dz, dist, dtot;
    double     *x, *y, *z;
    int        i, n, nmax, j, jdo;

    *xout = NULL;
    *yout = NULL;
    *zout = NULL;
    *n_out = 0;

/*
 * Obvious errors.
 */
    if (npts < 2) {
        return -1;
    }
    if (xin == NULL  ||  yin == NULL  ||  zin == NULL) {
        return -1;
    }
    if (avspace <= 0.0) {
        return -1;
    }

/*
 * Find the total distance along the line.
 */
    dtot = 0.0;
    for (i=1; i<npts; i++) {
        dx = xin[i-1] - xin[i];
        dy = yin[i-1] - yin[i];
        dz = zin[i-1] - zin[i];
        if (zin[i] < -1.e19  ||  zin[i] > 1.e19  ||
            zin[i-1] < -1.e19  ||  zin[i-1] > 1.e19) {
            dz = 0.0;
        }
        dist = dx * dx + dy * dy + dz * dz;
        dist = sqrt (dist);
        dtot += dist;
    }

/*
 * Allocate space for output arrays.
 */
    nmax = (int)(dtot / avspace + .5);
    if (nmax > 1000 * npts) {
        return -1;
    }
    nmax += npts;
    nmax *= 2;

    x = (double *)csw_Malloc (3 * nmax * sizeof(double));
    if (x == NULL) {
        return -1;
    }
    y = x + nmax;
    z = y + nmax;

    x[0] = xin[0];
    y[0] = yin[0];
    z[0] = zin[0];

/*
 * Find points at close to avspace distances along each segment
 * of the line and put them into the output arrays.
 */
    n = 1;
    for (i=1; i<npts; i++) {

        dx = xin[i] - xin[i-1];
        dy = yin[i] - yin[i-1];
        dz = zin[i] - zin[i-1];
        if (zin[i] < -1.e19  ||  zin[i] > 1.e19  ||
            zin[i-1] < -1.e19  ||  zin[i-1] > 1.e19) {
            dz = 0.0;
        }
        dist = dx * dx + dy * dy + dz * dz;
        dist = sqrt (dist);

        if (dist < avspace * 1.5) {
            x[n] = xin[i];
            y[n] = yin[i];
            z[n] = zin[i];
            n++;
            continue;
        }

        jdo = (int)(dist / avspace) + 1;
        dx /= jdo;
        dy /= jdo;
        dz /= jdo;

        for (j=1; j<jdo; j++) {
            xt = xin[i-1] + dx * j;
            yt = yin[i-1] + dy * j;
            zt = zin[i-1] + dz * j;
            x[n] = xt;
            y[n] = yt;
            z[n] = zt;
            n++;
        }

        x[n] = xin[i];
        y[n] = yin[i];
        z[n] = zin[i];
        n++;

    }

    *xout = x;
    *yout = y;
    *zout = z;
    *n_out = n;

    return 1;

}



/*----------------------------------------------------------------------------------------*/

/*
 * Return the area and centroid of a polygon.  On success, 1 is returned.
 * On error, -1 is returned.
 */
int gpf_CalcPolygonAreaAndCentroid (
    double      *x,
    double      *y,
    int         n,
    double      *xcenter,
    double      *ycenter,
    double      *area)
{
    int         i, j;
    double      ai, atmp, xtmp, ytmp;

    *xcenter = 1.e30;
    *ycenter = 1.e30;
    *area = 1.e30;

    if (n < 3) {
        return -1;
    }

    atmp = 0.0;
    xtmp = 0.0;
    ytmp = 0.0;

    for (i=n-1, j=0; j<n; i=j, j++) {
        ai = x[i] * y[j] - x[j] * y[i];
        atmp += ai;
        xtmp += (x[j] + x[i]) * ai;
        ytmp += (y[j] + y[i]) * ai;
    }
    if (atmp < 0.0) atmp = -atmp;
    *area = atmp / 2.0;

    if (atmp != 0.0) {
        *xcenter = xtmp / (3.0 * atmp);
        *ycenter = ytmp / (3.0 * atmp);
        return 1;
    }

    return -1;

}


/*-----------------------------------------------------------------------------*/

int gpf_CalcXYDistanceAlongLine (
    double      *x,
    double      *y,
    int         npts,
    double      *dout)
{
    int         i;
    double      dx, dy, dist, sum;

    *dout = 0.0;
    if (npts < 2) {
        return -1;
    }

    sum = 0;
    for (i=0; i<npts-1; i++) {
        dx = x[i] - x[i+1];
        dy = y[i] - y[i+1];
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        sum += dist;
    }

    *dout = sum;

    return 1;
}



/*-----------------------------------------------------------------------------*/

int gpf_CalcXYZDistanceAlongLine (
    double      *x,
    double      *y,
    double      *z,
    int         npts,
    double      *dout)
{
    int         i;
    double      dx, dy, dz, dist, sum;

    *dout = 0.0;
    if (npts < 2) {
        return -1;
    }

    sum = 0;
    for (i=0; i<npts-1; i++) {
        dx = x[i] - x[i+1];
        dy = y[i] - y[i+1];
        dz = z[i] - z[i+1];
        dist = dx * dx + dy * dy + dz * dz;
        dist = sqrt (dist);
        sum += dist;
    }

    *dout = sum;

    return 1;
}




/*-----------------------------------------------------------------------------*/

/*
 * Resample a line according to the spacings array.  The first and last points
 * are not changed.  The spacings array has a spacing for each input point.
 * This is translated to the output line according to distance along each line.
 * In other words, the input point closest to the current distance along the
 * output line is used for the spacing for the next output line point.
 */

int gpf_resample_variable_line (
    double *xin, double *yin, double *zin, int npts,
    double *spacings,
    double **xout, double **yout, double **zout, int *n_out)
{
    double     *line_dist;
    double     xt, yt, zt, dx, dy, dz, dist, dtot, dtotsav;
    double     *x, *y, *z;
    int        istat, i, nout, nmax, ibase, nextbase;

    *xout = NULL;
    *yout = NULL;
    *zout = NULL;
    *n_out = 0;

/*
 * Obvious errors.
 */
    if (npts < 2) {
        return -1;
    }
    if (xin == NULL  ||  yin == NULL  ||  zin == NULL) {
        return -1;
    }

/*
 * Allocate space for distance to point arrays.
 */
    line_dist = (double *)csw_Malloc (npts * sizeof(double));
    if (line_dist == NULL) {
        return -1;
    }

/*
 * Fill in distance along line at each point values.
 */
    line_dist[0] = 0.0;
    dtot = 0.0;
    for (i=1; i<npts; i++) {
        dx = xin[i-1] - xin[i];
        dy = yin[i-1] - yin[i];
        if (zin[i] < -1.e19  ||  zin[i] > 1.e19  ||
            zin[i-1] < -1.e19  ||  zin[i-1] > 1.e19) {
            dz = 0.0;
        }
        else {
            dz = zin[i-1] - zin[i];
        }
        dist = dx * dx + dy * dy + dz * dz;
        dist = sqrt (dist);
        dtot += dist;
        line_dist[i] = dtot;
    }

    dtotsav = dtot;

/*
 * Allocate space for output arrays.
 */
    nmax = npts * 4;
    nmax += 3;

    x = (double *)csw_Malloc (3 * nmax * sizeof(double));
    if (x == NULL) {
        csw_Free (line_dist);
        return -1;
    }
    y = x + nmax;
    z = y + nmax;

    ibase = 0;
    x[0] = xin[0];
    y[0] = yin[0];
    z[0] = zin[0];

    nout = 1;
/*
 * Find points at varying distances along the line and put them into
 * the output arrays.
 */
    dtot = 0.0;
    for (;;) {

        dtot += spacings[ibase];

        if (dtot >= dtotsav) {
            break;
        }

        istat = PointAtDistance (line_dist, npts,
                                 xin, yin, zin,
                                 ibase, dtot,
                                 &xt, &yt, &zt,
                                 &nextbase);
        if (istat != 1) {
            csw_Free (x);
            return -1;
        }

        ibase = nextbase;

        x[nout] = xt;
        y[nout] = yt;
        z[nout] = zt;
        nout++;
        if (nout >= nmax) {
            assert (0);
        }

    }

    dx = xin[npts-1] - x[nout-1];
    dy = yin[npts-1] - y[nout-1];
    dz = zin[npts-1] - z[nout-1];
    dist = dx * dx + dy * dy + dz * dz;
    dist = sqrt (dist);

    if (dist < 0.5 * spacings[npts-1]  &&  nout > 1) {
        x[nout-1] = xin[npts-1];
        y[nout-1] = yin[npts-1];
        z[nout-1] = zin[npts-1];
    }
    else {
        nout++;
        x[nout-1] = xin[npts-1];
        y[nout-1] = yin[npts-1];
        z[nout-1] = zin[npts-1];
    }

    *xout = x;
    *yout = y;
    *zout = z;
    *n_out = nout;

    csw_Free (line_dist);

    return 1;

}



/*
******************************************************************

              g p f _ p e r p i n t p o i n t 3

******************************************************************

  function name:    gpf_perpintpoint3      (int)

  call sequence:    gpf_perpintpoint3 (x1, y1, x2, y2, x3, y3,
                                       x, y, xp, yp)

  purpose:          Calculate a point on a line which is perpendicular to
                    a target point.  The target point can be on or off the
                    line.  This is the double precision version.

                    In this version, the perpendicular intersection is
                    calculated using the segment from x1,y1 to x2,y2.
                    However, the test for "inside segment" uses x1,y1 and
                    x3,y3.

  return value:     status code

                    0 = perpendicular point is outside the line segment
                    1 = perpendicular point is inside the line segment

  calling parameters:

    x1     r    double       x coordinate of first segment endpoint
    y1     r    double       y coordinate of first segment endpoint
    x2     r    double       x coordinate of second endpoint
    y2     r    double       y coordinate of second endpoint
    x3     r    double       x coordinate for auzuliary point
    y3     r    double       y coordinate for auzuliary point
    x      r    double       x coordinate of target point
    y      r    double       y coordinate of target point
    xp     w    double*      x coordinate of intersection point
    yp     w    double*      y coordinate of intersection point

*/

int gpf_perpintpoint3 (double x1, double y1, double x2, double y2,
                       double x3, double y3,
                       double x, double y, double *xp, double *yp)

{
    double     dx, dy, s1, s2, tmp1, tmp2, divide, b1, b2;
    double     tiny, dx2, dy2, dx3, dy3;
    int        istat;

    divide = 1000000.f;

/*
    Special cases for nearly vertical or nearly horizontal lines.
*/
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
        return 0;
    }

    if (tmp1 < tmp2/divide) {
        *xp = (x1 + x2) / 2.0f;
        *yp = y;
        if ((y1-y)*(y-y2) >= 0.0) {
            return 1;
        }
        return 0;
    }

    if (tmp2 < tmp1/divide) {
        *xp = x;
        *yp = (y1 + y2) / 2.0f;
        if ((x1-x)*(x-x2) >= 0.0) {
            return 1;
        }
        return 0;
    }

/*  intersect a perpendicular from the target with the segment  */

    s1 = dy / dx;
    s2 = -1.f / s1;
    b1 = y1 - s1 * x1;
    b2 = y - s2 * x;

    *xp = (b1 - b2) / (s2 - s1);
    *yp = s1 * *xp + b1;

/*  check if the intersection point is inside the segment endpoints  */

    dx3 = x3 - x1;
    if (dx3 < 0.0) dx3 = -dx3;
    dy3 = y3 - y1;
    if (dy3 < 0.0) dy3 = -dy3;
    if (dx3 > dy3) {
        if ((x1-*xp)*(*xp-x3) >= 0.0) {
            return 1;
        }
        else {
            dx2 = x2 - x1;
            dy2 = y2 - y1;
            tiny = dx2 * dx2 + dy2 * dy2;
            tiny = sqrt (tiny);
            tiny /= 1000.0;
            istat = checkGraze (
                x1,
                y1,
                *xp,
                *yp,
                tiny);
            if (istat == 1) {
                return 1;
            }
            istat = checkGraze (
                x3,
                y3,
                *xp,
                *yp,
                tiny);
            if (istat == 1) {
                return 1;
            }
            return 0;
        }
    }
    else {
        if ((y1-*yp)*(*yp-y3) >= 0.0) {
            return 1;
        }
        else {
            dx2 = x2 - x1;
            dy2 = y2 - y1;
            tiny = dx2 * dx2 + dy2 * dy2;
            tiny = sqrt (tiny);
            tiny /= 1000.0;
            istat = checkGraze (
                x1,
                y1,
                *xp,
                *yp,
                tiny);
            if (istat == 1) {
                return 1;
            }
            istat = checkGraze (
                x3,
                y3,
                *xp,
                *yp,
                tiny);
            if (istat == 1) {
                return 1;
            }
            return 0;
        }
    }

}  /*  end of function gpf_perpintpoint3  */

/*-----------------------------------------------------------------------------*/

/*
 * Resample a line at equally spaced points as close as possible to the specified
 * average spacing.  The first and last points of the line are not changed.
 * In this version, only the x and y coordinates are used to determine the new
 * sampling intervals.
 */

int gpf_resample_line_xy (
    double *xin, double *yin, double *zin, int npts,
    double avspace,
    double **xout, double **yout, double **zout, int *n_out)
{
    double     *line_dist;
    double     xt, yt, zt, dx, dy, dist, dtot;
    double     *x, *y, *z;
    int        istat, i, nout, nmax, ibase, nextbase;

    *xout = NULL;
    *yout = NULL;
    *zout = NULL;
    *n_out = 0;

/*
 * Obvious errors.
 */
    if (npts < 2) {
        return -1;
    }
    if (xin == NULL  ||  yin == NULL  ||  zin == NULL) {
        return -1;
    }
    if (avspace <= 0.0) {
        return -1;
    }

/*
 * Allocate space for distance to point arrays.
 */
    line_dist = (double *)csw_Malloc (npts * sizeof(double));
    if (line_dist == NULL) {
        return -1;
    }

/*
 * Fill in xy distance along line at each of the input points.
 */
    line_dist[0] = 0.0;
    dtot = 0.0;
    for (i=1; i<npts; i++) {
        dx = xin[i-1] - xin[i];
        dy = yin[i-1] - yin[i];
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        dtot += dist;
        line_dist[i] = dtot;
    }

/*
 * Allocate space for output arrays.
 */
    nmax = (int)(line_dist[npts-1] / avspace + .5);
    if (nmax > 1000 * npts) {
        csw_Free (line_dist);
        return -1;
    }
    nmax += 3;

    x = (double *)csw_Malloc (3 * nmax * sizeof(double));
    if (x == NULL) {
        csw_Free (line_dist);
        return -1;
    }
    y = x + nmax;
    z = y + nmax;

    nout = nmax - 1;

    avspace = line_dist[npts-1] / (nout - 1);
    ibase = 0;
    x[0] = xin[0];
    y[0] = yin[0];
    z[0] = zin[0];

/*
 * Find points at avspace distances along the line and put them into
 * the output arrays.
 */
    for (i=1; i<nout-1; i++) {

        dtot = i * avspace;
        istat = PointAtDistance (line_dist, npts,
                                 xin, yin, zin,
                                 ibase, dtot,
                                 &xt, &yt, &zt,
                                 &nextbase);
        if (istat != 1) {
            csw_Free (x);
            return -1;
        }

        ibase = nextbase;

        x[i] = xt;
        y[i] = yt;
        z[i] = zt;

    }

    x[nout-1] = xin[npts-1];
    y[nout-1] = yin[npts-1];
    z[nout-1] = zin[npts-1];

    *xout = x;
    *yout = y;
    *zout = z;
    *n_out = nout;

    csw_Free (line_dist);

    return 1;

}

/*
 *********************************************************************************

                  g p f _ c o n c a t _ l i n e s _ x y

 *********************************************************************************

  Concatenate lines that have endpoints less than or equal to tiny distance
  from each other.  The results will overwrite the specified arrays.

*/

int gpf_concat_lines_xy (
    double          *xlines,
    double          *ylines,
    double          *zlines,
    int             *lplines,
    int             *lflines,
    int             *nlinesio,
    double          tiny)
{
    double          *xw, *yw, *zw, *xw2, *yw2, *zw2,
                    *xw3, *yw3, *zw3,
                    x1, y1, x2, y2, x3, y3, x4, y4;
    int             i, j, k, nk, ntot, nlines, npts,
                    ipts, jpts, iflag, jflag, noff, ioff, joff,
                    *idone, *lw, *lf,
                    nlmax, npmax, prepend, append,
                    nout, nlout, ndone;

    nlines = *nlinesio;

/*
 * Do nothing if less than 2 lines.
 */
    if (nlines < 2) {
        return 1;
    }

/*
 * Allocate work space.
 */
    ntot = 0;
    for (i=0; i<nlines; i++) {
        ntot += lplines[i];
    }
    npmax = ntot + nlines * 2;
    if (npmax < 100) npmax = 100;

    xw = (double *)csw_Malloc (npmax * 9 * sizeof(double));
    if (xw == NULL) {
        return -1;
    }
    yw = xw + npmax;
    zw = yw + npmax;
    xw2 = zw + npmax;
    yw2 = xw2 + npmax;
    zw2 = yw2 + npmax;
    xw3 = zw2 + npmax;
    yw3 = xw3 + npmax;
    zw3 = yw3 + npmax;

    nlmax = nlines * 2;
    if (nlmax < 10) nlmax = 10;
    lw = (int *)csw_Calloc (nlmax * 3 * sizeof(int));
    if (lw == NULL) {
        csw_Free (xw);
        return -1;
    }
    lf = lw + nlmax;
    idone = lf + nlmax;

/*
 * Check each line for coincident endpoints and same flag values.
 * If these conditions are found, concatenate the lines.
 */
    noff = 0;
    ioff = 0;
    nout = 0;
    nlout = 0;

    for (i=0; i<nlines; i++) {

        ipts = lplines[i];
        iflag = lflines[i];
        npts = ipts;

    /*
     * If the line was concatenated to another line earlier,
     * do not use it again.
     */
        if (idone[i] == 1) {
            noff += ipts;
            continue;
        }

    /*
     * Copy this line into the "2" work space and get its end points.
     */
        memcpy (xw2, xlines+noff, npts * sizeof(double));
        memcpy (yw2, ylines+noff, npts * sizeof(double));
        memcpy (zw2, zlines+noff, npts * sizeof(double));
        x1 = xw2[0];
        y1 = yw2[0];
        x2 = xw2[npts-1];
        y2 = yw2[npts-1];

    /*
     * If the line closes on itself, no concatenation is done.
     */
        if (checkGraze (x1, y1, x2, y2, tiny)) {
            idone[i] = 1;
            lw[nlout] = ipts;
            lf[nlout] = iflag;
            memcpy (xw+nout, xw2, ipts * sizeof(double));
            memcpy (yw+nout, yw2, ipts * sizeof(double));
            memcpy (zw+nout, zw2, ipts * sizeof(double));
            nout += ipts;
            nlout++;
            noff += ipts;
            continue;
        }

        ioff = noff + ipts;
        noff = ioff;

        for (;;) {

        /*
         * Find lines to concatenate one at a time.  When no
         * line is found, break out of the loop.
         */
            ndone = 0;
            joff = ioff;
            for (j=i+1; j<nlines; j++) {

                jpts = lplines[j];
                if (idone[j] == 1) {
                    joff += jpts;
                    continue;
                }

                jflag = lflines[j];
                if (jflag != iflag) {
                    joff += jpts;
                    continue;
                }

            /*
             * Find the end points of this line.
             */
                x3 = xlines[joff];
                y3 = ylines[joff];
                x4 = xlines[joff+jpts-1];
                y4 = ylines[joff+jpts-1];

            /*
             * If the line closes on itself, it cannot be concatenated.
             */
                if (checkGraze (x3, y3, x4, y4, tiny)) {
                    idone[j] = 1;
                    lw[nlout] = jpts;
                    lf[nlout] = jflag;
                    memcpy (xw+nout, xlines+joff, jpts * sizeof(double));
                    memcpy (yw+nout, ylines+joff, jpts * sizeof(double));
                    memcpy (zw+nout, zlines+joff, jpts * sizeof(double));
                    nout += jpts;
                    nlout++;
                    joff += jpts;
                    continue;
                }

            /*
             * Check for coincident endpoints.  If such is found, copy the jth
             * line into the "3" workspace in the order which it will be output.
             */
                prepend = 0;
                append = 0;

                if (checkGraze (x1, y1, x3, y3, tiny)) {
                    nk = 0;
                    for (k=jpts-1; k>0; k--) {
                        xw3[nk] = xlines[joff+k];
                        yw3[nk] = ylines[joff+k];
                        zw3[nk] = zlines[joff+k];
                        nk++;
                    }
                    prepend = 1;
                }

                else if (checkGraze (x1, y1, x4, y4, tiny)) {
                    for (k=0; k<jpts-1; k++) {
                        xw3[k] = xlines[joff+k];
                        yw3[k] = ylines[joff+k];
                        zw3[k] = zlines[joff+k];
                    }
                    prepend = 1;
                }

                else if (checkGraze (x2, y2, x3, y3, tiny)) {
                    for (k=1; k<jpts; k++) {
                        xw3[k-1] = xlines[joff+k];
                        yw3[k-1] = ylines[joff+k];
                        zw3[k-1] = zlines[joff+k];
                    }
                    append = 1;
                }

                else if (checkGraze (x2, y2, x4, y4, tiny)) {
                    nk = 0;
                    for (k=jpts-2; k>=0; k--) {
                        xw3[nk] = xlines[joff+k];
                        yw3[nk] = ylines[joff+k];
                        zw3[nk] = zlines[joff+k];
                        nk++;
                    }
                    append = 1;
                }

            /*
             * Append "3" work space to "2" work space.
             */
                if (append == 1) {
                    memcpy (xw2+ipts, xw3, (jpts - 1) * sizeof(double));
                    memcpy (yw2+ipts, yw3, (jpts - 1) * sizeof(double));
                    memcpy (zw2+ipts, zw3, (jpts - 1) * sizeof(double));
                    ipts += jpts - 1;
                    x2 = xw2[ipts-1];
                    y2 = yw2[ipts-1];
                    idone[j] = 1;
                    ndone = 1;
                }

            /*
             * Append "2" work space to "3" work space and then copy
             * "3" back to 2, which effectively prepends "3" to "2".
             */
                else if (prepend == 1) {
                    memcpy (xw3+jpts-1, xw2, ipts * sizeof(double));
                    memcpy (yw3+jpts-1, yw2, ipts * sizeof(double));
                    memcpy (zw3+jpts-1, zw2, ipts * sizeof(double));
                    ipts += jpts - 1;
                    memcpy (xw2, xw3, ipts * sizeof(double));
                    memcpy (yw2, yw3, ipts * sizeof(double));
                    memcpy (zw2, zw3, ipts * sizeof(double));
                    x1 = xw2[0];
                    y1 = yw2[0];
                    idone[j] = 1;
                    ndone = 1;
                }

                joff += jpts;

            }

            if (ndone == 0) {
                break;
            }

        }

    /*
     * The ith line is completely done, so put it into the output.
     */
        idone[i] = 1;
        lw[nlout] = ipts;
        lf[nlout] = iflag;
        memcpy (xw+nout, xw2, ipts * sizeof(double));
        memcpy (yw+nout, yw2, ipts * sizeof(double));
        memcpy (zw+nout, zw2, ipts * sizeof(double));
        nout += ipts;
        nlout++;

    }

/*
 * Transfer the output work space back to the specified lines arrays.
 */
    memcpy (xlines, xw, nout * sizeof(double));
    memcpy (ylines, yw, nout * sizeof(double));
    memcpy (zlines, zw, nout * sizeof(double));
    memcpy (lplines, lw, nlout * sizeof(int));
    memcpy (lflines, lf, nlout * sizeof(int));
    *nlinesio = nlout;

    csw_Free (xw);
    csw_Free (lw);

    return 1;

}



/*
 *********************************************************************************

        g p f _ o r g a n i z e _ c r o s s i n g _ l i n e s _ x y

 *********************************************************************************

  Split lines so that there are no 4 way intersections, if possible.
  This is done by first separating lines that do not cross any other
  line.  For the remaining (crossing) lines the longest line is found.
  The longest line will remain intact and any shorter lines are split
  if they intersect with the longest line.  The longest line is put
  into the non crossing array and any other non crossing lines are put
  in as well.  The surviving crossing lines are resorted and the process
  is repeated until no crossing lines remain.

  The contents of the xlines, ylines, zlines, lplines and lflines
  arrays will usually be modified by this function.  If the reorganization
  requires more than npmax points, -1 is returned.  If a memory
  allocation error occurs internally to this function, -1 is returned.
  In any case that -1 is returned, the nlinesio parameter is set to zero.
  On success, 1 is returned.
*/

int gpf_organize_crossing_lines_xy (
    double          *xlines,
    double          *ylines,
    double          *zlines,
    int             *lplines,
    int             *lflines,
    int             *nlinesio,
    int             npmax,
    int             nlmax,
    double          tiny)
{
    int             i, j, nout, nlout, n, n2, nl, nw, nlw, nmax,
                    istat, npts, ntot, nlines, ioff, fmax;
    int             *lpw, *lfw, *idone, *npout, *nfout,
                    imax, omax;
    double          *xw, *yw, *zw,
                    *xout, *yout, *zout;
    double          dx, dy, dist, dtot, dmax;

    nlines = *nlinesio;
    if (nlines < 2) {
        return 1;
    }

/*
 * Assign output in case of error.
 */
    *nlinesio = 0;

/*
 * If npmax is less than the total number of input points,
 * return an error.
 */
    ntot = 0;
    for (i=0; i<nlines; i++) {
        ntot += lplines[i];
    }
    if (npmax < ntot) {
        return -1;
    }

/*
 * Allocate memory for workspace.
 */
    xw = (double *)csw_Malloc (6 * npmax * sizeof(double));
    if (xw == NULL) {
        return -1;
    }
    yw = xw + npmax;
    zw = yw + npmax;
    xout = zw + npmax;
    yout = xout + npmax;
    zout = yout + npmax;

    lpw = (int *)csw_Calloc (5 * nlmax * sizeof(int));
    if (lpw == NULL) {
        csw_Free (xw);
        return -1;
    }
    lfw = lpw + nlmax;
    idone = lfw + nlmax;
    npout = idone + nlmax;
    nfout = npout + nlmax;

    nout = 0;
    nlout = 0;

    for (i=0; i<npmax; i++) {
        xw[i] = 1.e60;
        yw[i] = 1.e60;
        zw[i] = 1.e60;
    }

/*
 * Loop that identifies non crossing lines and the longest
 * crossing line in each iteration.  These lines are put in
 * the output and then any crossing lines, other than the
 * longest crossing line, are split using the longest.
 * When there are no longer any crossing lines, this loop
 * is broken out of.
 */
    for (;;) {

    /*
     * If a line does not cross any other line, put it directly into the out arrays.
     * If a line does cross, put it into the xw, yw, zw arrays.
     */
        nlw = 0;
        nw = 0;
        istat =
          SeparateCrossingLines (
            xlines, ylines, zlines,
            lplines, lflines, nlines,
            xout, yout, zout,
            npout, nfout, &nlout, &nout,
            npmax, nlmax,
            xw, yw, zw,
            lpw, lfw, &nlw, &nw,
            tiny);
        if (istat == -1) {
            csw_Free (xw);
            csw_Free (lpw);
            return -1;
        }

    /*
     * If no crossing lines were found, break out of the loop.
     */
        if (nw < 1) {
            break;
        }

        for (i=0; i<nw; i++) {
            if (xw[i] > 1.e50  ||  yw[i] > 1.e50  ||  zw[i] > 1.e50) {
                xw[i] = 1.e60;
            }
        }

    /*
     * Put the crossing lines into the xlines, ylines, zlines arrays.
     */
        memcpy (xlines, xw, nw * sizeof(double));
        memcpy (ylines, yw, nw * sizeof(double));
        memcpy (zlines, zw, nw * sizeof(double));
        memcpy (lplines, lpw, nlw * sizeof(int));
        memcpy (lflines, lfw, nlw * sizeof(int));
        nlines = nlw;

    /*
     * Find the longest length crossing line.
     */
        n = 0;
        imax = -1;
        omax = -1;
        dmax = -1.0;
        nmax = 0;
        fmax = 0;
        ioff = 0;
        for (i=0; i<nlines; i++) {
            npts = lplines[i];
            dtot = 0.0;
            n = ioff;
            for (j=0; j<npts-1; j++) {
                dx = xlines[n] - xlines[n+1];
                dy = ylines[n] - ylines[n+1];
                n++;
                dist = dx * dx + dy * dy;
                dist = sqrt (dist);
                dtot += dist;
            }
            if (dtot > dmax) {
                dmax = dtot;
                imax = i;
                omax = ioff;
                nmax = npts;
                fmax = lflines[i];
            }
            ioff += npts;
        }

        if (imax == -1) {
            csw_Free (xw);
            csw_Free (lpw);
            return -1;
        }

    /*
     * Put longest line into work array to use for splitting.
     */
        memcpy (xw, xlines+omax, nmax*sizeof(double));
        memcpy (yw, ylines+omax, nmax*sizeof(double));
        memcpy (zw, zlines+omax, nmax*sizeof(double));
        nmax = lplines[imax];

    /*
     * Remove the longest line from the xlines, ylines, zlines arrays.
     */
        n = 0;
        n2 = 0;
        nl = 0;
        for (i=0; i<nlines; i++) {
            npts = lplines[i];
            if (i == imax) {
                n += npts;
                continue;
            }
            for (j=0; j<npts; j++) {
                xlines[n2] = xlines[n];
                ylines[n2] = ylines[n];
                zlines[n2] = zlines[n];
                n2++;
                n++;
            }
            lplines[nl] = lplines[i];
            lflines[nl] = lflines[i];
            nl++;
        }

        nlines = nl;

    /*
     * Split the lines that cross the longest.  Then
     * proceed with the next loop iteration.
     */
        istat =
          SplitAgainstLongest (
            xw, yw, zw, &nmax,
            xlines, ylines, zlines, lplines, lflines, &nlines,
            tiny, npmax, nlmax);
        if (istat == -1) {
            csw_Free (xw);
            csw_Free (lpw);
            return -1;
        }

        if (nout+nmax >= npmax) {
            csw_Free (xw);
            csw_Free (lpw);
            return -1;
        }

        if (nlout >= nlmax) {
            csw_Free (xw);
            csw_Free (lpw);
            return -1;
        }

    /*
     * Put the longest line into the output since it will not be
     * modified after this.
     */
        memcpy (xout+nout, xw, nmax*sizeof(double));
        memcpy (yout+nout, yw, nmax*sizeof(double));
        memcpy (zout+nout, zw, nmax*sizeof(double));
        npout[nlout] = nmax;
        nfout[nlout] = fmax;
        nlout++;
        nout += nmax;

    }

    memcpy (xlines, xout, nout * sizeof(double));
    memcpy (ylines, yout, nout * sizeof(double));
    memcpy (zlines, zout, nout * sizeof(double));
    memcpy (lplines, npout, nlout * sizeof(int));
    memcpy (lflines, nfout, nlout * sizeof(int));
    *nlinesio = nlout;

    csw_Free (xw);
    csw_Free (lpw);

    return 1;

}


/*
 * Return the lines that cross one another in the cross
 * arrays and those that do not cross one another in the
 * out arrays.
 */
static int SeparateCrossingLines (
    double      *x,
    double      *y,
    double      *z,
    int         *np,
    int         *nf,
    int         nline,
    double      *xout,
    double      *yout,
    double      *zout,
    int         *npout,
    int         *nfout,
    int         *nloutio,
    int         *noutio,
    int         maxout,
    int         maxlout,
    double      *xcross,
    double      *ycross,
    double      *zcross,
    int         *npcross,
    int         *nfcross,
    int         *nlcrossio,
    int         *ncrossio,
    double      tiny)
{
    int         i, j, ioff, joff,
                ipts, jpts, numint;
    int         nout, nlout, ncross, nlcross;
    double      *xt1, *yt1, *zt1, *xt2, *yt2;
    int         *iflags, intersects;

/*
 * Initialize output in case of error.
 */
    *nlcrossio = 0;
    *ncrossio = 0;

    iflags = (int *)csw_Calloc (maxlout * sizeof(int));
    if (iflags == NULL) {
        return -1;
    }

/*
 * Loop through each line, checking for crossing with other lines.
 */
    ioff = 0;
    joff = 0;
    ncross = 0;
    nlcross = 0;
    nout = *noutio;
    nlout = *nloutio;

    *nloutio = 0;
    *noutio = 0;

    for (i=0; i<nline; i++) {

        xt1 = x + ioff;
        yt1 = y + ioff;
        zt1 = z + ioff;
        ipts = np[i];
        numint = 0;
        ioff += ipts;
        joff = 0;
        intersects = 0;

    /*
     * If the ith line was determined to intersect in a previous
     * iteration (when it was the jth line) there is no need to
     * recheck it.  Just put it in the cross arrays and continue.
     */
        if (iflags[i] == 1) {
            memcpy (xcross+ncross, xt1, ipts*sizeof(double));
            memcpy (ycross+ncross, yt1, ipts*sizeof(double));
            memcpy (zcross+ncross, zt1, ipts*sizeof(double));
            npcross[nlcross] = ipts;
            nfcross[nlcross] = nf[i];
            ncross += ipts;
            nlcross++;
            continue;
        }

        for (j=0; j<nline; j++) {

            jpts = np[j];
            if (i == j) {
                joff += jpts;
                continue;
            }

            xt2 = x + joff;
            yt2 = y + joff;
            joff += jpts;

            numint =
              gpf_check_for_intersection (
                xt1, yt1, ipts,
                xt2, yt2, jpts,
                tiny);
            if (numint == -1) {
                csw_Free (iflags);
                return -1;
            }
            if (numint > 0) {
                intersects = 1;
                iflags[j] = 1;
            }

        }

    /*
     * Put the ith line into the crossing arrays.
     */
        if (intersects == 1) {
            memcpy (xcross+ncross, xt1, ipts*sizeof(double));
            memcpy (ycross+ncross, yt1, ipts*sizeof(double));
            memcpy (zcross+ncross, zt1, ipts*sizeof(double));
            npcross[nlcross] = ipts;
            nfcross[nlcross] = nf[i];
            ncross += ipts;
            nlcross++;
        }

    /*
     * Put the ith line into the out arrays.
     */
        else {
            if (nout + ipts >= maxout) {
                csw_Free (iflags);
                return -1;
            }
            if (nlout >= maxlout) {
                csw_Free (iflags);
                return -1;
            }
            memcpy (xout+nout, xt1, ipts*sizeof(double));
            memcpy (yout+nout, yt1, ipts*sizeof(double));
            memcpy (zout+nout, zt1, ipts*sizeof(double));
            npout[nlout] = ipts;
            nfout[nlout] = nf[i];
            nout += ipts;
            nlout++;
        }

    }

    csw_Free (iflags);
    iflags = NULL;

/*
 * Transfer line and point counts back to calling function and return.
 */
    *nloutio = nlout;
    *noutio = nout;
    *nlcrossio = nlcross;
    *ncrossio = ncross;

    return 1;

}


/*-----------------------------------------------------------------------------*/

static int SplitAgainstLongest (
    double    *xlong,
    double    *ylong,
    double    *zlong,
    int       *nlongio,
    double    *xlines,
    double    *ylines,
    double    *zlines,
    int       *nplines,
    int       *nflines,
    int       *nlinesio,
    double    tiny,
    int       npmax,
    int       nlmax)
{
    double    *xw, *yw, *zw,
              *xw3, *yw3, *zw3, *xw4, *yw4, *zw4;
    int       *lpw3, *lfw3, *intflags, *iw;
    double    x1, y1, x2, y2, x3, y3, x4, y4, xint, yint;
    int       i, j, k, nlong, nlines, n1, n3, n4, n3last,
              jpts, n, nl3, jflag, joff;
    int       istat;

    CSWPolyUtils  ply_utils_obj;

    nlong = *nlongio;
    nlines = *nlinesio;
    *nlongio = 0;
    *nlinesio = 0;

/*
 * Allocate work space.
 */
    xw = (double *)csw_Malloc (2 * npmax * sizeof(int) + 9 * npmax * sizeof(double));
    if (xw == NULL) {
        return -1;
    }
    yw = xw + npmax;
    zw = yw + npmax;
    xw3 = zw + npmax;
    yw3 = xw3 + npmax;
    zw3 = yw3 + npmax;
    xw4 = zw3 + npmax;
    yw4 = xw4 + npmax;
    zw4 = yw4 + npmax;
    intflags = (int *)(zw4 + npmax);
    iw = intflags + npmax;
    memset (intflags, 0, 2 * npmax * sizeof(int));

    lpw3 = (int *)csw_Calloc (2 * nlmax * sizeof(int));
    if (lpw3 == NULL) {
        csw_Free (xw);
        return -1;
    }
    lfw3 = lpw3 + nlmax;

/*
 * For each segment of the long line, check the segments
 * of the other lines for intersection.
 */
    n1 = 0;
    for (i=0; i<nlong-1; i++) {

        xw[n1] = xlong[i];
        yw[n1] = ylong[i];
        zw[n1] = zlong[i];
        n1++;
        x1 = xlong[i];
        y1 = ylong[i];
        x2 = xlong[i+1];
        y2 = ylong[i+1];

        n = 0;
        n3 = 0;
        n3last = 0;
        nl3 = 0;
        n4 = 0;
        joff = 0;

        for (j=0; j<nlines; j++) {

            jpts = nplines[j];
            jflag = nflines[j];
            n = joff;
            joff += jpts;

            for (k=0; k<jpts-1; k++) {
                xw3[n3] = xlines[n];
                yw3[n3] = ylines[n];
                zw3[n3] = zlines[n];
                n3++;
                if (n3 >= npmax) {
                    csw_Free (xw);
                    csw_Free (lpw3);
                    return -1;
                }
                x3 = xlines[n];
                y3 = ylines[n];
                x4 = xlines[n+1];
                y4 = ylines[n+1];
                n++;
                if (intflags[n] == 1  ||  intflags[n-1] == 1) {
                    istat = 1;
                    xint = x4;
                    yint = y4;
                }
                else {
                    istat =
                      ply_utils_obj.ply_segint (x1, y1, x2, y2, x3, y3, x4, y4,
                                  &xint, &yint);
                }
                if (istat == 0) {

                /*
                 * Do not split the jth line if the intersection point is exactly at
                 * the first point on the ith line or the last point on the ith line.
                 */
                    if (i == 0) {
                        if (checkGraze (xint, yint, x1, y1, tiny)) {
                            continue;
                        }
                    }
                    if (i == nlong-2) {
                        if (checkGraze (xint, yint, x2, y2, tiny)) {
                            continue;
                        }
                    }

                 /*
                  * Do not split if the intersect is at the first and only
                  * point of the new line.
                  */
                    if (n3 - n3last < 2) {
                        if (checkGraze (xint, yint, x3, y3, tiny)) {
                            if (n3 > n3last) {
                                iw[n3-1] = 1;
                            }
                            continue;
                        }
                    }
                    if (checkGraze (xint, yint, x4, y4, tiny)) {
                        xw3[n3] = x4;
                        yw3[n3] = y4;
                        zw3[n3] = zlines[n];
                        n3++;
                    }
                    else {
                        xw3[n3] = xint;
                        yw3[n3] = yint;
                        zw3[n3] = 1.e30;
                        n3++;
                    }
                    if (n3 >= npmax  ||  nl3 >= nlmax) {
                        csw_Free (xw);
                        csw_Free (lpw3);
                        return -1;
                    }
                    lpw3[nl3] = n3 - n3last;
                    lfw3[nl3] = jflag;
                    nl3++;
                    n3last = n3;
                    if (n3 > 0) {
                        iw[n3-1] = 1;
                    }

                    if (checkGraze (x1, y1, xint, yint, tiny) == 0  &&
                        checkGraze (x2, y2, xint, yint, tiny) == 0) {
                        xw4[n4] = xint;
                        yw4[n4] = yint;
                        zw4[n4] = 1.e30;
                        n4++;
                    }
                }
            }

        /*
         * do the last point.
         */
            if (n3 - n3last > 0) {
                xw3[n3] = xlines[n];
                yw3[n3] = ylines[n];
                zw3[n3] = zlines[n];
                n3++;
                n++;
                lpw3[nl3] = n3 - n3last;
                lfw3[nl3] = jflag;
                nl3++;
                n3last = n3;
            }
        }

    /*
     * Insert the intersection points into the long line.
     */
        if (n1 >= npmax) {
            csw_Free (xw);
            csw_Free (lpw3);
            return -1;
        }

        if (n4 > 0) {
            if (n4 == 1) {
                xw[n1] = xw4[0];
                yw[n1] = yw4[0];
                zw[n1] = zw4[0];
                n1++;
            }
            else {
            /* sort by distance from x1, y1 */
            }
        }

    /*
     * Copy the current xw3, yw3, zw3 lines into xlines, ylines, zlines
     * to split via the next long line segment.
     */
        memcpy (xlines, xw3, n3 * sizeof(double));
        memcpy (ylines, yw3, n3 * sizeof(double));
        memcpy (zlines, zw3, n3 * sizeof(double));
        memcpy (intflags, iw, n3 * sizeof(int));
        memcpy (nplines, lpw3, nl3 * sizeof(int));
        memcpy (nflines, lfw3, nl3 * sizeof(int));
        nlines = nl3;

    }

/*
 * Add last point to the long output if needed.
 */
    if (checkGraze (xw[n1-1], yw[n1-1],
                    xlong[nlong-1], ylong[nlong-1], tiny) == 0) {
        xw[n1] = xlong[nlong-1];
        yw[n1] = ylong[nlong-1];
        zw[n1] = zlong[nlong-1];
        n1++;
    }

    memcpy (xlong, xw, n1 * sizeof(double));
    memcpy (ylong, yw, n1 * sizeof(double));
    memcpy (zlong, zw, n1 * sizeof(double));
    *nlongio = n1;
    *nlinesio = nlines;

    csw_Free (xw);
    csw_Free (lpw3);

    return 1;

}
