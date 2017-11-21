
/*
         ************************************************
         *                                              *
         *    Copyright (1999-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_image.cc

    Implement the methods of the CSWGrdImage class.
*/

#include <math.h>
#include <float.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/surfaceworks/include/con_shared_structs.h"

/*#include "csw/utils/private_include/simulP.h"*/

#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/ply_utils.h"

#include "csw/surfaceworks/private_include/grd_image.h"



/*
 ******************************************************************

                      F r e e M e m

 ******************************************************************

  Free memory in the private pointers.

*/

void CSWGrdImage::FreeMem(void)
{
    csw_Free (Hcross);
    csw_Free (Dwork);
    csw_Free (Row1Cross);
    csw_Free (Col1Cross);
    csw_Free (Xpoly);

    Hcross = NULL;
    Dwork = NULL;
    Row1Cross = NULL;
    Col1Cross = NULL;
    Xpoly = NULL;
    Ypoly = NULL;
    Npoly = 0;
    Ncomp = NULL;
    Nvert = NULL;

    return;

}  /*  end of private FreeMem function  */





/*
 ******************************************************************

       g r d _ s e t u p _ i m a g e _ c o l o r _ b a n d s

 ******************************************************************

    Set up a color lookup table for translating from data grids
    to color images.  This table will be used by all subsequent
    grd_create_image calls until a new table is set up.

*/

void CSWGrdImage::grd_setup_image_color_bands (CSW_F *cmin, CSW_F *cmax,
                                  int *colors, int nbands)
{
    double           z1, z2, dz;
    int              i, i1, i2, j;

    z1 = 1.e30;
    z2 = -1.e30;

    for (i=0; i<nbands; i++) {
        if (cmin[i] < z1) z1 = cmin[i];
        if (cmax[i] > z2) z2 = cmax[i];
    }

    dz = (z2 - z1) /100.0;
    z1 -= dz;
    z2 += dz;
    dz = (z2 - z1) / 10000.0;

    Zmin = z1;
    Zinc = dz;

    for (i=0; i<10000; i++) {
        ColorTable[i] = COLOR_UNDEFINED;
    }

    for (i=0; i<nbands; i++) {
        i1 = (int)((cmin[i] - z1) / dz);
        i2 = (int)((cmax[i] - z1) / dz);
        i1--;
        i2++;
        if (i1 < 0) i1 = 0;
        if (i2 > 9999) i2 = 9999;
        for (j=i1; j<= i2; j++) {
            ColorTable[j] = colors[i];
        }
    }

    return;

}  /*  end of function grd_setup_image_color_bands  */





/*
 *********************************************************************

              g r d _ c r e a t e _ c l i p _ m a s k

 *********************************************************************

    Create a GRdImage structure that has 1 for all image locations inside
    the specified polygon or zero if outside the specified polygon.  The
    structure is allocated here and should be csw_Freed via grd_FreeImage
    when done.  A pointer to the structure is returned.

*/

GRdImage * CSWGrdImage::grd_create_clip_mask (double *xpoly, double *ypoly,
                                int npoly, int *ncomp, int *nvert,
                                double image_xmin, double image_ymin,
                                double image_xmax, double image_ymax,
                                int image_ncol, int image_nrow)
{
    GRdImage                *result = NULL;
    double                  dx;
    int                     istat, i, j, ntot, n;


    auto fscope = [&]()
    {
        FreeMem();
    };
    CSWScopeGuard func_scope_guard (fscope);


    Xmin = image_xmin;
    Ymin = image_ymin;
    Xmax = image_xmax;
    Ymax = image_ymax;

    dx = (Xmax - Xmin + Ymax - Ymin) / 2.0;
    XminChk = Xmin - dx;
    XmaxChk = Xmax + dx;

    Ncol = image_ncol;
    Nrow = image_nrow;

    Xspace = (Xmax - Xmin) / (double)(Ncol - 1);
    Yspace = (Ymax - Ymin) / (double)(Nrow - 1);
    Tiny = (Xspace + Yspace) / 20.0;

    Xtiny = Tiny * 10.0;
    Xfudge = Tiny * 11.0;

    ntot = 0;
    n = 0;
    for (i=0; i<npoly; i++) {
        for (j=0; j<ncomp[i]; j++) {
            ntot += nvert[n];
            n++;
        }
    }

    Xpoly = (double *)csw_Malloc (2 * ntot * sizeof(double));
    if (Xpoly == NULL) {
        grd_utils_ptr->grd_set_err(1);
        return NULL;
    }
    Ypoly = Xpoly + ntot;

    memcpy (Xpoly, xpoly, ntot * sizeof(double));
    memcpy (Ypoly, ypoly, ntot * sizeof(double));

    Npoly = npoly;

    Ncomp = ncomp;
    Nvert = nvert;

    Hcross = NULL;
    Dwork = NULL;
    Row1Cross = NULL;
    Col1Cross = NULL;

    Hcross = (unsigned int *)csw_Calloc
             (image_ncol*image_nrow*sizeof(unsigned int));
    if (Hcross == NULL) {
        grd_utils_ptr->grd_set_err(1);
        return NULL;
    }

    Dwork = (unsigned char *)csw_Calloc
            (image_ncol*image_nrow*sizeof(unsigned char));
    if (Dwork == NULL) {
        grd_utils_ptr->grd_set_err(1);
        return NULL;
    }

    istat = SetupRow1Col1();
    if (istat == -1) {
        return NULL;
    }

    SetEdges();

    FillMask();

    result = (GRdImage *)csw_Malloc(sizeof(GRdImage));
    if (!result) {
        grd_utils_ptr->grd_set_err(1);
        return NULL;
    }

    result->data = Dwork;
    Dwork = NULL;
    result->ncol = Ncol;
    result->nrow = Nrow;
    result->x1 = image_xmin;
    result->y1 = image_ymin;
    result->x2 = image_xmax;
    result->y2 = image_ymax;
    result->image_type = GRD_CLIP_MASK_IMAGE;

    return result;

}  /*  end of grd_create_clip_mask function  */





