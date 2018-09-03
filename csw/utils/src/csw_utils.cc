
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_utils.cc

    This file has a hodgepodge of general utility functions
    called by all other parts of the EasyX software.  Some of
    these functions act as interfaces to system functions.  The
    utilities are all put here so that they can be ported more
    easily to different system implementations.  Other functions
    are just general utilities that are used from many different
    places.
*/


/*
    system header files needed
*/
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef SOLARIS
#undef _POSIX_SOURCE
#endif
#include <time.h>
#ifdef SOLARIS
#define _POSIX_SOURCE
#endif

#include <math.h>

/*
    system headers needed for windows NT compile
*/
#ifdef WINNT
#  pragma warning(disable:4115)
/*#  include <windows.h>*/
#  pragma warning(default:4115)
#  include <direct.h>

#else

/*
    System headers needed for unix ansi compile.

    The unistd.h file is including sys/time.h when POSIX
    is defined.  This produces warnings under Solaris 2.4,
    so I undef POSIX prior to including unistd.h.
*/
#ifdef SOLARIS
#undef _POSIX_SOURCE
#endif
#  include <unistd.h>
#ifdef SOLARIS
#define _POSIX_SOURCE
#endif

/*
    The systeminfo.h file is only used in conjunction
    with the sysinfo call, which is only used on SOLARIS.
*/
#ifdef SOLARIS
#  include <sys/systeminfo.h>
#endif

#endif  /*  end of unix header block  */


/*
    private header files.
*/
#include "csw/utils/include/csw_.h"

/*
    redefine some function names for the windows NT compile
*/
#ifdef WINNT

#  ifndef stat
#    define chdir _chdir
#  endif

#  ifndef mkdir
#    define getcwd _getcwd
#  endif

#endif


/*
    static file functions
*/
static int            FormatNumber (CSW_F, CSW_F, char*);
static void           CleanupZeros (char*);
static void           CleanupNines (char*);

/*
    Declare function prototypes for UNIX system functions
    that are not in their proper include files (or more
    likely not being defined because of flags in the
    system include files).

    This is skipped for NT compiles
*/
#ifndef WINNT

  /*
      If a C++ compiler is running, all of the prototypes
      must be declared extern "C".
  */
#    if defined(__cplusplus)  ||  defined(c_plusplus)
        extern "C"
        {
#    endif

#  ifdef SOLARIS
    char                  *realpath (char*, char*);
#  endif
    long                  gethostid (void);

  /*
      The extern "C" block needs to be closed if this
      is being run through a C++ compiler.
  */
#    if defined (__cplusplus)  ||  defined (c_plusplus)
        }
#    endif

#endif

/*
  ****************************************************************

                   c s w _ C h e c k R a n g e

  ****************************************************************

    Given an array of CSW_F numbers, check if the range of the list
  is large enough relative to the mid point value of the list to allow
  the list to be manipulated with CSW_Float precision arithmetic.  If
  the range can work with CSW_F precision, 1 is returned.  If not,
  zero is returned.  If the pointer to the values is NULL, -1 is
  returned.

*/

int csw_CheckRange (CSW_F *vlist, int npts)
{
    int           i;
    double        v1, v2, range, vmid;

    if (vlist == NULL) return -1;

    v1 = 1.e30f;
    v2 = -1.e30f;
    for (i=0; i<npts; i++) {
        if ((double)vlist[i] < v1) v1 = (double)vlist[i];
        if ((double)vlist[i] > v2) v2 = (double)vlist[i];
    }
    range = v2 - v1;
    vmid = (v1 + v2) / 2.0;
    if (vmid < 0.0) vmid = -vmid;

    if (vmid > 50000.0 * range  &&  range > 0.0) {
        return 0;
    }

    return 1;

}  /*  end of function csw_CheckRange  */






/*
  ****************************************************************

             c s w _ C h e c k R a n g e Z e r o

  ****************************************************************

    Given an array of CSW_F numbers, check if the range of the list
  is large enough relative to the mid point value of the list to allow
  the list to be manipulated with CSW_Float precision arithmetic.  If
  the range can work with CSW_F precision, 1 is returned.  If not,
  zero is returned.  If the pointer to the values is NULL, -1 is
  returned.

    In this version, if the range is zero, zero is always returned.

*/

