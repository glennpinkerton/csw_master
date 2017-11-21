
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_string.c

      This file has some generic string manipulation functions
    not provided by the standard c library.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "csw/utils/include/csw_.h"


#define MAXSTRLEN 1024

/*
    function "prototypes" for static functions
*/
    static int              NumChars (const char *);


/*
  ****************************************************************

                c s w _ S t r L e f t J u s t

  ****************************************************************

  function name:      csw_StrLeftJust

  calling sequence:   csw_StrLeftJust(str)

  purpose:            left justify a character string

  calling parameters:

      str    r/w    char*    character string to left justify

*/

int csw_StrLeftJust(char *str)
{
    long int    i, nc, n1;
    char        tmp[MAXSTRLEN];

/*
    get length of input string
*/
    nc = NumChars(str);
    if(nc < 1) {
        str[0] = '\0';
        return 0;
    }
    if(nc > MAXSTRLEN) nc = MAXSTRLEN;

/*
    copy into tmp string and find first non blank character
*/
    strncpy(tmp, str, nc);

    n1 = 0;
    for(i=0; i<nc; i++) {
        if(tmp[i] != ' '  &&  tmp[i] != '\t') {
            n1 = i;
            break;
        }
    }

/*
    copy back into str
*/
    nc -= n1;
    if (nc < 1) {
        str[0] = '\0';
        return 0;
    }
    strncpy(str, tmp+n1, nc);
    str[nc] = '\0';
    return 0;

}  /*  end of csw_StrLeftJust function  */



/*
  ****************************************************************

                c s w _ S t r C l e a n 2

  ****************************************************************

  function name:    csw_StrClean2       (int)

  call sequence:    csw_StrClean2 (out, in, nc)

  purpose:          make a truncated, lower case, compressed copy
                    of the input string.

  return value:     always returns zero

  calling parameters:

    out        w    char*     cleaned copy of string
    in         r    char*     original string
    nc         r    int       number of characters for truncation

*/

int csw_StrClean2 (char *out,
                   const char *in,
                   int nc)
{
    csw_StrClean (out, in, nc);
    csw_StrCompress (out);

    return 1;

}  /*  end of function csw_StrClean2  */




/*
  ****************************************************************

                c s w _ S t r C l e a n

  ****************************************************************

  function name:    csw_StrClean     (int)

  call sequence:    csw_StrClean (textout, textin, lenout)

  purpose:          make a lower case left justified copy of a string
                    if the input string is longer than the output string
                    the input will be clipped to the output length by
                    deleting the end characters. If either string is longer
                    than 1000 characters, the output may be clipped to 1000
                    characters in some instances.

  return value:     always returns zero

  calling parameters:

    textout    w     char*      pointer to output string
    textin     r     char*      pointer to input string
    lenout     r     int        maximum length of output string

*/

int csw_StrClean (char *textout,
                  const char *textin,
                  int lenout)
{
    char               local[1001];
    long int           len;

    if (!textin) {
        textout[0] = '\0';
        return 0;
    }

/*
    The output is long enough for the input string.
*/
    len = strlen (textin);
    if (len < lenout) {
        strcpy(textout, textin);
        csw_StrLeftJust (textout);
    }

/*
    input longer than output
*/
    else {
        if (len > 1000) {
            len = 1000;
        }
        strncpy (local, textin, len);
        local[1000] = '\0';
        csw_StrLeftJust (local);
        strncpy(textout, local, lenout);
        textout[lenout-1] = '\0';
    }

    csw_StrLowerCase (textout);

    return 0;

}  /*  end of function csw_StrClean  */



/*
  ****************************************************************

                c s w _ S t r T r u n c a t e

  ****************************************************************

     Copy a string to a truncated string if the input is too long.
   This function should be used instead of strcpy if there is any
   chance that strcpy would copy outside the bounds of its output
   string.

*/

