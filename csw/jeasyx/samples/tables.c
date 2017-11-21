
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "csw/easyx/include/easyx_api.h"
#include "csw/easyx/private_include/gtx_sendmsgP.h"

static int        nr=10, nc=10;
static char       TextPlot[100];
static CSW_F      Angle;
static int        Ifont;
static int        Job;

static char       BgColor = 'w';
static int        Ntry = 3,
                  Mvec = 6,
                  Nvtotal = 0,
                  Outline = 0,
                  FirstSym, LastSym;
static CSW_F      Scale = -1.;
static int        Order = 0;
static int        FontOutlineColor, FontFillColor;

static char       Ans[200];

#ifdef NEED_PROTO

int setjob (int);
int setnrnc (int, int);
int setmvec (int);
int setAngle (CSW_F, int);
int setbgtest (char);
int testprim (void);
int testline (void);
int samplefill (void);
int sampletext (void);
int samplesymbol (void);
int testcolormap (void);
int testpattern (void);
int dashlinetable (void);
int sleep (int);

#endif



#ifdef NEED_PROTO

int main (int argc, char **argv)

#else

int main (argc, argv)

    int            argc;
    char**         argv;

#endif

{
    char           bgcolor[10];
    int            job, istat;
    int            numvec, nr, nc, ifont;
    CSW_F          fangle;
    double         dnumvec;
    int            color1, color2;

    argc = argc;
    argv = argv;

    istat = gtx_InitClient ("test");
    if (istat != 1) {
        printf ("\nCannot connect to server\n");
        exit (0);
    }

    for (;;) {

        printf ("\n\
0)  exit program\n\
1)  vector test\n\
2)  complex polygon test\n\
3)  text table\n\
4)  symbol table\n\
5)  color table\n\
6)  polygon pattern fill table\n\
7)  line dash pattern table\n\
\n\
Take a pick: ");

        csw_gets (Ans);
        sscanf (Ans, "%d", &job);

        if (job < 0  ||  job > 7) {
            continue;
        }

        if (job == 0) {
            gtx_RemoveClient ();
            break;
        }

        setjob (job);

        if (job == 1) {
            printf ("Enter number of vectors to draw: ");
            csw_gets (Ans);
            sscanf (Ans, "%d", &numvec);
            Nvtotal = numvec;
            numvec = numvec / 50;
            dnumvec = (double)numvec;
            dnumvec = sqrt (dnumvec);
            nr = (int)dnumvec + 1;
            if (nr < 2) {
                nr = 2;
            }
            nc = nr + 1;
            setnrnc (nr, nc);
            setmvec (12);
        }

        if (job == 2) {
            printf ("Enter number of polygons to draw: ");
            csw_gets (Ans);
            sscanf (Ans, "%d", &numvec);
            Nvtotal = numvec;
            dnumvec = (double)numvec;
            dnumvec = sqrt (dnumvec);
            nr = (int)dnumvec + 1;
            if (nr < 2) {
                nr = 2;
            }
            nc = nr + 1;
            setnrnc (nr, nc);
        }

        if (job == 3) {
            printf ("Enter font number and angle: ");
            csw_gets (Ans);
            sscanf (Ans, "%d%f", &ifont, &fangle);
            setAngle (fangle, ifont);
            if (ifont > 7  &&  ifont < 100) {
                printf ("Enter color numbers for text outline and text fill: ");
                csw_gets (Ans);
                sscanf (Ans, "%d %d", &color1, &color2);
                FontOutlineColor = color1;
                FontFillColor = color2;
            }
        }

        if (job == 4) {
            FirstSym = 0;
            LastSym = 10000;
        }

        printf ("Enter background color (b or w): ");
        csw_gets (Ans);
        if (Ans[0] == '\0') {
            strcpy (Ans, "w");
        }
        sscanf (Ans, "%s", bgcolor);
        setbgtest (bgcolor[0]);

        testprim ();
    }

    exit (0);

    return 1;

}



#ifdef NEED_PROTO

int setnrnc (int nrin, int ncin)

#else

int setnrnc (nrin, ncin)

    int         nrin, ncin;

#endif