int csw_CheckRangeZero (CSW_F *vlist, int npts)
{
    int           i;
    double        v1, v2, range, vmid;

    if (vlist == NULL) return -1;

    v1 = 1.e30;
    v2 = -1.e30;
    for (i=0; i<npts; i++) {
        if ((double)vlist[i] < v1) v1 = (double)vlist[i];
        if ((double)vlist[i] > v2) v2 = (double)vlist[i];
    }
    range = v2 - v1;
    vmid = (v1 + v2) / 2.0;
    if (vmid < 0.0) vmid = -vmid;

    if (vmid >= 50000.0 * range) {
        return 0;
    }

    return 1;

}  /*  end of function csw_CheckRangeZero  */




/*
  ****************************************************************

                 c s w _ C h e c k R a n g e 2

  ****************************************************************

    See if the corner point values are consistent with CSW_Float
  precision arithmetic.

*/

int csw_CheckRange2 (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2)
{
    double        v1, v2, range, vmid;

    v1 = x1;
    v2 = x2;
    range = v2 - v1;
    vmid = (v1 + v2) / 2.0f;
    if (vmid < 0.0f) vmid = -vmid;
    if (vmid > 50000.0f * range  &&  range > 0.0) {
        return 0;
    }

    v1 = y1;
    v2 = y2;
    range = v2 - v1;
    vmid = (v1 + v2) / 2.0f;
    if (vmid < 0.0f) vmid = -vmid;
    if (vmid > 50000.0f * range  &&  range > 0.0) {
        return 0;
    }

    return 1;

}  /*  end of function csw_CheckRange2  */





/*

  *****************************************************************

                      c s w _ G r i d Z e r o

  *****************************************************************

  function name:    csw_GridZero   (integer)

  call sequence:    csw_GridZero (grid, nbytes)

  synopsis:         Initialize an array to all zeros

  return values:    always returns zero

  usage:            This is a general utility called from several
                    spots to initialize arrays.

  calling parameters:

    grid    w    long int *    Pointer to the array to be initialized
    nbytes  r    long int      Number of bytes in grid to initialize.

*/

int csw_GridZero (char *grid,
                  int nbytes)
{

    memset (grid, 0, nbytes);

    return 0;

}  /*  end of csw_GridZero function  */



/*
  ****************************************************************

                 c s w _ U n z e r o F l o a t

  ****************************************************************

    convert a near zero CSW_F number to a positive or negative value
    corresponding to the tiny value passed.

*/

int csw_UnzeroFloat (CSW_F *val,
                     double tiny)
{

    if (*val > -tiny  &&  *val < tiny) {
        if (*val < 0.0f) {
            *val = (CSW_F)-tiny;
        }
        else {
            *val = (CSW_F)tiny;
        }
    }

    return 1;

}  /*  end of function csw_UnzeroFloat  */



/*
  ****************************************************************

                        c s w _ g e t e n v

  ****************************************************************

    Wrapper around the system getenv function.

*/

char *csw_getenv (const char *text)
{
    char      *ctmp;

    if (!text) {
        return NULL;
    }
    if (text[0] == '\0') {
        return NULL;
    }

    ctmp = (char *)getenv (text);

    return ctmp;

}  /*  end of function csw_getenv  */



/*
  ****************************************************************

                   c s w _ G e t H o s t I d

  ****************************************************************

    Return the 32 bit identifier for the current host or zero
  if the current host is not networked.  For Windows NT, this always
  returns zero.

*/

int csw_GetHostId (void)
{

#ifdef WINNT

    return 0;

#else

/*
    There are various ways for flavors of UNIX to report a host id.
    The C preprocessor logic here chooses the proper method for the
    flavor of UNIX we are compiling on.
*/
#  ifdef hpux
    int         i, n, len;
#  endif
    char        buf[128];

/*
    Set the buffer to all zeros because hp doesn't null
    terminate the host name in its gethostname function.
*/
    memset (buf, 0, 128);

#ifdef TCP_IP_FLAG

  /*
      For HP, there is no gethostid, so hash the
      characters in the host name to get an integer id.
  */
#    ifdef hpux
         csw_gethostname (buf, (size_t)128);
         n = 0;
         len = strlen(buf);
         for (i=0; i<len; i++) {
             n = n + i * buf[i];
         }
         return n;
#    endif

  /*
      For Solaris, use the sysinfo function.
  */
#    ifdef SOLARIS
         sysinfo (SI_HW_SERIAL, buf, 128);
         return (atoi (buf));

  /*
      For other supported platforms, use gethostid.
  */
#    else
         return gethostid ();
#    endif

#  endif



#  ifndef TCP_IP_FLAG
        return 0;
#  endif

#endif  /*  end of else block from WINNT check (i.e. end of UNIX code)  */

}  /*  end of function csw_GetHostId  */



