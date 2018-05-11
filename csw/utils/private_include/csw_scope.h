
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_scope.h
*/


/*
 *  This header defines the CSWScopeGuard class.
 *  
 *  The class is specifically designed for a single purpose,
 *  which is to provide a block of code to be executed when
 *  a stack scope is ended.  There is no intention for the
 *  CSWScopeGuard class to be used in any other fashion.
 *
 *  A simple example of using a lambda expression along with
 *  the CSWScopeGuard class for cleaning up is shown below.
 *  
 *  auto fscope = [&]()
 *  {
 *  };
 *  CSWScopeGuard  func_scope_guard (fscope);
 *
 */

#ifndef CSW_SCOPE_H
#define CSW_SCOPE_H

#include <functional>

class CSWScopeGuard
{

  private:

    std::function<void()>  func = NULL;

  public:

    CSWScopeGuard (std::function<void()> f) : func(f) {};

// When an instance of this class is created in some stack scope,
// the destructor is called when the scope ends for any reason.
// The code in the destructor will execute the block of code 
// specified by the func member.
    ~CSWScopeGuard ()
    {
        if (func != NULL) {
            func ();
        }
    };

//
// The copy constructor, assignment operator, move constructor
// and move assignment operator should never be called
//

    CSWScopeGuard (const CSWScopeGuard &other) = delete;
    const CSWScopeGuard &operator= (const CSWScopeGuard &other) = delete;
    CSWScopeGuard (CSWScopeGuard &&other)  = delete;
    const CSWScopeGuard &operator= (CSWScopeGuard &&other) = delete;

}; // end of class definition

#endif
/*
    end of header file
    add nothing below this endif
*/