/*
 ******************************************************************

             g r d _ c r e a t e _ i m a g e

 ******************************************************************

*/

int CSWGrdImage::grd_create_image (CSW_F *gridin, int ncol, int nrow,
                      double x1, double y1,
                      double x2, double y2,
                      GRdImage *clip_mask,
                      GRdImageOptions *options,
                      FAultLineStruct *faults,
                      int nfaults,
                      GRdImage *output_image)
{
    int          i, j, k, ibase, nc, nr, clook;
    int          istat, nc4, nr4;
    CSW_F        *wgrid = NULL, *grid = NULL, *wgrid2 = NULL;
    unsigned char   *dwork = NULL;
    int             bcolor;
    int             do_write;


    auto fscope = [&]()
    {
        csw_Free (wgrid);
        csw_Free (dwork);
        csw_Free (wgrid2);
        if (grid != gridin) csw_Free (grid);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Check obvious errors.
*/
    if (gridin == NULL  ||  output_image == NULL) {
        grd_utils_ptr->grd_set_err(2);
        return -1;
    }

    if (clip_mask != NULL) {
        istat = CompareImageGeometry (clip_mask,
                                      output_image);
        if (istat != 1) {
            grd_utils_ptr->grd_set_err(5);
            return -1;
        }
    }

    if (ncol < 2  ||  nrow < 2) {
        grd_utils_ptr->grd_set_err(3);
        return -1;
    }

    if (x1 >= x2  ||  y1 >= y2) {
        grd_utils_ptr->grd_set_err(4);
        return -1;
    }

    if (Zinc <= 1.e-20) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

/*
    Make sure faults and nfaults are consistent.
*/
    if (faults == NULL) {
        nfaults = 0;
    }
    else if (nfaults <= 0) {
        faults = NULL;
        nfaults = 0;
    }

/*
    Get options into private variables.
*/
    if (options) {
        ThicknessFlag = options->thickness_flag;
        bcolor = (int)options->background_color;
        NullValue = options->null_value;
        ClipGridMin = options->zmin;
        ClipGridMax = options->zmax;
        ZeroFillFlag = options->zerofillflag;
    }
    else {
        ThicknessFlag = 0;
        bcolor = 0;
        NullValue = 1.e30f;
        ClipGridMin = -1.e30f;
        ClipGridMax = 1.e30f;
        ZeroFillFlag = 0;
    }

/*
    Make sure options are reasonable.
*/
    if (ThicknessFlag < 0  ||  ThicknessFlag > 2) {
        ThicknessFlag = 0;
    }
    if (NullValue > -1.e10  &&  NullValue < 1.e10) {
        NullValue = 1.e30f;
    }
    if (bcolor > MAX_COLOR) bcolor = 0;

    BadColor = (unsigned char)bcolor;

/*
    set the output data to null in case an error occurs.
*/
    output_image->data = NULL;

/*
    Create a work grid for resampling and the output
    image data array.
*/
    nc = output_image->ncol;
    nr = output_image->nrow;

    wgrid = (CSW_F *)csw_Malloc(nc*nr*sizeof(CSW_F));
    if (!wgrid) {
        grd_utils_ptr->grd_set_err(1);
        return -1;
    }

    dwork = (unsigned char *)csw_Calloc(nc*nr*sizeof(unsigned char));
    if (!dwork) {
        grd_utils_ptr->grd_set_err(1);
        return -1;
    }

/*
    Adjust the input grid for the thickness flag if needed.
*/
    if (ThicknessFlag != 0) {
        grid = AdjustForThickness (gridin, ncol, nrow);
    }
    else {
        grid = gridin;
        GridMin = -1.e15f;
        GridMax = 1.e15f;
    }

    if (grid == NULL) grid = gridin;

    do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_fileio_ptr->grd_write_file ("adjust_thick.grd", NULL,
                    grid, NULL, NULL,
                    ncol, nrow,
                    (CSW_F)x1, (CSW_F)y1,
                    (CSW_F)x2, (CSW_F)y2,
                    GRD_NORMAL_GRID_FILE,
                    NULL, 0);
    }

/*
    Resample the grid into another grid with the same
    geometry as the image.
*/
    wgrid2 = NULL;
    if (faults == NULL  ||  nfaults == 0) {
        istat = grd_arith_ptr->grd_resample_grid (grid, NULL, ncol, nrow,
                                   (CSW_F)x1, (CSW_F)y1,
                                   (CSW_F)x2, (CSW_F)y2,
                                   faults, nfaults,
                                   wgrid, NULL,
                                   nc, nr,
                                   (CSW_F)output_image->x1,
                                   (CSW_F)output_image->y1,
                                   (CSW_F)output_image->x2,
                                   (CSW_F)output_image->y2,
                                   GRD_BICUBIC);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err(1);
            return -1;
        }
    }