/*
  ****************************************************************

                 c s w _ G e t H o s t N a m e

  ****************************************************************

    Return the name of the local host.  If the local host is not
  networked, return a null string.  The host name is returned in
  the name string.  The namelen string specifies the size of the
  name buffer.  If the actual host name is longer than namelen - 1
  characters, it is truncated to namelen - 1 characters.  The
  returned name will always be null terminated, regardless of
  whether or not it was truncated.

    If there is an error in the input parameters (null name string
  or namelen less than 2) or if the host is not networked, the
  return status is -1.  On success, the return status is 1.

*/

int csw_GetHostName (char *name,
                     int namelen)
{

/*
    There is a potential problem with retrieving the host name
    in NT.  The gethostname function can initiate a network
    connection in some cases.  Since the host name is only used
    as an identifier and not for actual network transfers, the
    host name is set to a null string for NT.  This means that
    multiple instances of the EasyX server cannot be run on an
    NT system.  Since EasyX doesn't run on NT yet, this is no
    great loss.
*/

#ifdef WINNT

    namelen = namelen;
    name[0] = '\0';
    return 1;

#else


/*
    The remainder of the function gets the host name
    for various UNIX systems.
*/

#ifdef TCP_IP_FLAG
    char        local[200];
    int         istat;
#endif

/*
    There are some differences in getting the host name
    on various UNIX platforms.
*/
#  ifdef TCP_IP_FLAG

#    ifdef SOLARIS
       istat = sysinfo (SI_HOSTNAME, local, 199);
       if (istat == -1) {
           name[0] = '\0';
           return -1;
       }
#    else
       istat = csw_gethostname (local, 199);
       if (istat != 0) {
           name[0] = '\0';
           return -1;
       }
#    endif

    csw_StrClean2 (name, local, namelen);
    return 1;
#  endif

#  ifndef TCP_IP_FLAG
    name[0] = '\0';
    namelen = namelen;   /*  to suppress warning  */
    return -1;
#  endif


#endif   /*  end of #else block from WINNT ifdef  */

}  /*  end of function csw_GetHostName  */





/*
  ******************************************************

                c s w _ D a t e T e x t

  ******************************************************

  function name:    csw_DateText     (int)

  call sequence:    csw_DateText (tname)

  synopsis:         return a string with the decimal month, day, and year

  return values:    status code

                    0 = error getting the time
                    1 = normal successful completion

  calling parameters:

    tname     w    char*   character string to write the date into
                           this should be at least 11 characters

*/

int csw_DateText (char *tname)
{
    char              *t1;
    int               i;
    struct tm         *tmstruct;
    time_t            seconds;

    tname[0] = '\0';

/*
    first get seconds since the time origin
*/
    seconds = time(NULL);
    if (seconds == -1) {
        return 0;
    }

/*
    Convert seconds into a time structure.
*/
    tmstruct = localtime(&seconds);
    if (!tmstruct) {
        return 0;
    }

/*
    Write the numbers for the date into the string.
*/
    t1 = tname;
    i = tmstruct->tm_mon + 1;
    sprintf(t1, "%02d", i);
    strcat (tname, "/");

    t1 += 3;
    sprintf(t1, "%02d", tmstruct->tm_mday);
    strcat (tname, "/");

    t1 += 3;
    i = tmstruct->tm_year + 1900;
    sprintf(t1, "%04d", i);

    return 1;

}  /*  end of function csw_DateText  */





/*
  ****************************************************************

                 c s w _ M i n M a x F l o a t

  ****************************************************************

  function name:    csw_MinMaxFloat       (int)

  call sequence:    csw_MinMaxFloat (list, n, min, max)

  purpose:          Find min and max of a list of CSW_F numbers.

  return value:     Returns -1 if n is less than 1, or returns
                    1 on success.

  calling parameters:

    list       r    CSW_F*     Array of CSW_F numbers.
    n          r    int        number of CSW_F numbers
    min        w    CSW_F*     minimum of the list
    max        w    CSW_F*     maximum of the list

  errors:

*/

