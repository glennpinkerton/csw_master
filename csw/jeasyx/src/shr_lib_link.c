
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

#include "csw/jeasyx/src/csw_jeasyx_src_JDisplayListBase.h"


void shr_lib_bootstrap(void);
void shr_lib_bootstrap(void) {

  /* 
   * This function is never called.  It only exists to fool the linker into
   * including libraries into the generated shared library.
   */

  Java_csw_jeasyx_src_JDisplayListBase_nativeDraw(
    NULL /* JNIEnv * */,
    NULL /* jobject */,
    0 /* dlist id */,
    0 /* threadid */
  );
}