{
    nr = nrin;
    nc = ncin;
    return 0;
}



#ifdef NEED_PROTO

int setmvec (int val)

#else

int setmvec (val)

    int   val;

#endif

{
    Mvec = val;
    return 0;
}



#ifdef NEED_PROTO

int setbgtest (char bg)

#else

int setbgtest (bg)

    char   bg;

#endif

{
    BgColor = bg;
    if (!(BgColor == 'b'  ||  BgColor == 'B')) {
        BgColor = 'w';
    }
    return 0;
}



#ifdef NEED_PROTO

int setjob (int job)

#else

int setjob (job)

    int job;

#endif

{
    Job = job;
    return 0;
}




#ifdef NEED_PROTO

int setAngle (CSW_F f, int ifont)

#else

int setAngle (f, ifont)

    CSW_F f;
    int   ifont;

#endif

{
    Angle = f;
    Ifont = ifont;
    return 0;
}



#ifdef NEED_PROTO

int testprim (void)

#else

int testprim ()

#endif

{

    if (Job == 1) {
        system ("date");
        testline ();
        system ("date");
    }
    else if (Job == 2) {
        system ("date");
        samplefill ();
        system ("date");
    }
    else if (Job == 3) {
        sampletext ();
    }
    else if (Job == 4) {
        samplesymbol ();
    }
    else if (Job == 5) {
        testcolormap ();
    }
    else if (Job == 6) {
        testpattern ();
    }
    else if (Job == 7) {
        dashlinetable ();
    }
    else {
        return 0;
    }

    return 0;

}


#ifdef NEED_PROTO

int sampletext (void)

#else

int sampletext ()

#endif

{
    CSW_F     tsize, thk, x, y, x1, y1, x2, y2;
    int       i, nc;
    char      title[200];

    x1 = 0.0;
    y1 = 0.0;
    x2 = 20.0;
    y2 = 15.0;

    sprintf (title, "Text Font Number %d", Ifont);

    i = gtx_BgnPlt (title, BgColor, 
                    x1, y1, x2, y2, Order, Scale, Ntry);
    if (i < 0) return 1;

    thk = 0.02;
    if (Ifont == 111) thk *= 2.0;
    gtx_TextThk (thk);
    gtx_Font (Ifont);
    tsize = 0.3;

    if (Ifont > 7  &&  Ifont < 100) {
        gtx_TextColor (FontOutlineColor);
        gtx_TextFillColor (FontFillColor);
        tsize = 0.8;

        nc = 22;
        x = 1.0;
        y = 10.0;
        strcpy(TextPlot, "!~@#$%^&*()_+|\\=-`:;\"'"); 
        gtx_Text (x, y, tsize, Angle, TextPlot, nc);

        nc = 10;
        x = 1.0;
        y = 8.5;
        strcpy(TextPlot, "{}[]<>,.?/"); 
        gtx_Text (x, y, tsize, Angle, TextPlot, nc);

        nc = 10;
        x = 1.0;
        y = 1.0;
        strcpy(TextPlot, "0123456789");
        gtx_Text (x, y, tsize, Angle, TextPlot, nc);

        nc = 13;
        x = 1.0;
        y = 2.5;
        strcpy(TextPlot, "abcdefghijklm");
        gtx_Text (x, y, tsize, Angle, TextPlot, nc);

        nc = 13;
        x = 1.0;
        y = 4.0;
        strcpy(TextPlot, "nopqrstuvwxyz");
        gtx_Text (x, y, tsize, Angle, TextPlot, nc);

        nc = 13;
        x = 1.0;
        y = 5.5;
        strcpy(TextPlot, "ABCDEFGHIJKLM");
        gtx_Text (x, y, tsize, Angle, TextPlot, nc);

        nc = 13;
        x = 1.0;
        y = 7.0;
        strcpy(TextPlot, "NOPQRSTUVWXYZ"); 
        gtx_Text (x, y, tsize, Angle, TextPlot, nc);

        gtx_EndPlt ();

        return 0;
    }
    
    nc = 32;
    x = 1.0;
    y = 6.0;
    strcpy(TextPlot, "!~@#$%^&*()_+|\\=-`:;\"'{}[]<>,.?/"); 
    gtx_Text (x, y, tsize, Angle, TextPlot, nc);

    nc = 10;
    x = 1.0;
    y = 1.0;
    strcpy(TextPlot, "0123456789");
    gtx_Text (x, y, tsize, Angle, TextPlot, nc);

    nc = 13;
    x = 1.0;
    y = 2.0;
    strcpy(TextPlot, "abcdefghijklm");
    gtx_Text (x, y, tsize, Angle, TextPlot, nc);

    nc = 13;
    x = 1.0;
    y = 3.0;
    strcpy(TextPlot, "nopqrstuvwxyz");
    gtx_Text (x, y, tsize, Angle, TextPlot, nc);

    nc = 13;
    x = 1.0;
    y = 4.0;
    strcpy(TextPlot, "ABCDEFGHIJKLM");
    gtx_Text (x, y, tsize, Angle, TextPlot, nc);

    nc = 13;
    x = 1.0;
    y = 5.0;
    strcpy(TextPlot, "NOPQRSTUVWXYZ"); 
    gtx_Text (x, y, tsize, Angle, TextPlot, nc);

    gtx_EndPlt ();

    return 0;

}