int csw_MinMaxFloat (CSW_F *list,
                     int n,
                     CSW_F *min,
                     CSW_F *max)
{
    CSW_F          t1, t2;
    int            i;

    if (n < 1) {
        return -1;
    }

    t1 = 1.e30f;
    t2 = -1.e30f;

    for (i=0; i<n; i++) {
        if (list[i] < t1) t1 = list[i];
        if (list[i] > t2) t2 = list[i];
    }

    *min = t1;
    *max = t2;

    return 1;

}  /*  end of function csw_MinMaxFloat  */



/*
  ****************************************************************

                    c s w _ G e t U s e r N a m e

  ****************************************************************

    Return the operating system (login) user name of the owner of
  the current process.

*/

int csw_GetUserName (char *name,
                     int len)
{
    char             ctmp[100];

#ifndef WINNT
    char             *ct;
#endif

#ifdef WINNT
    strcpy (ctmp, "local_user");

#else

    ct = (char *)csw_cuserid (ctmp);
    if (ct == NULL) ctmp[0] = '\0';

#endif

    csw_StrTruncate (name, ctmp, len);

    return 1;

}  /*  end of function csw_GetUserName  */



/*
  ****************************************************************

                 c s w _ R u n C o m m a n d

  ****************************************************************

    Execute an operating system command.  No indication of the
  command success or failure is returned.  The function always
  returns 1.

*/

int csw_RunCommand (const char *text)
{

    system (text);
    return 1;

}  /*  end of function csw_RunCommand  */



/*
  ****************************************************************

                     c s w _ m e m c p y

  ****************************************************************

    Copy from pointer p2 to p1.  This is a wrap around memcpy
  so that I can avoid the warnings generated by ansi C compilers
  when the pointers do not match memcpy's prototypes.

*/

char *csw_memcpy (void *p1, const void *p2, int n)
{

    return (char *)memcpy (p1, p2, n);

}  /*  end of function csw_memcpy  */



/*
  ****************************************************************

               c s w _ G e t C u r r e n t D i r

  ****************************************************************

    Return a string with the current working directory name
  or return NULL on error.

*/

char *csw_GetCurrentDir (void)
{
    char        name[1000], *cwd;
    int         n;

    cwd = (char *)getcwd (name, 1000);
    if (!cwd) {
        return NULL;
    }

    n = strlen(name);
    cwd = (char *)csw_Malloc ((n + 2) * sizeof(char));
    if (!cwd) {
        return NULL;
    }

    strcpy (cwd, name);

    return cwd;

}  /*  end of function csw_GetCurrentDir  */





/*
  ****************************************************************

                c s w _ F u l l P a t h N a m e

  ****************************************************************

    Given a string with a file name, return the full path name
  for the file in a string.  The returned string should be csw_Freed
  by the calling function.  On error, NULL is returned.

*/

char *csw_FullPathName (const char *filename)
{
    char        *cwd, *ftmp;
    int         n1, n2;

    if (!filename) return NULL;
    if (filename[0] == '\0') return NULL;

    cwd = csw_GetCurrentDir ();
    if (!cwd) return NULL;

    n1 = strlen (cwd);
    n2 = strlen (filename);

    n1 = n1 + n2 + 10;
    ftmp = (char *)csw_Malloc (n1 * sizeof(char));
    if (!ftmp) {
        csw_Free (cwd);
        return NULL;
    }

    strcpy (ftmp, cwd);
    strcat (ftmp, "/");
    strcat (ftmp, filename);

    csw_Free (cwd);

    return ftmp;

}  /*  end of function csw_FullPathName  */





/*
  ****************************************************************

                 c s w _ F l o a t R a n d o m

  ****************************************************************

  function name:    csw_FloatRandom   (CSW_F)

  call sequence:    csw_FloatRandom (iseed)

  synopsis:         return a quick and dirty random number between 0 and 1
                    This function should not be used if "true" randomness
                    is desired.  It is useful for things like generating
                    sample points to test programs, etc.

  return values:    A CSW_Float point number greater than or equal to 0.0
                    and less than 1.

  calling parameters:

    iseed    r/w    long int *  Integer seed starting the random number
                                generator.  The seed is modified and
                                returned for use in the next call.  The
                                seed should initially be a large positive
                                number, but less than 200000.  If a
                                negative seed is passed, the absolute value
                                is used.  If a seed greater than 199999
                                is passed, 199999 is used.

*/