/*
    Faulted grids take considerably longer to resample.  So, I resample
    into a grid that is effectively 2 pixel resolution using faults and
    then I resample the output of the faulted pass without faults.
*/
    else {
        nc4 = nc / 2;
        nr4 = nr / 2;
        if (nc4 > ncol  ||  nr4 > nrow) {
            wgrid2 = (CSW_F *)csw_Malloc (nc4 * nr4 * sizeof(CSW_F));
            if (wgrid2 == NULL) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            istat = grd_arith_ptr->grd_resample_grid (grid, NULL, ncol, nrow,
                                       (CSW_F)x1, (CSW_F)y1,
                                       (CSW_F)x2, (CSW_F)y2,
                                       faults, nfaults,
                                       wgrid2, NULL,
                                       nc4, nr4,
                                       (CSW_F)output_image->x1,
                                       (CSW_F)output_image->y1,
                                       (CSW_F)output_image->x2,
                                       (CSW_F)output_image->y2,
                                       GRD_BICUBIC);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err(1);
                return -1;
            }
            istat = grd_arith_ptr->grd_resample_grid (wgrid2, NULL, nc4, nr4,
                                       (CSW_F)output_image->x1,
                                       (CSW_F)output_image->y1,
                                       (CSW_F)output_image->x2,
                                       (CSW_F)output_image->y2,
                                       NULL, 0,
                                       wgrid, NULL,
                                       nc, nr,
                                       (CSW_F)output_image->x1,
                                       (CSW_F)output_image->y1,
                                       (CSW_F)output_image->x2,
                                       (CSW_F)output_image->y2,
                                       GRD_BICUBIC);
            csw_Free (wgrid2);
            wgrid2 = NULL;
            if (istat == -1) {
                grd_utils_ptr->grd_set_err(1);
                return -1;
            }
        }
        else {
            istat = grd_arith_ptr->grd_resample_grid (grid, NULL, ncol, nrow,
                                       (CSW_F)x1, (CSW_F)y1,
                                       (CSW_F)x2, (CSW_F)y2,
                                       NULL, 0,
                                       wgrid, NULL,
                                       nc, nr,
                                       (CSW_F)output_image->x1,
                                       (CSW_F)output_image->y1,
                                       (CSW_F)output_image->x2,
                                       (CSW_F)output_image->y2,
                                       GRD_BICUBIC);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err(1);
                return -1;
            }
        }
    }

#if DEBUG_WRITE_FILE
    grd_fileio_ptr->grd_write_file ("debug2.grd", NULL,
                    wgrid, NULL, NULL,
                    nc, nr,
                    (CSW_F)output_image->x1, (CSW_F)output_image->y1,
                    (CSW_F)output_image->x2, (CSW_F)output_image->y2,
                    GRD_NORMAL_GRID_FILE,
                    NULL, 0);
#endif

/*
 *  Fix overshoots that may have happened from the bicubic interpolation.
 *  For example, if an input grid had an upper limit of 1, and the input
 *  grid cell has 4 corners all at 1, then none of the interpolated points
 *  inside the cell should be values other than 1.
 */
    FixOvershoots (wgrid, nc, nr,
                   output_image->x1,
                   output_image->y1,
                   output_image->x2,
                   output_image->y2,
                   grid,
                   ncol, nrow,
                   x1, y1, x2, y2);

#if DEBUG_WRITE_FILE
    grd_fileio_ptr->grd_write_file ("debug3.grd", NULL,
                    wgrid, NULL, NULL,
                    nc, nr,
                    (CSW_F)output_image->x1, (CSW_F)output_image->y1,
                    (CSW_F)output_image->x2, (CSW_F)output_image->y2,
                    GRD_NORMAL_GRID_FILE,
                    NULL, 0);
#endif


/*
    Look up the color for each node in the resampled grid.
*/
    for (i=0; i<nr; i++) {
        ibase = i * nc;
        for (j=0; j<nc; j++) {
            k = ibase + j;
            if (clip_mask != NULL) {
                if (clip_mask->data[k] == 0) {
                    dwork[k] = BadColor;
                }
                else {
                    clook = LookupColor (wgrid[k]);
                    dwork[k] = (unsigned char)clook;
                }
            }
            else {
                clook = LookupColor (wgrid[k]);
                dwork[k] = (unsigned char)clook;
            }
        }
    }

    output_image->data = dwork;
    dwork = NULL;

    return 1;

}  /*  end of grd_create_image function  */





/*
 ******************************************************************

                   S e t u p R o w 1 C o l 1

 ******************************************************************

*/

