
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_.h

    This is the public header file for the csw_ function group.
    If any functions located in the src/csw directory are referenced
    by a module, the module should include this header file.
*/



#ifndef CSW__H
#define CSW__H

/*
    the csw_fgets prototype needs the FILE macro
    which is defined in stdio.h
*/
#   include <stdio.h>

/*
 * These files are included here because they are needed
 * by almost every csw utility which includes csw_.h
 */
#   include <string.h>
#   include <stdlib.h>

#   include <float.h>


/*
    The csw_hack.h file is used for prototypes of the functions
    that I had to isolate from system include files due to
    unworkable combinations of POSIX and ANSi, etc flags needed
    to get some system calls to compile without warnings.
*/
#   include "csw/utils/include/csw_hack.h"



/*
     Define the real data type here for all csw products.
     If double precision versions are needed, only these
     typedefs need to be changed.

   !!!!
     As of the 2017 refactoring, both CSW_F and CSW_Float MUST
     be set to double.  The flexibility implied in the preceding
     comment is not particularly useful and the code uses CSW_F,
     CSW_Float and double interchangeably in a few spots.
   !!!!
*/
    typedef double CSW_F;
    typedef double CSW_Float;

/*
    Define very near zero values for float and double.
*/
#ifdef FLT_EPSILON
#  define CSW_DOUBLE_EPSILON FLT_EPSILON
#  define CSW_FLOAT_EPSILON FLT_EPSILON
#else
#  define CSW_DOUBLE_EPSILON 1.e-6f
#  define CSW_FLOAT_EPSILON  1.e-6f
#endif

#ifdef FLT_MIN
#  define CSW_DOUBLE_ZERO FLT_MIN
#  define CSW_FLOAT_ZERO FLT_MIN
#else
#  define CSW_DOUBLE_ZERO 1.e-18f
#  define CSW_FLOAT_ZERO 1.e-18f
#endif




/*
 *  !!!! Never check in with the _MEMCOUNT_ defined.
 *       Always comment it out.
 */

//#define _MEMCOUNT_ 1

/*
    union for packing a 2 byte int into a
    2 byte character string
*/
typedef union {
    short int          i2;
    char               c2[2];
}  I2Equiv;

/*
    union for representing a CSW_F value as
    a 4 byte character string
*/
typedef union {
    CSW_F              fval;
    char               c4[4];
}  R4Equiv;

/*
    union for 4 byte int value.
*/
typedef union {
    int                i4;
    char               c4[4];
}  I4Equiv;

/*
    union for double value.
*/
typedef union {
    double             dval;
    int                ival[2];
    char               c8[8];
}  R8Equiv;

    /*
        extern C if C++
    */
