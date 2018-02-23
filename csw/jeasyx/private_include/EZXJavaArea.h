
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    EZXJavaArea.h

    Prototypes for functions to send data back to the java side
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ezx_javaP.h.
#endif

/*  add nothing above this ifdef  */

#ifndef _EZX_JAVA_AREA_H_
#define _EZX_JAVA_AREA_H_

#include <stdio.h>

#include "csw/utils/include/csw_.h"

#include "csw/jeasyx/private_include/gpf_fillP.h"
#include "csw/jeasyx/private_include/gpf_linePP.h"
#include "csw/jeasyx/private_include/gpf_shapeP.h"
#include "csw/jeasyx/private_include/gpf_symbPP.h"
#include "csw/jeasyx/private_include/gpf_textP.h"
#include "csw/jeasyx/private_include/gtx_frinfoP.h"



class EZXJavaArea
{

  private:

    static const int   MAX_FLOAT_CONVERT = 1000;

  public:

    EZXJavaArea () {};    
    ~EZXJavaArea () {};    

  private:

    int DrawingPriority = 0;
    int CurrentFrame = -1;
    int AlphaValue = 255;
    int ScreenDPI = 72;

    int ImageID = -1;

    FILE    *DebugFile = NULL;
    char    Lbuff[1000];

    void    *v_jenv = NULL;
    void    *v_jobj = NULL;

    float     _p_fxy[2 * MAX_FLOAT_CONVERT];

    int ConvertToFloat (CSW_F *xy, int npts, float **fxy);

  public:

    void SetJNIPtrs (void *vp1, void *vp2) {
        v_jenv = vp1;
        v_jobj = vp2;
    }

    void ezx_SetScreenDPI (int ival);
    void ezx_SetAlphaValue (int ival);
    
    int ezx_SetDrawingPriority (int ival);
    
    int ezx_SetFrameInJavaArea (int frame_num);
    
    void ezx_SetImageIDInJavaArea (int id);
    
    int ezx_AppendFrameToJavaArea (
        CSW_F xmin,
        CSW_F ymin,
        CSW_F xmax,
        CSW_F ymax,
        double fx1,
        double fy1,
        double fx2,
        double fy2,
        int   borderflag,
        int   clipflag,
        int   scaleable,
        int   scale_to_attach_frame,
        int   frame_num,
        char  *fname
    );
    
    int ezx_AppendFillToJavaArea (
        CSW_F *xy,
        CSW_F thickness,
        CSW_F patscale,
        int   npts,
        int   red,
        int   green,
        int   blue,
        int   pattern,
        int   selectable
    );
    
    int ezx_AppendLineToJavaArea (
        CSW_F *xy,
        int   npts,
        int   red,
        int   green,
        int   blue,
        int   pattern,
        CSW_F thick,
        int   image_id,
        int   selectable
    );
    
    int ezx_AppendTextToJavaArea (
        CSW_F scaled_x, 
        CSW_F scaled_y, 
        char  *text, 
        int   red, 
        int   green, 
        int   blue,
        CSW_F angle, 
        CSW_F size, 
        int   font,
        int   image_id,
        int   selectable
    );
    
    int ezx_AppendArcToJavaArea (
        CSW_F scaled_x, 
        CSW_F scaled_y, 
        CSW_F scaled_r1, 
        CSW_F scaled_r2,
        CSW_F ang1, 
        CSW_F ang2, 
        int closure,
        int red, 
        int green, 
        int blue, 
        CSW_F thick, 
        CSW_F angle,
        int   selectable
    );
    
    int ezx_AppendFilledArcToJavaArea (
        CSW_F scaled_x, 
        CSW_F scaled_y, 
        CSW_F scaled_r1, 
        CSW_F scaled_r2,
        CSW_F ang1, 
        CSW_F ang2, 
        int closure,
        int red, 
        int green, 
        int blue, 
        CSW_F thickness,
        CSW_F angle,
        int pattern,
        int   selectable
    );
    
    int ezx_AppendImageToJavaArea (
        CSW_F scaled_x1,
        CSW_F scaled_y1,
        CSW_F scaled_x2,
        CSW_F scaled_y2,
        int   ncol,
        int   nrow,
        unsigned char *red,
        unsigned char *green,
        unsigned char *blue,
        unsigned char *trans,
        int   has_lines,
        int   image_id,
        int   selectable
    );
    
    int ezx_AppendSelectedFillToJavaArea (
        FIllPrim       *fptr,
        char           *frame_name,
        char           *layer_name,
        char           *item_name
    );
    
    int ezx_AppendSelectedLineToJavaArea (
        LInePrim       *lptr,
        char           *frame_name,
        char           *layer_name,
        char           *item_name
    );
    
    int ezx_AppendSelectedTextToJavaArea (
        TExtPrim       *tptr,
        char           *frame_name,
        char           *layer_name,
        char           *item_name
    );
    
    int ezx_AppendSelectedSymbToJavaArea (
        SYmbPrim       *sptr,
        char           *frame_name,
        char           *layer_name,
        char           *item_name,
        char           *surface_name
    );
    
    int ezx_AppendSelectedArcToJavaArea (
        SHapePrim      *aptr,
        char           *frame_name,
        char           *layer_name,
        char           *item_name
    );
    
    int ezx_AppendSelectedRectangleToJavaArea (
        SHapePrim      *rptr,
        char           *frame_name,
        char           *layer_name,
        char           *item_name
    );
    
    int ezx_AppendSelectedContourToJavaArea (
        void           *cptr,
        char           *frame_name,
        char           *layer_name,
        char           *item_name,
        char           *surface_name,
        void           *dlprop
    );
    
    int ezx_AppendSelectedAxisToJavaArea (
        AXisPrim       *aptr,
        char           *frame_name,
        char           *layer_name,
        char           *item_name
    );
    
}; // end of main class
    
#endif
    
/*  add nothing below this endif  */
