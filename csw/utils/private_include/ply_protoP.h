
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_protoP.h

    This is a private header file that has function prototypes for the
    functions found in the ply_ C files.  These functions do not need any
    structure or type definitions other than CSW_F, so I group all of the
    prototypes into this single file.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_protoP.h.
#endif



#ifndef PLY_PROTO_P_H
#define PLY_PROTO_P_H

#include "csw/utils/include/csw_errnum.h"
#include "csw/utils/include/csw_timer.h"

#define PLY_INTERSECT              1
#define PLY_UNION                  2
#define PLY_XOR                    3
#define PLY_FRAGMENT               4

#endif
/*
    end of header file
    add nothing below this endif
*/
