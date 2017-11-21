
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_heap.c

    This file has heapsort functions for various data types.

*/

#include "csw/utils/include/csw_.h"


/*
  ******************************************************

              c s w _ H e a p S o r t L o n g

  ******************************************************

  function name:    csw_HeapSortLong    (int)

  call sequence:    csw_HeapSortLong (array, npt)

  synopsis:         Sort an array of int     egers in ascending order.

  return values:    always returns zero

  calling parameters:

    array      int  *       Unsorted array of int     egers.
    npt        int          number of points to sort.

*/ 

int csw_HeapSortLong (int *array,
                      int npt)
{
    int         i, j, k, ir, n, q;

/*
    if less than 2 elements, no sort is needed
*/
    n = npt-1;
    if (n < 1) {
        return 0;
    }

    k = n/2+1;
    ir = n;

    while (ir >= 0) {
        if (k > 0) {
            k--;
            q = array[k];
        }
        else {
            q = array[ir];
            array[ir] = array[0];
            ir--;
            if (ir == 0) {
                array[0] = q;
                return 0;
            }
        }
        i = k;
        j = k + k;
        if (j < 1) j=1;
        while (j <= ir) {
            if (j < ir) {
                if (array[j] < array[j+1]) {
                    j++;
                }
            }
            if (q < array[j]) {
                array[i] = array[j];
                i = j;
                j = j + j;
                if (j < 1) j=1;
            }
            else {
                j = ir + 1;
            }
        }
        array[i] = q;

    }  /*  end of while(1) loop  */

    return 0;

} /*  end of function csw_HeapSortLong  */




/*
  ******************************************************

              c s w _ H e a p S o r t L o n g 2

  ******************************************************

  function name:    csw_HeapSortLong2    (int)

  call sequence:    csw_HeapSortLong2 (array, ptrs, npt)

  synopsis:         Sort an array of integers in ascending order.
                    Move an array of pointers along also.

  return values:    always returns zero

  calling parameters:

    array      int  *       Unsorted array of int     egers.
    ptrs       void**       Array of pointers to move with sorting.
    npt        int          number of points to sort.

*/ 

int csw_HeapSortLong2 (int  *array,
                       void **ptrs,
                       int npt)
{
    int         i, j, k, ir, n, q;
    void        *ptmp;

/*
    if less than 2 elements, no sort is needed
*/
    n = npt-1;
    if (n < 1) {
        return 0;
    }

    k = n/2+1;
    ir = n;

    while (ir >= 0) {
        if (k > 0) {
            k--;
            q = array[k];
            ptmp = ptrs[k];
        }
        else {
            q = array[ir];
            ptmp = ptrs[ir];
            array[ir] = array[0];
            ptrs[ir] = ptrs[0];
            ir--;
            if (ir == 0) {
                array[0] = q;
                ptrs[0] = ptmp;
                return 0;
            }
        }
        i = k;
        j = k + k;
        if (j < 1) j=1;
        while (j <= ir) {
            if (j < ir) {
                if (array[j] < array[j+1]) {
                    j++;
                }
            }
            if (q < array[j]) {
                array[i] = array[j];
                ptrs[i] = ptrs[j];
                i = j;
                j = j + j;
                if (j < 1) j=1;
            }
            else {
                j = ir + 1;
            }
        }
        array[i] = q;
        ptrs[i] = ptmp;

    }  /*  end of while(1) loop  */

    return 0;

} /*  end of function csw_HeapSortLong2  */




/*
  ******************************************************

          c s w _ H e a p S o r t F l o a t

  ******************************************************

  function name:    csw_HeapSortFloat    (int)

  call sequence:    csw_HeapSortFloat (array, npt)

  synopsis:         Sort an array of CSW_F numbers in ascending order.

  return values:    always returns zero

  calling parameters:

    array      CSW_F *      Unsorted array of CSW_F numbers.
    npt        int          number of points to sort.

*/ 

int csw_HeapSortFloat (CSW_F *array,
                       int npt)
{
    int         i, j, k, ir, n;
    CSW_F       q;

/*
    if less than 2 elements, no sort is needed
*/
    n = npt-1;
    if (n < 1) {
        return 0;
    }

    k = n/2+1;
    ir = n;

    while (ir >= 0) {
        if (k > 0) {
            k--;
            q = array[k];
        }
        else {
            q = array[ir];
            array[ir] = array[0];
            ir--;
            if (ir == 0) {
                array[0] = q;
                return 0;
            }
        }
        i = k;
        j = k + k;
        if (j < 1) j=1;
        while (j <= ir) {
            if (j < ir) {
                if (array[j] < array[j+1]) {
                    j++;
                }
            }
            if (q < array[j]) {
                array[i] = array[j];
                i = j;
                j = j + j;
                if (j < 1) j=1;
            }
            else {
                j = ir + 1;
            }
        }
        array[i] = q;

    }  /*  end of while(1) loop  */

    return 0;

} /*  end of function csw_HeapSortFloat  */




