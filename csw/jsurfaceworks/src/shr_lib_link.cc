
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/* ANSI C includes */
#include <stddef.h>

/* local includes */
#include "JSurfaceWorksBase.h"

/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
void dll_bootstrap(void);
void dll_bootstrap(void) {
  /* 
   * This function is never called.  It only exists to fool the linker into
   * including libraries into the generated shared library.
   */

  Java_csw_jsurfaceworks_src_JSurfaceWorksBase_bootMe(
    NULL /* JNIEnv * */,
    NULL /* jobject */
  );

}
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
