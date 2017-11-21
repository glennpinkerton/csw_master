
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_errnum.h
*/



#ifndef CSW_ERRNUM_H
#define CSW_ERRNUM_H

class CSWErrNum
{

  private:

    int            ErrorNumber {0};

  public:

    CSWErrNum () {};

    void csw_SetErrNum (int en) {ErrorNumber = en;};
    int csw_ErrNum (void) {return ErrorNumber;};

}; // end of class definition

#endif
/*
    end of header file
    add nothing below this endif
*/