CSW_F csw_FloatRandom (int *iseed)
{
    unsigned long int         local;
    CSW_F                     f1, f2;
    unsigned long int         im=134456, ia=8121, ic=28411;
    CSW_F                     fm=134456.0f;

/*  check for bad values of iseed, and transfer to local  */

    if(*iseed < 0) *iseed = -(*iseed);
    if(*iseed > 199999) *iseed = 199999;
    local = *iseed;

/*  update local seed  */

    local = (local*ia +ic) % im;

/*  calculate random number  */

    f1 = (CSW_F)local;
    *iseed = (int)local;

    f2 = f1 / fm;

    return f2;

}  /*  end of csw_FloatRandom function  */

int csw_float_random_f_ (int *iseed, CSW_F *fdum)
{
    *fdum = csw_FloatRandom (iseed);
    return 1;
}

int csw_float_random_f (int *iseed, CSW_F *fdum)
{
    *fdum = csw_FloatRandom (iseed);
    return 1;
}








/*
  ****************************************************************

                  c s w _ S a m e S t r i n g

  ****************************************************************

    Return 1 if two sequences of n bytes are identical or return 0
  if there are any differences in the n bytes.  If n is specified
  as zero, the strlen of each pointer is used.

*/

int csw_SameString (const char *s1,
                    const char *s2,
                    int n)
{
    int           len;

/*
    return zero if either pointer is NULL
*/
    if (s1 == NULL  ||  s2 == NULL) return 0;

/*
    use strlen if n is less than 1
*/
    if (n < 1) {
        len = strlen (s1);
        if ((int)strlen (s2) != len) {
            return 0;
        }
    }
    else {
        len = n;
    }

    if (len < 1) return 0;

/*
    use memcmp for the comparison so null bytes
    do not terminate the comparison.
*/
    if (memcmp (s1, s2, len)) {
        return 0;
    }

    return 1;

}  /*  end of function csw_SameString  */



/*
  ****************************************************************

                    c s w _ E r r M s g

  ****************************************************************

    Print an error message to the error output.  This is currently
  just standard error.

*/

int csw_ErrMsg (const char *msg)
{

    fprintf (stderr, msg);
    return 1;

}  /*  end of function csw_ErrMsg  */



/*
  ****************************************************************

          c s w _ P r o c e s s D i r e c t o r y N a m e

  ****************************************************************

    Given the old directory name, and the new directory name
  which could be relative to the old name, produce a resulting
  directory path which is as simple as possible.

*/

int csw_ProcessDirectoryName (const char *olddir,
                              const char *newdir,
                              char *result)
{
    char           *ct, ctmp[1000], dsave[1000],
                   w1[1000], w2[1000], w3[1000];
    int            len;
#ifndef SOLARIS
    int            istat;
#endif

    result[0] = '\0';

/*
    first, save the processes current working directory
*/
    ct = (char *)getcwd (ctmp, 1000);
    if (!ct) {
        return -1;
    }
    csw_StrTruncate (dsave, ctmp, 1000);

/*
    clean up the old and new strings if needed
*/
    csw_StrTruncate (w1, olddir, 1000);
    csw_StrCompress (w1);
    if (w1[0] == '\0') {
        strcpy (w1, ".");
    }
    len = strlen (w1);
    if (w1[len-1] != '/') {
        strcat (w1, "/");
    }

    csw_StrTruncate (w2, newdir, 1000);
    csw_StrLeftJust (w2);
    if (!strncmp (w2, "cd ", 3)) {
        strncpy (w2, "   ", 3);
    }
    csw_StrCompress (w2);

/*
    build the total path from combining old and new
*/
    if (w2[0] == '/') {
        strcpy (ctmp, w2);
    }
    else {
        strcpy (ctmp, w1);
        strcat (ctmp, w2);
    }

/*
    The realpath function does not exist on NT, and it
    is prototyped differently on different flavors of
    UNIX.  As a result, I only use it on SOLARIS, and
    I roll my own using chdir to set a relative path
    followed by getcwd to retrieve the full path.
*/
#ifndef SOLARIS

    istat = chdir (ctmp);
    if (istat == -1) {
        chdir (dsave);
        return -1;
    }
    ct = (char *)getcwd (w3, 1000);
    if (!ct) {
        chdir (dsave);
        return -1;
    }

#else

/*
    call the system realpath function to clean up the
    . and .. stuff on SOLARIS
*/
    ct = (char *)realpath (ctmp, w3);

    if (!ct) {
        chdir (dsave);
        return -1;
    }

#endif

/*
    set back to saved directory in case realpath failed
*/
    chdir (dsave);

    strcpy (result, w3);

    return 1;

}  /*  end of function csw_ProcessDirectoryName  */



