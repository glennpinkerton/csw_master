
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*

    csw_stread.cc

    This file has functions to read arrays of numbers from character
    strings.
    
*/

#include "csw/utils/include/csw_.h"

#define MAXLEN 2000


/*
******************************************************************

                c s w _ S t r F l o a t R e a d

******************************************************************

  function name:    csw_StrFloatRead    (int)

  call sequence:    csw_StrFloatRead (str, fout, nout)

  purpose:          Read an array of CSW_F numbers from a character
                    string.  The fout array must be large enough to hold all 
                    of the numbers.  No bounds checking is done.

  return value:     status code

                    0 = memory allocation error
                        (nout is not changed in this case)
                    1 = normal successful completion

  calling parameters:

    str       r    char*      string to read from
    fout      w    CSW_F*     CSW_F array to output to
    nout      w    int*       number of CSW_Fing point values output

*/

int csw_StrFloatRead (const char *str,
                      CSW_F *fout,
                      int *nout)
{
    int          len, i, nf, freeflag, count;
    CSW_F        *fptr;
    char         *tmp;
    char         local[MAXLEN], *work;

/*
    copy str to work string, since it will be destroyed
*/
    len = strlen (str);
    freeflag = 0;

    if (len >= MAXLEN) {
        work = (char *)csw_Malloc(len+1);
        if (!work) {
            return 0;
        }
        freeflag=1;
    }
    else {
        work = local;
    }

    strcpy(work, str);

/*
    convert white space to blanks
*/
    for (i=0; i<len; i++) {
        if (isspace((int)(work[i]))) {
            work[i] = ' ';
        }
    }

/*
    use strtok along with sscanf to convert the string
*/
    fptr = fout;
    count = 0;
    tmp = strtok(work, ", ");
    while (tmp) {
        nf = sscanf(tmp, "%lf", fptr);
        if (nf == 1) {
            fptr++;
            count++;
        }
        tmp = strtok(NULL, ", ");
    }

    *nout = count;

    if (freeflag) {
        csw_Free (work);
    }

    return 1;

}  /*  end of function csw_StrFloatRead */





/*
******************************************************************

                 c s w _ S t r I n t R e a d

******************************************************************

  function name:    csw_StrIntRead    (int)

  call sequence:    csw_StrIntRead (str, iout, nout)

  purpose:          Read an array of integer numbers from a character
                    string.  The iout array must be large enough to hold all 
                    of the numbers.  No bounds checking is done.

  return value:     status code

                    0 = memory allocation error
                        (nout is not changed in this case)
                    1 = normal successful completion

  calling parameters:

    str       r    char*      string to read from
    iout      w    int*       int array to output to
    nout      w    int*       number of CSW_Fing point values output

*/

int csw_StrIntRead (const char *str,
                    int *iout,
                    int *nout)
{
    int          len, i, nf, freeflag, count;
    int          *iptr;
    char         *tmp;
    char         local[MAXLEN], *work;

/*
    copy str to work string, since it will be destroyed
*/
    len = strlen (str);
    freeflag = 0;

    if (len >= MAXLEN) {
        work = (char *)csw_Malloc(len+1);
        if (!work) {
            return 0;
        }
        freeflag=1;
    }
    else {
        work = local;
    }

    strcpy(work, str);

/*
    convert white space to blanks
*/
    for (i=0; i<len; i++) {
        if (isspace((int)(work[i]))) {
            work[i] = ' ';
        }
    }

/*
    use strtok along with sscanf to convert the string
*/
    iptr = iout;
    count = 0;
    tmp = strtok(work, ", ");
    while (tmp) {
        nf = sscanf(tmp, "%d", iptr);
        if (nf == 1) {
            iptr++;
            count++;
        }
        tmp = strtok(NULL, ", ");
    }

    *nout = count;

    if (freeflag) {
        csw_Free (work);
    }

    return 1;

}  /*  end of function csw_StrIntRead */


/*
******************************************************************

                c s w _ S t r D o u b l e R e a d

******************************************************************

  function name:    csw_StrDoubleRead    (int)

  call sequence:    csw_StrDoubleRead (str, fout, nout)

  purpose:          Read an array of double numbers from a character
                    string.  The fout array must be large enough to hold all 
                    of the numbers.  No bounds checking is done.

  return value:     status code

                    0 = memory allocation error
                        (nout is not changed in this case)
                    1 = normal successful completion

  calling parameters:

    str       r    char*      string to read from
    fout      w    double*     CSW_F array to output to
    nout      w    int*       number of CSW_Fing point values output

*/

int csw_StrDoubleRead (const char *str,
                       double *fout,
                       int *nout)
{
    int          len, i, nf, freeflag, count;
    double       *fptr;
    char         *tmp;
    char         local[MAXLEN], *work;

/*
    copy str to work string, since it will be destroyed
*/
    len = strlen (str);
    freeflag = 0;

    if (len >= MAXLEN) {
        work = (char *)csw_Malloc(len+1);
        if (!work) {
            return 0;
        }
        freeflag=1;
    }
    else {
        work = local;
    }

    strcpy(work, str);

/*
    convert white space to blanks
*/
    for (i=0; i<len; i++) {
        if (isspace((int)(work[i]))) {
            work[i] = ' ';
        }
    }

/*
    use strtok along with sscanf to convert the string
*/
    fptr = fout;
    count = 0;
    tmp = strtok(work, ", ");
    while (tmp) {
        nf = sscanf(tmp, "%lf", fptr);
        if (nf == 1) {
            fptr++;
            count++;
        }
        tmp = strtok(NULL, ", ");
    }

    *nout = count;

    if (freeflag) {
        csw_Free (work);
    }

    return 1;

}  /*  end of function csw_StrDoubleRead */
