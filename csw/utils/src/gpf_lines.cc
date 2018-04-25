
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_lines.cc

    This file has functions for calculating line primitives 
    for a variety of "fancy" line styles.  Lines can be solid
    or dashed.  They can have labels in the line or along side of
    the line.  They can have tick marks to the right or left or 
    both right and left.

*/

#include <stdio.h>
#include <math.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/gpf_line.h"

/*
    define constants for the file
*/

#define MAXDASH               8
#define MAXLABEL              100

/*
    define structures used only by this file
*/


/*
  ****************************************************************

              g p f _ R e s e t L i n e S t y l e

  ****************************************************************

    Return all of the dash, label and tick attributes to a 
  disabled state.

*/

int GPFLine::gpf_ResetLineStyle (void)
{
   
    LineLabel[0] = '\0';
    LabelFlag = 0;
    LabelDist = 0.0f;

    DashFlag = 0;
    Ndash = 0;

    TickFlag = 0;
    TickDist = 0.0f;

    return 1; 

}  /*  end of function gpf_ResetLineStyle  */


/*
  ****************************************************************

                  g p f _ S e t L i n e D a s h

  ****************************************************************

    Set or unset the line dash parameters.  If ndash is less than
  one or the dashes pointer is NULL, the line dashing is disabled.  
  If ndash is positive, the dash pattern is set to list specified in
  the dashes array.  The even members of the array have pen down
  distances.  The odd members have pen up distances.  If the
  number of defined dashes is odd, the last member is ignored.
  If the sum of the dashes array is less than or equal to zero,
  the dash flag is unset.

*/

int GPFLine::gpf_SetLineDash (CSW_F *dashes, int ndash)
{
    int             i;

    if (!dashes || ndash <= 1) {
        DashFlag = 0;
        Ndash = 0;
        DashLen = 0.0f;
        return 1;
    }

    if (ndash > MAXDASH) {
        ndash = MAXDASH;
    }

    if (ndash %2 != 0) {
        ndash--;
    }
    DashLen = 0.0f;
    for (i=0; i<ndash; i++) {
        DashDist[i] = dashes[i];
        DashLen += DashDist[i];
    }

    if (DashLen <= 0.0f) {
        DashFlag = 0;
        Ndash = 0;
        DashLen = 0.0f;
        return 1;
    }

    Ndash = ndash;
    DashFlag = 1;

    return 1;

}  /*  end of function gpf_SetLineDash  */


/*
  ****************************************************************

                g p f _ S e t L i n e L a b e l

  ****************************************************************

    Set or reset the line label parameters for subsequent gpf_LineCalc
  calls.  If any parameter is indicative of an invisible label, then
  the line labelling is disabled.  If all parameters are ok for a
  label definition, labelling is enabled.

*/

int GPFLine::gpf_SetLineLabel (const char *label, int type, int font, 
                      CSW_F size, CSW_F dist)
{
    
    if (type < 0  ||  font < 0  ||  !label  ||  
        dist <= 0.0f  ||  size <= 0.0f) {
        LabelFlag = 0;
        LineLabel[0] = '\0';
        LabelDist = 0.0f;
        return 1;
    }
    if (label[0] == '\0') {
        LabelFlag = 0;
        LineLabel[0] = '\0';
        LabelDist = 0.0f;
        return 1;
    }    

    LabelFlag = 1;
    LabelDist = dist;
    csw_StrTruncate (LineLabel, label, 100);

    return 1;

}  /*  end of function gpf_SetLineLabel  */



/*
  ****************************************************************

               g p f _ S e t L i n e T i c k s

  ****************************************************************

    Set or reset the line tick mark parameters.  If any of the 
  specified parameters would produce invisible ticks, the tick
  drawing is disabled.  To enable the tick drawing, all parameters
  must be viable for visible tick marks.

*/

int GPFLine::gpf_SetLineTicks (int angle, CSW_F dist, CSW_F length)
{
    
    if (angle <= 0  ||  dist <= 0.0f  ||  length <= 0.0f) {
        TickFlag = 0;
        TickDist = 0.0f;
        return 1;
    }

    TickFlag = 1;
    TickDist = dist;

    return 1;

}  /*  end of function gpf_SetLineTicks  */



/*
  ****************************************************************

                  g p f _ L i n e C a l c 2

  ****************************************************************

    Same functionality as gpf_LineCalc, but the x and y input
  coordinates are packed into one array.

*/

