
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    TextBounds.c

    This file isolates the functions to calculate width and possibly
    height of a text string.  For some fonts, this calculation is done
    using the Java font metrics methods.  For others, the calculation
    is done using the metrics of the easyx stroked fonts.
*/

#include <assert.h>

#include "csw/jeasyx/private_include/DisplayListJNI.h"
#include "csw/utils/private_include/TextBounds.h"

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/gpf_utils.h" 
#include "csw/utils/private_include/gpf_font.h" 

/*-----------------------------------------------------------------------------*/

static int (*JNIFunc)(int, const char*, int, float, float*) = NULL;

/*
 * This function must be called from the CreateNative code to make it
 * thread safe.  Only the first call will do anything.  Only one
 * text bounds jni function is allowed per JVM.
 */
void gtx_SetTextBoundsJNIFunction (int (*func)(int, const char*, int, float, float*))
{
    if (JNIFunc == NULL) {
        JNIFunc = func;
    }
}


/*
 * Return the height, width and ascent of the specified text, drawn at the
 * specified size and with the specified font.  The bounds return array must
 * be at least 3 elements long.
 */
void gtx_GetTextBounds (
    int        dlid,
    const char       *text,
    int        font_num,
    CSW_F      fsize,
    CSW_F      *bounds)
{
    int        istat = -1;

    bounds[0] = 0.0;
    bounds[1] = 0.0;
    bounds[2] = 0.0;

    if (text == NULL) {
        bounds[0] = 0.0;
        bounds[1] = 0.0;
        bounds[2] = 0.0;
        return;
    }

    float    fbounds[3];
    fbounds[0] = 0.0;
    fbounds[1] = 0.0;
    fbounds[2] = 0.0;

    float    ffsize = (float)fsize;
    
    if (font_num < 1000) {
        gpf_cctlen2 (
            text,
            strlen (text),
            font_num,
            fsize,
            bounds);
        bounds[1] = fsize;
        bounds[2] = 0;

        return;
    }

    else {
      if (dlid >= 0) {
        if (JNIFunc != NULL) {
            istat =
            (*JNIFunc) (
                dlid,
                text,
                font_num,
                ffsize,
                fbounds);
        }
        if (istat == -1) {
            bounds[0] = strlen(text) * fsize * .75f;
            bounds[1] = fsize;
            bounds[2] = 0;
        }
        else {
            bounds[0] = (double) (fbounds[0]);
            bounds[1] = (double) (fbounds[1]);
            bounds[2] = (double) (fbounds[2]);
        }
      }
      else {
        bounds[0] = strlen(text) * fsize * .75f;
        bounds[1] = fsize;
        bounds[2] = 0;
      }
    }

    return;

}
