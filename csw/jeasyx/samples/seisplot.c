
/*
         ****************************************************
         *                                                  *
         *       Copyright (1994) Colorado Softworks        *
         *                                                  *
         ****************************************************
*/

/*
    seisplot.c

        This file is a sample program that uses the EasyX library to
    draw seismic data from SEGY format files and to pick horizons
    from the data.  The purpose of this program is to provide an
    example of image drawing in EasyX.  This is not intended to be
    a full featured seismic plotting program.
*/

/*
    system include files
*/
#include <stdio.h>
#include <math.h>

/*
    define constants for the file
*/
#define MAX_PICK      1000

/*
    define data types used by this file
*/
typedef union {
    char    c1[4];
    float   f4;
    long    i4;
    short   i2[2];
}  SEgyDataType;

/*
    define static variables for the file
*/
static char           Ans[200];
static float          XBorder[] = {2.9, 2.9, 17.1, 17.1, 2.9};
static float          YBorder[] = {2.9, 12.1, 12.1, 2.9, 2.9};
static float          ImageX1 = 3.0,
                      ImageY1 = 3.0,
                      ImageX2 = 17.0,
                      ImageY2 = 12.0;


/*
    static function "prototypes"
*/
static int            ReadAndDrawSection ();
static int            PickHorizons ();
static int            ReadSegyData ();
static int            DrawTicksAndLabels ();

float                 csw_FloatRandom ();

#ifdef NEED_PROTO

int InitLegendData (void);
int DrawLegendFrame (int wnum, char *title,
                     float x1, float y1, float x2, float y2);
int AddLegendEntry (int wnum, int color, char *text);
int CleanupLegends (void);

#endif



/*
  ****************************************************************

                       m a i n   p r o g r a m

  ****************************************************************

    Initialize the EasyX server connection and run the main menu
  until the user exits.

*/

main ()

{
    int             pick, istat;

/*
    initialize the connection to the EasyX server
*/
    istat = gtx_InitClient ("seisplot");
    if (istat == -1) {
        printf ("\nI can't connect to the EasyX server.\n");
        exit (0);
    }

/*
    Initialize the legend data arrays.
*/
    InitLegendData ();

/*
    Post the main menu until the user chooses to exit.
*/
    while (1) {

        printf ("\n\n\
  MAIN MENU\n\n\
0)  Exit Program <default>\n\
1)  Read and Draw SEGY File\n\
2)  Pick Horizons From a Section\n\n\
  Take a pick: ");

        csw_gets (Ans);
        if (Ans[0] == '\0') {
            exit (0);
        }
        istat = sscanf (Ans, "%d", &pick);
        if (istat < 1) {
            exit (0);
        }
        if (pick == 0) {
            exit (0);
        }

    /*
        Read SEGY and draw.
    */
        if (pick == 1) {
            ReadAndDrawSection ();
        }

    /*
        Pick Horizon from a section.
    */
        else if (pick == 2) {
            PickHorizons ();
        }

    }  /*  end of while (1) loop  */

}  /*  end of function main  */




/*
  ****************************************************************

               R e a d A n d D r a w S e c t i o n

  ****************************************************************

    Prompt for the name of a SEGY file.  Read the file and draw the
  data as an image.

*/

static int ReadAndDrawSection ()