int csw_StrTruncate (char *out,
                     const char *in,
                     int lenout)
{
    int        len;

/*
    set output string to all NULL bytes
*/
    memset ((void *)out, 0, (size_t)lenout);

/*
    null input pointer returns null output string
*/
    if (!in) {
        out[0] = '\0';
        return 1;
    }

/*
    output long enough for input copy
*/
    len = strlen (in);
    if (len < lenout) {
        strcpy(out, in);
    }

/*
    input longer than output
*/
    else {
        strncpy (out, in, lenout);
        out[lenout-1] = '\0';
    }

    return 1;

}  /*  end of function csw_StrTruncate  */



/*
  ****************************************************************

                     N u m C h a r s

  ****************************************************************

  function name:         NumChars   (integer)

  calling sequence:      NumChars(str)

  purpose:               get number of characters not including
                         trailing blanks

  return value:          number of characters

  calling parameters:

      str     r    char*    string to check

  other dependencies:

      strlen    standard c library string length function

*/

static int NumChars(const char *str)
{
    long int    i, len;

    len = strlen(str);

    if (len < 1) {
        return 0;
    }

/*
    count backwards for first non white space character
*/
    for (i=len-1; i>=0; i--) {
        if (isspace (str[i])) {
            continue;
        }
        return i + 1;
    }

    return 0;

}  /*  end of NumChars function  */



/*
  ****************************************************************

                c s w _ S t r R i g h t J u s t

  ****************************************************************

  function name:    csw_StrRightJust   (integer)

  call sequence:    csw_StrRightJust(str)

  purpose:          right justify a character string

  return value:     always returns 0

  calling parameters:

    str   r/w   char*   string to right justify

*/

int csw_StrRightJust(char *str)
{
    long int       i, nc, n1, len;
    char           tmp[MAXSTRLEN];

/*
    get number of characters in str
*/
    nc = NumChars(str);
    len = strlen(str);
    if(nc == len) return 0;
    if(nc < 1) return 0;
    if(nc > MAXSTRLEN) nc = MAXSTRLEN;

/*
    copy into tmp and back into right side of str
*/
    strncpy(tmp, str, nc);
    n1 = len - nc;
    strncpy(str+n1, tmp, nc);

/*
    pad left side of str with blanks
*/
    for (i=0; i<n1; i++) {
        str[i] = ' ';
    }

    return 0;

}  /*  end of csw_StrRightJust function      */



/*
  ****************************************************************

               c s w _ S t r R i g h t S t r i p

  ****************************************************************

    Strip blanks from right of string.

*/

int csw_StrRightStrip (char *str)
{
    int           nc;

    nc = NumChars(str);
    str[nc] = '\0';

    return 1;

}  /*  end of function csw_StrRightStrip  */


/*
    Fortran callable versions
*/

int csw_strrightstrip_f (char *str)
{
    csw_StrRightStrip (str);
    return 1;

}  /*  end of function csw_StrRightStrip_f  */

int csw_strrightstrip_f_ (char *str)
{
    csw_StrRightStrip (str);
    return 1;

}  /*  end of function csw_StrRightStrip_f_  */



/*
  ****************************************************************

                     c s w _ s t r s t r

  ****************************************************************

  function name:    csw_strstr   (character pointer)

  call sequence:    csw_strstr (str1, str2)

  synopsis:         Find location of substring str2 within string
                    str1.  This is a wrapper or replacement for the
                    C library strstr function.

  return values:    Returns a pointer to the first character of the
                    substring.  If the substring is not found, a null
                    pointer (0) is returned.

  calling parameters:

    str1    r     char *    pointer to string which will be searched for
                            the substring.

    str2    r     char *    pointer to the substring

*/

char *csw_strstr (const char *str1,
                  const char *str2)
{
    long int   i, len1, len2;

/*
    get length of each string
*/
    len1 = strlen(str1);
    len2 = strlen(str2);

/*
    loop through str1, using strncmp to compare
*/
    for (i=0; i <= len1-len2; i++) {

        if(!strncmp(str1,str2,len2)) {
            return (char *)str1;
        }
        else {
            str1++;
        }
    }

/*
    no match, return a null pointer
*/
    return NULL;

}  /*  end of csw_strstr function  */