/*
  ****************************************************************

                    c s w _ F l o a t M o d

  ****************************************************************

    Calculate the modulus of two CSW_F numbers.

*/

CSW_F csw_FloatMod (CSW_F f1, CSW_F f2)
{
    CSW_F    x, y;

    x = (CSW_F)floor ((double)(f1/f2));
    y = f1 - x * f2;

    return y;

}  /*  end of function csw_FloatMod  */





/*
  ****************************************************************

              c s w _ C h e c k W i l d F l o a t

  ****************************************************************

    Check arrays of x and y values for obviusly wrong numbers.  If
  any value is bad the function returns 1.  If all are good, the
  function returns zero.

*/

int csw_CheckWildFloat (CSW_F *x,
                        CSW_F *y,
                        int npts,
                        CSW_F val)
{
    int             i;

    if (npts < 1) return 1;

    if (x) {
        for (i=0; i<npts; i++) {
            if (x[i] < -val  ||  x[i] > val) {
                return 1;
            }
        }
    }

    if (y) {
        for (i=0; i<npts; i++) {
            if (y[i] < -val  ||  y[i] > val) {
                return 1;
            }
        }
    }

    return 0;

}  /*  end of function csw_CheckWildFloat  */




/*
  ****************************************************************

                   c s w _ B a s e N a m e

  ****************************************************************

*/

int csw_BaseName (const char *in, char *out)
{

    int          i, len;

    if (!in  ||  !out) return -1;

    strcpy (out, in);
    len = strlen (in);

    for (i=len-1; i>=0; i--) {
        if (in[i] == '/') {
            strcpy (out, in+i+1);
            break;
        }
    }

    return 1;

}  /*  end of function csw_BaseName  */




/*
  ****************************************************************

               c s w _ N e a r E q u a l

  ****************************************************************

*/

int csw_NearEqual (CSW_F f1, CSW_F f2, CSW_F tiny)
{

    CSW_F        ftmp;

    ftmp = f1 - f2;
    if (ftmp > -tiny  &&  ftmp < tiny) return 1;

    return 0;

}  /*  end of function csw_NearEqual  */




/*
  ****************************************************************

                      c s w _ S e c o n d s

  ****************************************************************

    Return number of seconds since the beginning of "time".

*/

int csw_Seconds (void)
{

    return (int)time (NULL);

}  /*  end of function csw_Seconds  */




/*
  *****************************************************************

                 c s w _ F o r m a t N u m b e r

  *****************************************************************

    Encode a number into a text string, automatically deciding on
  the format.

*/

int csw_FormatNumber (CSW_F value,
                      char  *text)
{
    int            istat;

    istat = FormatNumber (value, 0.0f, text);
    return istat;

}  /*  end of function csw_FormatNumber  */



/*
  ****************************************************************

                  F o r m a t N u m b e r

  ****************************************************************

    Encode a linear or logarithmic number into a text buffer depending
  on the magnitude of the number.

*/

static int FormatNumber (CSW_F valuein, CSW_F logbase, char *buf)
{
    char         fmt[20], fmt2[10];
    int          ndec, ilog;
    CSW_F        fval, value;

    ndec = 7;
    fval = valuein;
    value = valuein;
    if (fval < 0.0f) fval = -fval;
    if (fval == 0.0f) {
        ilog = 1;
    }
    else {
        ilog = (int)log10 ((double)fval);
    }
    ilog--;
    if (fval < 0.5f) {
        ndec = -ilog + 2;
    }
    else if (fval < 5.0f) {
        ndec = 3;
    }
    else if (fval < 50.0f) {
        ndec = 2;
    }
    else if (fval < 500.0f) {
        ndec = 1;
    }
    else {
        ndec = 0;
    }
    if (ndec > 7) ndec = 7;

    if (logbase > 1.01f) {
        value = (CSW_F)pow ((double)logbase, (double)value);
        strcpy (fmt, "%8.6g");
    }
    else {
        strcpy (fmt, "%10.");
        sprintf (fmt2, "%d", ndec);
        strcat (fmt, fmt2);
        if (ndec >= 6) {
            strcat (fmt, "g");
        }
        else {
            strcat (fmt, "f");
        }
    }

    sprintf (buf, fmt, value);

    csw_StrLeftJust (buf);

    CleanupZeros (buf);

    if (buf[0] == '0'  &&  fval != 0.0) {
        if (buf[1] != '\0') {
            buf[0] = ' ';
            csw_StrLeftJust (buf);
        }
    }

    if (!strcmp (buf, "-0")) {
        strcpy (buf, "0");
    }

/*
    clean up repeating 9's at the end of a
    decimal number.
*/
    CleanupNines (buf);

    return 1;

}  /*  end of static FormatNumber function  */