{
    char           fname[100], title[200];
    int            ncol, nrow, istat, filenum, trace1, trace2,
                   time1, time2, wnum, imagenum, tracedelta,
                   timedelta, i;
    float          *imagedata, colordelta,
                   minval, maxval, colormin[22], colormax[22];
    int            colornum[22];

/*
    prompt for the SEGY file name
*/
    while (1) {

        printf ("\nEnter the SEGY file name\n\
<default = return to main menu>: ");
        csw_gets (fname);
        csw_StrLeftJust (fname);
        if (fname[0] == '\0') {
            return 1;
        }

        if (!csw_DoesFileExist (fname)) {
            printf ("This file doesn't exist, try again.\n");
            continue;
        }

        filenum = csw_OpenFile (fname, "r");
        if (filenum == -1) {
            printf ("I can't open this file, try again.\n");
            continue;
        }

        break;

    }

/*
    Read the image data from the file.
*/
    istat = ReadSegyData (filenum, &time1, &time2, &trace1, &trace2,
                          &ncol, &nrow, &tracedelta, &timedelta,
                          &imagedata, &minval, &maxval);
    csw_CloseFile (filenum);
    if (istat == -1) {
        printf ("Error reading data from SEGY file\n");
        return 1;
    }

/*
    Check if any previously plotted windows have been
    erased by the user and clean up the legend data
    for the erased windows.
*/
    CleanupLegends ();

/*
    Start a new window for the seismic section.
*/
    sprintf (title, "Data From %s", fname);
    wnum = gtx_BgnPlt (title, 'w', (float)0.0, (float)0.0,
                      (float)25.0, (float)16.0,
                      0, (float)0.0, 5);

/*
    Set the image name to the file name and
    set up image options for upper left origin,
    trace and time spacing as calculated from the
    SEGY header data.
*/
    gtx_SetImageName (fname);
    gtx_SetImageOptions (1, time1, trace1, timedelta, tracedelta);

/*
    Set up the image colors as gray levels in the default
    EasyX color map.  The most negative amplitudes are
    white and the most positive are black.
*/
    colordelta = (maxval - minval) / 22.0;
    for (i=0; i<22; i++) {
        colormin[i] = minval + i * colordelta;
        colormax[i] = colormin[i] + colordelta;
        colornum[i] = 10 + i;
    }
    colormin[0] -= .1;
    colormax[21] += .1;
    gtx_SetImageColors (colormin, colormax, colornum, 22);

/*
    Draw the data as a floating point data image.
*/
    istat = gtx_FloatImage (imagedata, ncol, nrow, (float)0.0,
                            ImageX1, ImageY1, ImageX2, ImageY2);
    free (imagedata);
    if (istat == -1) {
        istat = gtx_ErrNum ();
        printf ("Error number %d from gtx_FloatImage\n", istat);
        gtx_EndPlt ();
        return 1;
    }
    imagenum = istat;

/*
    Draw a border around the image.
*/
    gtx_LineThk ((float)0.02);
    gtx_Line (XBorder, YBorder, 5);

/*
    Draw and Label Tick marks along the border.  Tick mark spacing
    is calculated so that about 5 to 10 ticks are shown on each
    edge of the section.
*/
    DrawTicksAndLabels (wnum, imagenum, trace1, time1, trace2, time2);

/*
    Signal the server that the plot is finished for now.
*/
    gtx_EndPlt ();

/*
    Draw the legend frame for the seismic section.  The
    plot is reopened by the legend frame function and 
    then closed by the function when the legend frame is finished.
*/
    DrawLegendFrame (wnum, title,
                     ImageX2+1.0, ImageY1,
                     ImageX2+6.0, ImageY2);

    return 1;
    
}  /*  end of static ReadAndDrawImage function  */



/*
  ****************************************************************

                 D r a w T i c k s A n d L a b e l s

  ****************************************************************

    Label the traces and time axes of the section.

*/

static int DrawTicksAndLabels (wnum, imagenum, trace1, time1, trace2, time2)

    int        wnum, imagenum, trace1, time1, trace2, time2;

