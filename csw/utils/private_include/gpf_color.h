
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_color.h

    Define the GPFColor class, which refactors the old gpf_color.c
    files.

    This is a private header file that has function prototypes for the 
    functions in file gpf_color.c.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_colorP.h.
#endif



#ifndef GPF_COLOR_H
#define GPF_COLOR_H

#  include "csw/utils/include/csw_.h"

#define MAXNCOLOR        256
#define MAXHLS           100
#define MAXRGB           65535

typedef struct {
    int    hue,
           light,
           sat,
           hue2,
           light2,
           sat2;
} HLsDefRec;

typedef struct {
    int anchor,
        hshift;
}  JTshift;


class GPFColor
{

  private:

    HLsDefRec            ColorTable[MAXNCOLOR];

  public:

    int gpf_setdefhls (void);
    int gpf_getdefhls1 (int i, int *hout, int *lout, int *sout);
    int gpf_getdefhls2 (int i, int *hout, int *lout, int *sout);
    int gpf_hlstorgb (int hin, int lin, int sin,
                      int *rout, int *gout, int *bout);
    int gpf_rgbtohls (int rin, int gin, int bin,
                      int *hout, int *lout, int *sout);
    int gpf_ConvertToInkColor (int *inr, int *ing, int *inb);
};

#endif
/*
    end of header file
    add nothing below this endif
*/