/*
  ****************************************************************

               c s w _ S t r L o w e r C a s e

  ****************************************************************

  function name:    csw_StrLowerCase   (integer)

  call sequence:    csw_StrLowerCase (str)

  purpose:          convert string to lower case

  return value:     always returns 0

  calling parameters:

    str    r/w    char*   string to convert

*/

int csw_StrLowerCase (char *str)
{
    long int    i, len;

    len = strlen(str);

/*
    convert each upper case to lower case in string
*/
    for (i=0; i<len; i++) {
        if(str[i] >= 65  &&  str[i] <= 90) {
            str[i] += 32;
        }
    }

    return 0;

}  /*  end of csw_StrLowerCase function      */



/*
  ****************************************************************

              c s w _ S t r U p p e r C a s e

  ****************************************************************

  function name:    csw_StrUpperCase   (integer)

  call sequence:    csw_StrUpperCase (str)

  purpose:          convert string to upper case

  return value:     always returns 0

  calling parameters:

    str    r/w    char*   string to convert

*/

int csw_StrUpperCase (char *str)
{
    long int    i, len;

    len = strlen(str);

/*
    convert each lower case to upper case in string
*/
    for (i=0; i<len; i++) {
        if(str[i] >= 97  &&  str[i] <= 122) {
            str[i] -= 32;
        }
    }

    return 0;

}  /*  end of csw_StrUpperCase function      */




/*
  ****************************************************************

           c s w _ S t r R e m o v e C o m m e n t

  ****************************************************************

  function name:    csw_StrRemoveComment     (int)

  call sequence:    csw_StrRemoveComment (string)

  purpose:          Remove in line comments from a string.
                    In line comments are a ! or # followed by any number
                    of characters.  The ! or # is ignored if preceded by a \.
                    This function is called by various file interpreters.

  return value:     always returns zero

  calling parameters:

    string      r/w     char*     String modified to remove comment

*/

int csw_StrRemoveComment (char *string)
{
    int      n1, len, i;
    char     chr;

    len = strlen (string);
    if (len < 1) {
        return 0;
    }

/*
    change the first unprotected comment character to null
*/
    i = 0;
    while (i < len) {
        chr = string[i];
        if (chr == '\\') {
            i += 2;
            continue;
        }
        if (chr == '#'  ||  chr == '!') {
            string[i] = '\0';
            break;
        }
        i++;
    }

/*
    remove \ from # or !
*/
    n1 = 0;
    len = strlen (string);
    for (i=0; i<=len; i++) {
        if (string[i] == '\\') {
            chr = string[i+1];
            if (chr == '#'  ||  chr == '!') {
                continue;
            }
        }
        string[n1] = string[i];
        n1++;
    }
    string[n1] = '\0';

/*
    convert trailing space to nulls
*/
    for (i = n1-1; i>=0; i--) {
        if (isspace((int)(string[i]))) {
            string[i] = '\0';
        }
        else {
            break;
        }
    }

    return 0;

}  /*  end of function csw_StrRemoveComment  */




/*
  ******************************************************

              c s w _ S t r C o m p r e s s

  ******************************************************

    remove blanks, tabs and newlines from a string

*/

int csw_StrCompress (char *str)
{
    char      tmp[MAXSTRLEN];
    int       i, len, n;

    n = 0;
    len = strlen (str);
    if (len >= MAXSTRLEN - 1) {
        len = MAXSTRLEN - 2;
    }

    for (i=0; i<len; i++) {
        if (str[i] == ' ' ||  str[i] == '\t'  ||  str[i] == '\n') {
            continue;
        }
        tmp[n] = str[i];
        n++;
    }

    tmp[n] = '\0';
    strcpy (str, tmp);

    return 0;

}  /*  end of function csw_StrCompress  */