{
    double     range, delta, logdelta, mantissa, base;
    float      x[2], y[2], tlen, tsize, tangle, xmid, ymid, maxtlen;
    int        i, ilog, idelta, istart, nc, tfont;
    char       text[50];

    tsize = 0.1;
    tangle = 0.0;
    xmid = (XBorder[0] + XBorder[2]) / 2.0;
    ymid = (YBorder[0] + YBorder[1]) / 2.0;

/*
    draw the x (trace) axes first
*/
    range = trace2 - trace1;
    delta = range / 7.;
    logdelta = log10 (delta);
    ilog = (int)logdelta;
    base = pow ((double)10.0, (double)ilog);
    mantissa = delta / base;
   
/*
    decide a good even labelling interval
*/ 
    if (mantissa < 1.5) {
        delta = base;
    }
    else if (mantissa >= 1.5  &&  mantissa < 3.0) {
        delta = 2.0 * base;
    }
    else if (mantissa >= 3.0  &&  mantissa < 4.5) {
        delta = 4.0 * base;
    }
    else if (mantissa >= 4.5  &&  mantissa < 7.5) {
        delta = 5.0 * base;
    }
    else {
        delta = 10.0 * base;
    }

/*
    decide the starting label
*/
    idelta = (int)delta;
    if (trace1 % idelta == 0) {
        istart = trace1;
    }
    else {
        istart = (trace1 / idelta + 1) * idelta;
    }

/*
    draw tick marks on top and bottom 
    and tick labels on the top
*/
    tfont = 2;
    gtx_Font (tfont);
    for (i=istart; i<trace2; i+= idelta) {
        gtx_ImageToPage (imagenum, wnum, &time1, &i, 1, x, y);
        x[1] = x[0];
        y[0] = YBorder[1];
        y[1] = YBorder[1] - 0.05;
        gtx_Line (x, y, 2);
        y[0] = YBorder[0];
        y[1] = YBorder[0] + 0.05;
        gtx_Line (x, y, 2);
        sprintf (text, "%d", i);
        nc = strlen (text);
        gtx_TextLength (text, nc, tfont, tsize, &tlen);
        gtx_Text (x[0] - tlen/2.0, YBorder[1] + 0.03,
                  tsize, tangle, text, nc);
    }

/*
    draw a centered axis label on the top
*/
    tfont = 4;
    gtx_Font (tfont);
    gtx_TextLength ("Trace Number", 12, tfont, tsize*2.0, &tlen);
    gtx_Text (xmid - tlen / 2.0, YBorder[1] + tsize + 0.15, 
              tsize * 2.0, tangle, "Trace Number", 12);

/*
    draw the y (time) axes next
*/
    range = time2 - time1;
    delta = range / 7.;
    logdelta = log10 (delta);
    ilog = (int)logdelta;
    base = pow ((double)10.0, (double)ilog);
    mantissa = delta / base;
   
/*
    decide a good even labelling interval
*/ 
    if (mantissa < 1.5) {
        delta = base;
    }
    else if (mantissa >= 1.5  &&  mantissa < 3.0) {
        delta = 2.0 * base;
    }
    else if (mantissa >= 3.0  &&  mantissa < 4.5) {
        delta = 4.0 * base;
    }
    else if (mantissa >= 4.5  &&  mantissa < 7.5) {
        delta = 5.0 * base;
    }
    else {
        delta = 10.0 * base;
    }

/*
    decide the starting label
*/
    idelta = (int)delta;
    if (time1 % idelta == 0) {
        istart = time1;
    }
    else {
        istart = (time1 / idelta + 1) * idelta;
    }

/*
    draw tick marks on left and right 
    and tick labels on the left
*/
    maxtlen = 0.0;
    tfont = 2;
    gtx_Font (tfont);
    for (i=istart; i<time2; i+=idelta) {
        gtx_ImageToPage (imagenum, wnum, &i, &trace1, 1, x, y);
        x[0] = XBorder[0];
        x[1] = XBorder[0] + 0.05;
        y[1] = y[0];
        gtx_Line (x, y, 2);
        x[0] = XBorder[2];
        x[1] = XBorder[2] - 0.05;
        gtx_Line (x, y, 2);
        sprintf (text, "%d", i);
        nc = strlen (text);
        gtx_TextLength (text, nc, tfont, tsize, &tlen);
        gtx_Text (XBorder[0] - tlen - 0.03, y[0] - tsize/2.0,
                  tsize, tangle, text, nc);
        if (tlen > maxtlen) {
            maxtlen = tlen;
        }
    }

/*
    draw a centered axis label on the left
*/
    tangle = 90.0;
    tfont = 4;
    gtx_Font (tfont);
    gtx_TextLength ("Time In Milliseconds", 20, tfont, tsize*2.0, &tlen);
    gtx_Text (XBorder[0] - maxtlen - 0.2, ymid - tlen / 2.0, 
              tsize * 2.0, tangle, "Time In Milliseconds", 20);

    return 1;

}  /*  end of static DrawTicksAndLabels function  */



