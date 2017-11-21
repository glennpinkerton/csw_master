
/*
    seislegend.c

        This file has functions used by the seisplot program to keep
    track of legend entries for various windows and to actually draw
    the entries when needed.

        The code here shows one way of using frames in the EasyX 
    graphics library.  There is also code in the CleanupLegends function
    that illustrates how to detect if the user has deleted one of the 
    windows.
*/

/*
    system include files
*/
#include <stdio.h>

/*
    define constants for this file
*/
#define MAX_LEGEND_ENTRIES            20
#define MAX_LEGEND_FRAMES             20

/*
    define structures for this file
*/
typedef struct {
    int             numtext;
    char            *text[MAX_LEGEND_ENTRIES];
}  LEgendEntry;

typedef struct {
    int             wnum,
                    fnum;
    float           width,
                    height;
    LEgendEntry     legend;
}  LEgendFrame;

/*
    declare static file variables
*/
static LEgendFrame       LegendList[MAX_LEGEND_FRAMES];
static int               Nlegend = 0;

/*
    static function "prototypes"
*/

#ifdef NEED_PROTO

int InitLegendData (void);
int DrawLegendFrame (int wnum, char *title, 
                     float x1, float y1, float x2, float y2);
int AddLegendEntry (int wnum, int color, char *text);
int CleanupLegends (void);

#endif




/*
  ****************************************************************

                  I n i t L e g e n d D a t a

  ****************************************************************

    This is called once by the main seisplot program to set the
  legend data to its initial state.

*/

#ifdef NEED_PROTO

int InitLegendData (void)

#else

int InitLegendData ()

#endif

{
    int           i, j;
    static int    first = 1;

    if (!first) {
        return 1;
    }

    for (i=0; i<MAX_LEGEND_FRAMES; i++) {
        LegendList[i].wnum = 1;
        LegendList[i].fnum = -1;
        LegendList[i].legend.numtext = 0;
        for (j=0; j<MAX_LEGEND_ENTRIES; j++) {
            LegendList[i].legend.text[j] = NULL;
        }
    }

    first = 0;

    return 1;

}  /*  end of function InitLegendData  */




/*
  ****************************************************************

                    D r a w L e g e n d F r a m e

  ****************************************************************

    This is called once per window to create the legend frame
  and draw the title into the legend frame.  The frame is recorded
  in the LegendList also.

*/

#ifdef NEED_PROTO

int DrawLegendFrame (int wnum, char *title, 
                     float x1, float y1, float x2, float y2)

#else

int DrawLegendFrame (wnum, title, x1, y1, x2, y2)

    int        wnum;
    char       *title;
    float      x1, y1, x2, y2;

#endif

{
    double     fx1, fy1, fx2, fy2;
    float      tx1, ty1, tlen;
    int        istat, fnum, i, igood, nc;

/*
    First, check for any windows that have been deleted by the user.
*/
    CleanupLegends ();

/*
    See if this window number exists and if it already
    has a legend frame drawn for it.
*/
    for (i=0; i<Nlegend; i++) {
        if (LegendList[i].wnum == wnum) {
            if (LegendList[i].fnum >= 0) {
                return 1;
            }
        }
    }

/*
    Find an available slot in LegendList for the data
    describing this legend if possible.  If no slot is
    available, append to the end of LegendList.
*/
    igood = Nlegend;
    for (i=0; i<Nlegend; i++) {
        if (LegendList[i].wnum == -1) {
            igood = i;
            break;
        }
    }

    if (igood >= MAX_LEGEND_FRAMES) {
        return -1;
    }

    LegendList[igood].wnum = wnum;
    if (igood == Nlegend) {
        Nlegend++;
    }

/*
    create a frame from x1,y1 to x2,y2
*/
    fx1 = 0.0;
    fy1 = 0.0;
    fx2 = x2 - x1;
    fy2 = y2 - y1;

    LegendList[igood].height = fy2;
    LegendList[igood].width = fx2;
    
    istat = gtx_Restart (wnum);
    if (istat == -1) {
        return 1;
    }

    fnum = gtx_CreateFrame (fx1, fy1, fx2, fy2, 
                            x1, y1, x2, y2,
                            1, "legend");
    if (fnum == -1) {
        return -1;
    }
    gtx_SetFrameClip (1);

    LegendList[igood].fnum = fnum;

/*
    draw the title centered at the top of the frame
*/    
    nc = strlen(title);
    gtx_TextLength (title, nc, 6, (float)0.2, &tlen);
    ty1 = fy2 - 0.5;
    tx1 = (fx2 - tlen) / 2.0;

    gtx_Font (6);
    gtx_TextThk (0.02);
    gtx_Text (tx1, ty1, (float)0.2, (float)0.0, title, nc);
    gtx_UnsetFrame ();    
    gtx_EndPlt ();

    return 1;

}  /*  end of function DrawLegendFrame  */




