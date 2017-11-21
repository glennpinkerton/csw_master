
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_image.h

    Define the CSWGrdImage class, which refactors the old grd_image.c code.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file grd_protoP.h.
#endif


/*
    Add nothing above this line
*/
#ifndef GRD_IMAGE_H
#  define GRD_IMAGE_H

#  define MIN_COLS           2
#  define MIN_ROWS           2

#  include "csw/surfaceworks/include/grd_shared_structs.h"


#define DEBUG_WRITE_FILE  0

#define MAX_COLOR         255
#define COLOR_UNDEFINED   -99999


class CSWGrdImage;

#include "csw/surfaceworks/private_include/grd_utils.h"
#include "csw/surfaceworks/private_include/grd_fileio.h"
#include "csw/surfaceworks/private_include/grd_arith.h"

class CSWGrdImage
{

  private:

    CSWGrdArith    *grd_arith_ptr = NULL;
    CSWGrdFileio   *grd_fileio_ptr = NULL;
    CSWGrdUtils    *grd_utils_ptr = NULL;

  public:

    CSWGrdImage () {};
    ~CSWGrdImage () {};

    void SetGrdArithPtr (CSWGrdArith *p) {grd_arith_ptr = p;};
    void SetGrdFileioPtr (CSWGrdFileio *p) {grd_fileio_ptr = p;};
    void SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};

    void grd_setup_image_color_bands
              (CSW_F *cmin, CSW_F *cmax, int *colors, int nbands);

    GRdImage *grd_create_clip_mask
        (double *xpoly, double *ypoly,
         int npoly, int *ncomp, int *nvert,
         double image_xmin, double image_ymin,
         double image_xmax, double image_ymax,
         int image_ncol, int image_nrow);

    int grd_create_image (CSW_F *gridin, int ncol, int nrow,
                          double x1, double y1, double x2, double y2,
                          GRdImage *clip_mask, GRdImageOptions *options,
                          FAultLineStruct *faults, int nfaults,
                          GRdImage *output_image);

    int grd_create_ndp_colors (
        int *v1, int *v2, int *v3, int *v4,
        double *f1, double *f2, double *f3, double *f4,
        int ncol, int nrow,
        int nvals, int *vals,
        int *red, int *green, int *blue, int *alpha,
        CSW_F **rgrid_out,
        CSW_F **ggrid_out,
        CSW_F **bgrid_out,
        CSW_F **agrid_out);


  private:



/*
    Old static file variables are now private class variables.
*/

/*
 * The XminChk etc. variables are set to values about 3 times as
 * large as the xmin, ymin etc. extents.  The only reason for this
 * is to check if the bounds of a polygon which clips the
 * image is much larger than the image.  If so, then some
 * simple special handling is done to keep an integer from
 * possibly being outside 32 bit integer range.
 */
    double          Xmin = 0.0, XminChk = 0.0,
                    Ymin = 0.0,
                    Xmax = 0.0, XmaxChk = 0.0,
                    Ymax = 0.0,
                    Xspace = 0.0,
                    Yspace = 0.0;
    double          Xtiny = 0.0,
                    Xfudge = 0.0;
    double          *Xpoly = NULL,
                    *Ypoly = NULL;
    double          Tiny = 0.0;
    int             Ncol = 0,
                    Nrow = 0,
                    Npoly = 0,
                    *Ncomp = NULL,
                    *Nvert = NULL;
    unsigned int    *Hcross = NULL,
                    *Row1Cross = NULL,
                    *Col1Cross = NULL;
    int             Jrow1 = 0,
                    Jrow2 = 0;
    unsigned char   *Dwork = NULL;

    int             ColorTable[10000];
    unsigned char   BadColor = 0;
    double          Zmin = 0.0,
                       Zinc = 0.0;

    int             ThicknessFlag = 0;
    CSW_F           GridMin = 0.0,
                    GridMax = 0.0,
                    NullValue = 0.0;
    CSW_F           ClipGridMin = -1.e30f,
                    ClipGridMax = 1.e30f;

    int             ZeroFillFlag = 0;

/*
    Old static file functions are now private class methods.
*/

    int BlendColors (
    int v1, int v2, int v3, int v4,
    double f1, double f2, double f3, double f4,
    int nvals, int *vals,
    int *red, int *green, int *blue, int *alpha,
    CSW_F *fr, CSW_F *fg, CSW_F *fb, CSW_F *fa);

    int SetupRow1Col1(void);
    int SetEdges(void);
    void SetEdge(double x1, double y1, double x2, double y2);
    void SetRow1Crossing(double x1, double y1, double x2, double y2);
    void SetCol1Crossing(double x1, double y1, double x2, double y2);
    void FillMask(void);
    void FreeMem(void);
    int FirstInside(void);
    double TinyFudge(double val);
    int CompareImageGeometry (GRdImage *i1, GRdImage *i2);
    int LookupColor(CSW_F val);
    CSW_F* AdjustForThickness (CSW_F *grid, int ncol, int nrow);
    void AdjustForLeftSide(double *x1, double *x2);
    void FixOvershoots (CSW_F *grid,
                           int ncol,
                           int nrow,
                           double x1,
                           double y1,
                           double x2,
                           double y2,
                           CSW_F *basegrid,
                           int ncbase,
                           int nrbase,
                           double x1base,
                           double y1base,
                           double x2base,
                           double y2base);



};  // end of main class



/*
    Add nothing to this file below the following endif
*/
#endif