/*
  ****************************************************************

                       P i c k H o r i z o n s

  ****************************************************************

    Prompt for horizon attributes, pick the horizon and draw the
  picked points.

*/

static int PickHorizons ()

{
    char        name[200], msg[200];
    int         i, colornum, npts, wnum, fnum, imagenum,
                col[MAX_PICK], row[MAX_PICK];
    float       x[MAX_PICK], y[MAX_PICK];

/*
    Set up the easyx graphics in the pick state 
    with an easyx server lock on each pick.
*/
    gtx_SetupPick (0);

/*
    Loop through the horizon name and color prompts
    until the user decides to go back to the main menu.
*/
    while (1) {

        printf ("\nEnter the new horizon name\n\
<Default = return to MAIN MENU>: ");
        csw_gets (Ans);
        if (Ans[0] == '\0') {
            break;
        }
        strcpy (name, Ans);

        printf ("\n\
  Color for the %s horizon\n\
1) Red <Default>\n\
2) Green\n\
3) Blue\n\
4) Cyan\n\
5) Magenta\n\
6) Yellow\n\
  Choose a color number: ", name);
        csw_gets (Ans);
        if (Ans[0] == '\0') {
            strcpy (Ans, "1");
        }
        colornum = 1;
        sscanf (Ans, "%d", &colornum);
        if (colornum < 1) colornum = 1;
        if (colornum > 6) colornum = 1;

    /*
        Assuming the default color map is in effect, then
        the system color number is the menu color number + 1
    */
        colornum++;

    /*
        Specify the message to be displayed while
        picking this horizon.
    */
        sprintf (msg, "Pick a segment of the %s horizon", name);
        gtx_PickMsg (msg);

    /*
        Loop through picking and redrawing segments
        of the horizon until zero points are chosen 
        for a segment.  At that time, prompt for
        another horizon.
    */
        while (1) {

        /*
            Pick a line from the image.  Break from the loop 
            if less than 2 points are picked.  If the points
            were all picked outside of any image, then the
            loop is continued (i.e. gtx_PickLine is repeated).
        */
            npts = 0;
            gtx_PickLine (MAX_PICK, x, y, &npts, &wnum, &fnum, &imagenum);
            if (npts < 2) break;
            if (imagenum < 0) continue;

        /*
            Convert the page coordinates to image coordinates
            and then convert the image coordinates back to page
            coordinates.  This makes the final page coordinates
            agree exactly with the image coordinates.
        */
            gtx_PageToImage (imagenum, wnum, x, y, npts, col, row);
            gtx_ImageToPage (imagenum, wnum, col, row, npts, x, y);

        /*
            Draw the permanent version of the horizon segment.
        */
            gtx_Restart (wnum);
            gtx_Color (colornum);
            gtx_LineThk (0.04);
            gtx_Line (x, y, npts);
            gtx_EndPlt ();

        /*
            The picked segment would probably be stored in a database
            at this time.  For this sample program, I merely print out
            the picked points to standard output.
        */
            printf ("\nList of points picked form the %s horizon\n", name);
            for (i=0; i<npts; i++) {
                printf ("trace = %d  time = %d\n", row[i], col[i]);
            }

        /*
            Reopen the plot and draw the horizon name into the
            legend box if it has not been drawn there already.
        */
            AddLegendEntry (wnum, colornum, name);

        }  /*  end of while (1) loop that picks segments  */

    }  /*  end of while (1) loop that prompts for horizon name and color  */

    gtx_EndPick ();
            
    return 1;

}  /*  end of static PickHorizons function  */






/*
  ****************************************************************

                      R e a d S e g y D a t a

  ****************************************************************

    Read the segy data and decimate it as specified.  Return the
  data array and the min and max values for coloring.
*/