/*
  ****************************************************************

                   c s w _ V a l i d N a m e

  ****************************************************************

  function name:    csw_ValidName      (int)

  call sequence:    csw_ValidName (name, n)

  purpose:          determine if a text string is a valid name
                    (not null or blank or longer than 100 characters)

  return value:     status code

                    0 = invalid name
                    1 = valid name

  calling parameters:

    name      r    char*     text string to validate
    n         r    int       maximum length of valid string

*/

int csw_ValidName (const char *name,
                   int n)
{
    char    local[101];

    if (n > 100) {
        return 0;
    }
    if (!name) {
        return 0;
    }
    if (name[0] == '\0') {
        return 0;
    }
    csw_StrTruncate (local, name, n);
    csw_StrLeftJust (local);
    if (local[0] == '\0') {
        return 0;
    }
    return 1;

}  /*  end of function csw_ValidName  */


/*
  ****************************************************************

                  c s w _ S e p a r a t e L i n e s

  ****************************************************************

  function name:    csw_SeparateLines           (int)

  call sequence:    csw_SeparateLines (text, lines, nlines, maxlines)

  purpose:          Edit a string of characters into an array
                    of character pointers to the first character
                    after any newlines in the input string.  The
                    newlines are changed to null characters so
                    that each output pointer will point to a null
                    terminated string corresponding to a single
                    line of the input.  Note that the input IS
                    modified by the function.  DO NOT csw_Free or
                    overwrite the input text array until you
                    have finished using the lines array.  The
                    lines pointers point to addresses inside the
                    original text array.

  return value:     status code

                    -1 = maxlines is too small for the number of
                         lines in the input text.  In this case,
                         the input text is not modified.
                     1 = normal successful completion

  calling parameters:

    text         r/w  char*      Multiline text string.
    lines        w    char**     Array of char pointers for output.
    nlines       w    int*       Number of lines in output.
    maxlines     r    int        Size of the lines array passed.

*/

int csw_SeparateLines (char *text,
                       char **lines,
                       int *nlines,
                       int maxlines)
{
    int           n;
    char          *chptr;

/*
    obvious errors
*/
    if (text[0] == '\0') {
        *nlines = 0;
        return 1;
    }

    if (maxlines < 1) {
        return -1;
    }

/*
    The first line is defined as the start of the
    input text.  Subsequent line starts are defined
    as the first character after a newline.  Newlines
    are converted to nulls.
*/
    lines[0] = text;
    n = 1;

    for (chptr = text; *chptr != '\0'; chptr++) {
        switch (*chptr) {
          case '\n':
/*
            return with error condition if too many lines
*/
            if (n >= maxlines)
                return -1;
            lines[n] = chptr + 1;
            n++;
            *chptr = '\0';
            break;

          case '\r':
            *chptr = ' ';
            break;

          default:
            break;
        }
    }

/*
    set number of lines output and return success
*/
    *nlines = n;
    return 1;

}  /*  end of function csw_SeparateLines  */



/*
  ****************************************************************

             c s w _ I n s e r t S u b s t r i n g

  ****************************************************************

  function name:    csw_InsertSubstring      (int)

  call sequence:    csw_InsertSubstring (text, pos, substring, length, maxlen)

  purpose:

    Insert a substring of characters at a specified position
  in a string.  None of the current string's characters are
  overwritten.

  return value:

    1 = success, -1 = failure due to bad parameter or if the modified string
  is too long for the internal string buffer.

  calling parameters:

    text        r/w    char*     String to be insert into.
    pos         r      int       Position in string for the insertion.
                                 Substring is inserted just prior to the
                                 position.  The first character is position
                                 zero.  If pos is greater than the length
                                 of text, substring is appended to text.
    substring   r      char*     String to insert.
    length      r      int       Length of substring.
    maxlen      r      int       Maximum length of modified text.

*/

