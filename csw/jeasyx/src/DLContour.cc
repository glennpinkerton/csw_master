
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 *  DLContour.cc
 *
 *  This is the implementation of the DLContour class.
 *  This class is only used from the display list class.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "csw/jeasyx/private_include/DisplayList.h"
#include "csw/jeasyx/private_include/DLContour.h"
#include "csw/jeasyx/private_include/gtx_drawprim.h"

#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/include/csw_.h"

#include "csw/surfaceworks/include/grid_api.h"
#include "csw/surfaceworks/include/contour_api.h"


/*  constants for the file  */


/*------------------------------------------------------------------*/

/*
 * Empty constructor
 */

DLContour::DLContour (GTXDrawPrim *dp)
{

    if (dp == NULL) {
        assert (false);
    }

    gtx_drawprim_ptr = dp;

  /*
   * Initialize public data.
   */
    grid_index = -1;
    image_id = -1;

    frame_num = -1;
    layer_num = -1;
    item_num = -1;
    selectable_object_num = -1;

    deleted_flag = 0;
    draw_flag = 0;
    visible_flag = 1;

    prim_num = -1;

    in_extra = 0;

  /*
   * Initialize private data.
   */
    crec = NULL;
}

DLContour::~DLContour ()
{
    gtx_drawprim_ptr = NULL;

    if (crec != NULL) {
        conapi_obj.con_FreeContours (crec, 1);
    }
    crec = NULL;
}

int DLContour::SetCrec (COntourOutputRec *cp)
{
    if (crec) {
        conapi_obj.con_FreeContours (crec, 1);
        crec = NULL;
    }

    if (cp == NULL) {
        return 1;
    }

    crec = conapi_obj.con_CopyOutputRec (cp);

    if (crec) {
        return 1;
    }
    else {
        return -1;
    }
}

COntourOutputRec *DLContour::GetCrec (void)
{
    return crec;
}


void DLContour::SetInExtra (int ival)
{
    in_extra = ival;
}

int DLContour::GetInExtra ()
{
    return in_extra;
}


/*
 * This function returns the internal x and y pointers.  The calling function 
 * must treat these as read only!!!
 */
void DLContour::GetContourPoints (CSW_F **xpts, CSW_F **ypts, int *npts)
{
    *npts = 0;
    *xpts = NULL;
    *ypts = NULL;

    if (crec == NULL) {
        return;
    }

    *xpts = crec->x;
    *ypts = crec->y;
    *npts = crec->npts;

    return;

}


