
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_arith.h

    Define the CSWGrdArith class.  This class refactors the code in the
    old grd_apiP.h and grd_arith.c files.  An application should not use
    this class directly.  The CSWGridApi class (defined in csw/surfaceworks/
    include/grid_api.h header) should be used by an application.

*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file grd_arith.h.
#endif


/*
    Add nothing above this line
*/
#ifndef GRD_ARITH_H
#  define GRD_ARITH_H

#include <memory>

#include "csw/surfaceworks/include/grd_shared_structs.h"


/*
    Define constants for the class.
*/
#define MAX_COLUMNS          10000

class CSWGrdArith;

#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_utils.h"

class CSWGrdArith
{

  private:

    CSWGrdFault     *grd_fault_ptr = NULL;
    CSWGrdUtils     *grd_utils_ptr = NULL;

  public:

    CSWGrdArith () {};
    ~CSWGrdArith () {};

// It makes no sense to copy construct, move construct,
// assign or move assign an object of this class.  The
// various copy methods are flagged "delete" to prevent
// their use.

    CSWGrdArith (const CSWGrdArith &old) = delete;
    const CSWGrdArith &operator=(const CSWGrdArith &old) = delete;
    CSWGrdArith (CSWGrdArith &&old) = delete;
    const CSWGrdArith &operator=(CSWGrdArith &&old) = delete;

    void SetGrdFaultPtr (CSWGrdFault *p) {grd_fault_ptr = p;};
    void SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};

  private:

    GRidArithData   ArithData;

    int             IntersectGeometry (CSW_F, CSW_F, CSW_F, CSW_F, int, int,
                                       CSW_F, CSW_F, CSW_F, CSW_F, int, int,
                                       CSW_F*, CSW_F*, CSW_F*, CSW_F*, int*, int*);
    int             ResampleGrid (CSW_F*, char*, CSW_F, CSW_F, CSW_F, CSW_F, int, int,
                                  CSW_F*, char*, CSW_F, CSW_F, CSW_F, CSW_F,
                                  int, int, int);

    std::unique_ptr<CSW_F[]> x_maxc {new CSW_F[MAX_COLUMNS]};
    std::unique_ptr<CSW_F[]> y_maxc {new CSW_F[MAX_COLUMNS]};
//    CSW_F           x_maxc[MAX_COLUMNS];
//    CSW_F           y_maxc[MAX_COLUMNS];


  public:

    int grd_one_grid_arith (CSW_F*, CSW_F*,
                            int, int, int, CSW_F,
                            CSW_F, void(*)(GRidArithData *), void*);
    int grd_two_grid_arith (CSW_F*, char*,
                            CSW_F, CSW_F, CSW_F, CSW_F,
                            int, int,
                            FAultLineStruct*, int,
                            CSW_F*, char*,
                            CSW_F, CSW_F, CSW_F, CSW_F,
                            int, int,
                            FAultLineStruct*, int,
                            CSW_F,
                            CSW_F**, char**,
                            CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                            int*, int*, int, void(*)(GRidArithData *), void*);
    int grd_resample_grid (CSW_F*, char*, int, int,
                           CSW_F, CSW_F, CSW_F, CSW_F,
                           FAultLineStruct*, int,
                           CSW_F*, char*, int, int,
                           CSW_F, CSW_F, CSW_F, CSW_F,
                           int);
    int grd_back_interpolate (CSW_F*, int, int,
                              CSW_F, CSW_F, CSW_F, CSW_F,
                              FAultLineStruct*, int,
                              CSW_F*, CSW_F*, CSW_F*, int,
                              int);
    int grd_horizontal_gradient (CSW_F*, int, int,
                                 CSW_F, CSW_F, CSW_F, CSW_F,
                                 FAultLineStruct*, int,
                                 CSW_F*, CSW_F*);


};  // end of main class definition

/*
    Add nothing to this file below the following endif
*/
#endif
