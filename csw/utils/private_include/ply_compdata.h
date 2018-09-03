
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_compdata.h

    Define the CSWPolyCompdata class.  This is a refactor of the
    old ply_compdata.c functions.  Hopefully these refactors hone
    my C ++ skills a bit and also, the code will hopefully become
    closer to thread safe.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_compdata.h.
#endif



#ifndef PLY_COMPDATA_H
#define PLY_COMPDATA_H

class CSWPolyCompdata
{

  private:

    int          *comps {NULL},
                 *compc {NULL};
    int          ncomps {0},
                 ncompc {0};
    char         ptype {' '};

    int          noop {0};


  public:

    CSWPolyCompdata () {};
    ~CSWPolyCompdata () {ply_compfree ();};

// It makes no sense to copy construct, move construct,
// assign or move assign an object of this class.  The
// various copy methods are flagged "delete" to prevent
// their use.

    CSWPolyCompdata (const CSWPolyCompdata &old) = delete;
    const CSWPolyCompdata &operator=(const CSWPolyCompdata &old) = delete;
    CSWPolyCompdata (CSWPolyCompdata &&old) = delete;
    const CSWPolyCompdata &operator=(CSWPolyCompdata &&old) = delete;

    int ply_compinit (int sval, int cval);
    int ply_compfree (void);
    int ply_compflag (char flag);
    int ply_compchk (int val);
    int ply_compset (int val);
    int ply_compclear (int val);
    int ply_compnoop (char cin);

  private:

    void reset_for_copy (void);


}; // end of main class definition


#endif
/*
    end of header file
    add nothing below this endif
*/