int DLContour::drawContour (
                     COntourDrawOptions *draw_options,
                     DLContourProperties *dlprop,
                     void *vptr)
{
    CDisplayList      *dlist;
    COntourLinePrim   *lines, *tlines, *lp;
    COntourTextPrim   *text;
    int               nlines, ntext, ntlines;
    int               i, j, font, istat;
    CSW_F             widths[100], size;
    double            thick;

    if (crec == NULL) {
        return 0;
    }

    if (vptr == NULL  ||  draw_options == NULL  ||  dlprop == NULL) {
        assert (0);
    }

    if (crec->major == 0) {
        size = draw_options->minor_text_size;
        font = draw_options->minor_text_font;
    }
    else {
        size = draw_options->major_text_size;
        font = draw_options->major_text_font;
    }
    calc_char_widths (crec->text, widths, size, font);

    lines = NULL;
    nlines = 0;
    text = NULL;
    ntext = 0;
    tlines = NULL;
    ntlines = 0;

    dlist = (CDisplayList *)vptr;
    dlist->SetLayerNum (layer_num);
    dlist->SetItemNum (item_num);
    dlist->SetFrameNum (frame_num);
    dlist->SetGridNum (grid_index);
    dlist->SetLinePattern (0, 1.0);
    dlist->SetLineArrow (0);
    dlist->SetLineSymbol (0);
    dlist->SetLineSmoothFlag (0);

    istat = conapi_obj.con_DrawLine (crec, widths,
                          &lines, &nlines,
                          &text, &ntext,
                          draw_options);
    if (istat == -1) {
        return -1;
    }

    dlist->SetImageID (image_id);
    for (i=0; i<nlines; i++) {
        lp = lines+i;
        if (lp->majorflag) {
            dlist->SetLineThickness (dlprop->majorThickness);
            dlist->SetLineColor (dlprop->redMajor,
                                 dlprop->greenMajor,
                                 dlprop->blueMajor,
                                 dlprop->alphaMajor);
        }
        else {
            dlist->SetLineThickness (dlprop->minorThickness);
            dlist->SetLineColor (dlprop->redMinor,
                                 dlprop->greenMinor,
                                 dlprop->blueMinor,
                                 dlprop->alphaMinor);
        }
        dlist->AddContourLine (lp->x, lp->y, lp->npts);
    }

    conapi_obj.con_FreeDrawing (lines, nlines,
                     NULL, 0,
                     NULL, 0);
    lines = NULL;
    nlines = 0;

    if (text  &&  ntext > 0) {
        for (i=0; i<ntext; i++) {
            istat = conapi_obj.con_DrawText (text + i,
                                  &tlines,
                                  &ntlines);
            if (tlines && ntlines > 0) {
                for (j=0; j<ntlines; j++) {
                    lp = tlines + j;
                    if (lp->majorflag) {
                        thick = dlprop->majorThickness;
                        if (thick > 0.02) thick = 0.02;
                        dlist->SetLineThickness (thick);
                        dlist->SetLineColor (dlprop->redMajor,
                                             dlprop->greenMajor,
                                             dlprop->blueMajor,
                                             dlprop->alphaMajor);
                    }
                    else {
                        thick = dlprop->minorThickness;
                        if (thick > 0.02) thick = 0.02;
                        dlist->SetLineThickness (thick);
                        dlist->SetLineColor (dlprop->redMinor,
                                             dlprop->greenMinor,
                                             dlprop->blueMinor,
                                             dlprop->alphaMinor);
                    }
                    dlist->AddContourLine (lp->x, lp->y, lp->npts);
                }
                conapi_obj.con_FreeDrawing (tlines, ntlines,
                                 NULL, 0,
                                 NULL, 0);
                tlines = NULL;
                ntlines = 0;
            }
        }
    }
    
    conapi_obj.con_FreeDrawing (NULL, 0,
                     text, ntext,
                     NULL, 0);

    dlist->SetImageID (-1);

    text = NULL;
    ntext = 0;

    return 1;
}



int DLContour::calc_char_widths (char *text, CSW_F *widths, CSW_F size, int font)

{
    int               i, len;
    CSW_Float         sum, total;

    len = strlen (text);

    sum = 0.0;
    for (i=0; i<len; i++) {

        gpf_font_obj.gpf_TextLength2 (text+i, 1, font, size, widths+i);
        sum += widths[i];

    }

    widths[len] = 0.0;

    if (len > 1) {
        gpf_font_obj.gpf_TextLength2 (text, len, font, size, &total);
        sum = (total - sum) / (CSW_Float)(len-1);
        for (i=0; i<len-1; i++) {
            widths[i] += sum;
        }
    }

    return 1;

}


/*
 * Draw the contour directly to the device, rather than adding it
 * to the display list.  This is done for selected contours.
 */