int CSWGrdImage::SetupRow1Col1 (void)
{
    double        x1, y1, x2, y2, dx;
    int           i, j, n, n2, j1, j2;

    n = 0;
    n2 = 0;
    for (i=0; i<Npoly; i++) {
        for (j=0; j<Ncomp[i]; j++) {
            n2 += Nvert[n];
            n++;
        }
    }

    x1 = 1.e30;
    x2 = -1.e30;
    y1 = 1.e30;
    y2 = -1.e30;
    for (i=0; i<n2; i++) {
        if (Xpoly[i] < x1) x1 = Xpoly[i];
        if (Xpoly[i] > x2) x2 = Xpoly[i];
        if (Ypoly[i] < y1) y1 = Ypoly[i];
        if (Ypoly[i] > y2) y2 = Ypoly[i];
    }
    if (x1 >= x2) {
        grd_utils_ptr->grd_set_err(2);
        return -1;
    }
    if (y1 >= y2) {
        grd_utils_ptr->grd_set_err(2);
        return -1;
    }

/*
 * If the image is entirely outside the polygon,
 * do not set up the row1 and col1 crossing arrays.
 */
    if (x2 < Xmin  ||  x1 > Xmax  ||
        y2 < Ymin  ||  y1 > Ymax) {
        if (Row1Cross) csw_Free(Row1Cross);
        if (Col1Cross) csw_Free(Col1Cross);
        Row1Cross = NULL;
        Col1Cross = NULL;
        Jrow1 = 0;
        Jrow2 = 0;
        return 1;
    }

/*
 * If the polygon extents are larger than the image extents,
 * set them to the image extents plus a border.
 */
    if (x1 < XminChk) {
        x1 = XminChk;
    }
    if (x2 > XmaxChk) {
        x2 = XmaxChk;
    }

    dx = (x2  - x1) / 100.0;
    x1 -= dx;
    x2 += dx;

    j1 = (int)((x1 - Xmin) / Xspace);
    j1 -= 2;
    j2 = (int)((x2 - Xmin) / Xspace);
    j2 += 2;

    Jrow1 = j1;
    Jrow2 = j2;

    Row1Cross = (unsigned int *)csw_Calloc
                ((j2 - j1 + 2) * sizeof(unsigned int));
    if (Row1Cross == NULL) {
        grd_utils_ptr->grd_set_err(1);
        return -1;
    }

    Col1Cross = (unsigned int *)csw_Calloc
                ((Nrow + 2) * sizeof(unsigned int));
    if (Col1Cross == NULL) {
        grd_utils_ptr->grd_set_err(1);
        return -1;
    }

    return 1;

}  /*  end of private SetupRow1Col1 function  */





/*
 ******************************************************************

                       S e t E d g e s

 ******************************************************************

    Loop through all the edges  in the polygon(s) and set the spot
    in the image where they each cross each row in the image.  The
    Hcross array has a count of the number of edge crossings at each
    position in the image.

*/

int CSWGrdImage::SetEdges(void)
{
    int              i, j, k, n1, n2;
    double           x1, y1, x2, y2;

    if (Row1Cross == NULL  ||  Col1Cross == NULL) {
        return 1;
    }

    n1 = 0;
    n2 = 0;
    for (i=0; i<Npoly; i++) {
        for (j=0; j<Ncomp[i]; j++) {
            for (k=0; k<Nvert[n1]-1; k++) {
                n2++;
                AdjustForLeftSide(Xpoly+n2-1, Xpoly+n2);
            }
            n1++;
            n2++;
        }
    }

    n1 = 0;
    n2 = 0;
    for (i=0; i<Npoly; i++) {
        for (j=0; j<Ncomp[i]; j++) {
            for (k=0; k<Nvert[n1]-1; k++) {
                x1 = Xpoly[n2];
                y1 = Ypoly[n2];
                n2++;
                x2 = Xpoly[n2];
                y2 = Ypoly[n2];
                SetEdge(x1, y1, x2, y2);
                SetRow1Crossing(x1, y1, x2, y2);
                SetCol1Crossing(x1, y1, x2, y2);
            }
            n1++;
            n2++;
        }
    }

    return 1;

}  /*  end of private SetEdges function  */




/*
 ******************************************************************

               A d j u s t F o r L e f t S i d e

 ******************************************************************

  The x coordinates of a polygon segment's endpoints are passed to
  this function.  If both endpoints are within aproximately one
  pixel of the left side of the image, the first endpoint is moved
  to be more than one pixel from the left side.  The movement is to
  the left if the original point is less than the left side or to
  the right if the original point is more than the right side.

  This is done with all polygon segments prior to calculating the
  column 1 crossings.  Doing this insures that no nearly vertical
  segments graze the column 1 image sides.  Such grazing can skew
  the intersection calculations and mess up the inside/outside
  determination for the start of an image row.
*/

void CSWGrdImage::AdjustForLeftSide (double *x1p, double *x2p)
{
    double        xt1, x1;
    double        xt2, x2;
    double        tiny2;

    x1 = *x1p;
    x2 = *x2p;

    xt1 = x1 - Xmin;
    xt2 = x2 - Xmin;

    if (xt1 < 0.0) xt1 = -xt1;
    if (xt2 < 0.0) xt2 = -xt2;

/*
 * If both x endpoints are quite close to the minimum x,
 * the segment is for all practical purposes a vertical
 * segment that may or may not intersect the xmin edge.
 * Change the end point values to be far enough
 * from the xmin to make the intersection calculation
 * definitive.
 */
    if (xt1 <= Xtiny  &&  xt2 <= Xtiny) {
        if (x1 >= Xmin) {
            x1 += Xfudge;
        }
        else {
            x1 -= Xfudge;
        }
        if (x2 >= Xmin) {
            x2 += Xfudge;
        }
        else {
            x2 -= Xfudge;
        }
    }

/*
 * If one endpoint is very close to xmin and the other is not, move
 * the very close endpoint farther away from xmin.
 */
    else {
        tiny2 = Xtiny / 10.0;
        if (xt1 <= tiny2) {
            if (x1 >= Xmin) {
                x1 += Xfudge;
            }
            else {
                x1 -= Xfudge;
            }
        }
        if (xt2 <= tiny2) {
            if (x2 >= Xmin) {
                x2 += Xfudge;
            }
            else {
                x2 -= Xfudge;
            }
        }
    }

    *x1p = x1;
    *x2p = x2;

    return;

}  /*  end of private AdjustForLeftSide function  */




