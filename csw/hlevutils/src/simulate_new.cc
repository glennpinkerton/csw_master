
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * This is meant to simulate an error from the new operator.
 *
 * Throw a bad alloc exception randomly.  This is called via the SNF
 * macro defined in simulate_new.h.  This is not "thread safe"
 * but since it is just throwing an exception randomly, I don't
 * care about thread safety.  Whatever thread the exception gets
 * thrown on needs to respond appropriately.
 */

#include <stdlib.h>
#include <time.h>
#include <exception>

#include  "csw/hlevutils/src/simulate_new.h"

void  simulate_new ()
{

// chamge this if 0 to if 1 to enable simulations of failures
// chamge back to zero to disable simulations of failures
#if 0
  static    bool  bfirst = true;

  if (bfirst) {
    srand (time(NULL));
    bfirst = false;
  }

  int ithrow = rand() % 10 + 1;

  if (ithrow < 3) {
    throw std::bad_alloc ();
  }
#endif

}