#ifdef NEED_PROTO

int samplesymbol (void)

#else

int samplesymbol ()

#endif

{
    int              ncl, nrl, i, j, nsym, nchar;
    CSW_F            x, y, x1, y1, x2, y2, size, xt, yt, tsize, tang, sang;
    char             text[10];

    sang = 0.0;

    FirstSym = 0;
    LastSym = 500;

    x1 = -2.;
    y1 = -2.;
    x2 = 17.;
    y2 = 12.;
    i = gtx_BgnPlt ("Symbol Table", BgColor,
                x1, y1, x2, y2, Order, Scale, Ntry);
    if (i < 0) return 1;

    tsize = 0.2;
    size = 0.4;
    tang = 0.0;

    nsym = 1;
    ncl = 15;
    nrl = 10;

    for (i=0; i<nrl; i++) {
        y = 10 - i;
        for (j=0; j<ncl; j++) {
            x = j;
            if (nsym < FirstSym  ||  nsym > LastSym) {
                nsym++;
                text[0] = '\0';
                continue;
            }
            gtx_Symbol (x, y, size, sang, nsym);
            sprintf (text, "%d", nsym);
            xt = x - .45;
            yt = y - .45;
            nchar = strlen (text);
            gtx_Text (xt, yt, tsize, tang, text, nchar);
            text[0] = '\0';
            nsym++;
        }
    }

    gtx_EndPlt ();
    return 0;
}




#ifdef NEED_PROTO

int testline (void)

#else

int testline ()

#endif

{
    CSW_F      x[1000], y[1000], x1, y1, x2, y2;
    int        i, j, k, kk, m;
    CSW_F      xmid, ymid, dx;
    char       text[100];

    sprintf (text, "Drawing unstructured vectors nvec = %d", Nvtotal);
    gtx_set_msg_for_timer (text);

    x1 = -2.0;
    y1 = -2.0;
    x2 = nc + 2;
    y2 = nr + 2;
    i = gtx_BgnPlt ("Line Draw Test", BgColor,
                x1, y1, x2, y2, Order, Scale, Ntry);
    if (i < 0) return 1;

    m = Mvec;
    for (i=0; i<nr; i++) {
        ymid = (CSW_F)i;
        for (j=0; j<nc; j++) {
            xmid = (CSW_F)j;
            dx = .07;
            kk=0;
            for (k= -m; k<=m; k++) {
                x[kk] = xmid+dx;
                y[kk] = ymid + (CSW_F)k*.07;
                kk++;
                dx = -dx;
            }
            gtx_Line (x, y, kk);
            dx = .07;
            kk=0;
            for (k= -m; k<=m; k++) {
                x[kk] = xmid + (CSW_F)k*.07;
                y[kk] = ymid + dx;
                kk++;
                dx = -dx;
            }
            gtx_Line (x, y, kk);
        }
    }

    gtx_EndPlt ();

    return 0;

}




#ifdef NEED_PROTO