int GPFLine::gpf_LineCalc2 (CSW_F *xy, int npts)
{
    CSW_F           *xy2[2];
    int             istat, memflg;

    istat = gpf_xyseparate (xy, npts, xy2, &memflg);
    if (istat != 1) {
        return istat;
    }
    istat = gpf_LineCalc (xy2[0], xy2[1], npts);

    if (memflg) {
        csw_Free (xy2[0]);
        csw_Free (xy2[1]);
    }

    return istat;

}  /*  end of function gpf_LineCalc2  */



/*
  ****************************************************************

                   g p f _ L i n e C a l c

  ****************************************************************

  This is the primary function for calculating line and text
  primitives that make up a fancy line.  The dashes, labels, and
  ticks data set when this is called are used for the line.  If
  none of the fancy line attributes are turned on, the function
  returns zero.  If a memory allocation error or bad parameter
  error is encountered, -1 is returned.  On success, 1 is returned.

  The x and y arrays must be allocated for at least 3 points.
  The dash generator needs at least 3 points, and if a line only
  has 2 points, a third point a tiny distance from the seconf
  point is created for dashed line calculation.

*/

int GPFLine::gpf_LineCalc (CSW_F *xin, CSW_F *yin, int npts)
{
    int             istat, i, j;
    CSW_F           dtot, *x = NULL, *y = NULL;
    CSW_F           xloc[3], yloc[3];

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            Cleanup ();
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    check for obvious errors;
*/
    if (!xin  ||  !yin  ||  npts < 2) {
        return -1;
    }

    if (!DashFlag  &&  !TickFlag  &&  !LabelFlag) {
        return 0;
    }

/*
    if data pointers have not been csw_Freed since last call,
    csw_Free them now
*/
    Cleanup ();

/*
    if less than 3 points, copy into the local storage
*/
    if (npts < 3) {
        xloc[0] = xin[0];
        yloc[0] = yin[0];
        xloc[1] = xin[1];
        yloc[1] = yin[1];
        x = xloc;
        y = yloc;
    }
    else {
        x = xin;
        y = yin;
    }

/*
    get line distance at each point
*/
    istat = CalcLineDistances (x, y, npts);
    if (istat != 1) {
        return -1;
    }

/*
    allocate space for output line and text data
*/
    dtot = LineDistance[npts-1];

    i = 0;
    if (DashFlag) {
        i = (int)(dtot / DashLen) * Ndash / 2;
        i += npts;
        i += 100;
    }

    j = 0;
    if (TickFlag) {
        j = (int)(dtot / TickDist) + 100;
    }

    if (i + j > 0) {
        PointRecArray = (LInePointRec *)csw_Malloc ((i + j) * sizeof(LInePointRec));
        if (!PointRecArray) {
            return -1;
        }

        PointArray = (CSW_F *)csw_Malloc ((i + j) * 4 * sizeof(CSW_F));
        if (!PointArray) {
            return -1;
        }
        CurrentPoint = PointArray;
    }

    i = 0;
    if (LabelFlag) {
        i = (int)(dtot / LabelDist) + 100;
        j = strlen (LineLabel);
        TextRecArray = (LIneTextRec *)csw_Malloc (i * j * sizeof(LIneTextRec));
        if (!TextRecArray) {
            return -1;
        }
        TextArray = (char *)csw_Malloc (i * j * 2 * sizeof(char));
        if (!TextArray) {
            return -1;
        }
    }
  
    NpointRec = 0;
    NtextRec = 0;

/*
    calculate and save the dashed line segments
*/
    if (DashFlag) {
        CalcLineDashes (x, y, npts);
    }

/*
    all of the simple line segments and text segments 
    have been stored.  The calling function must use
    gpf_GetLinePoints and gpf_GetLineText to retrieve
    the data.
*/
    bsuccess = true;

    return 1;
     
}  /*  end of function gpf_LineCalc  */



/*
  ****************************************************************

            g p f _ G e t L i n e P o i n t s

  ****************************************************************

  Get a pointer to the array of LInePointRecs calculated by the 
  most recent gpf_LineCalc call.  The number of records is also
  returned.  The application should not csw_Free this pointer directly,
  because a private copy of the pointer is maintained here.  The
  application should call gpf_CleanupLineCalc to csw_Free this pointer.
  If another call to gpf_LineCalc is made prior to csw_Freeing the
  pointer, the pointer is csw_Freed automatically by gpf_LineCalc.

*/

