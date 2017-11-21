
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/* 
    file csw_memmgt_class.cc

    This file has the bodies of the methods defined in the CSWMemmgt
    class.  Eventually, this class should replace the stand alone
    functions originally found in csw_memmgt.c
*/
  
#include <stdlib.h>
#include <stdio.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_memmgt.h"


/*
    define constants for the file
*/

#define BLOCKSIZE 100



/*
******************************************************************

                c s w _ S t a c k M a l l o c

******************************************************************

  function name:    csw_StackMalloc    (char *)

  call sequence:    pointer = csw_StackMalloc (nbytes)

  purpose:          Allocate memory and save on a stack.

  return value:     Pointer to memory allocated or NULL if error.

  calling parameters:

    nbytes    r    int         number of bytes to allocate

*/

char* CSWMemmgt::csw_StackMalloc (int nbytes)
{
    char       *tmp;

/*
    expand the stack if needed
*/
    if (Nptr >= Maxptr) {
        Maxptr += BLOCKSIZE;
        if (PtrArray) {
            PtrArray = (char **)csw_Realloc (PtrArray, Maxptr * sizeof(char *));
        }
        else {
            PtrArray = (char **)csw_Malloc (Maxptr * sizeof(char *));
        }
        if (!PtrArray) {
            return NULL;
        }
    }

/*
    Allocate memory as called and place the pointer
    on the stack if csw_Malloc succeeds.
*/
    tmp = (char *)csw_Malloc(nbytes);
    if(!tmp) {
        return NULL;
    }
    
    PtrArray[Nptr] = tmp;
    Nptr++;

    return (char *) tmp;

}  /*  end of csw_StackMalloc function  */





/*
******************************************************************

                  c s w _ S t a c k F r e e

******************************************************************

  function name:    csw_StackFree    (integer)

  call sequence:    csw_StackFree(pointer)

  purpose:          Free all memory allocated since pointer was
                    allocated, including pointer memory.  The stack
                    of pointers is scanned for a match to the specified
                    pointer.  If no match is found, the function does
                    nothing.  If a match is found, all pointers from 
                    the end of the stack to the match, including the
                    match, are removed from the stack and csw_Freed.

  return value:     always returns zero

  calling parameters:

    pointer    r    char *    Pointer to memory allocated with csw_StackMalloc.
                              If a NULL is specified, the entire stack is
                              cleaned.

*/

int CSWMemmgt::csw_StackFree(void *pmem)
{
    int         nt;
    char        *tmp;

    if(Nptr == 0) return 0;

/*  
    if pmem is not NULL, make sure there is a match in the stack
*/
    if(pmem != NULL) {
        nt = Nptr;
        while(nt > 0) {
            nt--;
            if(PtrArray[nt] == pmem) {
                nt = Nptr;
                break;
            }
        }

    /*
        no match found
    */
        if(nt != Nptr) return 0;

    }

/*  
    csw_Free memory until match
*/
    while (Nptr > 0) {
        Nptr--;
        tmp = PtrArray[Nptr];
        if(tmp) {
            if(pmem  &&  tmp == pmem) {
                csw_Free (tmp);
                break;
            }
            csw_Free (tmp);
        }
    }

    return 0;

}  /*  end of csw_StackFree function  */    





