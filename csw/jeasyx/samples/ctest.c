/*
    ctest.c

    This is just a little program that can be used as a sanity check
    making sure that the easyX server can be run.
*/

#include "csw/easyx/include/easyx_api.h"


#ifdef NEED_PROTO

int testcprim (void);

#endif




main ()

{
    char      name[50], title[50], color;
    int       status, ntry, i1, i2;
    float     x1, y1, x2, y2;

/*
    initialize the server connection
*/
    strcpy (name, "ftest");
    status = gtx_InitClient (name);
    if (status == -1) {
        printf ("Error connecting to EasyX server\n");
        exit (1);
    }

/*
    modify some of the alias tables from their default file values
*/
    i1 = 7;
    i2 = 1;
    gtx_SetColorAlias (&i1, &i2, 1);

/*
    create a window with server default redraw order
    and variable scale 
*/
    strcpy (title, "C Test");
    color = 'w';
    x1 = 0.;
    y1 = 0.;
    x2 = 10.;
    y2 = 10.;
    ntry = 3;

    status = gtx_BgnPlt (title, color, x1, y1, x2, y2, 
                         0, (CSW_Float)-1., ntry);
    if (status == -1) {
        printf ("Error in BgnPlt\n");
    }

/*
    draw some miscellaneous objects to the window
*/
    testcprim ();

/*
    end the drawing to the window and unlock the server connection
*/
    gtx_EndPlt ();

/*
    cleanly end the connection to the server
*/
    gtx_RemoveClient ();

    exit (0);

}


#ifdef NEED_PROTO

int testcprim (void)

#else

int testcprim ()

#endif

{
    float     x[10], y[10];
    int       i1, i, j;

    static float    xt[10] = {0., 0., .5, .5},
                    yt[10] = {0., .5, .5};


/*
    draw text in various colors, fonts, and angles
*/
    gtx_Font (0);

    gtx_Color (2);
    gtx_TextThk (.01);
    gtx_Text (1., 1., .2, 0., "testcolor2", 12);

    gtx_Color (3);
    gtx_TextThk ((CSW_F).05);
    gtx_Text ((CSW_F)3., (CSW_F)1., (CSW_F).2, (CSW_F)45., "test color 3", 12);

    gtx_Color (4);
    gtx_Font (1);
    gtx_Text ((CSW_F)5.,(CSW_F)1.,(CSW_F).2,0.,"Test Client Font 1", 11);
    gtx_Text ((CSW_F)7.,(CSW_F)1.,(CSW_F).2,0.,"A",-1);
    gtx_Font (0);

    gtx_Color (1);

/*
    draw boxes with gtx_plot, changing color and line thickness
    for each box
*/
    for (i=0; i<4; i++) {
        gtx_Color (i+1);
        gtx_LineThk ((i+1)*(i+1)*.0025);
        for (j=0;j<5;j++) {
            x[j] = xt[j] + i + 1;
            y[j] = yt[j] + 3.0;
        }
        gtx_Plot (x[0], y[0], 3);
        for (j=1; j<5; j++) {
            gtx_Plot (x[j], y[j], 2);
        }
    }

    gtx_Plot (0.,0.,3);

    gtx_LineThk ((CSW_F).01);

/*
    draw some filled areas, and symbols in different colors
*/
    i1 = 5;
    for (i=0; i<6; i++) {
        gtx_Color (i+2);
        for (j=0; j<5; j++) {
            x[j] = xt[j] + i + 1;
            y[j] = yt[j] + 4.0;
        }
        gtx_Polygon (x, y, &i1, 1, 1);
        gtx_Symbol (x[1]+(CSW_F).5, (CSW_F)5.5, (CSW_F)0.5, 0.0, i+1);
        for (j=0; j<5; j++) {
            y[j] = y[j] + 3.0;
        }
        gtx_Line (x, y, 5);
    }

    return  1;

}