int csw_InsertSubstring (char *text,
                         int pos,
                         const char *substring,
                         int length,
                         int maxlen)
{
    int          len1, len2;
    char         c1[MAXSTRLEN], c2[MAXSTRLEN];

/*
    obvious errors
*/
    if (!text  ||  !substring) {
        return -1;
    }
    if (substring[0] == '\0'  ||  length < 1) {
        return -1;
    }
    len1 = strlen (text);
    len2 = strlen (substring);
    if (length > len2) {
        length = len2;
    }
    if (len1 + length >= MAXSTRLEN - 1) {
        return -1;
    }

/*
    append to end of text if pos is past end of text
*/
    if (pos >= len1) {
        strcpy (c1, text);
        strncat (c1, substring, length);
    }

/*
    separate text into string before pos and straing
    after and including pos.  The before string, substring
    and after string are then concatenated.
*/
    else {
        strcpy (c2, text+pos);
        text[pos] = '\0';
        strcpy (c1, text);
        strncat (c1, substring, length);
        strcat (c1, c2);
    }

    csw_StrTruncate (text, c1, maxlen);

    return 1;

}  /*  end of function csw_InsertSubstring  */



/*
  ****************************************************************

           c s w _ O v e r w r i t e S u b s t r i n g

  ****************************************************************

  function name:    csw_OverwriteSubstring      (int)

  call sequence:    csw_OverwriteSubstring (text, pos, substring, length, maxlen)

  purpose:

     Overwrite a substring of characters at a specified position
  in a string.  Starting at pos, the next length characters in
  the text are changed to the substring characters.

  return value:

    1 = success, -1 = failure due to bad parameter or if the modified string
  is too long for the internal string buffer.

  calling parameters:

    text        r/w    char*     String to be overwrite into.
    pos         r      int       Position in string for the overwrite.
                                 The first character is position zero.
                                 If pos is greater than the length of
                                 text, substring is appended to text.
    substring   r      char*     String to use for the overwrite.
    length      r      int       Length of substring.
    maxlen      r      int       Maximum length of modified text.

*/

int csw_OverwriteSubstring (char *text,
                            int pos,
                            const char *substring,
                            int length,
                            int maxlen)
{
    int          i, len1, len2;
    char         c1[MAXSTRLEN];

/*
    obvious errors
*/
    if (!text  ||  !substring) {
        return -1;
    }
    if (substring[0] == '\0'  ||  length < 1) {
        return -1;
    }
    len1 = strlen (text);

/*
    make sure lengths are ok
*/
    len2 = strlen (substring);
    if (length > len2) {
        length = len2;
    }

    if (len1 + length >= MAXSTRLEN - 1) {
        return -1;
    }

    if (pos > len1) {
        pos = len1;
    }

/*
    overwrite the characters using a work space array
*/
    strcpy (c1, text);
    for (i=0; i<length; i++) {
        c1[i+pos] = substring[i];
    }

    if (pos+length >= len1) {
        c1[pos+length] = '\0';
    }

/*
    copy back to text, truncating if needed
*/
    csw_StrTruncate (text, c1, maxlen);

    return 1;

}  /*  end of function csw_OverwriteSubstring  */




/*
  ******************************************************

               c s w _ F i x u p T a b s

  ******************************************************

  function name:    csw_FixupTabs      (int)

  call sequence:    csw_FixupTabs (in, out, size)

  synopsis:         replace tabs with blanks at the tabstop spacing.

  return values:    always returns 1

  calling parameters:

    in        r     char*    input text string
    out       w     char*    output text string
    size      r     int      number of blanks for tab

*/

int csw_FixupTabs (const char *in,
                   char *out,
                   int size)
{
    int         i, j, k, len, n;

    len = strlen (in);
    n = 0;

    for (i=0; i<len; i++) {
        if (in[i] == '\t') {
            j = i % size;
            j = size - j;
            for (k=0; k<j; k++) {
                out[n] = ' ';
                n++;
            }
        }
        else {
            out[n] = in[i];
            n++;

        }
    }

    out[n] = '\0';

    return 1;

}  /*  end of function csw_FixupTabs  */