#   if defined(c_plusplus) || defined(__cplusplus)
        extern "C" {
#   endif

    /*
     * Functions to get lines and fills for graphics.
     */
        int gpf_get_lines_for_text (char *text,
                                    int nc,
                                    double anglein,
                                    double sizein,
                                    int anchor,
                                    int ifontin,
                                    double *xline,
                                    double *yline,
                                    int *icline,
                                    int *nline,
                                    int maxpts,
                                    int maxcomp);
        
    /*
        functions from csw_filenam.cc
    */
        int csw_HomeFile (const char *, char *);
        int csw_SystemFile (const char *, char *);

    /*
        functions from csw_heap.c
    */
        int csw_HeapSortLong (int *, int);
        int csw_HeapSortLong2 (int *, void **, int);
        int csw_HeapSortFloat (CSW_F *, int);
        int csw_HeapSortFloat2 (CSW_F *, void **, int);
        int csw_HeapSortDouble (double *, int);
        int csw_HeapSortDouble2 (double *, void **, int);


        void *csw_Malloc (int size);
        void *csw_Realloc (void *p, int size);
        void *csw_Calloc (int size);
        void csw_Free (void *p);

        void csw_InitMemCount ();
        void csw_ShowMemCount ();


    /*
        functions from csw_stread
    */
        int csw_StrFloatRead (const char *, CSW_F *, int *);
        int csw_StrIntRead (const char *, int *, int *);
        int csw_StrDoubleRead (const char *, double *, int *);

    /*
        functions from csw_string.c
    */
        int csw_StrInsertCommas (char *text, int maxlen);
        int csw_StrLeftJust (char *);
        int csw_StrClean2 (char *, const char *, int);
        int csw_StrClean (char *, const char *, int);
        int csw_StrTruncate (char *, const char *, int);
        int csw_StrRightJust (char *);
        int csw_StrRightStrip (char *);
        int csw_strrightstrip_f_ (char *str);
        int csw_strrightstrip_f (char *str);
        char *csw_strstr (const char *, const char *);
        int csw_StrLowerCase (char *);
        int csw_StrUpperCase (char *);
        int csw_CharLowerCase (char *);
        int csw_CharUpperCase (char *);
        int csw_StrRemoveComment (char *);
        int csw_StrCompress (char *);
        int csw_StrOneBlank (char *);
        int csw_ValidName (const char *, int);
        int csw_SeparateLines (char *, char **, int *, int);
        int csw_InsertSubstring (char *, int, const char *, int, int);
        int csw_OverwriteSubstring (char *, int, const char *, int, int);
        int csw_FixupTabs (const char *, char *, int);
        int csw_LineChars (const char *);
        int csw_NewLineDelete (char *);
        int csw_NewLineConvert (char *);
        int csw_NewLineInsert (char *);
        int csw_SplitLeftRight (char*, char*, char*, const char*);
        char *csw_StringLeftMatch (char*, char**, int, int, int*);
        void csw_StrValidFileName (char *text, char ichar);

    /*
        functions from csw_utils.cc
    */
        int csw_do_nothing (void);
        int csw_GridZero (char *, int);
        int csw_UnzeroFloat (CSW_F *, double);
        char *csw_getenv (const char *);
        int csw_GetHostId (void);
        int csw_GetHostName (char *, int);
        int csw_DateText (char *);
        int csw_MinMaxFloat (CSW_F *, int, CSW_F *, CSW_F *);
        int csw_GetUserName (char *, int);
        int csw_RunCommand (const char *);
        char *csw_GetCurrentDir (void);
        char *csw_FullPathName (const char *);
        CSW_F csw_FloatRandom (int *);
        int csw_float_random_f_ (int*, CSW_F*);
        int csw_float_random_f (int*, CSW_F*);
        int csw_SameString (const char *, const char *, int);
        int csw_ErrMsg (const char *);
        int csw_ProcessDirectoryName (const char *, const char *, char *);
        CSW_F csw_FloatMod (CSW_F, CSW_F);
        int csw_CheckWildFloat (CSW_F *, CSW_F *, int, CSW_F);
        int csw_BaseName (const char *, char *);
        int csw_NearEqual (CSW_F, CSW_F, CSW_F);
        int csw_Seconds (void);
        int csw_FormatNumber (CSW_F, char *);
        char *csw_memcpy (void*, const void*, int);   /* void pointers are intentional */
        int csw_CheckRange (CSW_F*, int);
        int csw_CheckRangeZero (CSW_F*, int);
        int csw_CheckDoubleRange (double*, int);
        int csw_CheckIntRange (int*, int);
        int csw_CheckRange2 (CSW_F, CSW_F, CSW_F, CSW_F);
        int csw_PackDirectColor (int, int, int);
        int csw_UnpackDirectColor (int, int*, int*, int*);
        int csw_FitLine (CSW_F *, CSW_F *, int, CSW_F *, CSW_F *);
        int csw_FitDoubleLine (double *, double *, int, double *, double *);
        int csw_GetDoWrite (void);

    /*
        end of extern C if C++
    */
#   if defined(c_plusplus) || defined(__cplusplus)
        }
#   endif

/*
    Define constants for csw_ function calls
*/
#define CSW_NO_TIME_FORMAT     0
#define CSW_HOUR_MIN_SEC       1
#define CSW_MIN_SEC            2
#define CSW_HOUR_MIN           3
#define CSW_HOUR               4
#define CSW_MIN                5
#define CSW_DAY_HOUR_MIN       6
#define CSW_DAY_HOUR           7
#define CSW_DAY                8
#define CSW_MONTH_DATE         9
#define CSW_MONTH             10
#define CSW_YEAR_MONTH_DATE   11
#define CSW_YEAR_MONTH        12
#define CSW_YEAR              13
#define CSW_MONTH_DATE_YEAR   14
#define CSW_MONTH_YEAR        15
#define CSW_DATE              16
#define CSW_HOUR_MIN_DATE     17
#define CSW_JUL_DAY           18
#define CSW_JUL_DAY_YEAR      19
#define CSW_DATE_MONTH_YEAR   20
#define CSW_DATE_MONTH        21

#define CSW_FULL_DATE         99

#define CSW_USE_MONTH_NUMBER   1
#define CSW_USE_MONTH_NAME     2

#endif
/*
    end of header file
    add nothing below this endif
*/