int GPFLine::gpf_GetLinePoints (LInePointRec **ptrarray, int *nline)
{

    *ptrarray = PointRecArray;
    *nline = NpointRec;

    return 1;

}  /*  end of function gpf_GetLinePoints  */



/*
  ****************************************************************

                g p f _ G e t L i n e T e x t

  ****************************************************************

  Get a pointer to the array of LIneTextRecs calculated by the 
  most recent gpf_LineCalc call.  The number of records is also
  returned.  The application should not csw_Free this pointer directly,
  because a private copy of the pointer is maintained here.  The
  application should call gpf_CleanupLineCalc to csw_Free this pointer.
  If another call to gpf_LineCalc is made prior to csw_Freeing the
  pointer, the pointer is csw_Freed automatically by gpf_LineCalc.

*/

int GPFLine::gpf_GetLineText (LIneTextRec **ptrarray, int *ntext)
{

    *ptrarray = TextRecArray;
    *ntext = NtextRec;

    return 1;

}  /*  end of function gpf_GetLineText  */



/*
  ****************************************************************

             g p f _ C l e a n u p L i n e C a l c

  ****************************************************************

  Free memory and reset private variables after finished with
  gpf_LineCalc stuff.

*/

int GPFLine::gpf_CleanupLineCalc (void)
{

    Cleanup ();
    return 1;

}  /*  end of function gpf_CleanupLineCalc  */





/*
  ****************************************************************

                       R e s e t F o r C o p y

  ****************************************************************

  The pointed to memory in a GPFLine object is only meant for use
  as scratch space for various operations (public methods).  It
  make no sense to copy or move this scratch space to a copy of
  the GPFLine object.
*/

void GPFLine::ResetForCopy (void)
{

    PointRecArray = NULL;
    PointArray = NULL;
    CurrentPoint = NULL;
    NpointRec = 0;

    LineDistance = NULL;

    TextRecArray = NULL;
    TextArray = NULL;
    NtextRec = 0;

    return;

}  /*  end of private ResetForCopy function  */


/*
  ****************************************************************

                         C l e a n u p

  ****************************************************************

    Free and reset private pointers used in line calculations.

*/

void GPFLine::Cleanup (void)
{
    
    if (PointRecArray) csw_Free (PointRecArray);
    if (PointArray) csw_Free (PointArray);
    if (LineDistance) csw_Free (LineDistance);
    if (TextRecArray) csw_Free (TextRecArray);
    if (TextArray) csw_Free (TextArray);

    PointRecArray = NULL;
    PointArray = NULL;
    CurrentPoint = NULL;
    NpointRec = 0;

    LineDistance = NULL;

    TextRecArray = NULL;
    TextArray = NULL;
    NtextRec = 0;

    return;

}  /*  end of private Cleanup function  */



/*
  ****************************************************************

                C a l c L i n e D i s t a n c e s

  ****************************************************************

    Calculate the along line distance at each point on the line.
  These are stored in the LineDisance array.  If this array cannot
  be csw_Malloced, -1 is returned.  On success, 1 is returned.

    Note, the distance squared is actually used here as well as
  everywhere else in the line calculations.  This saves doing a
  bunch of square root calculations.

*/

int GPFLine::CalcLineDistances (CSW_F *x, CSW_F *y, int npts)
{
    int              i;
    CSW_F            dx, dy, danc;

/*
    allocate LineDistance array
*/

    LineDistance = (CSW_F *)csw_Malloc ((npts + 1) * sizeof(CSW_F));
    if (!LineDistance) {
        return -1;
    }

/*
    set first point at zero distance
*/

    LineDistance[0] = 0.0f;
    danc = 0.0f;

/*
    calculate the rest of the distances along the line
*/

    for (i=1; i<npts; i++) {
        dx = x[i] - x[i-1];
        dy = y[i] - y[i-1];
        LineDistance[i] = (CSW_F)(sqrt((double)(dx*dx + dy*dy)) + danc);
        danc = LineDistance[i];
    }

    return 1;

}  /*  end of private CalcLineDistances function  */