/*
  ******************************************************

              c s w _ L i n e C h a r s

  ******************************************************

    return number of characters until newline or null

*/

int csw_LineChars (const char *text)
{
    int         n;
    char        *tmp;

    if (!text) return 0;

    tmp = (char *)text;
    n = 0;

    while (*tmp != '\0') {
        if (*tmp == '\n') {
            return n;
        }
        tmp++;
        n++;
    }

    return n;

}  /*  end of function csw_LineChars  */





/*
  ******************************************************

              c s w _ N e w L i n e D e l e t e

  ******************************************************

  function name:    csw_NewLineDelete    (int)

  call sequence:    csw_NewLineDelete (text)

  synopsis:         convert newlines to blanks

  return values:    always returns zero

  calling parameters:

    text    r/w   char*     character string to convert
                            (the string is modified by this function)

*/

int csw_NewLineDelete (char *text)
{
    long int   i, len;

    if (text == NULL) {
        return 0;
    }

    len = strlen (text);

/*
    change newlines to spaces
*/
    for (i=0; i<len; i++) {
        if(text[i] == '\n') {
            text[i] = ' ';
        }
    }

    return 0;

}  /*  end of function csw_NewLineDelete  */



/*
  ******************************************************

            c s w _ N e w L i n e C o n v e r t

  ******************************************************

  function name:    csw_NewLineConvert   (int)

  call sequence:    csw_NewLineConvert (text)

  synopsis:         convert newlines to \n

  return values:    always returns zero

  calling parameters:

    text    r/w   char*     character string to convert
                            (the string is modified by this function)

*/

int csw_NewLineConvert (char *text)
{
    long int   i, len, memflag, n;
    char       local[2001], *c;

/*
    check obvious errors
*/
    if (text == NULL) {
        return 0;
    }

/*
    allocate work string if needed
*/
    len = strlen (text);
    if (len >= 2000) {
        c = (char *)csw_Malloc ((len*2) * sizeof (char));
        if (!c) {
            return -1;
        }
        memflag = 1;
    }
    else {
        c = local;
        memflag = 0;
    }

/*
    change newlines to \n
*/
    c[0] = '\0';
    n = 0;
    for (i=0; i<len; i++) {
        if(text[i] == '\n') {
            c[n] = '\\';
            n++;
            c[n] = 'n';
            n++;
        }
        else {
            c[n] = text[i];
            n++;
        }
    }
    c[n] = '\0';

    strcpy (text, c);

    if (memflag) {
        csw_Free (c);
    }

    return 0;

}  /*  end of function csw_NewLineConvert  */



/*
  ******************************************************

            c s w _ N e w L i n e I n s e r t

  ******************************************************

  function name:    csw_NewLineInsert   (int)

  call sequence:    csw_NewLineInsert (text)

  synopsis:         convert \n to newlines

  return values:    always returns zero

  calling parameters:

    text    r/w   char*     character string to convert
                            (the string is modified by this function)

*/

int csw_NewLineInsert (char *text)
{
    long int   i, len, memflag, n, good;
    char       local[2001], *c;

/*
    Check obvious errors.
*/
    if (text == NULL) {
        return 0;
    }

/*
    allocate work string if needed
*/
    len = strlen (text);
    if (len >= 2000) {
        c = (char *)csw_Malloc ((len*2) * sizeof (char));
        if (!c) {
            return -1;
        }
        memflag = 1;
    }
    else {
        c = local;
        memflag = 0;
    }

/*
    change \n to newlines, \t to tabs and \0 to nulls
*/
    c[0] = '\0';
    n = 0;
    i = 0;
    good = 0;
    while (i < len-1) {
        good = 0;
        if(text[i] == '\\'  &&  (text[i+1] == 'n' || text[i+1] == 'N')) {
            c[n] = '\n';
            n++;
            i++;
        }
        else if(text[i] == '\\'  &&  (text[i+1] == 't' || text[i+1] == 'T')) {
            c[n] = '\t';
            n++;
            i++;
        }
        else if(text[i] == '\\'  &&  text[i+1] == '0') {
            break;
        }
        else {
            c[n] = text[i];
            good = 1;
            n++;
        }
        i++;
    }
    if (good) {
        c[n] = text[i];
        n++;
    }
    c[n] = '\0';

    strcpy (text, c);

    if (memflag) {
        csw_Free (c);
    }

    return 0;

}  /*  end of function csw_NewLineInsert  */




