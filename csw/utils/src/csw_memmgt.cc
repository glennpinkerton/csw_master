
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
        free (plist[i]);
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
    
    if (ptr) {
        cptr = (void *)realloc (ptr, size);
    }
    else {
        cptr = (void *)malloc (size);
    }

    return cptr;

}  /*  end of function csw_realloc  */


void *csw_Malloc (int size)
{
    void *ptr = malloc (size);
    return ptr;
}


void *csw_Calloc (int size)
{
    if (size <= 0) {
        return NULL;
    }

    void *ptr = malloc (size);
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
}