/*
  ******************************************************

         c s w _ H e a p S o r t F l o a t 2

  ******************************************************

  function name:    csw_HeapSortFloat2    (int)

  call sequence:    csw_HeapSortFloat2 (array, ptrs, npt)

  synopsis:         Sort an array of CSW_Fs in ascending order.
                    Move an associated array of pointers also.

  return values:    always returns zero

  calling parameters:

    array      CSW_F*       Unsorted array of CSW_F numbers.
    ptrs       void**       Array of pointers to move along with sorting.
    npt        int          number of points to sort.

*/ 

int csw_HeapSortFloat2 (CSW_F *array,
                        void **ptrs,
                        int npt)
{
    int         i, j, k, ir, n;
    CSW_F       q;
    void        *ptmp;

/*
    if less than 2 elements, no sort is needed
*/
    n = npt-1;
    if (n < 1) {
        return 0;
    }

    k = n/2+1;
    ir = n;

    while (ir >= 0) {
        if (k > 0) {
            k--;
            q = array[k];
            ptmp = ptrs[k];
        }
        else {
            q = array[ir];
            ptmp = ptrs[ir];
            array[ir] = array[0];
            ptrs[ir] = ptrs[0];
            ir--;
            if (ir == 0) {
                array[0] = q;
                ptrs[0] = ptmp;
                return 0;
            }
        }
        i = k;
        j = k + k;
        if (j < 1) j=1;
        while (j <= ir) {
            if (j < ir) {
                if (array[j] < array[j+1]) {
                    j++;
                }
            }
            if (q < array[j]) {
                array[i] = array[j];
                ptrs[i] = ptrs[j];
                i = j;
                j = j + j;
                if (j < 1) j=1;
            }
            else {
                j = ir + 1;
            }
        }
        array[i] = q;
        ptrs[i] = ptmp;

    }  /*  end of while(1) loop  */

    return 0;

} /*  end of function csw_HeapSortFloat2  */




/*
  ******************************************************

          c s w _ H e a p S o r t D o u b l e

  ******************************************************

  function name:    csw_HeapSortDouble    (int)

  call sequence:    csw_HeapSortDouble (array, npt)

  synopsis:         Sort an array of double numbers in ascending order.

  return values:    always returns zero

  calling parameters:

    array      double *     Unsorted array of double numbers.
    npt        int          number of points to sort.

*/ 

int csw_HeapSortDouble (double *array,
                        int npt)
{
    int         i, j, k, ir, n;
    double      q;

/*
    if less than 2 elements, no sort is needed
*/
    n = npt-1;
    if (n < 1) {
        return 0;
    }

    k = n/2+1;
    ir = n;

    while (ir >= 0) {
        if (k > 0) {
            k--;
            q = array[k];
        }
        else {
            q = array[ir];
            array[ir] = array[0];
            ir--;
            if (ir == 0) {
                array[0] = q;
                return 0;
            }
        }
        i = k;
        j = k + k;
        if (j < 1) j=1;
        while (j <= ir) {
            if (j < ir) {
                if (array[j] < array[j+1]) {
                    j++;
                }
            }
            if (q < array[j]) {
                array[i] = array[j];
                i = j;
                j = j + j;
                if (j < 1) j=1;
            }
            else {
                j = ir + 1;
            }
        }
        array[i] = q;

    }  /*  end of while(1) loop  */

    return 0;

} /*  end of function csw_HeapSortDouble  */




/*
  ******************************************************

         c s w _ H e a p S o r t D o u b l e 2

  ******************************************************

  function name:    csw_HeapSortDouble2    (int)

  call sequence:    csw_HeapSortDouble2 (array, ptrs, npt)

  synopsis:         Sort an array of doubles in ascending order.
                    Move an associated array of pointers also.

  return values:    always returns zero

  calling parameters:

    array      double*      Unsorted array of CSW_F numbers.
    ptrs       void**       Array of pointers to move along with sorting.
    npt        int          number of points to sort.

*/ 

int csw_HeapSortDouble2 (double *array,
                         void **ptrs,
                         int npt)
{
    int         i, j, k, ir, n;
    double      q;
    void        *ptmp;

/*
    if less than 2 elements, no sort is needed
*/
    n = npt-1;
    if (n < 1) {
        return 0;
    }

    k = n/2+1;
    ir = n;

    while (ir >= 0) {
        if (k > 0) {
            k--;
            q = array[k];
            ptmp = ptrs[k];
        }
        else {
            q = array[ir];
            ptmp = ptrs[ir];
            array[ir] = array[0];
            ptrs[ir] = ptrs[0];
            ir--;
            if (ir == 0) {
                array[0] = q;
                ptrs[0] = ptmp;
                return 0;
            }
        }
        i = k;
        j = k + k;
        if (j < 1) j=1;
        while (j <= ir) {
            if (j < ir) {
                if (array[j] < array[j+1]) {
                    j++;
                }
            }
            if (q < array[j]) {
                array[i] = array[j];
                ptrs[i] = ptrs[j];
                i = j;
                j = j + j;
                if (j < 1) j=1;
            }
            else {
                j = ir + 1;
            }
        }
        array[i] = q;
        ptrs[i] = ptmp;

    }  /*  end of while(1) loop  */

    return 0;

} /*  end of function csw_HeapSortDouble2  */