static int ReadSegyData (fnum, time1, time2, trace1, trace2,
                         ncol, nrow, tracedelta, timedelta,
                         imagedata, minval, maxval)

    int           fnum, *time1, *time2, *trace1, *trace2,
                  *ncol, *nrow, *tracedelta, *timedelta;
    float         **imagedata, *minval, *maxval;

{
    char          fileheader[3201],
                  lineheader[401],
                  traceheader[241];
    SEgyDataType  segydata;
    int           srate, nsamp, format, istat,
                  fsize, ntrace, dsize, i, j, tracelen;
    float         *fdata, *fbuff;
    long          *i4buff;
    short         *i2buff;

    i2buff = NULL;
    i4buff = NULL;

/*
    Get the size of the file.
*/
    fsize = csw_FileSize (fnum);

/*
    Read the segy file header and line header.
*/
    istat = csw_BinFileRead (fileheader, 3200, 1, fnum);
    if (istat <= 0) return -1;
    istat = csw_BinFileRead (lineheader, 400, 1, fnum);
    if (istat <= 0) return -1;

/*
    Get sample rate, number of samples per trace,
    and the data format from the line header.
*/
    csw_memcpy (segydata.c1, lineheader+16, 2);
    srate = segydata.i2[0] / 1000;

    csw_memcpy (segydata.c1, lineheader+20, 2);
    nsamp = segydata.i2[0];

    csw_memcpy (segydata.c1, lineheader+24, 2);
    format = segydata.i2[0];

/*
    Get data size based on the format code.
*/
    if (format == 1) {
        dsize = sizeof(float);
    }
    else if (format == 2) {
        dsize = sizeof(long);
        i4buff = (long *)malloc (nsamp * dsize);
        if (!i4buff) return -1;
    }
    else if (format == 3) {
        dsize = sizeof(short);
        i2buff = (short *)malloc (nsamp * dsize);
        if (!i2buff) return -1;
    }
    else {
        return -1;
    }

/*  
    Get the number of traces in the file.
*/
    tracelen = 240 + dsize * nsamp;
    fsize -= 3600;
    if (fsize % tracelen != 0) return -1;
    ntrace = fsize / tracelen;

/*
    Set the returned parameters.
*/
    *time1 = 0;
    *time2 = nsamp * srate;
    *trace1 = 1;
    *trace2 = ntrace;
    *ncol = nsamp;
    *nrow = ntrace;
    *tracedelta = 1;
    *timedelta = srate;

/*
    Allocate a float data array for the line data.
*/
    fdata = (float *)malloc (*ncol * *nrow * sizeof(float));
    if (!fdata) {
        return -1;
    }

/*
    Read each trace, which consists of 240 bytes of trace
    header followed by the trace data.  If the format is
    not floating point, convert to floating point.
*/
    *minval = 1.e30;
    *maxval = -1.e30;

    for (i=0; i<ntrace; i++) {

        istat = csw_BinFileRead (traceheader, 240, 1, fnum);
        if (istat <= 0) return -1;
        
        fbuff = fdata + nsamp * i;
        if (format == 1) {
            istat = csw_BinFileRead (fbuff, dsize * nsamp, 1, fnum);
            if (istat <= 0) return -1;
        }
        else if (format == 2) {
            istat = csw_BinFileRead (i4buff, dsize * nsamp, 1, fnum);
            if (istat <= 0) return -1;
            for (j=0; j<nsamp; j++) {
                fbuff[j] = i4buff[j];
            }
        }
        else {
            istat = csw_BinFileRead (i2buff, dsize * nsamp, 1, fnum);
            if (istat <= 0) return -1;
            for (j=0; j<nsamp; j++) {
                fbuff[j] = i2buff[j];
            }
        }

    /*
        Find the min and max values of the data.
    */
        for (j=0; j<nsamp; j++) {
            if (fbuff[j] < *minval) *minval = fbuff[j];
            if (fbuff[j] > *maxval) *maxval = fbuff[j];
        }

    }

    *imagedata = fdata;
    
    return 1;

}  /*  end of static ReadSegyData function  */