int samplefill (void)

#else

int samplefill ()

#endif

{
    CSW_F            x[100], y[100], dx3, dx6;
    int              istat, i, j, nfill, ncomp;
    CSW_F            xmid, ymid, dx, x1, y1, x2, y2;
    char             text[100];

    sprintf (text, "Drawing unstructured polygons npoly = %d", Nvtotal);
    gtx_set_msg_for_timer (text);

    x1 = -2.;
    y1 = -2.;
    x2 = nc + 2;
    y2 = nr + 2;

    i = gtx_BgnPlt ("Polyfill Test", BgColor,
                x1, y1, x2, y2, Order, Scale, Ntry);
    if (i < 0) return 1;

    ncomp = 1;

    dx = .3;

    nfill = 17;
    dx3 = dx / 1.5;
    dx6 = dx / 4.;

    for (i=0; i<nr; i++) {
        ymid = (CSW_F)i;
        y[0] = ymid;
        y[1] = ymid + dx;
        y[2] = ymid ;
        y[3] = ymid - dx;
        y[4] = ymid ;
        y[5] = ymid;
        y[6] = ymid + dx3;
        y[7] = ymid;
        y[8] = ymid - dx3;
        y[9] = ymid;
        y[10] = ymid;
        y[11] = ymid;
        y[12] = ymid + dx6;
        y[13] = ymid;
        y[14] = ymid -dx6;
        y[15] = ymid;
        y[16] = ymid;
        for (j=0; j<nc; j++) {
            xmid = (CSW_F)j;
            x[0] = xmid - dx;
            x[1] = xmid;
            x[2] = xmid + dx;
            x[3] = xmid;
            x[4] = xmid - dx;
            x[5] = xmid - dx3;
            x[6] = xmid;
            x[7] = xmid + dx3;
            x[8] = xmid;
            x[9] = xmid - dx3;
            x[10] = xmid - dx;
            x[11] = xmid - dx6;
            x[12] = xmid;
            x[13] = xmid + dx6;
            x[14] = xmid;
            x[15] = xmid - dx6;
            x[16] = xmid - dx;
            istat = gtx_Fill (x, y, &nfill, ncomp, Outline);
            if (istat == -1) {
                istat = gtx_ErrNum ();
                printf ("\nerror from polygon fill = %d\n\n", istat);
                gtx_EndPlt ();
                return 0;
            }
        }
    }

    gtx_EndPlt ();

    return 0;

}






#ifdef NEED_PROTO

int testcolormap (void)

#else

int testcolormap ()

#endif

{
    CSW_F      x[5], y[5], x0, y0;
    CSW_F      x1, y1, x2, y2, xt, yt, tsize, tang;
    int        ncolor, i, ndo, nmod, len, bflag, npts, ncomp;
    char       str[50];

    x1 = -2.0;
    y1 = -2.0;
    x2 = 20.;
    y2 = 20.;

    i = gtx_BgnPlt ("Color Map", BgColor, 
                x1, y1, x2, y2, Order, Scale, Ntry);
    if (i < 0) {
        printf ("Error from bgnplt = %d\n", gtx_ErrNum());
        return 1;
    }

    xt = 0.01;
    gtx_TextThk (xt);
    gtx_LineThk (xt);
    tsize = 0.2;
    tang = 0.0;
    bflag = 2;
    npts = 5;
    ncomp = 1;
    
    x0 = 0.0;
    y0 = 18.0;

    ndo = 256;
    nmod = 16;
    ncolor = 1;

    gtx_LineColor (1);

    for (i=0; i<ndo; i++) {
        if (i%nmod == 0) {
            x0 = 0.0;
            y0 -= 1.0;
        }
        x[0] = x0;
        y[0] = y0;
        x[1] = x0;
        y[1] = y0+.4;
        x[2] = x0+.5;
        y[2] = y0+.4;
        x[3] = x0+.5;
        y[3] = y0;
        x[4] = x0;
        y[4] = y0;

        sprintf (str, "%d", i);
        xt = x0;
        yt = y0 -.22;
        len = strlen (str);
        ncolor = 1;
        gtx_TextColor (ncolor);
        gtx_Text (xt, yt, tsize, tang, str, len);
        ncolor = i;
        gtx_FillColor (ncolor);
        gtx_Fill (x, y, &npts, ncomp, bflag);

        x0 += 1.0;
    }

    gtx_EndPlt ();

    return 0;

}





