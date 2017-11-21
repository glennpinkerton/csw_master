
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_ldash.c

    This file has functions used to manage dashed line data
    for the easyx plotting functions.  These functions can be
    used to store and retrieve dash patterns.

    These functions are now methods of the GPFLdash class.
*/

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/gpf_ldash.h"


/*
  ****************************************************************

            g p f _ g e t s e r v e r d a s h d a t a

  ****************************************************************

    Return the dash line elements for a pattern number.  On success,
  status is 1.  On failure, status is zero.

*/

int GPFLdash::gpf_getserverdashdata (int ipat, int *ndash, CSW_F *fdash)
{
    int          i, n;

    if (ipat < 1  ||  ipat > MAXDASHPAT) {
        *ndash = 0;
        return 0;
    }

    i = ipat -1;
    if (DashData[i].ndash < 2) {
        *ndash = 0;
        return 0;
    }

    *ndash = DashData[i].ndash;
    for (n=0; n<*ndash; n++) {
        fdash[n] = DashData[i].dash[n];
    }

    return 1;

}  /*  end of function gpf_getserverdashdata  */



/*
  ****************************************************************

            g p f _ s e t s e r v e r d a s h d a t a

  ****************************************************************

    Set the dash line move draw elements for a pattern number.
    The number of dash elements must be even.

*/

int GPFLdash::gpf_setserverdashdata (int ipat, int ndash, CSW_F *fdash)
{
    int          i, n;

    if (ndash % 2  !=  0) {
        return -1;
    }

    if (ipat < 1  ||  ipat > MAXDASHPAT) {
        return -1;
    }

    i = ipat -1;
    DashData[i].ndash = ndash;
    for (n=0; n<ndash; n++) {
        DashData[i].dash[n] = fdash[n];
    }

    return 1;

}  /*  end of function gpf_setserverdashdata  */



/*
  ****************************************************************

           g p f _ i n i t s e r v e r d a s h d a t a

  ****************************************************************

    Initialize to hard coded dahed lines.

*/

int GPFLdash::gpf_initserverdashdata (void)
{
    int      i, n, j;
    CSW_F    fdash[10];
    const  CSW_F *ftmp = NULL;

/*
    first zero everything
*/

    for (i=0; i<MAXDASHPAT; i++) {
        DashData[i].ndash = 0;
        for (j=0; j<MAXDASHPARTS; j++) {
            DashData[i].dash[j]= 0.0f;
        }
    }

/*
    setup with the data from the Idash and Fdash arrays
*/

    ftmp = Fdash;
    for (i=0; i<NDEFAULTDASH; i++) {
        n = Idash[i];
        for (j=0; j<n; j++) {
            fdash[j] = *ftmp;
            ftmp++;
        }
        gpf_setserverdashdata (i+1, n, fdash);
    }

    return 1;

}  /*  end of function gpf_initserverdashdata  */