int DLContour::drawContourDirect (
                     COntourDrawOptions *draw_options,
                     DLContourProperties *dlprop,
                     void *vptr)
{
    CDisplayList      *dlist;
    COntourLinePrim   *lines, *tlines, *lp;
    COntourTextPrim   *text;
    int               nlines, ntext, ntlines;
    int               i, j, font, istat;
    CSW_F             widths[100], size;
    double            thick;
    int               red, green, blue;
    CSW_F             *xy;

    if (crec == NULL) {
        return 0;
    }

    if (vptr == NULL  ||  draw_options == NULL  ||  dlprop == NULL) {
        assert (0);
    }

    if (crec->major == 0) {
        size = draw_options->minor_text_size;
        font = draw_options->minor_text_font;
    }
    else {
        size = draw_options->major_text_size;
        font = draw_options->major_text_font;
    }
    calc_char_widths (crec->text, widths, size, font);

    lines = NULL;
    nlines = 0;
    text = NULL;
    ntext = 0;
    tlines = NULL;
    ntlines = 0;

    dlist = (CDisplayList *)vptr;
    dlist->SetLayerNum (layer_num);
    dlist->SetItemNum (item_num);
    dlist->SetFrameNum (frame_num);
    dlist->SetGridNum (grid_index);
    dlist->SetLinePattern (0, 1.0);
    dlist->SetLineArrow (0);
    dlist->SetLineSymbol (0);
    dlist->SetLineSmoothFlag (0);

    istat = conapi_obj.con_DrawLine (crec, widths,
                          &lines, &nlines,
                          &text, &ntext,
                          draw_options);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<nlines; i++) {
        lp = lines+i;
        if (lp->majorflag) {
            thick = dlprop->majorThickness;
            red = dlprop->redMajor;
            green = dlprop->greenMajor;
            blue = dlprop->blueMajor;
        }
        else {
            thick = dlprop->minorThickness;
            red = dlprop->redMinor;
            green = dlprop->greenMinor;
            blue = dlprop->blueMinor;
        }
        xy = (CSW_F *)csw_Malloc (lp->npts * 2 * sizeof(CSW_F));
        if (xy == NULL) {
            continue;
        }
        gpf_packxy2 (lp->x, lp->y, lp->npts,
                     xy);
        dlist->AddContourLine (lp->x, lp->y, lp->npts);
        gtx_drawprim_ptr->gtx_cliplineprim(xy,
                         lp->npts,
                         0, /* line smoothing flag */
                         (CSW_F)thick,
                         red,
                         green,
                         blue,
                         0, /* dashed line pattern */
                         1.0, /* dash scale */
                         0); /* arrow flag */
        csw_Free (xy);
        xy = NULL;

    }

    conapi_obj.con_FreeDrawing (lines, nlines,
                     NULL, 0,
                     NULL, 0);
    lines = NULL;
    nlines = 0;

    if (text  &&  ntext > 0) {
        for (i=0; i<ntext; i++) {
            istat = conapi_obj.con_DrawText (text + i,
                                  &tlines,
                                  &ntlines);
            if (tlines && ntlines > 0) {
                for (j=0; j<ntlines; j++) {
                    lp = tlines + j;
                    if (lp->majorflag) {
                        thick = dlprop->majorThickness * .75;
                        if (thick > 0.02) thick = 0.02;
                        red = dlprop->redMajor;
                        green = dlprop->greenMajor;
                        blue = dlprop->blueMajor;
                    }
                    else {
                        thick = dlprop->minorThickness * .75;
                        if (thick > 0.02) thick = 0.02;
                        red = dlprop->redMinor;
                        green = dlprop->greenMinor;
                        blue = dlprop->blueMinor;
                    }
                    xy = (CSW_F *)csw_Malloc (lp->npts * 2 * sizeof(CSW_F));
                    if (xy == NULL) {
                        continue;
                    }
                    gpf_packxy2 (lp->x, lp->y, lp->npts,
                                 xy);
                    dlist->AddContourLine (lp->x, lp->y, lp->npts);
                    gtx_drawprim_ptr->gtx_cliplineprim(xy,
                                     lp->npts,
                                     0, /* line smoothing flag */
                                     (CSW_F)thick,
                                     red,
                                     green,
                                     blue,
                                     0, /* dashed line pattern */
                                     1.0, /* dash scale */
                                     0); /* arrow flag */
                    csw_Free (xy);
                    xy = NULL;

                }
                conapi_obj.con_FreeDrawing (tlines, ntlines,
                                 NULL, 0,
                                 NULL, 0);
                tlines = NULL;
                ntlines = 0;
            }
        }
    }
    
    conapi_obj.con_FreeDrawing (NULL, 0,
                     text, ntext,
                     NULL, 0);
    text = NULL;
    ntext = 0;

    return 1;
}



void DLContour::CalcBounds (double *cx1, double *cy1, double *cx2, double *cy2)
{
    CSW_F     *xpts, *ypts;
    int       i, npts;

    *cx1 = 1.e30;
    *cy1 = 1.e30;
    *cx2 = -1.e30;
    *cy2 = -1.e30;

    if (crec == NULL) {
        return;
    }    

    xpts = crec->x;
    ypts = crec->y;
    npts = crec->npts;

    for (i=0; i<npts; i++) {
        if (xpts[i] > 1.e20  ||  xpts[i] < -1.e20  ||
            ypts[i] > 1.e20  ||  ypts[i] < -1.e20) {
            continue;
        }
        if (xpts[i] < *cx1) *cx1 = xpts[i];
        if (ypts[i] < *cy1) *cy1 = ypts[i];
        if (xpts[i] > *cx2) *cx2 = xpts[i];
        if (ypts[i] > *cy2) *cy2 = ypts[i];
    }

    return;
}
