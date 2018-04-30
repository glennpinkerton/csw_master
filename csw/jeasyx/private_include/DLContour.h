
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * This is the class definition for the DLContour class.  This class
 * is used to store contours that are calculated for grids or trimeshes.
 * A contour can also be directly added via the AddContour method.
 * This class is the only place where an actual contour line is drawn.
 */

#ifndef _DLCONTOUR_H
#define _DLCONTOUR_H

#include "csw/jeasyx/private_include/gtx_drawprim.h"

#include "csw/surfaceworks/include/contour_api.h"

#include "csw/utils/private_include/gpf_font.h"

typedef struct {
    int            showContours,
                   showColorFills,
                   showNodes,
                   showNodeValues,
                   showCellEdges,
                   showFaultLines;
    int            showOutline;
    int            isThickness;
    double         contourMinValue,
                   contourMaxValue,
                   contourInterval,
                   dataLogBase,
                   fillMinValue,
                   fillMaxValue;
    int            majorInterval;
    int            labelMajor,
                   labelMinor;
    int            tickMajor,
                   tickMinor;
    int            redMajor,
                   greenMajor,
                   blueMajor,
                   alphaMajor,
                   redMinor,
                   greenMinor,
                   blueMinor,
                   alphaMinor,
                   redNode,
                   greenNode,
                   blueNode,
                   alphaNode,
                   redNodeValue,
                   greenNodeValue,
                   blueNodeValue,
                   alphaNodeValue,
                   redCellEdge,
                   greenCellEdge,
                   blueCellEdge,
                   alphaCellEdge,
                   redFaultLine,
                   greenFaultLine,
                   blueFaultLine,
                   alphaFaultLine;
    int            redOutline,
                   greenOutline,
                   blueOutline,
                   alphaOutline;
    double         minorThickness,
                   majorThickness,
                   cellEdgeThickness,
                   faultThickness;
    int            contourFont,
                   nodeFont;
    double         minorLabelSize,
                   majorLabelSize,
                   minorLabelSpacing,
                   majorLabelSpacing,
                   minorTickLength,
                   majorTickLength,
                   minorTickSpacing,
                   majorTickSpacing;
    int            nodeSymbol;
    double         nodeSymbolSize,
                   nodeValueSize;
    double         hardMin,
                   hardMax;
    int            contourSmoothing;
    double         zUnitsConversionFactor;
}  DLContourProperties;


class DLContour {

  private:

    CSWContourApi       conapi_obj;
    GPFFont             gpf_font_obj;

    GTXDrawPrim         *gtx_drawprim_ptr = NULL;

  public:

    DLContour (GTXDrawPrim *dp);
    ~DLContour ();

// In the jni sheme of things, there is no use for copying, moving,
// etc of DLContour objects.  The methods to do copy sorts of stuff
// are made private to prevent copies from happening.

  private: 

    DLContour (const DLContour &other) {};
    const DLContour &operator= (const DLContour &other) {return *this;};
    DLContour (const DLContour &&other) {};
    const DLContour &operator= (const DLContour &&other) {return *this;};

  public:

    int                 grid_index;
    int                 image_id;

    int                 frame_num,
                        layer_num,
                        item_num,
                        selectable_object_num;

    int                 deleted_flag;
    int                 draw_flag;
    int                 visible_flag;

    int                 prim_num;

    int drawContour (COntourDrawOptions *draw_options,
                     DLContourProperties *dlprop,
                     void *vptr);
    int drawContourDirect (COntourDrawOptions *draw_options,
                     DLContourProperties *dlprop,
                     void *vptr);

    int SetCrec (COntourOutputRec *crec);
    COntourOutputRec *GetCrec (void);
    void GetContourPoints (CSW_F **xpts, CSW_F **ypts, int *npts);

    void CalcBounds (double *cx1, double *cy1, double *cx2, double *cy2);

    void SetInExtra (int ival);
    int  GetInExtra (void);

  private:

    int              in_extra;

    int calc_char_widths (char *text, CSW_F *widths, CSW_F size, int font);
    COntourOutputRec    *crec;
};

/*  do not add anything after this endif  */
#endif