/*
 ******************************************************************

                     S e t E d g e

 ******************************************************************

    Set the edge crossings in the Hcross array for a single edge.

*/

void CSWGrdImage::SetEdge(double x1, double y1,
                    double x2, double y2)
{
    int             i, j, i1, i2, j1, j2, ibase;
    double          xt, yt, dx, dy, slope, yint;

    i1 = (int)((y1 - Ymin) / Yspace);
    i2 = (int)((y2 - Ymin) / Yspace);

    if (i1 == i2) {
        return;
    }

    if (i1 > i2) {
        j = i1;
        i1 = i2;
        i2 = j;
    }

    i1++;

    if (i1 < 0) i1 = 0;
    if (i2 > Nrow-1) i2  = Nrow-1;
    dx = x2 - x1;
    dy = y2 - y1;

    if (dy == 0.0) {
        return;
    }

    j1 = (int)((x1 - Xmin) / Xspace);
    j2 = (int)((x2 - Xmin) / Xspace);

    if (dx == 0.0  ||  j1 == j2) {
        if (x1 < XminChk  ||  x1 > XmaxChk) {
            return;
        }
        j = (int)((x1 - Xmin) / Xspace);
        if (j < 0  ||  j > Ncol-1) {
            return;
        }
        for (i=i1; i<=i2; i++) {
            Hcross[i*Ncol+j]++;
        }
    }

    else {
        dy = TinyFudge(dy);
        dx = TinyFudge(dx);
        slope = dy / dx;
        yint = y1 - slope * x1;
        for (i=i1; i<=i2; i++) {
            ibase = i * Ncol;
            yt = i * Yspace + Ymin;
            xt = (yt - yint) / slope;
            if (xt < XminChk  ||  xt > XmaxChk) {
                continue;
            }
            j = (int)((xt - Xmin) / Xspace);
            if (xt < Xmin) j--;
            if (j < 0  ||  j >= Ncol) {
                continue;
            }
            Hcross[ibase+j]++;
        }
    }

    return;

}  /*  end of private SetEdge function  */





/*
 ******************************************************************

               S e t R o w 1 C r o s s i n g

 ******************************************************************

   Set the crossing flags on the lowermost row of the image.  The row
   is extended to the limits of the polygon (plus a bit).  This way,
   the first point in the row array is always outside the polygon, and
   crossings can be counted between that point and the first point in
   the actual image to determine if the lower left pixel in the image
   is inside or outside the polygon(s).

   One edge at a time is passed to this.

*/

void CSWGrdImage::SetRow1Crossing(double x1, double y1,
                            double x2, double y2)
{
    int              j, j1, j2;
    double           xt, dx, dy, slope, yint;

    if (y1 > Ymin  &&  y2 > Ymin) {
        return;
    }
    if (y1 < Ymin  &&  y2 < Ymin) {
        return;
    }

    if (Row1Cross == NULL) {
        return;
    }

    dx = x2 - x1;
    dy = y2 - y1;

    if (dy == 0.0) {
        return;
    }

    j1 = (int)((x1 - Xmin) / Xspace);
    j2 = (int)((x2 - Xmin) / Xspace);

    if (dx == 0.0  ||  j1 == j2) {
        xt = x1;
    }
    else {
        dy = TinyFudge(dy);
        dx = TinyFudge(dx);
        slope = dy / dx;
        yint = y1 - x1 * slope;
        xt = (Ymin - yint) / slope;
    }

    if (xt < XminChk  ||  xt > XmaxChk) {
        return;
    }

    j = (int)((xt - Xmin) / Xspace);
    if (xt < Xmin) j--;

    if (j < Jrow1 ||  j >= Jrow2) return;

    j -= Jrow1;
    Row1Cross[j]++;

    return;

}  /*  end of private SetRow1Crossing function  */





/*
 ******************************************************************

               S e t C o l 1 C r o s s i n g

 ******************************************************************

   Set the crossing flags on the leftmost column of the image.  This
   information is used in conjunction with the in/out flag of the
   lower left pixel to determine whether the first pixel in each row
   is inside or outside the polygon(s).

   One edge at a time is passed to this.

*/

void CSWGrdImage::SetCol1Crossing(double x1, double y1,
                            double x2, double y2)
{
    int              i, istat;
    double           xint, yint;

    CSWPolyUtils     ply_utils_obj;

    if (Col1Cross == NULL) {
        return;
    }

    if (x1 > Xmin + Xtiny  &&  x2 > Xmin + Xtiny) {
        return;
    }
    if (x1 < Xmin - Xtiny  &&  x2 < Xmin - Xtiny) {
        return;
    }

/*
 * Check if the specified line segment actually intersects the
 * left edge of the image.
 */
    istat =
        ply_utils_obj.ply_segint (x1, y1, x2, y2, Xmin, Ymin, Xmin, Ymax, &xint, &yint);

    if (istat != 0) {
        return;
    }

    i = (int)((yint - Ymin) / Yspace);
    if (i < 0  ||  i >= Nrow) {
        return;
    }

    Col1Cross[i]++;

    return;

}  /*  end of private SetCol1Crossing function  */