/*
  ****************************************************************

                   C a l c L i n e D a s h e s

  ****************************************************************

    This function is only called from gtx_LineCalc.  The segments
  of a dashed line are calculated and stored in the line point arrays.
  If npts is less than 2, zero is returned.  On success, 1 is returned.

*/

int GPFLine::CalcLineDashes (CSW_F *x, CSW_F *y, int npts)
{
    int              i, j, k, nrep, n1, n2, idash;
    CSW_F            dx, dy, dt1, dt2, xp, yp, dtot, d1, r;

/*
    return if there are not enough points
*/

    if (npts < 2) {
        return 0;
    }

/*
    if there are less than 3 points, create a 3rd point a tiny
    distance from the second point
*/

    if (npts < 3) {
        npts = 3;
        dx = (x[1] - x[0]) / 10000.f;
        dy = (y[1] - y[0]) / 10000.f;
        x[2] = x[1] + dx;
        y[2] = y[1] + dy;
        LineDistance[2] = (CSW_F)(LineDistance[1] + sqrt ((double)(dx*dx + dy*dy)));
    }

/*
    save the first point in the output arrays
*/

    AddDashPoint (x[0], y[0], 1);

/*
    calculate the number of dash patterns repeated in the line
*/

    dtot = LineDistance[npts-1];
    nrep = (int)(dtot / DashLen) + 1;

/*
    initialize stuff for the start of the dashed line
*/

    dt1 = 0.0f;
    dt2 = 0.0f;
    d1 = 0.0f;
    n1 = 0;
    n2 = 0;
    idash = 0;
    
/*
    do nrep repetitions of the basic dash pattern
    in each repetition, do Ndash dashed segments
*/
    for (i=0; i<nrep; i++) {

        for (j=0; j<Ndash; j++) {
        
            d1 += DashDist[j];
            idash = j%2;

            for (k=0; k<npts; k++) {
                n2 = n1 + k;
                if (n2 >= npts) {
                    goto DASH_LINE_DONE;
                }
                if (d1 <= LineDistance[n2]) {
                    break;
                }
            }

            n2--;
            if (n2 < 0) n2 = 0;

        /*
            add intermediate points traversed by the dash segment
        */
            if (n2 > n1) {
                for (k=n1+1; k<=n2; k++) {
                    AddDashPoint (x[k], y[k], idash);
                }
            }

        /*
            add the end point of the dash segment
        */
            if (d1 > dtot) d1 = dtot;
            dt1 = d1 - LineDistance[n2];
            dt2 = LineDistance[n2+1] - LineDistance[n2];
            r = dt1 / dt2;

            xp = (x[n2+1] - x[n2]) * r + x[n2];
            yp = (y[n2+1] - y[n2]) * r + y[n2];

            AddDashPoint (xp, yp, idash);

            n1 = n2;

        }
    }

DASH_LINE_DONE:

    AddDashPoint (x[npts-1], y[npts-1], idash);

    return 1;
    
}  /*  end of private CalcLineDashes function  */



/*
  ****************************************************************

                   A d d D a s h P o i n t

  ****************************************************************

  Add a point to the dashed line output arrays.  The point is appended
  to the current segment if flag = 0.  If flag is 1, the point is
  saved in private class variables until a flag = 0 point is encountered.
  The saved point is then used as the 1st point of a new segment.

*/

void GPFLine::AddDashPoint (CSW_F x, CSW_F y, int flag)
{

/*
    if flag = 1, save the point as the anchor for
    the next append flag
*/
    if (flag == 1) {
        p_xanc = x;
        p_yanc = y;
        p_lastanc = 1;
        return;
    }

/*
    if the previous call was an anchor, start a new line
*/
    if (p_lastanc) {
        PointRecArray[NpointRec].coords = CurrentPoint;
        PointRecArray[NpointRec].npts = 2;
        *CurrentPoint = p_xanc;
        CurrentPoint++;
        *CurrentPoint = p_yanc;
        CurrentPoint++;
        *CurrentPoint = x;
        CurrentPoint++;
        *CurrentPoint = y;
        CurrentPoint++;
        NpointRec++;
        p_lastanc = 0;
    }

/*
    if the previous point was not an anchor, append
    to the current line
*/
    else {
        PointRecArray[NpointRec-1].npts++;
        *CurrentPoint = x;
        CurrentPoint++;
        *CurrentPoint = y;
        CurrentPoint++;
    }

    return;
        
}  /*  end of private AddDashPoint function  */