/*
  ****************************************************************

             c s w _ C h a r U p p e r C a s e

  ****************************************************************

    Convert a single lower case char to upper case.

*/

int csw_CharUpperCase (char *c1)
{

    if (islower((int)*c1)) *c1 = (char)toupper((int)*c1);
    return 1;

}  /*  end of function csw_CharUpperCase  */





/*
  ****************************************************************

             c s w _ C h a r L o w e r C a s e

  ****************************************************************

    Convert a single upper case char to lower case.

*/

int csw_CharLowerCase (char *c1)
{

    if (isupper((int)*c1)) *c1 = (char)tolower((int)*c1);
    return 1;

}  /*  end of function csw_CharLowerCase  */




/*
  ****************************************************************

                   c s w _ S t r O n e B l a n k

  ****************************************************************

    Compress adjacent white space inside a string to a single
  blank character.

*/

int csw_StrOneBlank (char *text)
{
    char         tw1[1000], tw2[1000], c1;
    int          i, len, last, n;

    csw_StrTruncate (tw1, text, 1000);
    csw_StrLeftJust (tw1);
    csw_StrRightStrip (tw1);

    len = strlen (tw1);

    if (len < 1) {
        text[0] = '\0';
        return 1;
    }

    for (i=0; i<len; i++) {
        if (isspace ((int)(tw1[i]))) {
            tw1[i] = ' ';
        }
    }

    n = 0;
    last = 0;

    for (i=0; i<len; i++) {
        c1 = tw1[i];
        if (c1 == ' ') {
            if (!last) {
                tw2[n] = c1;
                n++;
            }
            last = 1;
        }
        else {
            last = 0;
            tw2[n] = c1;
            n++;
        }
    }

    tw2[n] = '\0';

    strcpy (text, tw2);

    return 1;

}  /*  end of function csw_StrOneBlank  */




/*
  ****************************************************************

               c s w _ S p l i t L e f t R i g h t

  ****************************************************************

    Split a string into the left side and right side based on a
  set of separator characters.

*/

int csw_SplitLeftRight (char *line, char *left, char *right, const char *sep)
{
    int           i, l1, lsep;
    char          *ctmp;

    left[0] = '\0';
    right[0] = '\0';
    lsep = strlen (sep);

    for (i=0; i<lsep; i++) {
        ctmp = strchr (line, sep[i]);
        if (ctmp) {
            l1 = ctmp - line;
            strncpy (left, line, l1);
            left[l1] = '\0';
            strcpy (right, ctmp+1);
            return 1;
        }
    }

    strcpy (left, line);

    return 1;

}  /*  end of function csw_SplitLeftRight  */



/*
  ******************************************************

          c s w _ S t r i n g L e f t M a t c h

  ******************************************************

  function name:    csw_StringLeftMatch      (char *)

  call sequence:    csw_StringLeftMatch (string, list, nlist, maxlen, nout)

  synopsis:         see if a string has a match or close enough to
                    a match in a list of strings.  Each string in
                    the list must be unique for the partial matching
                    to work correctly.

  return values:    returns a pointer to the string in list that matches
                    string.  if no match is found, a NULL pointer is
                    returned

  description of calling parameters:

    string      r    char*       string to match
    list        r    char**      list of possible candidates for the match
    nlist       r    int        number of strings in list
                                 (the algorithm is not suited for very large
                                 lists, probably 200 at most will work well)
    maxlen      r    int        maximum length of string in list
                                 if maxlen is less than 1, the maximum length
                                 is calculated internally
    nout        r    int    *   index of list for matching string
                                 -1 if no match

*/