#ifdef NEED_PROTO

int testpattern (void)

#else

int testpattern ()

#endif

{
    int              ncl, nrl, i, j, k, nsym, nchar, nfill;
    CSW_F            x, y, x1, y1, x2, y2, 
                     xt, yt, tsize, tang;
    CSW_F            xa[10], ya[10], fdum;
    char             text[100];

    static CSW_F     xp[] = {-.35, -.35, 1.35, 1.35, -.35},
                     yp[] = {-.35, 1.15, 1.15, -.35, -.35},
                     xl[] = {0., 0., 1., 1., 0.},
                     yl[] = {0., 1., 1., 0., 0.};

    x1 = -1.;
    y1 = -1.;
    x2 = 22.;
    y2 = 15.;
    i = gtx_BgnPlt ("Fill Pattern Table", BgColor,
                x1, y1, x2, y2, Order, Scale, Ntry);
    if (i < 0) return 1;

    tsize = 0.15;
    tang = 0.0;
    gtx_LineThk ((CSW_F)0.04);
    gtx_TextThk ((CSW_F)0.02);
    gtx_Color (1);
    gtx_FillColor (10);

    nsym = 1;
    ncl = 9;
    nrl = 5;
    nfill = 5;

    for (i=0; i<nrl; i++) {
        y = 11. - 2. * i;
        for (j=0; j<ncl; j++) {
            x = j * 2. + 1.;
            if (nsym > 45) {
                nsym++;
                continue;
            }
            for (k=0; k<5; k++) {
                xa[k] = x + xp[k];
                ya[k] = y + yp[k];
            }
            fdum = 0.005;
            gtx_FillPattern (nsym, (CSW_F)2.0, fdum);
            gtx_Fill (xa, ya, &nfill, 1, 1);
            for (k=0; k<5; k++) {
                xa[k] = x + xl[k];
                ya[k] = y + yl[k];
            }

            sprintf (text, "pattern %d", nsym);
            xt = x - .25;
            yt = y + 1.25;
            nchar = strlen (text);
            gtx_Text (xt, yt, tsize, tang, text, nchar);
            text[0] = '\0';
            nsym++;
        }
    }

    gtx_EndPlt ();

    return 0;

}  /*  end of testpattern function  */





#ifdef NEED_PROTO

int dashlinetable (void)

#else

int dashlinetable ()

#endif

{
    CSW_F            hx[100], hy[100], x1, x2, y1, y2, scale;
    int              i, j;
    int              nc;
    char             title[100];
    char             text[40];
    
    x1 = 0.;
    y1 = 0.;
    x2 = 15.;
    y2 = 10.;
    scale = 1.0;

    sprintf (title, "Line Dash Pattern Table");

    i = gtx_BgnPlt (title, BgColor,
                x1, y1, x2, y2, Order, Scale, Ntry);
    if (i < 0) return 1;

    gtx_LineThk ((CSW_F)0.04);
    gtx_TextThk ((CSW_F)0.02);
    gtx_Color (1);

    for (i=1; i<31; i++) {
        gtx_LinePattern (i, scale);
        if (i < 11) {
            hx[0] = 1.;
            hx[1] = 4.;
        }
        if (i > 10  &&  i < 21) {
            hx[0] = 6.;
            hx[1] = 9.;
        }
        if (i > 20  &&  i < 31) {
            hx[0] = 11.;
            hx[1] = 14.;
        } 

        j = (i-1) % 10;
        hy[0] = 8.6 - .8 * j - .5;
        hy[1] = 8.6 - .8 * j - .5;

        sprintf (text, "pattern # %d", i);
        nc = strlen (text);

        gtx_Text (hx[0], hy[0] + .2, (CSW_F)0.2, (CSW_F)0.0, text, nc);
        gtx_Line (hx, hy, 2);
    }

    gtx_EndPlt ();

    return 0;

}
