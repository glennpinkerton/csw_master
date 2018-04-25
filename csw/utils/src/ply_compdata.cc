
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_compdata.c  

      This file is a collection of functions to initialize, set and get
    information about which polygon components have been used in the polygon
    boolean operations.  These are only used internal to the polygon boolean
    operations, and should not be called in any other situation
*/

#include <stdlib.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/ply_compdata.h"


/*
  ************************************************************

                     p l y _ c o m p i n i t

  ************************************************************

  function name:    ply_compinit    (integer)

  call sequence:    ply_compinit (sval, cval)

  synopsis:         initialize arrays for polygon component tags

  return values:    status code

                    0 = normal successful completion
                    -1 = could not allocate memory

  description of calling parameters:

    sval    r   int         number of source components
    cval    r   int         number of clip components

*/

int CSWPolyCompdata::ply_compinit (int sval, int cval)
{
    int       i;

    if (noop) {
        return 0;
    }

/*  csw_Free old memory and allocate new  */

    ply_compfree ();
    comps = (int *) csw_Malloc (sval * sizeof(int     ));
    if(!comps) {
        return -1;
    }
    compc = (int *) csw_Malloc (cval * sizeof(int     ));
    if(!compc) {
        csw_Free (comps);
        comps = 0;
        return -1;
    }

/*  save number of components and initialize flags to zero  */

    ncomps = sval;
    ncompc = cval;

    for (i=0; i<ncomps; i++) {
        comps[i] = 0;
    }
    for (i=0; i<ncompc; i++) {
        compc[i] = 0;
    }

    return 0;

}  /*  end of function ply_compinit  */

/*

  ************************************************************

                      p l y _ c o m p f r e e

  ************************************************************

  function name:    ply_compfree   (integer)

  call sequence:    ply_compfree ()

  synopsis:         csw_Free memory allocated for polygon component flags

  return values:    always returns zero

*/

int CSWPolyCompdata::ply_compfree (void)
{
    if (noop) {
        return 0;
    }

    if (comps) {
        csw_Free (comps);
        comps = NULL;
    }
    if (compc) {
        csw_Free (compc);
        compc = NULL;
    }

    return 0;

}  /*  end of function ply_compfree  */



// An object of this class has no reason to be copied, moved
// copy constructed or move constructed.  As part of making
// this fairly foolproof, all the funcs to do these things are
// private and they all call this private method, which sets
// the internal pointers to NUll just as an original clean
// construction of the object does.

void CSWPolyCompdata::reset_for_copy (void)
{
    comps = NULL;
    compc = NULL;

    return;

}  /*  end of function reset_for_copy  */


/*

  ************************************************************

                      p l y _ c o m p f l a g

  ************************************************************

  function name:    ply_compflag    (integer)

  call sequence:    ply_compflag (flag)

  synopsis:         set source or clip flag for polygon components

  return values:    always returns zero

  description of calling parameters:

    flag    r    char    's' = source components will be flagged
                         'c' = clip components will be flagged

*/

int CSWPolyCompdata::ply_compflag (char flag)
{
    if (noop) {
        return 0;
    }

    if (flag == 's'  ||  flag == 'S') {
        ptype = 's';
    }
    else if (flag == 'c'  ||  flag == 'C') {
        ptype = 'c';
    }

    return 0;

}  /*  end of function ply_compflag  */


/*

  ************************************************************

                      p l y _ c o m p c h k

  ************************************************************

  function name:    ply_compchk    (integer)

  call sequence:    ply_compchk (val)

  synopsis:         check if a component has been used

  return values:    flag for whether component was used

                    0 = component not used yet
                    1 = component has been used

  description of calling parameters:

    val    r   int          Component number to check

*/

int CSWPolyCompdata::ply_compchk (int val)
{
    if (noop) {
        return 0;
    }

    if(ptype == 's') {
        return (comps[val]);
    }
    else {
        return (compc[val]);
    }

}  /*  end of function ply_compchk  */


/*

  ************************************************************

                    p l y _ c o m p s e t

  ************************************************************

  function name:    ply_compset    (integer)

  call sequence:    ply_compset (val)

  synopsis:         set component flag to 1 for used

  return values:    always returns zero

  description of calling parameters:

    val     r    int         component number to set

*/

int CSWPolyCompdata::ply_compset (int val)
{
    if (noop) {
        return 0;
    }

    if (ptype == 's') {
        comps[val] = 1;
    }
    else {
        compc[val] = 1;
    }

    return 0;

}  /*  end of function ply_compset  */


/*

  ************************************************************

                      p l y _ c o m p c l e a r

  ************************************************************

  function name:    ply_compclear    (integer)

  call sequence:    ply_compclear (val)

  synopsis:         reset a component to zero

  return values:    always returns zero

  description of calling parameters:

    val    r    int         component number to clear

*/

int CSWPolyCompdata::ply_compclear (int val)
{
    if (noop) {
        return 0;
    }

    if (ptype == 's') {
        comps[val] = 0;
    }
    else {
        compc[val] = 0;
    }

    return 0;

}  /*  end of function ply_compclear  */



/*
******************************************************************

                   p l y _ c o m p n o o p

******************************************************************

    set or clear the polygon component no operation flag
    if the flag is set, the component functions do nothing
*/

int CSWPolyCompdata::ply_compnoop (char cin)
{
    if (cin == 'y') {
        noop = 1;
    }
    else {
        noop = 0;
    }

    return 0;

}  /*  end of function ply_compnoop  */
