
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/* 
    file csw_memmgt.c

      This file contains functions used to allocate and free groups 
    of pointers.  A stack of pointers is maintained and when a pointer
    on the stack is freed, all pointers which were placed on the stack
    after the specified pointer are also freed.
*/
  
#include <stdlib.h>
#include <stdio.h>

#include "csw/utils/include/csw_.h"


/*
    define constants for the file
*/

#define BLOCKSIZE 100


#ifdef _MEMCOUNT_
  static long int   MemTot = 0;
  static long int   MemAlloc = 0;
  static long int   MemFree = 0;
  static int   msize[500000];
  static void  *mptrs[500000];
  static int   maxptr = 0;
  static bool  init_done = false;
#endif


void csw_ShowMemCount ()
{
#ifdef _MEMCOUNT_
  printf ("\nMemTot = %ld\n", MemTot);
  printf ("MemAlloc = %ld\n", MemAlloc);
  printf ("MemFree = %ld\n\n", MemFree);
  fflush (stdout);
#endif
}

void csw_InitMemCount ()
{
#ifdef _MEMCOUNT_
  if (init_done) return;
  for (int i=0; i<500000; i++) {
    mptrs[i] = NULL;
    msize[i] = 0;
  }
  init_done = true;
#endif
}





#ifdef _MEMCOUNT_

static void AddPtr (void *vp, int size)
{
  if (vp == NULL  ||  size <= 0) return;
  if (!init_done) return;
  MemAlloc += size;
  MemTot += size;
  csw_ShowMemCount ();
  for (int i=0; i<maxptr; i++) {
    if (mptrs[i] == NULL) {
      mptrs[i] = vp;
      msize[i] = size;
      return;
    }
  }
  mptrs[maxptr] = vp;
  msize[maxptr] = size;
  maxptr++;    
  if (maxptr > 499999) {
    maxptr = 499999;
  }
}

static void DelPtr (void *vp)
{
  if (vp == NULL) return;
  if (!init_done) return;
  for (int i=0; i<maxptr; i++) {
    if (mptrs[i] == vp) {
      MemTot -= msize[i];
      MemFree += msize[i];
      mptrs[i] = NULL;
      msize[i] = 0;
      csw_ShowMemCount ();
      return;
    }
  }

  printf ("Cannot find pointer being csw_Freed\n");
  fflush (stdout);
}

#endif



/*
******************************************************************

                   c s w _ F r e e L i s t

******************************************************************

    free a list of pointers that were allocated with the malloc 
    system call.

*/

int csw_FreeList (char **plist,
                  int n)
{
    int      i;

    for (i=0; i<n; i++) {
        csw_Free (plist[i]);
    }

    return 0;

}  /*  end of function csw_FreeList  */




/*
  ****************************************************************

                    c s w _ r e a l l o c

  ****************************************************************

  This is a thin wrap around the realloc function.  Experience
  shows that not all reallocs will work if the existing pointer
  is NULL.  This function uses malloc if the existing pointer is
  NULL and uses realloc otherwise.

  No ANSI function definition is used here because the calling
  functions do not cast their pointers to char *.  The resulting
  plethora of warnings are not important and they can obscure
  more important warnings.

*/

void *csw_Realloc (void *ptr, int size)
{
    void      *cptr;

    if (size <= 0) return ptr;
    
    cptr = (void *)realloc (ptr, size);

#ifdef _MEMCOUNT_
    DelPtr (ptr);
    AddPtr (cptr, size);
#endif

    return cptr;

}  /*  end of function csw_realloc  */


void *csw_Malloc (int size)
{
    if (size <= 0) {
        return NULL;
    }

    void *ptr = malloc (size);

#ifdef _MEMCOUNT_
    AddPtr (ptr, size);
#endif

    return ptr;
}


void *csw_Calloc (int size)
{
    if (size <= 0) {
        return NULL;
    }

    void *ptr = csw_Malloc (size);
    if (ptr) {
        memset (ptr, 0, size);
    }

    return ptr;
}

void csw_Free (void *ptr)
{
    if (ptr != NULL) {
        free (ptr);
    }
#ifdef _MEMCOUNT_
    DelPtr (ptr);
#endif
}