/*
 ******************************************************************

                    F i l l M a s k

 ******************************************************************

    Do a flood fill to create the polygon mask.

*/

void CSWGrdImage::FillMask(void)
{
    int                i, j, k, startio, ncross, ncross_col1, ibase;

    if (Row1Cross == NULL  ||  Col1Cross == NULL) {
        return;
    }

    startio = FirstInside();

    ncross_col1 = startio;
    for (i=0; i<Nrow; i++) {
        ncross = startio;
        ibase = i * Ncol;
        Dwork[ibase] = (unsigned char)(ncross % 2);
        for (j=0; j<Ncol-1; j++) {
            k = ibase + j;
            ncross += Hcross[k];
            Dwork[k+1] = (unsigned char)(ncross % 2);
        }
        ncross_col1 += Col1Cross[i];
        startio = ncross_col1 % 2;
    }

    return;

}  /*  end of private FillMask function  */






/*
 ******************************************************************

                   F i r s t I n s i d e

 ******************************************************************

  Use the coordinates of the lower left corner of the image to
  see if that point is inside the polygon.

*/

int CSWGrdImage::FirstInside(void)
{
    int           istat, i, j, n, hstart;
    double        **xv, **yv;

    CSWPolyUtils  ply_utils_obj;
    n = 0;
    for (i=0; i<Npoly; i++) {
        for (j=0; j<Ncomp[i]; j++) {
            n++;
        }
    }
    n++;
    xv = (double **)csw_Calloc (n * 2 * sizeof(double *));
    if (xv == NULL) {
        return 1;
    }
    yv = xv + n;

    n = 0;
    hstart = 0;
    for (i=0; i<Npoly; i++) {
        for (j=0; j<Ncomp[i]; j++) {
            xv[j] = Xpoly + n;
            yv[j] = Ypoly + n;
            n += Nvert[hstart+j];
        }
        istat = ply_utils_obj.ply_pointpa (xv,
                             yv,
                             Ncomp[i],
                             Nvert+hstart,
                             Xmin,
                             Ymin);
        if (istat != -1) {
            csw_Free (xv);
            return 1;
        }
        hstart += Ncomp[i];
    }

    csw_Free (xv);

    return 0;

}  /*  end of private FirstInside function  */





/*
 ******************************************************************

                   T i n y F u d g e

 ******************************************************************

*/

double CSWGrdImage::TinyFudge (double val)
{
    if (val > -Tiny  &&  val <= 0.0) {
        val = -Tiny;
    }
    if (val > 0.0  &&  val < Tiny) {
        val = -Tiny;
    }
    return val;

}  /*  end of private TinyFudge function  */






/*
 ******************************************************************

           C o m p a r e I m a g e G e o m e t r y

 ******************************************************************

    Compare the two images.  Return 1 if almost identical or
    zero if different.

*/

int CSWGrdImage::CompareImageGeometry (GRdImage *i1, GRdImage *i2)
{
    double          tiny;

    if (i1->ncol != i2->ncol) {
        return 0;
    }
    if (i1->nrow != i2->nrow) {
        return 0;
    }
    if (i1->ncol < 2  ||  i1->nrow < 2) {
        return 0;
    }

    tiny = (i1->x2 - i1->x1 + i1->y2 - i1->y1) /
           (double)(i1->ncol + i1->nrow);
    tiny /= 2.0;

    if (i1->x1 - i2->x1 < -tiny  ||
        i1->x1 - i2->x1 > tiny) {
        return 0;
    }

    if (i1->x2 - i2->x2 < -tiny  ||
        i1->x2 - i2->x2 > tiny) {
        return 0;
    }

    if (i1->y1 - i2->y1 < -tiny  ||
        i1->y1 - i2->y1 > tiny) {
        return 0;
    }

    if (i1->y2 - i2->y2 < -tiny  ||
        i1->y2 - i2->y2 > tiny) {
        return 0;
    }

    i2->x1 = i1->x1;
    i2->y1 = i1->y1;
    i2->x2 = i1->x2;
    i2->y2 = i1->y2;

    return 1;

}  /*  end of private CompareImageGeometry function  */






/*
 ******************************************************************

                   L o o k u p C o l o r

 ******************************************************************

    Return the color number for a grid value.

*/

int CSWGrdImage::LookupColor (CSW_F val)
{
    int              i, cn;

/*
 *  Bug 7902, need to return bad color if the value is null.
 */
    if (val > NullValue / 100.0f) {
        return (int)BadColor;
    }

    if (GridMin > GridMax) {
        return (int)BadColor;
    }

    if (ZeroFillFlag == 0) {
        if (val < ClipGridMin) {
            return (int)BadColor;
        }
    }

    if (val < ClipGridMin) val = ClipGridMin;
    if (val > ClipGridMax) val = ClipGridMax;

    i = (int)((val - Zmin) / Zinc + 0.5f);
    if (i < 0  ||  i >= 10000) {
        return (int)BadColor;
    }

    cn = ColorTable[i];
    if (cn == COLOR_UNDEFINED) {
        return (int)BadColor;
    }

    return cn;

}  /*  end of private LookupColor function  */




/*
 ******************************************************************

              A d j u s t F o r T h i c k n e s s

 ******************************************************************

*/