/*
  ****************************************************************

                     C l e a n u p N i n e s

  ****************************************************************

    Delete repeating decimal nines at the end of a number and adjust the
  significant digits appropriately.

*/

static void CleanupNines (char *buf)
{
    char      clast, *cdot, tbuf[100];
    int       i, idot, i1;

    strcpy (tbuf, buf);

    cdot = strchr (buf, '.');
    if (!cdot) {
        return;
    }

    cdot++;
    if (*cdot == '\0') {
        return;
    }

    i = strlen (buf) - 1;
    i1 = i;

    clast = buf[i];
    if (clast != '9') return;

    while (buf[i-1] == clast) {
        buf[i] = '\0';
        i--;
    }

    if (buf[i-1] == '.'  ||  i == i1) {
        strcpy (buf, tbuf);
        return;
    }

    buf[i] = '\0';

    sscanf (cdot, "%d", &idot);
    idot++;
    sprintf (cdot, "%d", idot);

    return;

}  /*  end of static CleanupNines function  */




/*
  ****************************************************************

                     C l e a n u p Z e r o s

  ****************************************************************

    Delete repeating decimal zeros at the end of a number and adjust the
  significant digits appropriately.

*/

static void CleanupZeros (char *buf)
{
    char      clast, *cdot, tbuf[100];
    int       i;

    strcpy (tbuf, buf);

    cdot = strchr (buf, '.');
    if (!cdot) {
        return;
    }

    cdot++;
    if (*cdot == '\0') {
        return;
    }

    i = strlen (buf) - 1;

    clast = buf[i];
    if (clast != '0') return;

    while (buf[i-1] == clast) {
        buf[i] = '\0';
        i--;
    }

    if (buf[i-1] == '.') {
        strcpy (buf, tbuf);
        return;
    }

    buf[i] = '\0';

    return;

}  /*  end of static CleanupZeros function  */



int csw_do_nothing (void)
{
    return 1;
}





/*
  ****************************************************************

              c s w _ C h e c k I n t R a n g e

  ****************************************************************

    Given an array of int numbers, check if all the values are the same.
  If all are the same, return zero, otherwise return 1.

*/

int csw_CheckIntRange (int *vlist, int npts)
{
    int           i, i1;

    if (vlist == NULL) return -1;

    i1 = vlist[0];
    for (i=1; i<npts; i++) {
        if (vlist[i] != i1) {
            return 1;
        }
    }

    return 0;

}  /*  end of function csw_CheckIntRange  */





/*
  ****************************************************************

             c s w _ C h e c k D o u b l e R a n g e

  ****************************************************************

    Given an array of double numbers, check if the range of the list
  is large enough relative to the mid point value of the list to allow
  the list to be manipulated with single precision arithmetic.  If
  the range can work with single precision, 1 is returned.  If not,
  zero is returned.  If the pointer to the values is NULL, -1 is
  returned.

*/

int csw_CheckDoubleRange (double *vlist, int npts)
{
    int           i;
    double        v1, v2, range, vmid;

    if (vlist == NULL) return -1;

    v1 = 1.e30f;
    v2 = -1.e30f;
    for (i=0; i<npts; i++) {
        if ((double)vlist[i] < v1) v1 = (double)vlist[i];
        if ((double)vlist[i] > v2) v2 = (double)vlist[i];
    }
    range = v2 - v1;
    vmid = (v1 + v2) / 2.0;
    if (vmid < 0.0) vmid = -vmid;

    if (vmid > 50000.0 * range  &&  range > 0.0) {
        return 0;
    }

    return 1;

}  /*  end of function csw_CheckDoubleRange  */




/*
  ****************************************************************************

                  c s w _ P a c k D i r e c t C o l o r

  ****************************************************************************

    Pack the red, green and blue color parts into a 32 bit integer in
  a way that is independent of byte order.

*/

int csw_PackDirectColor (int r, int g, int b)
{
    int           icolor;

    icolor = b + 256 * g + 65536 * r;
    return icolor;

}  /*  end of function csw_PackDirectColor  */