char *csw_StringLeftMatch (char *string, char **list, int nlist,
                           int maxlen, int *nout)
{
    int           i, tmp, nmatch, imatch, j;
    char          local[5000];

    *nout = -1;

    if (nlist < 1) {
        return NULL;
    }

/*  find maximum string length if it was not passed  */

    if (maxlen < 1) {
        maxlen = 0;
        for (i=0; i<nlist; i++) {
            tmp = strlen(list[i]);
            if (tmp > maxlen) {
                maxlen = tmp;
            }
        }
    }

    if (maxlen < 1) {
        return NULL;
    }

/*  check for an exact match with the entire string  */

    for (j=0; j<nlist; j++) {
        csw_StrClean2 (local, list[j], 5000);
        if (!strcmp(string, local)) {
            *nout = j;
            return (list[j]);
        }
    }

/*  check leftmost characters until either 1 or 0 matches are found  */

    for (i=1; i<=maxlen; i++) {

        nmatch = 0;
        imatch = -1;

        for (j=0; j<nlist; j++) {
            csw_StrClean2 (local, list[j], 5000);
            if (!strncmp(string, local, i)) {
                nmatch++;
                imatch = j;
            }
        }

        if (nmatch == 1) {
            *nout = imatch;
            return (list[imatch]);
        }
        else if (nmatch == 0) {
            return NULL;
        }

    }

/*  no unique partial match in the list  */

    return NULL;

}  /*  end of function csw_StringLeftMatch  */


/*
 ************************************************************************

            c s w _ S t r V a l i d F i l e N a m e

 ************************************************************************

*/

void csw_StrValidFileName (char *text, char ichar)
{
    int            i, nc;
    char           c1;

    if (text == NULL) {
        return;
    }

    nc = strlen (text);
    for (i=0; i<nc; i++) {
        c1 = text[i];
        if (isalnum ((int) c1)  ||
            c1 == '.'  ||
            c1 == '/'  ||
            c1 == '\\'  ||
            c1 == '-'  ||
            c1 == '_') {
            continue;
        }
        text[i] = ichar;
    }

    return;
}



/*
 *****************************************************************************

                    c s w _ S t r I n s e r t C o m m a s

 *****************************************************************************

  If the specified text is all numeric or the minus sign, insert commas
  every third character, starting on the right.  A dollar sign in the first
  left justified character is also ok.

*/

int csw_StrInsertCommas (char *text, int maxlen)
{
    char            buf[100];
    int             i, j, n, len, dollar;

    if (text == NULL  ||  maxlen < 10) {
        return 1;
    }

    if (maxlen > 99) maxlen = 99;
    csw_StrLeftJust (text);
    dollar = 0;
    if (text[0] == '$') {
        text[0] = ' ';
        dollar = 1;
    }
    csw_StrLeftJust (text);
    len = strlen (text);

    for (i=0; i<len; i++) {
        if (text[i] == '-'  ||
            isdigit ((int) text[i])) {
            continue;
        }
        return 1;
    }

    for (i=0; i<maxlen; i++) {
        buf[i] = ' ';
    }
    buf[maxlen-1] = '\0';

    n = maxlen - 2;
    j = 0;
    for (i=len-1; i>=0; i--) {
        if (j%3 == 0  &&  j > 0) {
            buf[n] = ',';
            n--;
        }
        if (n < 1) {
            break;
        }
        buf[n] = text[i];
        n--;
        j++;
    }
    if (dollar) {
        buf[n] = '$';
    }

    csw_StrLeftJust (buf);

    strcpy (text, buf);

    return 1;

}