CSW_F* CSWGrdImage::AdjustForThickness (CSW_F *grid,
                                  int ncol, int nrow)
{
    CSW_F        *tgrid = NULL, cnull, gmin;
    int          i, istat;

    bool      bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (tgrid);
            FreeMem ();
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    GridMin = 1.e30f;
    GridMax = -1.e30f;
    cnull = NullValue / 10.0f;

    for (i=0; i<ncol*nrow; i++) {
        if (grid[i] < cnull) {
            if (grid[i] < GridMin) GridMin = grid[i];
            if (grid[i] > GridMax) GridMax = grid[i];
        }
    }

    tgrid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (!tgrid) {
        grd_utils_ptr->grd_set_err (1);
        return NULL;
    }
    memcpy (tgrid, grid, ncol * nrow * sizeof(CSW_F));

    if (ThicknessFlag == CON_POSITIVE_THICKNESS) {
        gmin = 0.0f;
        istat = grd_utils_ptr->grd_fill_valley (tgrid, ncol, nrow,
                                 gmin, NullValue, GridMax - GridMin);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return NULL;
        }
        if (GridMin < 0.0) GridMin = 0.0f;
    }

    if (ThicknessFlag == CON_NEGATIVE_THICKNESS) {
        gmin = 0.0f;
        istat = grd_utils_ptr->grd_fill_plateau (tgrid, ncol, nrow,
                                  gmin, NullValue, GridMax - GridMin);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return NULL;
        }
        if (GridMax > 0.0) GridMax = 0.0f;
    }

    bsuccess = true;

    return tgrid;

}  /*  end of private AdjustForThickness function  */




/*
 **********************************************************************************

                           F i x O v e r s h o o t s

 **********************************************************************************

  If any grid node that lies inside a basegrid cell has a value outside the range
  of the basegrid cell and the basegrid cell is on a plateau or valley limit of
  the grid (all 4 corners at the ClipGridMax or ClipGridMin value), then set the
  grid node value to the appropriate clip value.

  This removes artifacts introduced near the edges of data from bicubic interpolation.

*/

void CSWGrdImage::FixOvershoots (CSW_F *grid,
                           int nc, int nr,
                           double x1, double y1, double x2, double y2,
                           CSW_F *basegrid,
                           int ncbase, int nrbase,
                           double x1base, double y1base,
                           double x2base, double y2base)
{
    int           ibase, jbase, kbase, i, j, k, offset, baseoffset;
    int           ntop, nbottom;
    double        xt, yt, xspace, yspace, basexspace, baseyspace;
    double        yb1, yb2;
    CSW_F         z1, zlow, zhigh, ztiny;
    CSW_F         gmin, gmax;

/*
 *  Don't bother with this if the grid is not being clipped.
 */
    if (ClipGridMin < -1.e20  &&  ClipGridMax > 1.e20) return;

/*
 *  Find the min and max of the base grid and use it
 *  to adjust the clip limits to slightly inside the
 *  original clip limits.
 */
    gmin = 1.e20f;
    gmax = -1.e20f;
    for (i=0; i<ncbase*nrbase; i++) {
        if (basegrid[i] < 1.e20f) {
            if (basegrid[i] < gmin) gmin = basegrid[i];
            if (basegrid[i] > gmax) gmax = basegrid[i];
        }
    }
    ztiny = (gmax - gmin) / 100000.0f;

    zlow = ClipGridMin;
    if (ClipGridMin > -1.e20) {
        zlow += ztiny;
    }
    zhigh = ClipGridMax;
    if (ClipGridMax < 1.e20) {
        zhigh -= ztiny;
    }

/*
 *  Get the node spacings for both grids.
 */
    xspace = (x2 - x1) / (double)(nc - 1);
    yspace = (y2 - y1) / (double)(nr - 1);
    basexspace = (x2base - x1base) / (double)(ncbase - 1);
    baseyspace = (y2base - y1base) / (double)(nrbase - 1);

    yb1 = y1base - yspace / 10.0;
    yb2 = y2base + yspace / 10.0;

/*
 *  Loop through the grid, and find the basegrid cell for each
 *  node in the grid.  If the node is outside the basegrid limits,
 *  do nothing with the node.
 */
    for (i=0; i<nr; i++) {
        yt = y1 + i * yspace;
        ibase = (int)((yt - y1base) / baseyspace);
        if (ibase < 0  &&  yt > yb1) ibase = 0;
        if (ibase > nrbase - 2  &&  yt < yb2) ibase = nrbase - 2;
        if (ibase < 0  ||  ibase > nrbase - 2) continue;
        offset = i * nc;
        baseoffset = ibase * ncbase;

        for (j=0; j<nc; j++) {
            xt = x1 + j * xspace;
            jbase = (int)((xt - x1base) / basexspace);
            if (jbase < 0  ||  jbase >= ncbase - 1) continue;
            k = offset + j;
            if (grid[k] > 1.e20) continue;
            kbase = baseoffset + jbase;
            ntop = 0;
            nbottom = 0;
            z1 = basegrid[kbase];
            if (z1 > 1.e30f) continue;
            if (z1 <= zlow) nbottom++;
            if (z1 >= zhigh) ntop++;
            z1 = basegrid[kbase+1];
            if (z1 > 1.e30f) continue;
            if (z1 <= zlow) nbottom++;
            if (z1 >= zhigh) ntop++;
            z1 = basegrid[kbase+ncbase];
            if (z1 > 1.e30f) continue;
            if (z1 <= zlow) nbottom++;
            if (z1 >= zhigh) ntop++;
            z1 = basegrid[kbase+ncbase+1];
            if (z1 > 1.e30f) continue;
            if (z1 <= zlow) nbottom++;
            if (z1 >= zhigh) ntop++;

        /*
         *  If all 4 corners are at the min or max,
         *  all nodes in the cell should also be
         *  at or outside of the min/max range.
         */
            if (ntop == 4) {
                if (grid[k] < ClipGridMax) {
                    grid[k] = ClipGridMax;
                }
                continue;
            }
            else if (nbottom == 4) {
                if (grid[k] > ClipGridMin) {
                    grid[k] = ClipGridMin;
                }
                continue;
            }

        /*
         * If no corner is less than the clip min,
         * make sure the pixel is also not less than
         * the clip min.  Do similarly for the top.
         */
            if (nbottom == 0) {
                if (grid[k] < ClipGridMin + ztiny) {
                    grid[k] = ClipGridMin + ztiny;
                }
            }
            if (ntop == 0) {
                if (grid[k] > ClipGridMax - ztiny) {
                    grid[k] = ClipGridMax - ztiny;
                }
            }

        }
    }

    return;

}  /* end of private FixOvershoots function */