/*
  ****************************************************************************

             c s w _ U n p a c k D i r e c t C o l o r

  ****************************************************************************

    Return the individual r, g, b coordinates for a packed color.
  The color must have been packed by csw_PackDirectColor.

*/

int csw_UnpackDirectColor (int icolor, int *r, int *g, int *b)
{
    int             i1;

    *r = icolor / 65536;
    i1 = icolor - *r * 65536;
    *g = i1 / 256;
    *b = i1 % 256;

    return 1;

}  /*  end of function csw_UnpackDirectColor  */






/*
  ****************************************************************************

                          c s w _ F i t L i n e

  ****************************************************************************

  Fit a straight line to a set of points.  There must be at least 2 points
  to fit a line.  On success, 1 is returned.  On error -1 is returned.

*/

int csw_FitLine (CSW_F *x, CSW_F *y, int npts, CSW_F *slope, CSW_F *yint)
{
    double    *dx, *dy, dslope, dyint;
    int       istat, i;

    istat = -1;
    dx = (double *)csw_Malloc (npts * 2 * sizeof(double));
    if (dx) {
        dy = dx + npts;
        for (i=0; i<npts; i++) {
            dx[i] = (double)x[i];
            dy[i] = (double)y[i];
        }
        istat = csw_FitDoubleLine (dx, dy, npts, &dslope, &dyint);
        csw_Free (dx);
        if (istat == 1) {
            *slope = (CSW_F)dslope;
            *yint = (CSW_F)dyint;
        }
    }

    return istat;

}  /*  end of function csw_FitLine  */






/*
  ****************************************************************************

                   c s w _ F i t D o u b l e L i n e

  ****************************************************************************

    Fit a line through a set of double precision points.  The returned slope
  and intercept are also double precision.  On error, -1 is returned.  On
  success, 1 is returned.

*/

int csw_FitDoubleLine (double *x, double *y, int npts, double *slope, double *yint)
{

    double        a, b, sumx, sumy, avgx, tmp, st2;
    int           i;
    double        xmid, ymid, xmin, xmax, ymin, ymax, aspect;
    double        a1, b1, b2;

    double        zero_slope = .0001;

    if (npts < 2) {
        return -1;
    }

  /*
   * The tricky part is getting some sort of reasonable results 
   * for a vertical line fit.  This is only a problem if the
   * bounding box of the points is relatively vertical.  So,
   * I first check the aspect ratio of the bounding box of
   * the points.
   */
    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;
    for (i=0; i<npts; i++) {
        if (x[i] < xmin) xmin = x[i];
        if (y[i] < ymin) ymin = y[i];
        if (x[i] > xmax) xmax = x[i];
        if (y[i] > ymax) ymax = y[i];
    }

    aspect = (ymax - ymin) / (xmax - xmin);
    xmid = (xmin + xmax) / 2.0;
    ymid = (ymin + ymax) / 2.0;

  /*
   * Fit a line to x and y.
   */
    sumx = 0.0;
    sumy = 0.0;
    st2 = 0.0;
    b = 0.0;

    for (i=0; i<npts; i++) {
        sumx += x[i];
        sumy += y[i];
    }

    avgx = sumx / (double)npts;

    for (i=0; i<npts; i++) {
        tmp = x[i] - avgx;
        st2 += tmp * tmp;
        b += tmp * y[i];
    }

    b /= st2;
    if (b < zero_slope  &&  b > -zero_slope) {
        if (b >= 0.0) {
            b = zero_slope;
        }
        else {
            b = -zero_slope;
        }
    }
    a = (sumy - sumx * b) / (double)npts;
    a1 = a;
    b1 = b;

  /*
   * If the fit behaves properly, the slope cannot be significantly 
   * outside of the aspect ratio of the enclosing rectangle (or the
   * negative of the aspect ratio).
   */
    b2 = b1;
    if (b2 < 0.0) {
        b2 = -b2;
    }
    if (aspect > 1.0) {
        if (b2 < .5 * aspect) {
            if (b1 < 0.0) {
                b1 = aspect;
            }
            else {
                b1 = -aspect;
            }
            a1 = ymid - xmid * b1;
        }
    }
            
    *slope = b1;
    *yint = a1;

    return 1;

}  /*  end of function csw_FitDoubleLine  */



/*
 * Function to fool lint into thinking that
 * the local do_write variable (used for debug
 * output in a lot of places) is not always zero.
 */
int csw_GetDoWrite (void)
{
    return 0;
}
