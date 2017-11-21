
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_memmgt.h

    Make a c++ version of the csw_StackMalloc family of functions.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_calc.h.
#endif
*/



#ifndef CSW_MEMMGT_H
#define CSW_MEMMGT_H

int csw_FreeList (char **, int);


class CSWMemmgt
{

  public:

    CSWMemmgt () {};

// This destructor should free all memory allocated by the object
// even if csw_StackFree is not explicitly called.  This should
// deal with the case where an object is created on the stack and
// the creation scope is exited without csw_StackFree being called.
// Specifically, this destructor should clean up if an exception
// is thrown in the creation scope.
    ~CSWMemmgt () 
    {
        FreeAll ();
    };

  private:

    char         **PtrArray {NULL};
    int          Nptr {0};
    int          Maxptr {0};


  public:

    char *csw_StackMalloc (int);
    int csw_StackFree (void *);

    void FreeAll () {
        csw_FreeList (PtrArray, Nptr);
        free (PtrArray);
        PtrArray = NULL;
        Nptr = 0;
        Maxptr = 0;
    };

}; // End of main class definition

#endif
/*
    end of header file
    add nothing below this endif
*/