/*
  ****************************************************************

                 A d d L e g e n d E n t r y

  ****************************************************************

    Draw a text string in the next available legend spot if the
  text has not been drawn before.  The seisplot program draws
  legend entries for each horizon picked.  The text is drawn in
  the same color as the hozizon.

*/

#ifdef NEED_PROTO

int AddLegendEntry (int wnum, int color, char *text)

#else

int AddLegendEntry (wnum, color, text)

    int          wnum, color;
    char         *text;

#endif

{
    int          fnum, nentry, i, nc, istat;
    LEgendEntry  *legend;
    float        height, xpos, ypos;
    char         ctmp1[200], ctmp2[200], *cptr;

/*
    Find the legend entry record for this window number.
*/
    height = 0.0;
    nentry = 0;
    fnum = 0;
    legend = NULL;
    for (i=0; i<Nlegend; i++) {
        if (wnum == LegendList[i].wnum) {
            legend = &(LegendList[i].legend);
            fnum = LegendList[i].fnum;
            height = LegendList[i].height;
            nentry = legend->numtext;
            break;
        }
    }

    if (!legend) {
        return -1;
    }

/*
    See if the text has already been entered by comparing it
    to the current entries.  Case and white space are ignored
    in the comparison.
*/
    csw_StrClean2 (ctmp1, text, 200);
    for (i=0; i<nentry; i++) {
        csw_StrClean2 (ctmp2, legend->text[i], 200);
        if (strcmp (ctmp1, ctmp2) == 0) {
            return 1;
        }
    }

/*
    draw the legend entry
*/
    nc = strlen (text);
    xpos = 0.5;
    ypos = height - 1.0 - 0.3 * nentry;
    
    istat =  gtx_Restart (wnum);
    if (istat == -1) {
        return -1;
    }
    istat = gtx_SetFrameNum (fnum);
    if (istat == -1) {
        istat = gtx_ErrNum ();
    }
    gtx_SetFrameClip (1);
    gtx_Font (2);
    gtx_TextThk ((float)0.01);
    gtx_TextColor (color);
    gtx_Text (xpos, ypos, (float)0.15, (float)0.0, text, nc);
    gtx_EndPlt ();

/*
    Save the text in the legend text list.
*/
    cptr = (char *)malloc ((nc+1) * sizeof(char));
    strcpy (cptr, text);
    legend->text[nentry] = cptr;
    legend->numtext++;

    return 1;
    
}  /*  end of function AddLegendEntry  */



/*
  ****************************************************************

                 C l e a n u p L e g e n d s

  ****************************************************************

    This is called from time to time to check if windows have been
  deletec by the user.  If a window has been deleted, then its 
  legend data are removed and the slot it took up in the LegendList
  array is made available for a future legend list.

*/

#ifdef NEED_PROTO

int CleanupLegends (void)

#else

int CleanupLegends ()

#endif

{
    int          i, istat, j, nentry, ierr, wnum;
    LEgendEntry  *legend;

/*
    For each window number in the LegendList array, 
    try and restart the window.
*/
    for (i=0; i<Nlegend; i++) {
        wnum = LegendList[i].wnum;
        if (wnum == -1) {
            continue;
        }
        istat = gtx_Restart (wnum);

    /*
        If the restart fails because of a user delete
        (error number 2 or 999) clean up the legend
        resources for the window.
    */
        if (istat == -1) {
            ierr = gtx_ErrNum ();
            if (ierr == 2  ||  ierr == 999) {
                LegendList[i].wnum = -1;
                LegendList[i].fnum = -1;
                legend = &(LegendList[i].legend);
                nentry = legend->numtext;
                for (j=0; j<nentry; j++) {
                    if (legend->text[j]) {
                        free (legend->text[j]);
                        legend->text[j] = NULL;
                    }
                }
                legend->numtext = 0;
            }
        }

    /*
        If the restart succeeded, then end the plotting to
        the window without doing anything else.
    */
        else {
            gtx_EndPlt ();
        }
    }

    return 1;

}  /*  end of CleanupLegends function  */
