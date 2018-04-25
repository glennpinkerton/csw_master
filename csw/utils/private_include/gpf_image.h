
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_image.h

    Define the GPFImage class.  This refactors the old gpf_image.c
    functions.

*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_image.h.
#endif



#ifndef GPF_IMAGE_H
#define GPF_IMAGE_H


/*
    define constants for the file
*/
#define MAXBANDCELLS        2000
#define MAX_ROWS            10000

#define DEG2RAD             .017453f

#define IMAGE_COLOR_DATA            0
#define IMAGE_8BIT_DATA             1
#define IMAGE_16BIT_DATA            2
#define IMAGE_32BIT_DATA            3
#define IMAGE_FLOAT_DATA            4
#define IMAGE_RAW_COLOR_DATA        5

#define MAX_IMAGE_COLORS            256
#define MAXBANDS                    257


/*
    define structures for the file
*/
typedef union {
    int           ival;
    CSW_F         fval;
}  FIunion;

typedef struct {
    FIunion       min,
                  max;
    int           num;
}  IMageColorRec;

typedef struct {
    CSW_F              min,
                       max;
    int                num;
} IMageColorBand;


class GPFImage
{

  public:

    GPFImage () {};

  private:

/*
    Old static file variables become private class variables.
    Note that the ColorTable and ColorLookup pointers are not
    persistent across public method calls.  All public methods 
    that allocate either of these pointers must free the allocations
    prior to returning.  After the free, set the pointers to NULL.
*/
    IMageColorRec           *ColorTable = NULL;
    int                     *ColorLookup = NULL;
    int                     Ncolors = 0;
    int                     InullVal = 0;
    CSW_F                   FnullVal = 0.0f;
    unsigned char           BackgroundColor = 255;
    int                     SparseImageFlag = 0;

    int                     LookupMin = 0,
                            LookupSize = 0;
    CSW_F                   LookupMinF = 0.0f,
                            LookupSizeF = 0.0f;
    int                     LookupNcells = MAXBANDCELLS;


/*
    Old file static functions become private class methods.
*/
    int                     SubImage (const unsigned char *datain,
                                      CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                                      int nc1, int nr1,
                                      unsigned char **subimage,
                                      CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                                      int *nc2, int *nr2);
    int                     ClipImageRow (CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                                          CSW_F *x21, CSW_F *x22, int *nc2);
    int                     ClipImage (CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                                       CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                                       int *nc2, int *nr2);
    int                     SortIntColorTable (void);
    int                     SortFloatColorTable (void);
    int                     HalfColorImage (const unsigned char *datain,
                                            int ncol, int nrow, int seed,
                                            unsigned char **dataout,
                                            int *ncout, int *nrout);
    int                     FillInHalvedImage (unsigned char *data, int nc, int nr);

    int                     ColorImage8 (const char *datain, int nc, int nr,
                                         unsigned char *dataout),
                            ColorImage16 (const short *datain, int nc, int nr,
                                          unsigned char *dataout),
                            ColorImage32 (const int *datain, int nc, int nr,
                                          unsigned char *dataout),
                            ColorImageFloat (const CSW_F *datain, int nc, int nr,
                                             unsigned char *dataout);

    int                     ColorImage8Row (const char *datain, int nc,
                                            unsigned char *dataout),
                            ColorImage16Row (const short *datain, int nc,
                                             unsigned char *dataout),
                            ColorImage32Row (const int *datain, int nc,
                                             unsigned char *dataout),
                            ColorImageFloatRow (const CSW_F *datain, int nc,
                                                unsigned char *dataout);

    int                     DoubleImage8 (const char *datain, char *dataout,
                                          int nc1, int nr1, int nc2, int nr2),
                            DoubleImage16 (const short *datain, short *dataout,
                                           int nc1, int nr1, int nc2, int nr2),
                            DoubleImage32 (const int *datain, int *dataout,
                                           int nc1, int nr1, int nc2, int nr2),
                            DoubleImageFloat (const CSW_F *datain, CSW_F *dataout,
                                              int nc1, int nr1, int nc2, int nr2);

