
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_font.h

    This is a private header file that has function prototypes for the 
    functions in file gpf_font.c.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_fontP.h.
#endif



#ifndef GPF_FONT_H
#define GPF_FONT_H



#define FONTINT             int
#define FIRSTCHAR           30
#define MINSIZERATIO        0.01f
#define LOCALMAXWORK        1000

#define DEGREE_CHAR         127

#define MIN_HERSHEY         101
#define MAX_HERSHEY         112

/*
    typedefs for structures
*/

typedef struct {
    int         offset, npts;
    char        ichar;
} VFontchar;

typedef struct {
    int         firstcomp,
                ncomp;
}  FIllFontChar;

typedef struct {
    int         firsthole,
                nholes;
}  FIllFontComp;

typedef struct {
    int         firstpt,
                npts;
}  FIllFontHole;

/*
    set up static variables
*/

#include "csw/utils/private_include/gpf_flistP.h"

/*

********    coordinate definitions for fonts    ********

*/                


#include "csw/utils/private_include/gpf_sfont.h"
#include "csw/utils/private_include/gpf_cfont.h"
#include "csw/utils/private_include/gpf_sfill.h"
#include "csw/utils/private_include/gpf_bfill.h"
#include "csw/utils/private_include/gpf_lefts.h"
#include "csw/utils/private_include/gpf_rights.h"
#include "csw/utils/private_include/gpf_leftb.h"
#include "csw/utils/private_include/gpf_rightb.h"

#include "csw/utils/private_include/gpf_hersh1.h"
#include "csw/utils/private_include/gpf_hersh2.h"
#include "csw/utils/private_include/gpf_hersh3.h"
#include "csw/utils/private_include/gpf_hersh4.h"
#include "csw/utils/private_include/gpf_hersh5.h"
#include "csw/utils/private_include/gpf_hersh6.h"
#include "csw/utils/private_include/gpf_hersh7.h"
#include "csw/utils/private_include/gpf_hersh8.h"
#include "csw/utils/private_include/gpf_hersh9.h"
#include "csw/utils/private_include/gpf_hersh10.h"
#include "csw/utils/private_include/gpf_hersh11.h"
#include "csw/utils/private_include/gpf_hersh12.h"


#include "csw/utils/private_include/gpf_flistP.h"

#if defined(__cplusplus)  ||  defined(c_plusplus)
extern "C" {
#endif
    
    int gpf_cctlen2 (const char *textin, int nc, int font,
                         CSW_F size, CSW_F *len);
#if defined(__cplusplus)  ||  defined(c_plusplus)
};
#endif


#if defined(__cplusplus)  ||  defined(c_plusplus)
class GPFFont
{

  private:

    int          FontCurve=0,
                 ShadowNumber = 0,
                 HersheyFlag = 0,
                 FontNumber=1;
    CSW_F        SlantX=0.0f,
                 SlantY=0.0f,
                 ShadowThickMult = 1.0f,
                 GapFraction = 0.2f;
    CSW_F        Xwork[LOCALMAXWORK],
                 Ywork[LOCALMAXWORK];
    CSW_F        Ox1 = 0.0, Oy1 = 0.0, Ox2 = 0.0, Oy2 = 0.0,
                 X1 = 0.0, Y1 = 0.0, X2 = 0.0, Y2 = 0.0;


  public:

    int gpf_setfont (int font);
    int gpf_GetFontNumber (int font);
    int gpf_getfontstrokes (char charin, CSW_F size, CSW_F *x, CSW_F *y,
                            int *parts, int *nparts, int maxxy, int maxparts);
    int gpf_getsimplefont (char charin, CSW_F size, CSW_F *x, CSW_F *y,
                           int *parts, int *nparts, int maxxy, int maxparts);
    int gpf_getcomplexfont (char charin, CSW_F size, CSW_F *x, CSW_F *y,
                            int *parts, int *nparts, int maxxy, int maxparts);
    int gpf_rotatechar (CSW_F *x, CSW_F *y, int n,
                        CSW_F cosa, CSW_F sina);
    int gpf_CheckForFontDot (CSW_F *x, CSW_F *y, CSW_F size);
    int gpf_ConnectFontStrokes (int *parts, int *nparts,
                                CSW_F *x, CSW_F *y, CSW_F size);
    int gpf_CharWidth (char ichar, CSW_F size, CSW_F *width);
    int gpf_fontxylim (CSW_F *x, CSW_F *y, int n,
                       CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int gpf_fontscalexy (CSW_F *x, CSW_F *y, int n,
                         CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                         CSW_F xx1, CSW_F yy1, CSW_F xx2, CSW_F yy2);
    int gpf_getpolyfontpts (char charin, CSW_F size,
                            CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole);
    int gpf_getsfillfont (char charin, CSW_F size,
                          CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole);
    int gpf_getfontshadow  (char charin, CSW_F size,
                            CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole);
    int gpf_getleftsfont (char charin, CSW_F size,
                          CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole);
    int gpf_getrightsfont (char charin, CSW_F size,
                           CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole);
    int gpf_getbfillfont (char charin, CSW_F size,
                          CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole);
    int gpf_getleftbfont (char charin, CSW_F size,
                          CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole);
    int gpf_getrightbfont (char charin, CSW_F size,
                           CSW_F *x, CSW_F *y, int *ncomp, int *icomp, int *ihole);
    int gpf_gethersheyfont (char charin, CSW_F size,
                            CSW_F *x, CSW_F *y, int *parts, int *nparts,
                            int maxxy, int maxparts);
    int gpf_TextLength2 (const char *textin, int nc, int font,
                         CSW_F size, CSW_F *len);

};
#endif


#endif
/*
    end of header file
    add nothing below this endif
*/