/*----------------------------------------------------------------*/

/*
 */
int CSWGrdImage::grd_create_ndp_colors (
    int *v1, int *v2, int *v3, int *v4,
    double *f1, double *f2, double *f3, double *f4,
    int ncol, int nrow,
    int nvals, int *vals,
    int *red, int *green, int *blue, int *alpha,
    CSW_F **rgrid_out,
    CSW_F **ggrid_out,
    CSW_F **bgrid_out,
    CSW_F **agrid_out)
{
    CSW_F          *rgrid = NULL, *ggrid = NULL,
                   *bgrid = NULL, *agrid = NULL;
    CSW_F          fr, fg, fb, fa;
    int            i, j, k, offset, istat, ntot;

    bool      bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (rgrid);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * First, blend the values at each input grid node to
 * get red, green, blue and alpha values at each input node.
 */
    ntot = ncol * nrow;

    rgrid = (CSW_F *)csw_Malloc (ntot * 4 * sizeof(CSW_F));
    if (rgrid == NULL) {
        return -1;
    }
    ggrid = rgrid + ntot;
    bgrid = ggrid + ntot;
    agrid = bgrid + ntot;

    for (i=0; i<nrow; i++) {
        offset = ncol * i;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            istat =
              BlendColors (
                v1[k], v2[k], v3[k], v4[k],
                f1[k], f2[k], f3[k], f4[k],
                nvals, vals, red, green, blue, alpha,
                &fr, &fg, &fb, &fa);
            if (istat != 1) {
                rgrid[k] = 0.0f;
                ggrid[k] = 0.0f;
                bgrid[k] = 0.0f;
                agrid[k] = 0.0f;
            }
            else {
                rgrid[k] = fr;
                ggrid[k] = fg;
                bgrid[k] = fb;
                agrid[k] = fa;
            }
        }
    }

    *rgrid_out = rgrid;
    *ggrid_out = ggrid;
    *bgrid_out = bgrid;
    *agrid_out = agrid;

    bsuccess = true;

    return 1;
}


int CSWGrdImage::BlendColors (
    int v1, int v2, int v3, int v4,
    double f1, double f2, double f3, double f4,
    int nvals, int *vals,
    int *red, int *green, int *blue, int *alpha,
    CSW_F *fr, CSW_F *fg, CSW_F *fb, CSW_F *fa)
{
    double        rsum1, gsum1, bsum1, asum1, sum2;
    int           i;

    *fr = *fg = *fb = *fa = 0.0f;

    rsum1 = 0.0;
    gsum1 = 0.0;
    bsum1 = 0.0;
    asum1 = 0.0;
    sum2 = 0.0;

    if (v1 >= 0) {
        for (i=0; i<nvals; i++) {
            if (vals[i] == v1) {
                rsum1 += red[i] * f1;
                gsum1 += green[i] * f1;
                bsum1 += blue[i] * f1;
                asum1 += alpha[i] * f1;
                sum2 += f1;
                break;
            }
        }
    }

    if (v2 >= 0) {
        for (i=0; i<nvals; i++) {
            if (vals[i] == v2) {
                rsum1 += red[i] * f2;
                gsum1 += green[i] * f2;
                bsum1 += blue[i] * f2;
                asum1 += alpha[i] * f2;
                sum2 += f2;
                break;
            }
        }
    }

    if (v3 >= 0) {
        for (i=0; i<nvals; i++) {
            if (vals[i] == v3) {
                rsum1 += red[i] * f3;
                gsum1 += green[i] * f3;
                bsum1 += blue[i] * f3;
                asum1 += alpha[i] * f3;
                sum2 += f3;
                break;
            }
        }
    }

    if (v4 >= 0) {
        for (i=0; i<nvals; i++) {
            if (vals[i] == v4) {
                rsum1 += red[i] * f4;
                gsum1 += green[i] * f4;
                bsum1 += blue[i] * f4;
                asum1 += alpha[i] * f4;
                sum2 += f4;
                break;
            }
        }
    }

    if (sum2 < 0.001) {
        return 1;
    }

    *fr = (CSW_F)(rsum1 / sum2);
    *fg = (CSW_F)(gsum1 / sum2);
    *fb = (CSW_F)(bsum1 / sum2);
    *fa = (CSW_F)(asum1 / sum2);

    return 1;

}