  public:

    int gpf_InterpColorImage (unsigned char *datain,
                              CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                              int nc1, int nr1,
                              unsigned char *dataout,
                              CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                              int *nc2, int *nr2);
    int gpf_InterpImage8 (const char *datain,
                          CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                          int nc1, int nr1,
                          char *dataout,
                          CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                          int *nc2, int *nr2);
    int gpf_InterpImage16 (short *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                           int nc1, int nr1,
                           short *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                           int *nc2, int *nr2);
    int gpf_InterpImage32 (int *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                           int nc1, int nr1,
                           int *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                           int *nc2, int *nr2);
    int gpf_InterpImageFloat (CSW_F *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                              int nc1, int nr1,
                              CSW_F *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                              int *nc2, int *nr2);
    int gpf_SetIntImageColorBands (int *min, int *max, int *num, int nband);
    int gpf_SetFloatImageColorBands (CSW_F *min, CSW_F *max, int *num, int nband);
    int gpf_SetImageBackgroundColor (int val);
    int gpf_SetSparseImageFlag (int val);
    int gpf_SetIntNullValue (int val);
    int gpf_SetFloatNullValue (CSW_F val);
    int gpf_ConvertToColorImage (const void *datain, int nc, int nr, int type,
                                 unsigned char *dataout, int bgpix);
    int gpf_MirrorColorImageRows (unsigned char *image, int nc, int nr);
    int gpf_InterpImage8B (const char *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                           int nc1, int nr1, int nullval,
                           char *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                           int *nc2, int *nr2);
    int gpf_InterpImage16B (short *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                            int nc1, int nr1, int nullval,
                            short *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                            int *nc2, int *nr2);
    int gpf_InterpImage32B (int *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                            int nc1, int nr1, int nullval,
                            int *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                            int *nc2, int *nr2);
    int gpf_InterpImageFloatB (CSW_F *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                               int nc1, int nr1, CSW_F fnullval,
                               CSW_F *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                               int *nc2, int *nr2);
    int gpf_InterpImage8Row (const char *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                             int nc1, int nr1,
                             char *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                             int *nc2, int *nr2);
    int gpf_InterpImage24Row (const char *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                              int nc1, int nr1,
                              char *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                              int *nc2, int *nr2);
    int gpf_InterpImage8BRow (const char *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                              int nc1, int nr1, int nullval,
                              char *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                              int *nc2, int *nr2);
    int gpf_ConvertToColorImageRow (const void *datain, int nc, int type,
                                    unsigned char *dataout);
    int gpf_InterpImage16BRow (short *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                               int nc1, int nr1, int nullval,
                               short *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                               int *nc2, int *nr2);
    int gpf_InterpImage32BRow (int *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                               int nc1, int nr1, int nullval,
                               int *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                               int *nc2, int *nr2);
    int gpf_InterpImageFloatBRow (CSW_F *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                                  int nc1, int nr1, CSW_F nullval,
                                  CSW_F *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22,
                                  int *nc2, int *nr2);
    int gpf_ImageRotateCCW (const char *datain, int nc, int nr, int type,
                            char **dataout);
    int gpf_ImageRotateCCW24 (const char *datain, int nc, int nr, int type,
                              char **dataout);
    int gpf_ImageReoriginCCW (const char *datain, int nc, int nr, int type,
                              char **dataout);
    int gpf_rotate_color_image (int, unsigned char*, int, int,
                                CSW_F, CSW_F, CSW_F, CSW_F, CSW_F,
                                unsigned char**, int*, int*,
                                CSW_F*, CSW_F*, CSW_F*, CSW_F*);

}; // end of main class

#endif
/*
    end of header file
    add nothing below this endif
*/
