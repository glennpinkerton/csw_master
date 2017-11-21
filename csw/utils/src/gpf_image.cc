
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_image.cc

    This file has general purpose image manipulation functions.
    These can be used from a device specific layer to help with the
    processing and display of image data.

*/

#include <stdio.h>
#include <math.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/gpf_image.h"


/*
  ****************************************************************

            g p f _ I n t e r p C o l o r I m a g e

  ****************************************************************

    Create a subimage of the specified color image at a specified
  image geometry.  If the subimage has a larger cell size than the
  original image, a copy of the original image is subdivided until 
  it has a larger cell size than the output image.  Then, the copy
  of the original image is replicated into the output image using
  a closest cell approach.

*/

int GPFImage::gpf_InterpColorImage (unsigned char *datain, 
                          CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                          int nc1, int nr1,
                          unsigned char *dataout, 
                          CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                          int *nc2, int *nr2)
{
    int                  istat, tnc, tnr, iseed, seed;
    CSW_F                gx1, gy1, gx2, gy2,
                         tx1, ty1, tx2, ty2;
    unsigned char        *subimage = NULL, *tmpimage1 = NULL, *tmpimage2 = NULL;

    auto fscope = [&]()
    {
        if (tmpimage1 != tmpimage2) {
            csw_Free (tmpimage2);
        }
        csw_Free (tmpimage1);
        csw_Free (subimage);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    save output geometry in temp local variables
*/
    tx1 = *x21;
    ty1 = *y21;
    tx2 = *x22;
    ty2 = *y22;

/*
    extract a subimage from the input that is slightly
    larger than the desired output.
*/
    istat = SubImage (datain, x11, y11, x12, y12, nc1, nr1,
                      &subimage, &tx1, &ty1, &tx2, &ty2, &tnc, &tnr);
    if (istat != 1) {
        return -1;
    }

/*
    get cell sizes of subimage and output image
*/
    gx1 = (tx2 - tx1) / (tnc - 1);
    gy1 = (ty2 - ty1) / (tnr - 1);
    gx2 = (*x22 - *x21) / (*nc2 - 1);
    gy2 = (*y22 - *y21) / (*nr2 - 1);
    
    tmpimage1 = subimage;
    tmpimage2 = subimage;
    subimage = NULL;

/*
    If the desired output cell size is larger than the subimage 
    cell size, double the subimage cell size until the subimage
    cell size is larger than the output image cell size.
*/
    iseed = 0;
    seed = 0;
    while (gx2 > gx1  ||  gy2 > gy1) {

        istat = HalfColorImage (tmpimage1, tnc, tnr, seed,
                                &tmpimage2, &tnc, &tnr);

    /*
        return an error if the image cannot be halved or
        if the halved image is too small.
    */
        if (istat != 1  ||  tnc < 2  ||  tnr < 2) {
            return -1;
        }

    /*
        double the working image cell size, adjust the x and y maximums
        and csw_Free the old image input to HalfColorImage
    */
        gx1 *= 2.f;
        gy1 *= 2.f;
        tx2 = tx1 + gx1 * (tnc - 1);
        ty2 = ty1 + gy1 * (tnr - 1); 
        csw_Free (tmpimage1);

    /*
        the current output from HalfColorImage becomes the next input
    */
        tmpimage1 = tmpimage2;

    /*
        vary the starting cell in the 2x2 group of image cells
        used for matching each time HalfColorImage is called
    */
        iseed++;
        if (SparseImageFlag) {
            seed = iseed % 2;
        }
        else {
            seed = iseed;
        }

    } 

/*
    the sub image (in tmpimage2) is now coarser than the 
    desired output image, so I can use gpf_InterpImage8
    to get the final interpolated image.
*/
    istat = gpf_InterpImage8 ((char *)tmpimage2, tx1, ty1, tx2, ty2, tnc, tnr,
                              (char *)dataout, x21, y21, x22, y22, nc2, nr2);
    if (istat == -1) {
        return -1;
    }

    return 1;

}  /*  end of function gpf_InterpColorImage  */



/*
  ****************************************************************

                        S u b I m a g e

  ****************************************************************

  Extract a subimage from a specified image.  The subimage will
  have the same cell sizes as the input image and it will be slightly
  larger than the specified subimage limits (unless the limits are
  outside of the input limits).  

  This is used by gpf_InterpColorImage to get a subset of the image
  to be interpolated prior to starting the image halving operations.

  The memory for the subimage is allocated here and should be csw_Freed
  by the calling function.

*/

int GPFImage::SubImage (const unsigned char *datain, 
                     CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                     int nc1, int nr1,
                     unsigned char **subimage, 
                     CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                     int *nc2, int *nr2)
{
    int               istat, ii, i, i1, i2, j1, j2, 
                      nr, nc, base1, base2, rowsize;
    CSW_F             gx, gy;
    unsigned char     *ctmp = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (ctmp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    calculate cell sizes (same for input and output)
*/

    gx = (x12 - x11) / (nc1 - 1);
    gy = (y12 - y11) / (nr1 - 1);

/*
    tweek desired output limits slightly larger
    (by 2 cell sizes in all directions)
*/

    *x21 -= gx * 2;
    *y21 -= gy * 2;
    *x22 += gx * 2;
    *y22 += gy * 2;
    *nc2 = (int)((*x22 - *x21) / gx + .1f);
    *nr2 = (int)((*y22 - *y21) / gy + .1f);

/*
    clip the output limits to the input limits if needed
*/

    istat = ClipImage (x11, y11, x12, y12,
                       x21, y21, x22, y22, nc2, nr2);
    if (istat != 1) {
        return -1;
    }

/*
    get start and end rows and columns of subimage
*/

    j1 = (int)((*x21 - x11) / gx + .1f);
    j2 = (int)((*x22 - x11) / gx + 1.1f);
    i1 = (int)((*y21 - y11) / gy + .1f);
    i2 = (int)((*y22 - y11) / gy + 1.1f);

    if (j1 < 0) j1 = 0;
    if (i1 < 0) i1 = 0;

    if (j2 > nc1) j2 = nc1;
    if (i2 > nr1) i2 = nr1;

    nr = i2 - i1;
    nc = j2 - j1;

    if (nr > nr1) nr = nr1;
    if (nc > nc1) nc = nc1;

/*
    allocate subimage memory
*/

    ctmp = (unsigned char *)csw_Malloc (nc * nr * sizeof(unsigned char));
    if (!ctmp) {
        return -1;
    }

/*
    put data into the subimage
*/

    rowsize = nc * sizeof(unsigned char);

    for (i=i1; i<i2; i++) {
        ii = i - i1;
        base1 = i * nc1 + j1;
        base2 = ii * nc;
        csw_memcpy (ctmp+base2, datain+base1, rowsize);
    }

    *nc2 = nc;
    *nr2 = nr;
    *subimage = ctmp;

    bsuccess = true;

    return 1;

}  /*  end of private SubImage function  */




/*
  ****************************************************************

                    H a l f C o l o r I m a g e

  ****************************************************************

  Given a color image, decimate it by a factor of two in x and y
  directions.  The decimation is done by picking one of the four
  input cells which overlap the output cell and assigning that 
  color to the output cell.  The cell chosen changes with row and 
  column to decrease the hazard of completely eliminating vertical
  and horizontal lines in the image.  The seed parameter is used
  to assign which cell is chosen for the first column in the first
  row.  The seed modulus 4 is used for the first cell.

  The algorithm is not reversable.  In other words, if you double
  the output image, you will not get the original input image.

  The output image is allocated by this function, or -1 is returned
  if the allocation failed.  The calling function must csw_Free the output
  image when it is done with it.

*/

int GPFImage::HalfColorImage (const unsigned char *datain, int ncol, int nrow, int seed,
                           unsigned char **dataout, int *ncout, int *nrout)
{
    int              ii, jj, i1, j1, i2, j2, nr2, nc2, ncm1, icol1, icol2,
                     offset[4], offset2[4], rowsize;
    unsigned char    *dwork = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (dwork);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

    *dataout = NULL;
    *ncout = 0;
    *nrout = 0;

/*
    allocate the output image  
*/

    nr2 = nrow / 2 + 1;
    nc2 = ncol / 2 + 1;

    dwork = (unsigned char *)csw_Malloc (nr2 * nc2 * sizeof(unsigned char));
    if (!dwork) {
        return -1;
    }

/*  
    initialize the output grid to zero
*/

    rowsize = nc2 * sizeof(unsigned char);

    for (ii=0; ii<nr2; ii++) {
        icol2 = ii * nc2;
        memset ((char *)(dwork + icol2), 0, rowsize);
    }

    if (nrow % 2 == 0) {
        nr2--;
    }

    if (ncol % 2 == 0) {
        nc2--;
    }

/*
    loop through the input image taking the cells in 2 by 2 groups
*/

    ncm1 = ncol - 1;
    i1 = 0;

    for (ii=0; ii<nrow; ii+=2) {

    /*
        the offset array has the input image offsets for the lower
        left, lower right, upper left and upper right cells in the
        2 by 2 group.  This is corrected for the top row also.
    */

        offset[0] = 0;
        offset[1] = 1;
        offset[2] = ncol;
        offset[3] = ncol+1;
        offset2[0] = 1;
        offset2[1] = ncol;
        offset2[2] = 1;
        offset2[3] = ncol;
        if (SparseImageFlag) {
            offset[1] = ncol+1;
            offset[2] = 0;
            offset2[0] = 1;
            offset2[1] = ncol;
            offset2[2] = 1;
            offset2[3] = ncol;
        }
        if (ii == nrow - 1) {
            offset[0] = 0;
            offset[1] = 1;
            offset[2] = 0;
            offset[3] = 1;
            offset2[0] = 0;
            offset2[1] = 1;
            offset2[2] = 0;
            offset2[3] = 1;
        }

    /* vary the starting offset for each row.  */
        
        i2 = (i1 + seed) % 4;

        icol1 = ncol * ii;
        icol2 = nc2 * i1;

        i1++;
        j1 = 0;

        for (jj=0; jj<ncol; jj+=2) {
        
        /* correct offsets for right edge */

            if (jj == ncm1) {
                offset[0] = 0;
                offset[1] = 0;
                offset[2] = ncol;
                offset[3] = ncol;
                offset2[0] = 0;
                offset2[1] = 0;
                offset2[2] = ncol;
                offset2[3] = ncol;
            }

        /* vary the offset index for each cell in the row */

            j2 = offset[(j1+i2)%4];
            dwork[icol2+j1] = datain[icol1+jj+j2];

        /*  try another offset index combination if the image is sparse  */

            if (SparseImageFlag) {
                if (dwork[icol2+j1] == BackgroundColor) {
                    j2 = offset2[(j1+i2)%4];
                    dwork[icol2+j1] = datain[icol1+jj+j2];
                }
            }
            j1++;
        }
    }

    if (SparseImageFlag) {
        FillInHalvedImage (dwork, nc2, nr2);
    }

/*
    all done
*/

    *ncout = nc2;
    *nrout = nr2;
    *dataout = dwork;

    bsuccess = true;

    return 1;

}  /*  end of private function HalfColorImage  */



/*
  ****************************************************************

                 F i l l I n H a l v e d I m a g e

  ****************************************************************

    This function is only called by HalfColorImage.  The function
  attempts to connect linear features of the same color by filling
  in the intermediate cell between two cells of the same color.

    This is useful for monochrome images that have linear features.
  The BackgroundColor is defined by calling gpf_SetImageBackgroundColor.
  If the background is defined as 255, this function is skipped and
  the data is returned unchanged.

    If the work space needed cannot be allocated, -1 is returned.  For
  all other cases, 1 is returned.

*/

int GPFImage::FillInHalvedImage (unsigned char *data, int nc, int nr)
{

    int              i, j, i1, jt1, nc2;
    unsigned char    *dwork = NULL;

    auto fscope = [&]()
    {
        csw_Free (dwork);
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
    obvious errors
*/
    if (BackgroundColor == 255) {
        return 1;
    }

    if (nc < 5  ||  nr < 5) {
        return 1;
    }

/*
    allocate workspace
*/
    dwork = (unsigned char *)csw_Malloc (nc * nr * sizeof(unsigned char));
    if (!dwork) {
        return -1;
    }
    memset (dwork, BackgroundColor, nc * nr * sizeof(unsigned char));

/*
    loop through the center of the image and fill in as needed
    store the filled in values in the work array
*/

    nc2 = nc * 2;

    for (i=2; i<nr-2; i++) {
        i1 = nc * i;

        for (j=2; j<nc-2; j++) {
            jt1 = i1 + j;
            if (data[jt1] == BackgroundColor) {
                continue;
            }

        /*  check vertical column  */

            if (data[jt1] == data[jt1-nc2]  &&
                data[jt1] == data[jt1+nc2]) {
                if (data[jt1-nc] == BackgroundColor) {
                    dwork[jt1-nc] = data[jt1];
                }
                if (data[jt1+nc] == BackgroundColor) {
                    dwork[jt1+nc] = data[jt1];
                }
                continue;
            }

        /*  check horizontal row  */

            if (data[jt1] == data[jt1-2]  &&
                data[jt1] == data[jt1+2]) {
                if (data[jt1-1] == BackgroundColor) {
                    dwork[jt1-1] = data[jt1];
                }
                if (data[jt1+1] == BackgroundColor) {
                    dwork[jt1+1] = data[jt1];
                }
                continue;
            }

        /*  check diagonal upper right to lower left  */

            if (data[jt1] == data[jt1+nc2+2]  &&
                data[jt1] == data[jt1-nc2-2]) {
                if (data[jt1+nc+1] == BackgroundColor) {
                    dwork[jt1+nc+1] = data[jt1];
                }
                if (data[jt1-nc-1] == BackgroundColor) {
                    dwork[jt1-nc-1] = data[jt1];
                }
                continue;
            }

        /*  check diagonal upper left to lower right  */

            if (data[jt1] == data[jt1+nc2-2]  &&
                data[jt1] == data[jt1-nc2+2]) {
                if (data[jt1+nc-1] == BackgroundColor) {
                    dwork[jt1+nc-1] = data[jt1];
                }
                if (data[jt1-nc+1] == BackgroundColor) {
                    dwork[jt1-nc+1] = data[jt1];
                }
                continue;
            }
        }
    }

/*
    transfer work values to data array
*/

    for (i=0; i<nr; i++) {
        i1 = nc * i;

        for (j=0; j<nc; j++) {
            jt1 = i1 + j;
            if (dwork[jt1] != BackgroundColor  &&  
                data[jt1] == BackgroundColor) {
                data[jt1] = dwork[jt1];
            }
        }
    }

    return 1;

}  /*  end of private FillInHalvedImage function  */



/*
  ****************************************************************

                 g p f _ I n t e r p I m a g e 8

  ****************************************************************

    For each cell in a output image, find the overlapping cell in the
  input image and assign that value to the output image cell.  This
  version is for 8 bit signed or unsigned image data.

    If the output image extends beyond the input image boundaries, the
  specified limits of the output image will be modified to the part that
  overlaps the input image.

*/

int GPFImage::gpf_InterpImage8 (const char *datain, 
                      CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                      int nc1, int nr1,
                      char *dataout, 
                      CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                      int *nc2, int *nr2)
{
    int          ii, jj, it, i1, i2, istat,
                 ilast, lasti2, rowsize;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2, gy2;
    int          jtsave[MAX_ROWS];

    lasti2 = 0;

/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1  ||  *nr2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImage (x11, y11, x12, y12, 
                       x21, y21, x22, y22, nc2, nr2);
    if (istat == -1) {
        return -1;
    }

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);
    gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);
    gy2 = (*y22 - *y21) / (CSW_F)(*nr2 - 1);

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

/*
    each output image row will map the same to the input
    image row, so precalculate the mapping and save it
    in the jtsave array.
*/

    for (jj=0; jj<*nc2; jj++) {
        xt = jj * gx2 + dx1;
        jtsave[jj] = (int)(xt / gx1 + 0.5f);
    }

/*
    loop through the output image, assigning values from 
    the input image.
*/

    rowsize = *nc2 * sizeof(char);
    ilast = -100000;
    ii = 0;

    while (ii < *nr2) {

        yt = ii * gy2 + dy1;
        it = (int)(yt / gy1 + 0.5f);
        if (it > nr1 - 1) it = nr1 - 1;
        if (it < 0) it = 0;
        i1 = it * nc1;
        i2 = ii * *nc2;

        if (it != ilast) {
            for (jj=0; jj<*nc2; jj++) {
                dataout[i2+jj] = datain[i1+jtsave[jj]];
            }
            ilast = it;
        }
        else {
            csw_memcpy (dataout+i2, dataout+lasti2, rowsize);
        }

        lasti2 = i2;
        ii++;

    }

    return 1;

}  /*  end of function gpf_InterpImage8  */



/*
  ****************************************************************

                 g p f _ I n t e r p I m a g e 1 6

  ****************************************************************

    For each cell in a output image, find the overlapping cell in the
  input image and assign that value to the output image cell.  This
  version is for 16 bit signed image data.

    If the output image extends beyond the input image boundaries, the
  specified limits of the output image will be modified to the part that
  overlaps the input image.

*/

int GPFImage::gpf_InterpImage16 (short *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                       int nc1, int nr1,
                       short *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                       int *nc2, int *nr2)
{
    int          ii, jj, it, i1, i2, istat;
    int          rowsize, ilast, lasti2;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2, gy2;
    int          jtsave[MAX_ROWS];

    lasti2 = 0;

/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1  ||  *nr2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImage (x11, y11, x12, y12, 
                       x21, y21, x22, y22, nc2, nr2);
    if (istat == -1) {
        return -1;
    }

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);
    gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);
    gy2 = (*y22 - *y21) / (CSW_F)(*nr2 - 1);

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

/*
    each output image row will map the same to the input
    image row, so precalculate the mapping and save it
    in the jtsave array.
*/

    for (jj=0; jj<*nc2; jj++) {
        xt = jj * gx2 + dx1;
        jtsave[jj] = (int)(xt / gx1 + 0.5f);
    }

/*
    loop through the output image, assigning values from 
    the input image.
*/

    rowsize = *nc2 * sizeof(short);
    ilast = -100000;
    ii = 0;

    while (ii < *nr2) {

        yt = ii * gy2 + dy1;
        it = (int)(yt / gy1 + 0.5f);
        if (it > nr1 - 1) it = nr1 - 1;
        if (it < 0) it = 0;
        i1 = it * nc1;
        i2 = ii * *nc2;

        if (it != ilast) {
            for (jj=0; jj<*nc2; jj++) {
                dataout[i2+jj] = datain[i1+jtsave[jj]];
            }
            ilast = it;
        }
        else {
            csw_memcpy (dataout+i2, dataout+lasti2, rowsize);
        }

        lasti2 = i2;
        ii++;

    }

    return 1;

}  /*  end of function gpf_InterpImage16  */



/*
  ****************************************************************

                 g p f _ I n t e r p I m a g e 3 2

  ****************************************************************

  For each cell in a output image, find the overlapping cell in the
  input image and assign that value to the output image cell.  This
  version is for 32 bit signed image data.

  If the output image extends beyond the input image boundaries, the
  specified limits of the output image will be modified to the part that
  overlaps the input image.

*/

int GPFImage::gpf_InterpImage32 (int *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                       int nc1, int nr1,
                       int *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                       int *nc2, int *nr2)
{
    int          ii, jj, it, i1, i2, istat;
    int          rowsize, ilast, lasti2;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2, gy2;
    int          jtsave[MAX_ROWS];

    lasti2 = 0;

/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1  ||  *nr2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImage (x11, y11, x12, y12, 
                       x21, y21, x22, y22, nc2, nr2);
    if (istat == -1) {
        return -1;
    }

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);
    gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);
    gy2 = (*y22 - *y21) / (CSW_F)(*nr2 - 1);

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

/*
    each output image row will map the same to the input
    image row, so precalculate the mapping and save it
    in the jtsave array.
*/

    for (jj=0; jj<*nc2; jj++) {
        xt = jj * gx2 + dx1;
        jtsave[jj] = (int)(xt / gx1 + 0.5f);
    }

/*
    loop through the output image, assigning values from 
    the input image.
*/

    rowsize = *nc2 * sizeof(int);
    ilast = -100000;
    ii = 0;

    while (ii < *nr2) {

        yt = ii * gy2 + dy1;
        it = (int)(yt / gy1 + 0.5f);
        if (it > nr1 - 1) it = nr1 - 1;
        if (it < 0) it = 0;
        i1 = it * nc1;
        i2 = ii * *nc2;

        if (it != ilast) {
            for (jj=0; jj<*nc2; jj++) {
                dataout[i2+jj] = datain[i1+jtsave[jj]];
            }
            ilast = it;
        }
        else {
            csw_memcpy (dataout+i2, dataout+lasti2, rowsize);
        }

        lasti2 = i2;
        ii++;

    }

    return 1;

}  /*  end of function gpf_InterpImage32  */




/*
  ****************************************************************

             g p f _ I n t e r p I m a g e F l o a t

  ****************************************************************

    For each cell in a output image, find the overlapping cell in the
  input image and assign that value to the output image cell.  This
  version is for CSW_F image data.

    If the output image extends beyond the input image boundaries, the
  specified limits of the output image will be modified to the part that
  overlaps the input image.

*/

int GPFImage::gpf_InterpImageFloat (CSW_F *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                          int nc1, int nr1,
                          CSW_F *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                          int *nc2, int *nr2)
{
    int          ii, jj, it, i1, i2, istat;
    int          rowsize, ilast, lasti2;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2, gy2;
    int          jtsave[MAX_ROWS];

    lasti2 = 0;

/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1  ||  *nr2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImage (x11, y11, x12, y12, 
                       x21, y21, x22, y22, nc2, nr2);
    if (istat == -1) {
        return -1;
    }

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);
    gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);
    gy2 = (*y22 - *y21) / (CSW_F)(*nr2 - 1);

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

/*
    each output image row will map the same to the input
    image row, so precalculate the mapping and save it
    in the jtsave array.
*/

    for (jj=0; jj<*nc2; jj++) {
        xt = jj * gx2 + dx1;
        jtsave[jj] = (int)(xt / gx1 + 0.5f);
    }

/*
    loop through the output image, assigning values from 
    the input image.
*/

    rowsize = *nc2 * sizeof(CSW_F);
    ilast = -100000;
    ii = 0;

    while (ii < *nr2) {

        yt = ii * gy2 + dy1;
        it = (int)(yt / gy1 + 0.5f);
        if (it > nr1 - 1) it = nr1 - 1;
        if (it < 0) it = 0;
        i1 = it * nc1;
        i2 = ii * *nc2;

        if (it != ilast) {
            for (jj=0; jj<*nc2; jj++) {
                dataout[i2+jj] = datain[i1+jtsave[jj]];
            }
            ilast = it;
        }
        else {
            csw_memcpy (dataout+i2, dataout+lasti2, rowsize);
        }

        lasti2 = i2;
        ii++;

    }

    return 1;

}  /*  end of function gpf_InterpImageFloat  */




/*
  ****************************************************************

                      C l i p I m a g e

  ****************************************************************

    If the second image extends beyond the boundary of the first
  image, clip it to the first image.  If there is no overlapping area,
  return -1.  On success, return 1.  The limits of the second grid
  may be adjusted if needed.

*/

int GPFImage::ClipImage (CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                      CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                      int *nc2, int *nr2)
{
    CSW_F        gx, gy;

/*
    cell sizes of second image
*/

    gx = (*x22 - *x21) / (CSW_F)(*nc2 - 1);
    gy = (*y22 - *y21) / (CSW_F)(*nr2 - 1);

/*
    check for no overlap
*/

    if (x11 >= *x22  ||  y11 >= *y22  ||
        x12 <= *x21  ||  y12 <= *y21) {
        return -1;
    }

/*
    adjust image 2 limits if needed
*/

    if (*x22 > x12) {
        *nc2 = (int)((x12 - *x21) / gx + 1.1f);
        *x22 = *x21 + (*nc2 - 1) * gx;
    }

    if (*y22 > y12) {
        *nr2 = (int)((y12 - *y21) / gy + 1.1f);
        *y22 = *y21 + (*nr2 - 1) * gy;
    }

    if (*x21 < x11) {
        *nc2 = (int)((*x22 - x11) / gx + 1.1f);
        *x21 = *x22 - (*nc2 - 1) * gx;
    }

    if (*y21 < y11) {
        *nr2 = (int)((*y22 - y11) / gy + 1.1f);
        *y21 = *y22 - (*nr2 - 1) * gy;
    }

    return 1;

}  /*  end of private ClipImage function  */




/*
  ****************************************************************

       g p f _ S e t I n t I m a g e C o l o r B a n d s

  ****************************************************************

    Set up color bands for a subsequent gpf_ColorIntImage call.
  The bands are specified as arrays of start and end values and a color
  number associated with the band.  Use this for 8 bit, 16 bit or 32
  bit data images.

*/

int GPFImage::gpf_SetIntImageColorBands (int *min, int *max, int *num, int nband)
{
    int        i, istat;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (ColorTable);
            ColorTable = NULL;
            csw_Free (ColorLookup);
            ColorLookup = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

    if (nband < 1) {
        return -1;
    }

    csw_Free (ColorTable);
    ColorTable = NULL;
    csw_Free (ColorLookup);
    ColorLookup = NULL;

    ColorTable = (IMageColorRec *)csw_Malloc (nband * sizeof(IMageColorRec));
    if (!ColorTable) {
        return -1;
    }
    Ncolors = nband;

    for (i=0; i<nband; i++) {
        ColorTable[i].min.ival = min[i];
        ColorTable[i].max.ival = max[i];
        ColorTable[i].num = num[i];
    }

    istat = SortIntColorTable ();

    bsuccess = true;

    return istat;

}  /*  end of function gpf_SetIntImageColorBands  */




/*
  ****************************************************************

       g p f _ S e t F l o a t I m a g e C o l o r B a n d s

  ****************************************************************

    Set up color bands for a subsequent gpf_ColorFloatImage call.
  The bands are specified as arrays of start and end values and a color
  number associated with the band.  Use this for CSW_F data images.

*/

int GPFImage::gpf_SetFloatImageColorBands (CSW_F *min, CSW_F *max, int *num, int nband)
{
    int        i, istat;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (ColorTable);
            ColorTable = NULL;
            csw_Free (ColorLookup);
            ColorLookup = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

    if (nband < 1) {
        return -1;
    }

    if (ColorTable) {
        csw_Free (ColorTable);
        ColorTable = NULL;
    }
    if (ColorLookup) {
        csw_Free (ColorLookup);
        ColorLookup = NULL;
    }

    ColorTable = (IMageColorRec *)csw_Malloc (nband * sizeof(IMageColorRec));
    if (!ColorTable) {
        return -1;
    }
    Ncolors = nband;

    for (i=0; i<nband; i++) {
        ColorTable[i].min.fval = min[i];
        ColorTable[i].max.fval = max[i];
        ColorTable[i].num = num[i];
    }

    istat = SortFloatColorTable ();

    bsuccess = true;

    return istat;

}  /*  end of function gpf_SetFloatImageColorBands  */




/*
  ****************************************************************

                S o r t I n t C o l o r T a b l e

  ****************************************************************

  Make a color lookup table from the current color bands defined.
  This is used for converting a data image into a color image.  On
  success, 1 is returned.  If there is a memory allocation failure
  or if the defined color bands are in error, -1 is returned after
  csw_Freeing ColorTables and ColorLookup.

*/

int GPFImage::SortIntColorTable (void)
{
    int         i, j, j1, j2, jt, ncells;
    int         minval, maxval, minband, tmp, range, tiny;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (ColorTable);
            ColorTable = NULL;
            csw_Free (ColorLookup);
            ColorLookup = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
    find the overall min and max of the banded data
    and the smallest band in the data
*/
    minval = 2000000000;
    maxval = -2000000000;
    minband = minval;

    for (i=0; i<Ncolors; i++) {

        if (ColorTable[i].min.ival < minval) {
            minval = ColorTable[i].min.ival;
        }
        if (ColorTable[i].max.ival < minval) {
            minval = ColorTable[i].max.ival;
        }
        if (ColorTable[i].min.ival > maxval) {
            maxval = ColorTable[i].min.ival;
        }
        if (ColorTable[i].max.ival > maxval) {
            maxval = ColorTable[i].max.ival;
        }

        tmp = ColorTable[i].max.ival - ColorTable[i].min.ival;
        if (tmp < 0) {
            tmp = -tmp;
        }
        if (tmp < minband) {
            minband = tmp;
        }
    }

/*
    calculate the size of and number of lookup cells based
    on the overall min and max and the smallest band
*/
    range = maxval - minval;
    if (range <= 0) {
        return -1;
    }
    tiny = minband / 10;
    if (tiny < range / (MAXBANDCELLS - 1)) {
        tiny = range / (MAXBANDCELLS - 1);
    }
    if (tiny < 1) {
        tiny = 1;
    }
    ncells = range / tiny + 2;

/*
    allocate the color lookup table
*/
    ColorLookup = (int *)csw_Malloc ((ncells + 1) * sizeof(int));
    if (!ColorLookup) {
        return -1;
    }

    for (i=0; i<=ncells; i++) {
        ColorLookup[i] = -1;
    }

/*
    fill in color lookup table with colors from band data
*/
    for (i=0; i<Ncolors; i++) {
        
        j1 = (ColorTable[i].min.ival - minval) / tiny;
        j2 = (ColorTable[i].max.ival - minval) / tiny;
        if (j1 > j2) {
            jt = j1;
            j1 = j2;
            j2 = jt;
        }
        if (j1 > 0) j1--;
        if (j2 < ncells) j2++;
        for (j=j1; j<=j2; j++) {
            ColorLookup[j] = ColorTable[i].num;
        }
    }

/*
    if any lookup table entries are still -1, substitute with the 
    background color
*/
    for (i=0; i<=ncells; i++) {
        if (ColorLookup[i] == -1) {
            ColorLookup[i] = (int)BackgroundColor;
        }
    }

/*
    set lookup table private file data and return
*/
    LookupNcells = ncells;
    LookupMin = minval;
    LookupSize = tiny;

    bsuccess = true;

    return 1;

}  /*  end of private SortIntColorTable function  */




/*
  ****************************************************************

                S o r t F l o a t C o l o r T a b l e

  ****************************************************************

  Make a color lookup table from the current color bands defined.
  This is used for converting a data image into a color image.  On
  success, 1 is returned.  If there is a memory allocation failure
  or if the defined color bands are in error, -1 is returned after
  csw_Freeing ColorTables and ColorLookup.

*/

int GPFImage::SortFloatColorTable (void)
{
    int         i, j, j1, j2, jt, ncells;
    CSW_F       minval, maxval, minband, tmp, range, tiny;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (ColorTable);
            ColorTable = NULL;
            csw_Free (ColorLookup);
            ColorLookup = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
    find the overall min and max of the banded data
    and the smallest band in the data
*/
    minval = 1.e20f;
    maxval = -1.e20f;
    minband = minval;

    for (i=0; i<Ncolors; i++) {

        if (ColorTable[i].min.fval < minval) {
            minval = ColorTable[i].min.fval;
        }
        if (ColorTable[i].max.fval < minval) {
            minval = ColorTable[i].max.fval;
        }
        if (ColorTable[i].min.fval > maxval) {
            maxval = ColorTable[i].min.fval;
        }
        if (ColorTable[i].max.fval > maxval) {
            maxval = ColorTable[i].max.fval;
        }

        tmp = ColorTable[i].max.fval - ColorTable[i].min.fval;
        if (tmp < 0) {
            tmp = -tmp;
        }
        if (tmp < minband) {
            minband = tmp;
        }
    }

/*
    calculate the size of and number of lookup cells based
    on the overall min and max and the smallest band
*/
    range = maxval - minval;
    if (range <= 0.0f) {
        return -1;
    }
    tiny = minband / 10.0f;
    if (tiny < range / (CSW_F)(MAXBANDCELLS - 1)) {
        tiny = range / (CSW_F)(MAXBANDCELLS - 1);
    }
    ncells = (int)(range / tiny) + 2;

/*
    allocate the color lookup table
*/
    ColorLookup = (int *)csw_Malloc ((ncells + 1) * sizeof(int));
    if (!ColorLookup) {
        return -1;
    }

    for (i=0; i<=ncells; i++) {
        ColorLookup[i] = -1;
    }

/*
    fill in color lookup table with colors from band data
*/
    for (i=0; i<Ncolors; i++) {
        
        j1 = (int)((ColorTable[i].min.fval - minval) / tiny - .1f);
        j2 = (int)((ColorTable[i].max.fval - minval) / tiny + .1f);
        if (j1 > j2) {
            jt = j1;
            j1 = j2;
            j2 = jt;
        }
        if (j1 > 0) j1--;
        if (j2 < ncells) j2--;
        for (j=j1; j<=j2; j++) {
            ColorLookup[j] = ColorTable[i].num;
        }
    }

/*
    if any lookup table entries are still -1, 
    substitute the background color
*/
    for (i=0; i<=ncells; i++) {
        if (ColorLookup[i] == -1) {
            ColorLookup[i] = (int)BackgroundColor;
        }
    }

/*
    set lookup table private file data and return
*/
    LookupNcells = ncells;
    LookupMinF = minval;
    LookupSizeF = tiny;

    bsuccess = true;

    return 1;

}  /*  end of private SortFloatColorTable function  */



/*
  ****************************************************************

       g p f _ S e t I m a g e B a c k g r o u n d C o l o r

  ****************************************************************

    Set the color number that will be used for null values when coloring
  data images.  This should be 0 to 255.

*/

int GPFImage::gpf_SetImageBackgroundColor (int val)
{
    
    if (val < 0  ||  val > 255) {
        return -1;
    }

    BackgroundColor = (unsigned char)val;

    return 1;

}  /*  end of function gpf_SetImageBackgroundColor  */



/*
  ****************************************************************

           g p f _ S e t S p a r s e I m a g e F l a g

  ****************************************************************

    Set the flag for a sparse color image.  If this is set, the
  color image halfing algorithm is changed slightly.

*/

int GPFImage::gpf_SetSparseImageFlag (int val)
{
    
    SparseImageFlag = val;

    return 1;

}  /*  end of function gpf_SetSparseImageFlag  */



/*
  ****************************************************************

              g p f _ S e t I n t N u l l V a l u e

  ****************************************************************

    Set the null value for int, short or char image to be colored.
  A value of zero means no nulls are in the image.

*/

int GPFImage::gpf_SetIntNullValue (int val)
{
    
    InullVal = val;
    return 1;

}  /*  end of function gpf_SetIntNullValue  */




/*
  ****************************************************************

              g p f _ S e t F l o a t N u l l V a l u e

  ****************************************************************

    Set the null value for CSW_F image to be colored.
  A value of zero means no nulls are in the image.

*/

int GPFImage::gpf_SetFloatNullValue (CSW_F val)
{
    
    FnullVal = val;
    if (FnullVal < 0.1f  &&  FnullVal > -0.1f) {
        FnullVal = 0.0f;
    }

    return 1;

}  /*  end of function gpf_SetFloatNullValue  */





/*
  ****************************************************************

         g p f _ C o n v e r t T o C o l o r I m a g e

  ****************************************************************

    This function uses the colors set up with the set color bands
  functions to convert a data image into a color image.  If the
  function succeeds, 1 is returned.  If the color bands have not been
  set up, then -1 is returned and no processing is done.

    type = 1, 8 bit signed char data
    type = 2, 16 bit signed short data
    type = 3, 32 bit signed int data
    type = 4, CSW_F data

    If type is out of range, -1 is returned.

*/

int GPFImage::gpf_ConvertToColorImage (const void *datain, int nc, int nr, int type, 
                             unsigned char *dataout, int bgpix)
{
    int            bgsav;

    if (!ColorLookup) {
        return -1;
    }

    bgsav = BackgroundColor;
    BackgroundColor = (unsigned char)bgpix;
    
    switch (type) {

        case 1:
            ColorImage8 ((char *)datain, nc, nr, dataout);
            break;

        case 2:
            ColorImage16 ((short *)datain, nc, nr, dataout);
            break;

        case 3:
            ColorImage32 ((int *)datain, nc, nr, dataout);
            break;

        case 4:
            ColorImageFloat ((CSW_F *)datain, nc, nr, dataout);
            break;

        default:
            return -1;

    }

    BackgroundColor = (unsigned char)bgsav;

    return 1;

}  /*  end of function gpf_ConvertToColorImage  */



/*
  ****************************************************************

                     C o l o r I m a g e 8

  ****************************************************************

    Use lookup table to color an 8 bit data image.  The color image 
  is in window coordinates, with origin at the upper left.  The 
  input data image is in page coordinates with origin at the lower
  left.

*/

int GPFImage::ColorImage8 (const char *datain, int nc, int nr, 
                        unsigned char *dataout)
{
    int             i, j, i1, i2, iout, jj;

/*
    version for no null values
*/

    if (InullVal == 0) {
        for (i=0; i<nr; i++) {
            i1 = i * nc;
            iout = (nr - i - 1) * nc;

            for (j=0; j<nc; j++) {
                i2 = (datain[i1 + j] - LookupMin) / LookupSize;
                if (i2 < 0  ||  i2 > LookupNcells) {
                    dataout[iout+j] = BackgroundColor;
                    continue;
                }
                dataout[iout + j] = (unsigned char)ColorLookup[i2];
            }
        }
    }

/*
    version for possible null values
*/

    else {
        for (i=0; i<nr; i++) {
            i1 = i * nc;
            iout = (nr - i - 1) * nc;

            for (j=0; j<nc; j++) {
                jj = i1 + j;
                if (datain[jj] >= InullVal) {
                    dataout[iout + j] = BackgroundColor;
                }
                else {
                    i2 = (datain[jj] - LookupMin) / LookupSize;
                    if (i2 < 0  ||  i2 > LookupNcells) {
                        dataout[iout+j] = BackgroundColor;
                        continue;
                    }
                    dataout[iout + j] = (unsigned char)ColorLookup[i2];
                }
            }
        }
    }

    return 1;

}  /*  end of private ColorImage8 function  */



/*
  ****************************************************************

                     C o l o r I m a g e 1 6

  ****************************************************************

    Use lookup table to color an 16 bit data image.  The color image 
  is in window coordinates, with origin at the upper left.  The 
  input data image is in page coordinates with origin at the lower
  left.

*/

int GPFImage::ColorImage16 (const short *datain, int nc, int nr, 
                         unsigned char *dataout)
{
    int             i, j, i1, i2, iout, jj;

/*
    version for no null values
*/

    if (InullVal == 0) {
        for (i=0; i<nr; i++) {
            i1 = i * nc;
            iout = (nr - i - 1) * nc;

            for (j=0; j<nc; j++) {
                i2 = (datain[i1 + j] - LookupMin) / LookupSize;
                if (i2 < 0  ||  i2 > LookupNcells) {
                    dataout[iout+j] = BackgroundColor;
                    continue;
                }
                dataout[iout + j] = (unsigned char)ColorLookup[i2];
            }
        }
    }

/*
    version for possible null values
*/

    else {
        for (i=0; i<nr; i++) {
            i1 = i * nc;
            iout = (nr - i - 1) * nc;

            for (j=0; j<nc; j++) {
                jj = i1 + j;
                if (datain[jj] >= InullVal) {
                    dataout[iout + j] = BackgroundColor;
                }
                else {
                    i2 = (datain[jj] - LookupMin) / LookupSize;
                    if (i2 < 0  ||  i2 > LookupNcells) {
                        dataout[iout+j] = BackgroundColor;
                        continue;
                    }
                    dataout[iout + j] = (unsigned char)ColorLookup[i2];
                }
            }
        }
    }

    return 1;

}  /*  end of private ColorImage16 function  */



/*
  ****************************************************************

                     C o l o r I m a g e 3 2

  ****************************************************************

    Use lookup table to color a 32 bit data image.  The color image 
  is in window coordinates, with origin at the upper left.  The 
  input data image is in page coordinates with origin at the lower
  left.

*/

int GPFImage::ColorImage32 (const int *datain, int nc, int nr, 
                         unsigned char *dataout)
{
    int             i, j, i1, i2, iout, jj;

/*
    version for no null values
*/

    if (InullVal == 0) {
        for (i=0; i<nr; i++) {
            i1 = i * nc;
            iout = (nr - i - 1) * nc;

            for (j=0; j<nc; j++) {
                i2 = (datain[i1 + j] - LookupMin) / LookupSize;
                if (i2 < 0  ||  i2 > LookupNcells) {
                    dataout[iout+j] = BackgroundColor;
                    continue;
                }
                dataout[iout + j] = (unsigned char)ColorLookup[i2];
            }
        }
    }

/*
    version for possible null values
*/

    else {
        for (i=0; i<nr; i++) {
            i1 = i * nc;
            iout = (nr - i - 1) * nc;

            for (j=0; j<nc; j++) {
                jj = i1 + j;
                if (datain[jj] >= InullVal) {
                    dataout[iout + j] = BackgroundColor;
                }
                else {
                    i2 = (datain[jj] - LookupMin) / LookupSize;
                    if (i2 < 0  ||  i2 > LookupNcells) {
                        dataout[iout+j] = BackgroundColor;
                        continue;
                    }
                    dataout[iout + j] = (unsigned char)ColorLookup[i2];
                }
            }
        }
    }

    return 1;

}  /*  end of private ColorImage32 function  */



/*
  ****************************************************************

                 C o l o r I m a g e F l o a t

  ****************************************************************

    Use lookup table to color a CSW_F data image.  The color image 
  is in window coordinates, with origin at the upper left.  The 
  input data image is in page coordinates with origin at the lower
  left.

*/

int GPFImage::ColorImageFloat (const CSW_F *datain, int nc, int nr, 
                            unsigned char *dataout)
{
    int             i, j, i1, i2, iout, jj;

/*
    version for no null values
*/
    if (FnullVal == 0.0f) {
        for (i=0; i<nr; i++) {
            i1 = i * nc;
            iout = (nr - i - 1) * nc;

            for (j=0; j<nc; j++) {
                i2 = (int)((datain[i1 + j] - LookupMinF) / LookupSizeF + .5f);
                if (i2 < 0  ||  i2 > LookupNcells) {
                    dataout[iout+j] = BackgroundColor;
                    continue;
                }
                dataout[iout + j] = (unsigned char)ColorLookup[i2];
            }
        }
    }

/*
    version for possible null values
*/

    else {
        for (i=0; i<nr; i++) {
            i1 = i * nc;
            iout = (nr - i - 1) * nc;

            for (j=0; j<nc; j++) {
                jj = i1 + j;
                if (datain[jj] >= FnullVal) {
                    dataout[iout + j] = BackgroundColor;
                }
                else {
                    i2 = (int)((datain[jj] - LookupMinF) / LookupSizeF + .5f);
                    if (i2 < 0  ||  i2 > LookupNcells) {
                        dataout[iout+j] = BackgroundColor;
                        continue;
                    }
                    dataout[iout + j] = (unsigned char)ColorLookup[i2];
                }
            }
        }
    }

    return 1;

}  /*  end of private ColorImageFloat function  */



/*
  ****************************************************************

         g p f _ M i r r o r C o l o r I m a g e R o w s

  ****************************************************************

  Swap the rows of an image so that the bottom is on top and the
  top is on bottom.  This can either mirror an image or change the
  ordering of an image to fit different coordinate conventions.  The
  specified image is modified by the function.

  The image pointer cannot be a NULL pointer, and both nc and
  nr must be at least 2.

  On success, 1 is returned.  On csw_Malloc failure, -1 is returned.
  If a parameter is bad, zero is returned.

*/

int GPFImage::gpf_MirrorColorImageRows (unsigned char *image, int nc, int nr)
{
    unsigned char      *row = NULL;
    int                i, i1, i2, rm1, rsize;


    auto fscope = [&]()
    {
        csw_Free (row);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    check for obvious errors
*/

    if (!image  ||  nc < 2  ||  nr < 2) {
        return 0;
    }

/*
    allocate temporary storage for a row of the image
*/

    rsize = nc * sizeof(unsigned char);
    row = (unsigned char *)csw_Malloc (rsize);
    if (!row) {
        return -1;
    }

/*
    swap bottom and top rows, 2nd and 2nd from top, etc.
*/

    rm1 = nr-1;
    for (i=0; i<nr/2; i++) {
        i1 = i * nc;
        csw_memcpy (row, image+i1, rsize);
        i2 = (rm1 - i) * nc;
        csw_memcpy (image+i1, image+i2, rsize);
        csw_memcpy (image+i2, row, rsize);
    }

    return 1;

}  /*  end of function gpf_MirrorColorImageRows  */



/*
  ****************************************************************

               g p f _ I n t e r p I m a g e 8 B

  ****************************************************************

    For a specified location in the output image, do a bilinear interpolation 
  of the four cells in the input image which surround the output.  If more
  than one of the input image corners is null, output a null.  If only one
  of the input is null, assign it a value that is the average of the adjacent
  non null corners and proceed with the interpolation.

    If the output image extends beyond the input image boundaries, the
  specified limits of the output image will be modified to the part that
  overlaps the input image.

    This function is used for 8 bit signed image data.

*/

int GPFImage::gpf_InterpImage8B (const char *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                       int nc1, int nr1, int nullval,
                       char *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                       int *nc2, int *nr2)
{
    int          ii, jj, it, jt, i1, i2, istat, n, jlast;
    int          nct, nrt;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2, gy2;
    CSW_F        xpct, ypct, dpct, z1, z2, z3, z4, zleft, zright;
    char         *datatmp = NULL;
    CSW_F        xpctsave[MAX_ROWS];
    int          jlastsave[MAX_ROWS],
                 jtsave[MAX_ROWS];

    auto fscope = [&]()
    {
        if (datatmp != dataout) {
            csw_Free (datatmp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1  ||  *nr2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImage (x11, y11, x12, y12, 
                       x21, y21, x22, y22, nc2, nr2);
    if (istat == -1) {
        return -1;
    }

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);

/*
    halve the output image size for bilinear interpolation
    this is then doubled after the bilinear loop is done
*/

    nrt = *nr2 / 2;
    if (*nr2 % 2  == 1) nrt++;
    nct = *nc2 / 2;
    if (*nc2 % 2  == 1) nct++;
    gx2 = (*x22 - *x21) / (CSW_F)(nct - 1);
    gy2 = (*y22 - *y21) / (CSW_F)(nrt - 1);

    datatmp = (char *)csw_Malloc (nct * nrt * sizeof(char));
    if (!datatmp) {
        datatmp = dataout;
        nct = *nc2;
        nrt = *nr2;
        gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);
        gy2 = (*y22 - *y21) / (CSW_F)(*nr2 - 1);
    }

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

    dpct = gx2 / gx1;

/*
    each output image row will map the same to the input
    image row, so precalculate the mapping and save it
    in the xpctsave, jlastsave and jtsave array.
*/

    for (jj=0; jj<nct; jj++) {
        xt = jj * gx2 + dx1;
        jt = (int)(xt / gx1);
        if (jt > nc1 - 2) jt = nc1 - 2;
        xpct = (xt - jt * gx1) / gx1;
        jlast = (int)((gx1 - xpct * gx1) / gx2) + jj;
        if (jlast < jj) jlast = jj;
        if (jlast > nct - 1) jlast = nct - 1;
        xpctsave[jj] = xpct;
        jlastsave[jj] = jlast;
        jtsave[jj] = jt;
    }

/*
    loop through the output image, interpolating values from 
    the input image.
*/

    for (ii=0; ii<nrt; ii++) {

        yt = ii * gy2 + dy1;
        it = (int)(yt / gy1);
        if (it > nr1 - 2) it = nr1 - 2;
        if (it < 0) it = 0;
        i1 = it * nc1;
        i2 = ii * nct;
        ypct = (yt - it * gy1) / gy1;

        jj = 0;
        while (jj < nct) {

            jlast = jlastsave[jj];
            xpct = xpctsave[jj];
            jt = jtsave[jj];

        /*
            get the four input image cell values that immediately
            surround the output image cell location
        */
            z1 = (CSW_F)datain[i1 + jt];
            z2 = (CSW_F)datain[i1 + jt + 1];
            z3 = (CSW_F)datain[i1 + jt + nc1];
            z4 = (CSW_F)datain[i1 + jt + nc1 + 1];

        /*
            if there is only one null corner, assign the output a value
            that is the average of the adjacent good corners.  If
            there is more than one null corner, assign the output
            cell a null value.
        */
            if (nullval != 0) {
                n = 0;
                if (z1 >= nullval) {
                    z1 = (z2 + z3) / 2.0f;
                    n++;
                }
                if (z2 >= nullval) {
                    z2 = (z1 + z4) / 2.0f;
                    n++;
                }
                if (z3 >= nullval) {
                    z3 = (z1 + z4) / 2.0f;
                    n++;
                }
                if (z4 >= nullval) {
                    z4 = (z2 + z3) / 2.0f;
                    n++;
                }
                if (n > 1) {
                    while (jj <= jlast) {
                        datatmp[i2+jj] = (char)nullval;
                        jj++;
                    }
                    continue;
                }
            }

        /*
            do the bilinear interpolation
        */
            zleft = z1 + (z3 - z1) * ypct;
            zright = z2 + (z4 - z2) * ypct;

            while (jj <= jlast) {
                datatmp[i2 + jj] = (char)(zleft + (zright - zleft) * xpct);
                xpct += dpct;
                jj++;
            }

        }

    }

/*
    double the image if needed
*/
    if (datatmp != dataout) {
        DoubleImage8 (datatmp, dataout, nct, nrt, *nc2, *nr2);
    }

    return 1;

}  /*  end of function gpf_InterpImage8B  */




/*
  ****************************************************************

                    D o u b l e I m a g e 8

  ****************************************************************

  Double the number of rows and cells in an 8 bit signed image
  by replication of adjacent pixels.  The output image is effectively
  half the visual resolution of the input.

*/

int GPFImage::DoubleImage8 (const char *datain, char *dataout, 
                         int nc1, int nr1, int nc2, int nr2)
{
    int             i, j, i1, i2, ii, jj, rowsize;
    int             nc1orig;

    nc1orig = nc1;

    rowsize = nc2 * sizeof(char);
    if (nc1 * 2  >  nc2) {
        nc1 = nc2 / 2;
    }
    if (nr1 * 2  >  nr2) {
        nr1 = nr2 / 2;
    }

    ii = 0;

    for (i=0; i<nr1; i++) {
        i1 = i * nc1orig;
        i2 = ii * nc2;
        jj = i2;
        for (j=0; j<nc1; j++) {
            dataout[jj] = datain[i1+j];
            jj++;
            dataout[jj] = datain[i1+j];
            jj++;
        }
        dataout[i2 + nc2 - 1] = datain[i1 + nc1orig - 1];
        memcpy (dataout + i2 + nc2, dataout + i2, rowsize);
        ii += 2;
    }

    if (ii < nr2) {
        i2 = (ii - 1) * nc2;
        memcpy (dataout + i2 + nc2, dataout + i2, rowsize);
    }

    return 1;

}  /*  end of private DoubleImage8 function  */




/*
  ****************************************************************

               g p f _ I n t e r p I m a g e 1 6 B

  ****************************************************************

    For a specified location in the output image, do a bilinear interpolation 
  of the four cells in the input image which surround the output.  If more
  than one of the input image corners is null, output a null.  If only one
  of the input is null, assign it a value that is the average of the adjacent
  non null corners and proceed with the interpolation.

    If the output image extends beyond the input image boundaries, the
  specified limits of the output image will be modified to the part that
  overlaps the input image.

    This function is used for 16 bit signed image data.

*/

int GPFImage::gpf_InterpImage16B (short *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                        int nc1, int nr1, int nullval,
                        short *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                        int *nc2, int *nr2)
{
    int          ii, jj, it, jt, i1, i2, istat, n, jlast;
    int          nct, nrt;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2, gy2;
    CSW_F        xpct, ypct, dpct, z1, z2, z3, z4, zleft, zright;
    short        *datatmp = NULL;
    CSW_F        xpctsave[MAX_ROWS];
    int          jlastsave[MAX_ROWS],
                 jtsave[MAX_ROWS];

    auto fscope = [&]()
    {
        if (datatmp != dataout) {
            csw_Free (datatmp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1  ||  *nr2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImage (x11, y11, x12, y12, 
                       x21, y21, x22, y22, nc2, nr2);
    if (istat == -1) {
        return -1;
    }

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);

/*
    halve the output image size for bilinear interpolation
    this is then doubled after the bilinear loop is done
*/

    nrt = *nr2 / 2;
    if (*nr2 % 2  == 1) nrt++;
    nct = *nc2 / 2;
    if (*nc2 % 2  == 1) nct++;
    gx2 = (*x22 - *x21) / (CSW_F)(nct - 1);
    gy2 = (*y22 - *y21) / (CSW_F)(nrt - 1);

    datatmp = (short *)csw_Malloc (nct * nrt * sizeof(short));
    if (!datatmp) {
        datatmp = dataout;
        nct = *nc2;
        nrt = *nr2;
        gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);
        gy2 = (*y22 - *y21) / (CSW_F)(*nr2 - 1);
    }

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

    dpct = gx2 / gx1;

/*
    each output image row will map the same to the input
    image row, so precalculate the mapping and save it
    in the xpctsave, jlastsave and jtsave array.
*/

    for (jj=0; jj<nct; jj++) {
        xt = jj * gx2 + dx1;
        jt = (int)(xt / gx1);
        if (jt > nc1 - 2) jt = nc1 - 2;
        xpct = (xt - jt * gx1) / gx1;
        jlast = (int)((gx1 - xpct * gx1) / gx2) + jj;
        if (jlast < jj) jlast = jj;
        if (jlast > nct - 1) jlast = nct - 1;
        xpctsave[jj] = xpct;
        jlastsave[jj] = jlast;
        jtsave[jj] = jt;
    }

/*
    loop through the output image, interpolating values from 
    the input image.
*/
    for (ii=0; ii<nrt; ii++) {

        yt = ii * gy2 + dy1;
        it = (int)(yt / gy1);
        if (it > nr1 - 2) it = nr1 - 2;
        if (it < 0) it = 0;
        i1 = it * nc1;
        i2 = ii * nct;
        ypct = (yt - it * gy1) / gy1;

        jj = 0;
        while (jj < nct) {

            jlast = jlastsave[jj];
            xpct = xpctsave[jj];
            jt = jtsave[jj];

        /*
            get the four input image cell values that imediately
            surround the output image cell location
        */
            z1 = (CSW_F)datain[i1 + jt];
            z2 = (CSW_F)datain[i1 + jt + 1];
            z3 = (CSW_F)datain[i1 + jt + nc1];
            z4 = (CSW_F)datain[i1 + jt + nc1 + 1];

        /*
            if there is only one null corner, assign the output a value
            that is the average of the adjacent good corners.  If
            there is more than one null corner, assign the output
            cell a null value.
        */
            if (nullval != 0) {
                n = 0;
                if (z1 >= nullval) {
                    z1 = (z2 + z3) / 2.0f;
                    n++;
                }
                if (z2 >= nullval) {
                    z2 = (z1 + z4) / 2.0f;
                    n++;
                }
                if (z3 >= nullval) {
                    z3 = (z1 + z4) / 2.0f;
                    n++;
                }
                if (z4 >= nullval) {
                    z4 = (z2 + z3) / 2.0f;
                    n++;
                }
                if (n > 1) {
                    while (jj <= jlast) {
                        datatmp[i2+jj] = (short)nullval;
                        jj++;
                    }
                    continue;
                }
            }

        /*
            do the bilinear interpolation
        */
            zleft = z1 + (z3 - z1) * ypct;
            zright = z2 + (z4 - z2) * ypct;

            while (jj <= jlast) {
                datatmp[i2 + jj] = (short)(zleft + (zright - zleft) * xpct);
                xpct += dpct;
                jj++;
            }

        }

    }

/*
    double the image if needed
*/
    if (datatmp != dataout) {
        DoubleImage16 (datatmp, dataout, nct, nrt, *nc2, *nr2);
    }

    return 1;

}  /*  end of function gpf_InterpImage16B  */




/*
  ****************************************************************

                  D o u b l e I m a g e 1 6

  ****************************************************************

    Double the number of rows and cells in a 16 bit signed image
  by replication of adjacent pixels.  The output image is effectively
  half the visual resolution of the input.

*/

int GPFImage::DoubleImage16 (const short *datain, short *dataout, 
                          int nc1, int nr1, int nc2, int nr2)
{
    int             i, j, i1, i2, ii, jj, rowsize;
    int             nc1orig;

    nc1orig = nc1;

    rowsize = nc2 * sizeof(short);
    if (nc1 * 2  >  nc2) {
        nc1 = nc2 / 2;
    }
    if (nr1 * 2  >  nr2) {
        nr1 = nr2 / 2;
    }

    ii = 0;

    for (i=0; i<nr1; i++) {
        i1 = i * nc1orig;
        i2 = ii * nc2;
        jj = i2;
        for (j=0; j<nc1; j++) {
            dataout[jj] = datain[i1+j];
            jj++;
            dataout[jj] = datain[i1+j];
            jj++;
        }
        dataout[i2 + nc2 - 1] = datain[i1 + nc1orig - 1];
        memcpy (dataout + i2 + nc2, dataout + i2, rowsize);
        ii += 2;
    }

    if (ii < nr2) {
        i2 = (ii - 1) * nc2;
        memcpy (dataout + i2 + nc2, dataout + i2, rowsize);
    }

    return 1;

}  /*  end of private DoubleImage16 function  */




/*
  ****************************************************************

               g p f _ I n t e r p I m a g e 3 2 B

  ****************************************************************

    For a specified location in the output image, do a bilinear interpolation 
  of the four cells in the input image which surround the output.  If more
  than one of the input image corners is null, output a null.  If only one
  of the input is null, assign it a value that is the average of the adjacent
  non null corners and proceed with the interpolation.

    If the output image extends beyond the input image boundaries, the
  specified limits of the output image will be modified to the part that
  overlaps the input image.

    This function is used for 32 bit signed image data.

*/

int GPFImage::gpf_InterpImage32B (int *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                        int nc1, int nr1, int nullval,
                        int *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                        int *nc2, int *nr2)
{
    int          ii, jj, it, jt, i1, i2, istat, n, jlast;
    int          nct, nrt;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2, gy2;
    CSW_F        xpct, ypct, dpct, z1, z2, z3, z4, zleft, zright;
    int          *datatmp = NULL;
    CSW_F        xpctsave[MAX_ROWS];
    int          jlastsave[MAX_ROWS],
                 jtsave[MAX_ROWS];


    auto fscope = [&]()
    {
        if (datatmp != dataout) {
            csw_Free (datatmp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1  ||  *nr2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImage (x11, y11, x12, y12, 
                       x21, y21, x22, y22, nc2, nr2);
    if (istat == -1) {
        return -1;
    }

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);

/*
    halve the output image size for bilinear interpolation
    this is then doubled after the bilinear loop is done
*/

    nrt = *nr2 / 2;
    if (*nr2 % 2  == 1) nrt++;
    nct = *nc2 / 2;
    if (*nc2 % 2  == 1) nct++;
    gx2 = (*x22 - *x21) / (CSW_F)(nct - 1);
    gy2 = (*y22 - *y21) / (CSW_F)(nrt - 1);

    datatmp = (int  *)csw_Malloc (nct * nrt * sizeof(int ));
    if (!datatmp) {
        datatmp = dataout;
        nct = *nc2;
        nrt = *nr2;
        gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);
        gy2 = (*y22 - *y21) / (CSW_F)(*nr2 - 1);
    }

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

    dpct = gx2 / gx1;

/*
    each output image row will map the same to the input
    image row, so precalculate the mapping and save it
    in the xpctsave, jlastsave and jtsave array.
*/

    for (jj=0; jj<nct; jj++) {
        xt = jj * gx2 + dx1;
        jt = (int)(xt / gx1);
        if (jt > nc1 - 2) jt = nc1 - 2;
        xpct = (xt - jt * gx1) / gx1;
        jlast = (int)((gx1 - xpct * gx1) / gx2) + jj;
        if (jlast < jj) jlast = jj;
        if (jlast > nct - 1) jlast = nct - 1;
        xpctsave[jj] = xpct;
        jlastsave[jj] = jlast;
        jtsave[jj] = jt;
    }

/*
    loop through the output image, interpolating values from 
    the input image.
*/
    for (ii=0; ii<nrt; ii++) {

        yt = ii * gy2 + dy1;
        it = (int)(yt / gy1);
        if (it > nr1 - 2) it = nr1 - 2;
        if (it < 0) it = 0;
        i1 = it * nc1;
        i2 = ii * nct;
        ypct = (yt - it * gy1) / gy1;

        jj = 0;
        while (jj < nct) {

            jlast = jlastsave[jj];
            xpct = xpctsave[jj];
            jt = jtsave[jj];

        /*
            get the four input image cell values that imediately
            surround the output image cell location
        */
            z1 = (CSW_F)datain[i1 + jt];
            z2 = (CSW_F)datain[i1 + jt + 1];
            z3 = (CSW_F)datain[i1 + jt + nc1];
            z4 = (CSW_F)datain[i1 + jt + nc1 + 1];

        /*
            if there is only one null corner, assign the output a value
            that is the average of the adjacent good corners.  If
            there is more than one null corner, assign the output
            cell a null value.
        */
            if (nullval != 0) {
                n = 0;
                if (z1 >= nullval) {
                    z1 = (z2 + z3) / 2.0f;
                    n++;
                }
                if (z2 >= nullval) {
                    z2 = (z1 + z4) / 2.0f;
                    n++;
                }
                if (z3 >= nullval) {
                    z3 = (z1 + z4) / 2.0f;
                    n++;
                }
                if (z4 >= nullval) {
                    z4 = (z2 + z3) / 2.0f;
                    n++;
                }
                if (n > 1) {
                    while (jj <= jlast) {
                        datatmp[i2+jj] = nullval;
                        jj++;
                    }
                    continue;
                }
            }

        /*
            do the bilinear interpolation
        */
            zleft = z1 + (z3 - z1) * ypct;
            zright = z2 + (z4 - z2) * ypct;

            while (jj <= jlast) {
                datatmp[i2 + jj] = (int )(zleft + (zright - zleft) * xpct);
                xpct += dpct;
                jj++;
            }

        }

    }

/*
    double the image if needed
*/
    if (datatmp != dataout) {
        DoubleImage32 (datatmp, dataout, nct, nrt, *nc2, *nr2);
    }

    return 1;

}  /*  end of function gpf_InterpImage32B  */




/*
  ****************************************************************

                  D o u b l e I m a g e 3 2

  ****************************************************************

    Double the number of rows and cells in a 32 bit signed image
  by replication of adjacent pixels.  The output image is effectively
  half the visual resolution of the input.

*/

int GPFImage::DoubleImage32 (const int *datain, int *dataout, 
                          int nc1, int nr1, int nc2, int nr2)
{
    int             i, j, i1, i2, ii, jj, rowsize;
    int             nc1orig;

    nc1orig = nc1;

    rowsize = nc2 * sizeof(int );
    if (nc1 * 2  >  nc2) {
        nc1 = nc2 / 2;
    }
    if (nr1 * 2  >  nr2) {
        nr1 = nr2 / 2;
    }

    ii = 0;

    for (i=0; i<nr1; i++) {
        i1 = i * nc1orig;
        i2 = ii * nc2;
        jj = i2;
        for (j=0; j<nc1; j++) {
            dataout[jj] = datain[i1+j];
            jj++;
            dataout[jj] = datain[i1+j];
            jj++;
        }
        dataout[i2 + nc2 - 1] = datain[i1 + nc1orig - 1];
        memcpy (dataout + i2 + nc2, dataout + i2, rowsize);
        ii += 2;
    }

    if (ii < nr2) {
        i2 = (ii - 1) * nc2;
        memcpy (dataout + i2 + nc2, dataout + i2, rowsize);
    }

    return 1;

}  /*  end of private DoubleImage32 function  */




/*
  ****************************************************************

           g p f _ I n t e r p I m a g e F l o a t B

  ****************************************************************

    For a specified location in the output image, do a bilinear interpolation 
  of the four cells in the input image which surround the output.  If more
  than one of the input image corners is null, output a null.  If only one
  of the input is null, assign it a value that is the average of the adjacent
  non null corners and proceed with the interpolation.

    If the output image extends beyond the input image boundaries, the
  specified limits of the output image will be modified to the part that
  overlaps the input image.

    This function is used for CSW_F image data.

*/

int GPFImage::gpf_InterpImageFloatB (CSW_F *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                           int nc1, int nr1, CSW_F fnullval,
                           CSW_F *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                           int *nc2, int *nr2)
{
    int          ii, jj, it, jt, i1, i2, istat, n, jlast;
    int          nct, nrt;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2, gy2;
    CSW_F        xpct, ypct, dpct, z1, z2, z3, z4, zleft, zright;
    CSW_F        *datatmp = NULL;
    CSW_F        xpctsave[MAX_ROWS];
    int          jlastsave[MAX_ROWS],
                 jtsave[MAX_ROWS];


    auto fscope = [&]()
    {
        if (datatmp != dataout) {
            csw_Free (datatmp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1  ||  *nr2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImage (x11, y11, x12, y12, 
                       x21, y21, x22, y22, nc2, nr2);
    if (istat == -1) {
        return -1;
    }

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);

/*
    halve the output image size for bilinear interpolation
    this is then doubled after the bilinear loop is done
*/

    nrt = *nr2 / 2;
    if (*nr2 % 2  == 1) nrt++;
    nct = *nc2 / 2;
    if (*nc2 % 2  == 1) nct++;
    gx2 = (*x22 - *x21) / (CSW_F)(nct - 1);
    gy2 = (*y22 - *y21) / (CSW_F)(nrt - 1);

    datatmp = (CSW_F *)csw_Malloc (nct * nrt * sizeof(CSW_F));
    if (!datatmp) {
        datatmp = dataout;
        nct = *nc2;
        nrt = *nr2;
        gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);
        gy2 = (*y22 - *y21) / (CSW_F)(*nr2 - 1);
    }

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

    dpct = gx2 / gx1;

/*
    each output image row will map the same to the input
    image row, so precalculate the mapping and save it
    in the xpctsave, jlastsave and jtsave array.
*/

    for (jj=0; jj<nct; jj++) {
        xt = jj * gx2 + dx1;
        jt = (int)(xt / gx1);
        if (jt > nc1 - 2) jt = nc1 - 2;
        xpct = (xt - jt * gx1) / gx1;
        jlast = (int)((gx1 - xpct * gx1) / gx2) + jj;
        if (jlast < jj) jlast = jj;
        if (jlast > nct - 1) jlast = nct - 1;
        xpctsave[jj] = xpct;
        jlastsave[jj] = jlast;
        jtsave[jj] = jt;
    }

/*
    loop through the output image, interpolating values from 
    the input image.
*/
    for (ii=0; ii<nrt; ii++) {

        yt = ii * gy2 + dy1;
        it = (int)(yt / gy1);
        if (it >= nr1-1) it--;
        if (it < 0) it++;
        i1 = it * nc1;
        i2 = ii * nct;
        ypct = (yt - it * gy1) / gy1;

        jj = 0;
        while (jj < nct) {

            jlast = jlastsave[jj];
            xpct = xpctsave[jj];
            jt = jtsave[jj];

        /*
            get the four input image cell values that imediately
            surround the output image cell location
        */
            z1 = (CSW_F)datain[i1 + jt];
            z2 = (CSW_F)datain[i1 + jt + 1];
            z3 = (CSW_F)datain[i1 + jt + nc1];
            z4 = (CSW_F)datain[i1 + jt + nc1 + 1];

        /*
            if there is only one null corner, assign the output a value
            that is the average of the adjacent good corners.  If
            there is more than one null corner, assign the output
            cell a null value.
        */
            if (fnullval != 0.0f) {
                n = 0;
                if (z1 >= fnullval) {
                    z1 = (z2 + z3) / 2.0f;
                    n++;
                }
                if (z2 >= fnullval) {
                    z2 = (z1 + z4) / 2.0f;
                    n++;
                }
                if (z3 >= fnullval) {
                    z3 = (z1 + z4) / 2.0f;
                    n++;
                }
                if (z4 >= fnullval) {
                    z4 = (z2 + z3) / 2.0f;
                    n++;
                }
                if (n > 1) {
                    while (jj <= jlast) {
                        datatmp[i2 + jj] = fnullval * 10.0f;
                        jj++;
                    }
                    continue;
                }
            }

        /*
            do the bilinear interpolation
        */
            zleft = z1 + (z3 - z1) * ypct;
            zright = z2 + (z4 - z2) * ypct;

            while (jj <= jlast) {
                datatmp[i2 + jj] = zleft + (zright - zleft) * xpct;
                xpct += dpct;
                jj++;
            }

        }

    }

/*
    double the image if needed
*/

    if (datatmp != dataout) {
        DoubleImageFloat (datatmp, dataout, nct, nrt, *nc2, *nr2);
    }

    return 1;

}  /*  end of function gpf_InterpImageFloatB  */




/*
  ****************************************************************

              D o u b l e I m a g e F l o a t

  ****************************************************************

    Double the number of rows and cells in a CSW_Fing point image
  by replication of adjacent pixels.  The output image is effectively
  half the visual resolution of the input.

*/

int GPFImage::DoubleImageFloat (const CSW_F *datain, CSW_F *dataout, 
                             int nc1, int nr1, int nc2, int nr2)
{
    int             i, j, i1, i2, ii, jj, rowsize;
    int             nc1orig;

    nc1orig = nc1;

    rowsize = nc2 * sizeof(CSW_F);
    if (nc1 * 2  >  nc2) {
        nc1 = nc2 / 2;
    }
    if (nr1 * 2  >  nr2) {
        nr1 = nr2 / 2;
    }

    ii = 0;

    for (i=0; i<nr1; i++) {
        i1 = i * nc1orig;
        i2 = ii * nc2;
        jj = i2;
        for (j=0; j<nc1; j++) {
            dataout[jj] = datain[i1+j];
            jj++;
            dataout[jj] = datain[i1+j];
            jj++;
        }
        dataout[i2 + nc2 - 1] = datain[i1 + nc1orig - 1];
        memcpy (dataout + i2 + nc2, dataout + i2, rowsize);
        ii += 2;
    }

    if (ii < nr2) {
        i2 = (ii - 1) * nc2;
        memcpy (dataout + i2 + nc2, dataout + i2, rowsize);
    }

    return 1;

}  /*  end of private DoubleImageFloat function  */



/*
  ****************************************************************

              g p f _ I n t e r p I m a g e 8 R o w

  ****************************************************************

    For each cell in a output image row, find the overlapping cell in the
  input image and assign that value to the output image cell.  This
  version is for 8 bit signed or unsigned image data.

    If the output image row extends beyond the input image boundaries, the
  specified limits of the output image row will be modified to the part that
  overlaps the input image.

*/

int GPFImage::gpf_InterpImage8Row (const char *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                         int nc1, int nr1,
                         char *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                         int *nc2, int *nr2)
{
    int          jj, it, jt, i1, istat;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2;

/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImageRow (x11, y11, x12, y12, 
                          x21, x22, nc2);
    if (istat == -1) {
        return -1;
    }

/*  suppress warnings  */

    y22 = y22;
    nr2 = nr2;

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);

    gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);
    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

/*
    loop through the output image row, assigning values from 
    the input image.
*/

    yt = dy1;
    it = (int)(yt / gy1 + 0.5f);
    if (it > nr1 - 1) it = nr1 - 1;
    if (it < 0) it = 0;
    i1 = it * nc1;

    for (jj=0; jj<*nc2; jj++) {
        xt = jj * gx2 + dx1;
        jt = (int)(xt / gx1 + 0.5f);
        dataout[jj] = datain[i1+jt];
    }

    return 1;

}  /*  end of function gpf_InterpImage8Row  */



/*
  ****************************************************************

            g p f _ I n t e r p I m a g e 8 B R o w

  ****************************************************************

        Do a bilinear interpolation of a single output image row.
    This function is used for 8 bit signed image data.

*/

int GPFImage::gpf_InterpImage8BRow (const char *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                          int nc1, int nr1, int nullval,
                          char *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                          int *nc2, int *nr2)
{
    int          i1, jj, it, jt, istat, n, jlast;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2;
    CSW_F        xpct, ypct, dpct, z1, z2, z3, z4, zleft, zright;

/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImageRow (x11, y11, x12, y12, 
                          x21, x22, nc2);
    if (istat == -1) {
        return -1;
    }

/*  suppress warnings  */

    y22 = y22;
    nr2 = nr2;

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);
    gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

    dpct = gx2 / gx1;

/*
    loop through the output image row, interpolating values from 
    the input image.
*/

    yt = dy1;
    it = (int)(yt / gy1);
    if (it > nr1 - 2) it = nr1 - 2;
    if (it < 0) it = 0;
    i1 = it * nc1;
    ypct = (yt - it * gy1) / gy1;

    jj = 0;
    while (jj < *nc2) {

        xt = jj * gx2 + dx1;
        jt = (int)(xt / gx1);
        if (jt > nc1 - 2) jt = nc1 - 2;
        xpct = (xt - jt * gx1) / gx1;
        jlast = (int)((gx1 - xpct * gx1) / gx2) + jj;
        if (jlast < jj) jlast = jj;
        if (jlast > *nc2 - 1) jlast = *nc2 - 1;

        /*
            get the four input image cell values that immediately
            surround the output image cell location
        */
        z1 = (CSW_F)datain[i1 + jt];
        z2 = (CSW_F)datain[i1 + jt + 1];
        z3 = (CSW_F)datain[i1 + jt + nc1];
        z4 = (CSW_F)datain[i1 + jt + nc1 + 1];

        /*
            if there is only one null corner, assign the output a value
            that is the average of the adjacent good corners.  If
            there is more than one null corner, assign the output
            cell a null value.
        */
        if (nullval != 0) {
            n = 0;
            if (z1 >= nullval) {
                z1 = (z2 + z3) / 2.0f;
                n++;
            }
            if (z2 >= nullval) {
                z2 = (z1 + z4) / 2.0f;
                n++;
            }
            if (z3 >= nullval) {
                z3 = (z1 + z4) / 2.0f;
                n++;
            }
            if (z4 >= nullval) {
                z4 = (z2 + z3) / 2.0f;
                n++;
            }
            if (n > 1) {
                while (jj <= jlast) {
                    dataout[jj] = (char)nullval;
                    jj++;
                }
                continue;
            }
        }

    /*
        do the linear interpolation 
    */
        zleft = z1 + (z3 - z1) * ypct;
        zright = z2 + (z4 - z2) * ypct;

        while (jj <= jlast) {
            dataout[jj] = (char)(zleft + (zright - zleft) * xpct);
            xpct += dpct;
            jj++;
        }

    }

    return 1;

}  /*  end of function gpf_InterpImage8BRow  */




/*
  ****************************************************************

                   C l i p I m a g e R o w

  ****************************************************************

    If the image row extends beyond the boundary of the first
  image, clip it to the first image.  If there is no overlapping area,
  return -1.  On success, return 1.  The limits of the image row
  may be adjusted if needed.

*/

int GPFImage::ClipImageRow (CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                         CSW_F *x21, CSW_F *x22, int *nc2)
{
    CSW_F        gx;

/*
    cell sizes of second image
*/

    gx = (*x22 - *x21) / (CSW_F)(*nc2 - 1);

/*
    check for no overlap
*/

    if (x11 >= *x22  ||  x12 <= *x21) {
        return -1;
    }

/*  suppress warnings  */

    y11 = y11;
    y12 = y12;

/*
    adjust image row limits if needed
*/

    if (*x22 > x12) {
        *nc2 = (int)((x12 - *x21) / gx + 1.1f);
        *x22 = *x21 + (*nc2 - 1) * gx;
    }

    if (*x21 < x11) {
        *nc2 = (int)((*x22 - x11) / gx + 1.1f);
        *x21 = *x22 - (*nc2 - 1) * gx;
    }

    return 1;

}  /*  end of private ClipImageRow function  */



/*
  ****************************************************************

       g p f _ C o n v e r t T o C o l o r I m a g e R o w

  ****************************************************************

    This function uses the colors set up with the set color bands
  functions to convert a data image into a color image.  If the
  function succeeds, 1 is returned.  If the color bands have not been
  set up, then -1 is returned and no processing is done.

    type = 1, 8 bit signed char data
    type = 2, 16 bit signed short data
    type = 3, 32 bit signed int data
    type = 4, CSW_F data

    If type is out of range, -1 is returned.

*/

int GPFImage::gpf_ConvertToColorImageRow (const void *datain, int nc, int type, 
                                unsigned char *dataout)
{

    if (!ColorLookup) {
        return -1;
    }
    
    switch (type) {

        case 1:
            ColorImage8Row ((char *)datain, nc, dataout);
            break;

        case 2:
            ColorImage16Row ((short *)datain, nc, dataout);
            break;

        case 3:
            ColorImage32Row ((int *)datain, nc, dataout);
            break;

        case 4:
            ColorImageFloatRow ((CSW_F *)datain, nc, dataout);
            break;

        default:
            return -1;

    }

    return 1;

}  /*  end of function gpf_ConvertToColorImage  */



/*
  ****************************************************************

               C o l o r I m a g e 8 R o w

  ****************************************************************

    Use lookup table to color an 8 bit data image row.  

*/

int GPFImage::ColorImage8Row (const char *datain, int nc, unsigned char *dataout)
{
    int             j, i2;

/*
    version for no null values
*/

    if (InullVal == 0) {
        for (j=0; j<nc; j++) {
            i2 = (datain[j] - LookupMin) / LookupSize;
            dataout[j] = (unsigned char)ColorLookup[i2];
        }
    }

/*
    version for possible null values
*/

    else {
        for (j=0; j<nc; j++) {
            if (datain[j] >= InullVal) {
                dataout[j] = BackgroundColor;
            }
            else {
                i2 = (datain[j] - LookupMin) / LookupSize;
                dataout[j] = (unsigned char)ColorLookup[i2];
            }
        }
    }

    return 1;

}  /*  end of private ColorImage8Row function  */



/*
  ****************************************************************

                C o l o r I m a g e 1 6 R o w

  ****************************************************************

    Use lookup table to color a row in a 16 bit data image.

*/

int GPFImage::ColorImage16Row (const short *datain, int nc, unsigned char *dataout)
{
    int             j, i2;

/*
    version for no null values
*/

    if (InullVal == 0) {
        for (j=0; j<nc; j++) {
            i2 = (datain[j] - LookupMin) / LookupSize;
            dataout[j] = (unsigned char)ColorLookup[i2];
        }
    }

/*
    version for possible null values
*/

    else {
        for (j=0; j<nc; j++) {
            if (datain[j] >= InullVal) {
                dataout[j] = BackgroundColor;
            }
            else {
                i2 = (datain[j] - LookupMin) / LookupSize;
                dataout[j] = (unsigned char)ColorLookup[i2];
            }
        }
    }

    return 1;

}  /*  end of private ColorImage16Row function  */



/*
  ****************************************************************

             C o l o r I m a g e 3 2 R o w

  ****************************************************************

    Use lookup table to color a row in a 32 bit data image.

*/

int GPFImage::ColorImage32Row (const int *datain, int nc, unsigned char *dataout)
{
    int             j, i2;

/*
    version for no null values
*/

    if (InullVal == 0) {
        for (j=0; j<nc; j++) {
            i2 = (datain[j] - LookupMin) / LookupSize;
            dataout[j] = (unsigned char)ColorLookup[i2];
        }
    }

/*
    version for possible null values
*/

    else {
        for (j=0; j<nc; j++) {
            if (datain[j] >= InullVal) {
                dataout[j] = BackgroundColor;
            }
            else {
                i2 = (datain[j] - LookupMin) / LookupSize;
                dataout[j] = (unsigned char)ColorLookup[i2];
            }
        }
    }

    return 1;

}  /*  end of private ColorImage32Row function  */



/*
  ****************************************************************

            C o l o r I m a g e F l o a t R o w

  ****************************************************************

    Use lookup table to color a row in a CSW_F data image.

*/

int GPFImage::ColorImageFloatRow (const CSW_F *datain, int nc, unsigned char *dataout)
{
    int             j, i2;

/*
    version for no null values
*/

    if (FnullVal == 0.0f) {
        for (j=0; j<nc; j++) {
            i2 = (int)((datain[j] - LookupMinF) / LookupSizeF + .5f);
            dataout[j] = (unsigned char)ColorLookup[i2];
        }
    }

/*
    version for possible null values
*/

    else {
        for (j=0; j<nc; j++) {
            if (datain[j] >= FnullVal) {
                dataout[j] = BackgroundColor;
            }
            else {
                i2 = (int)((datain[j] - LookupMinF) / LookupSizeF + .5f);
                dataout[j] = (unsigned char)ColorLookup[i2];
            }
        }
    }

    return 1;

}  /*  end of private ColorImageFloatRow function  */





/*
  ****************************************************************

          g p f _ I n t e r p I m a g e 1 6 B R o w

  ****************************************************************

        Do a bilinear interpolation of a single output image row.
    This function is used for 16 bit signed image data.

*/

int GPFImage::gpf_InterpImage16BRow (short *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                           int nc1, int nr1, int nullval,
                           short *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                           int *nc2, int *nr2)
{
    int          i1, jj, it, jt, istat, n, jlast;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2;
    CSW_F        xpct, ypct, dpct, z1, z2, z3, z4, zleft, zright;

/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImageRow (x11, y11, x12, y12, 
                          x21, x22, nc2);
    if (istat == -1) {
        return -1;
    }

/*  suppress warnings */

    y22 = y22;
    nr2 = nr2;

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);
    gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

    dpct = gx2 / gx1;

/*
    loop through the output image row, interpolating values from 
    the input image.
*/

    yt = dy1;
    it = (int)(yt / gy1);
    if (it > nr1 - 2) it = nr1 - 2;
    if (it < 0) it = 0;
    i1 = it * nc1;
    ypct = (yt - it * gy1) / gy1;

    jj = 0;
    while (jj < *nc2) {

        xt = jj * gx2 + dx1;
        jt = (int)(xt / gx1);
        if (jt > nc1 - 2) jt = nc1 - 2;
        xpct = (xt - jt * gx1) / gx1;
        jlast = (int)((gx1 - xpct * gx1) / gx2) + jj;
        if (jlast < jj) jlast = jj;
        if (jlast > *nc2 - 1) jlast = *nc2 - 1;

    /*
        get the four input image cell values that immediately
        surround the output image cell location
    */
        z1 = (CSW_F)datain[i1 + jt];
        z2 = (CSW_F)datain[i1 + jt + 1];
        z3 = (CSW_F)datain[i1 + jt + nc1];
        z4 = (CSW_F)datain[i1 + jt + nc1 + 1];

    /*
        if there is only one null corner, assign the output a value
        that is the average of the adjacent good corners.  If
        there is more than one null corner, assign the output
        cell a null value.
    */
        if (nullval != 0) {
            n = 0;
            if (z1 >= nullval) {
                z1 = (z2 + z3) / 2.0f;
                n++;
            }
            if (z2 >= nullval) {
                z2 = (z1 + z4) / 2.0f;
                n++;
            }
            if (z3 >= nullval) {
                z3 = (z1 + z4) / 2.0f;
                n++;
            }
            if (z4 >= nullval) {
                z4 = (z2 + z3) / 2.0f;
                n++;
            }
            if (n > 1) {
                while (jj <= jlast) {
                    dataout[jj] = (short)nullval;
                    jj++;
                }
                continue;
            }
        }

    /*
        do the bilinear interpolation
    */
        zleft = z1 + (z3 - z1) * ypct;
        zright = z2 + (z4 - z2) * ypct;

        while (jj <= jlast) {
            dataout[jj] = (short)(zleft + (zright - zleft) * xpct);
            xpct += dpct;
            jj++;
        }

    }

    return 1;

}  /*  end of function gpf_InterpImage16BRow  */





/*
  ****************************************************************

          g p f _ I n t e r p I m a g e 3 2 B R o w

  ****************************************************************

        Do a bilinear interpolation of a single output image row.
    This function is used for 32 bit signed image data.

*/

int GPFImage::gpf_InterpImage32BRow (int *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                           int nc1, int nr1, int nullval,
                           int *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                           int *nc2, int *nr2)
{
    int          i1, jj, it, jt, istat, n, jlast;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2;
    CSW_F        xpct, ypct, dpct, z1, z2, z3, z4, zleft, zright;

/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImageRow (x11, y11, x12, y12, 
                          x21, x22, nc2);
    if (istat == -1) {
        return -1;
    }

/*  suppress warnings  */
    
    y22 = y22;
    nr2 = nr2;

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);
    gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

    dpct = gx2 / gx1;

/*
    loop through the output image row, interpolating values from 
    the input image.
*/

    yt = dy1;
    it = (int)(yt / gy1);
    if (it > nr1 - 2) it = nr1 - 2;
    if (it < 0) it = 0;
    i1 = it * nc1;
    ypct = (yt - it * gy1) / gy1;

    jj = 0;
    while (jj < *nc2) {

        xt = jj * gx2 + dx1;
        jt = (int)(xt / gx1);
        if (jt > nc1 - 2) jt = nc1 - 2;
        xpct = (xt - jt * gx1) / gx1;
        jlast = (int)((gx1 - xpct * gx1) / gx2) + jj;
        if (jlast < jj) jlast = jj;
        if (jlast > *nc2 - 1) jlast = *nc2 - 1;

    /*
        get the four input image cell values that immediately
        surround the output image cell location
    */
        z1 = (CSW_F)datain[i1 + jt];
        z2 = (CSW_F)datain[i1 + jt + 1];
        z3 = (CSW_F)datain[i1 + jt + nc1];
        z4 = (CSW_F)datain[i1 + jt + nc1 + 1];

    /*
        if there is only one null corner, assign the output a value
        that is the average of the adjacent good corners.  If
        there is more than one null corner, assign the output
        cell a null value.
    */
        if (nullval != 0) {
            n = 0;
            if (z1 >= nullval) {
                z1 = (z2 + z3) / 2.0f;
                n++;
            }
            if (z2 >= nullval) {
                z2 = (z1 + z4) / 2.0f;
                n++;
            }
            if (z3 >= nullval) {
                z3 = (z1 + z4) / 2.0f;
                n++;
            }
            if (z4 >= nullval) {
                z4 = (z2 + z3) / 2.0f;
                n++;
            }
            if (n > 1) {
                while (jj <= jlast) {
                    dataout[jj] = nullval;
                    jj++;
                }
                continue;
            }
        }

    /*
        do the bilinear interpolation
    */
        zleft = z1 + (z3 - z1) * ypct;
        zright = z2 + (z4 - z2) * ypct;

        while (jj <= jlast) {
            dataout[jj] = (int)(zleft + (zright - zleft) * xpct);
            xpct += dpct;
            jj++;
        }

    }

    return 1;

}  /*  end of function gpf_InterpImage32BRow  */





/*
  ****************************************************************

          g p f _ I n t e r p I m a g e F l o a t B R o w

  ****************************************************************

        Do a bilinear interpolation of a single output image row.
    This function is used for CSW_F image data.

*/

int GPFImage::gpf_InterpImageFloatBRow (CSW_F *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                              int nc1, int nr1, CSW_F nullval,
                              CSW_F *dataout,
                              CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                              int *nc2, int *nr2)
{
    int          i1, jj, it, jt, istat, n, jlast;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2;
    CSW_F        xpct, ypct, dpct, z1, z2, z3, z4, zleft, zright;

/*
    check obvious errors
*/
    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1) {
        return -1;
    }

/*  suppress warnings  */

    y22 = y22;
    nr2 = nr2;

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/
    istat = ClipImageRow (x11, y11, x12, y12, 
                          x21, x22, nc2);
    if (istat == -1) {
        return -1;
    }

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/
    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);
    gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);

    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

    dpct = gx2 / gx1;

/*
    loop through the output image row, interpolating values from 
    the input image.
*/

    yt = dy1;
    it = (int)(yt / gy1);
    if (it > nr1 - 2) it = nr1 - 2;
    if (it < 0) it = 0;
    i1 = it * nc1;
    ypct = (yt - it * gy1) / gy1;

    jj = 0;
    while (jj < *nc2) {

        xt = jj * gx2 + dx1;
        jt = (int)(xt / gx1);
        if (jt > nc1 - 2) jt = nc1 - 2;
        xpct = (xt - jt * gx1) / gx1;
        jlast = (int)((gx1 - xpct * gx1) / gx2) + jj;
        if (jlast < jj) jlast = jj;
        if (jlast > *nc2 - 1) jlast = *nc2 - 1;

    /*
        get the four input image cell values that immediately
        surround the output image cell location
    */
        z1 = (CSW_F)datain[i1 + jt];
        z2 = (CSW_F)datain[i1 + jt + 1];
        z3 = (CSW_F)datain[i1 + jt + nc1];
        z4 = (CSW_F)datain[i1 + jt + nc1 + 1];

    /*
        if there is only one null corner, assign the output a value
        that is the average of the adjacent good corners.  If
        there is more than one null corner, assign the output
        cell a null value.
    */
        if (nullval != 0) {
            n = 0;
            if (z1 >= nullval) {
                z1 = (z2 + z3) / 2.0f;
                n++;
            }
            if (z2 >= nullval) {
                z2 = (z1 + z4) / 2.0f;
                n++;
            }
            if (z3 >= nullval) {
                z3 = (z1 + z4) / 2.0f;
                n++;
            }
            if (z4 >= nullval) {
                z4 = (z2 + z3) / 2.0f;
                n++;
            }
            if (n > 1) {
                while (jj <= jlast) {
                    dataout[jj] = nullval;
                    jj++;
                }
                continue;
            }
        }

    /*
        do the bilinear interpolation
    */
        zleft = z1 + (z3 - z1) * ypct;
        zright = z2 + (z4 - z2) * ypct;

        while (jj <= jlast) {
            dataout[jj] = (zleft + (zright - zleft) * xpct);
            xpct += dpct;
            jj++;
        }

    }

    return 1;

}  /*  end of function gpf_InterpImageFloatBRow  */


/*
  ****************************************************************

              g p f _ I m a g e R o t a t e C C W

  ****************************************************************

  Rotate an image 90 degrees CCW.  The first cell (lower left)
  rotates to the lower right of the new image.  This is used when
  a plot needs to be rotated to fit on a sheet of paper.

*/

int GPFImage::gpf_ImageRotateCCW (const char *datain, int nc, int nr, int type,
                        char **dataout)
{

    int             i, j, k, inew, size;
    char            *ctmp = NULL;
    short           *stmp1 = NULL, *stmp2 = NULL;
    int             *ltmp1 = NULL, *ltmp2 = NULL;
    CSW_F           *ftmp1 = NULL, *ftmp2 = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            *dataout = NULL;
            csw_Free (ctmp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    allocate the output image
*/
    switch (type) {
        
        case 0:
            size = sizeof(unsigned char);
            break;

        case 1:
            size = sizeof(char);
            break;

        case 2:
            size = sizeof(short);
            break;

        case 3:
            size = sizeof(int );
            break;

        case 4:
            size = sizeof(CSW_F);
            break;

        default:
            return -1;

    }

    ctmp = (char *)csw_Malloc (nc * nr * size);
    if (!ctmp) {
        return -1;
    }

/*
    rotate char or unsigned char image
*/
    if (type == 0  ||  type == 1) {
        for (i=0; i<nc; i++) {
            inew = (i + 1) * nr - 1;
            k = 0;
            for (j=0; j<nr; j++) {
                ctmp[inew+k] = datain[nc*j+i];
                k--;
            }
        }
        *dataout = ctmp;
        bsuccess = true;
        return 1;
    }

/*
    rotate 16 bit short image
*/
    if (type == 2) {
        stmp1 = (short *)datain;
        stmp2 = (short *)ctmp;
        for (i=0; i<nc; i++) {
            inew = (i + 1) * nr - 1;
            k = 0;
            for (j=0; j<nr; j++) {
                stmp2[inew+k] = stmp1[nc*j+i];
                k--;
            }
        }
        *dataout = (char *)stmp2;
        bsuccess = true;
        return 1;
    }
        
/*
    rotate 32 bit int  image
*/
    if (type == 3) {
        ltmp1 = (int  *)datain;
        ltmp2 = (int  *)ctmp;
        for (i=0; i<nc; i++) {
            inew = (i + 1) * nr - 1;
            k = 0;
            for (j=0; j<nr; j++) {
                ltmp2[inew+k] = ltmp1[nc*j+i];
                k--;
            }
        }
        *dataout = (char *)ltmp2;
        bsuccess = true;
        return 1;
    }
        
/*
    rotate CSW_F image
*/
    if (type == 4) {
        ftmp1 = (CSW_F *)datain;
        ftmp2 = (CSW_F *)ctmp;
        for (i=0; i<nc; i++) {
            inew = (i + 1) * nr - 1;
            k = 0;
            for (j=0; j<nr; j++) {
                ftmp2[inew+k] = ftmp1[nc*j+i];
                k--;
            }
        }
        *dataout = (char *)ftmp2;
        bsuccess = true;
        return 1;
    }

    return -1;

}  /*  end of function gpf_ImageRotateCCW  */





/*
  ****************************************************************

         g p f _ I m a g e R e o r i g i n C C W

  ****************************************************************

    Rotate an image so that its current upper left corner moves
  to its lower left corner and the number of rows and columns
  are switched.

*/

int GPFImage::gpf_ImageReoriginCCW (const char *datain, int nc, int nr, int type, 
                          char **dataout)
{

    int             i, j, k, inew, size;
    char            *ctmp = NULL;
    short           *stmp1 = NULL, *stmp2 = NULL;
    int             *ltmp1 = NULL, *ltmp2 = NULL;
    CSW_F           *ftmp1 = NULL, *ftmp2 = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            *dataout = NULL;
            csw_Free (ctmp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    allocate the output image
*/

    switch (type) {
        
        case 0:
            size = sizeof(unsigned char);
            break;

        case 1:
            size = sizeof(char);
            break;

        case 2:
            size = sizeof(short);
            break;

        case 3:
            size = sizeof(int );
            break;

        case 4:
            size = sizeof(CSW_F);
            break;

        default:
            return -1;

    }

    ctmp = (char *)csw_Malloc (nc * nr * size);
    if (!ctmp) {
        return -1;
    }

/*
    rotate char or unsigned char image
*/
    if (type == 0  ||  type == 1) {
        for (i=0; i<nc; i++) {
            inew = (nc - i - 1) * nr;
            k = 0;
            for (j=0; j<nr; j++) {
                ctmp[inew+k] = datain[nc*j+i];
                k++;
            }
        }
        *dataout = ctmp;
        bsuccess = true;
        return 1;
    }

/*
    rotate 16 bit short image
*/

    if (type == 2) {
        stmp1 = (short *)datain;
        stmp2 = (short *)ctmp;
        for (i=0; i<nc; i++) {
            inew = (nc - i - 1) * nr;
            k = 0;
            for (j=0; j<nr; j++) {
                stmp2[inew+k] = stmp1[nc*j+i];
                k++;
            }
        }
        *dataout = (char *)stmp2;
        bsuccess = true;
        return 1;
    }
        
/*
    rotate 32 bit int image
*/

    if (type == 3) {
        ltmp1 = (int  *)datain;
        ltmp2 = (int  *)ctmp;
        for (i=0; i<nc; i++) {
            inew = (nc - i - 1) * nr;
            k = 0;
            for (j=0; j<nr; j++) {
                ltmp2[inew+k] = ltmp1[nc*j+i];
                k++;
            }
        }
        *dataout = (char *)ltmp2;
        bsuccess = true;
        return 1;
    }
        
/*
    rotate CSW_F image
*/

    if (type == 4) {
        ftmp1 = (CSW_F *)datain;
        ftmp2 = (CSW_F *)ctmp;
        for (i=0; i<nc; i++) {
            inew = (nc - i - 1) * nr;
            k = 0;
            for (j=0; j<nr; j++) {
                ftmp2[inew+k] = ftmp1[nc*j+i];
                k++;
            }
        }
        *dataout = (char *)ftmp2;
        bsuccess = true;
        return 1;
    }

    return -1;

}  /*  end of function gpf_ImageReoriginCCW  */




/* 
  ****************************************************************************

               g p f _ r o t a t e _ c o l o r _ i m a g e

  ****************************************************************************

    Rotate an image an arbitary angle.  A new image with the same sized pixels
  is returned in rdata.

*/

int GPFImage::gpf_rotate_color_image (int dsize, unsigned char *data, int nc, int nr, 
                            CSW_F x1, CSW_F y1, CSW_F xsp, CSW_F ysp, CSW_F angle,
                            unsigned char **rdata, int *nc2, int *nr2,
                            CSW_F *x1out, CSW_F *y1out, CSW_F *xspout, CSW_F *yspout)
{
    int                 i, j, k, offset, ii, jj, kk, kd, nnc, nnr;
    unsigned char       *crot = NULL, *trow = NULL;
    CSW_F               cosang, sinang, width, height, 
                        xt1, yt1, xt2, yt2, xxsp, yysp,
                        xmin, ymin, xmax, ymax, yt1sang, yt1cang;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (trow);
        if (bsuccess == false) {
            csw_Free (crot);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Initialize some outputs in case an error occurs.
*/
    *rdata = NULL;
    *nc2 = 0;
    *nr2 = 0;

/*
    Get the min and max x and y for the new rotated image.
*/
    angle *= DEG2RAD;
    cosang = (CSW_F)cos ((double)angle);    
    sinang = (CSW_F)sin ((double)angle);    

    width = xsp * (CSW_F)(nc - 1);
    height = ysp * (CSW_F)(nr - 1);

    xmin = x1;
    ymin = y1;
    xmax = x1;
    ymax = y1;

    xt1 = x1 + width * cosang;
    yt1 = y1 + width * sinang;
    if (xt1 < xmin) xmin = xt1;
    if (yt1 < ymin) ymin = yt1;
    if (xt1 > xmax) xmax = xt1;
    if (yt1 > ymax) ymax = yt1;

    xt2 = xt1 - height * sinang;
    yt2 = yt1 + height * cosang;
    if (xt2 < xmin) xmin = xt2;
    if (yt2 < ymin) ymin = yt2;
    if (xt2 > xmax) xmax = xt2;
    if (yt2 > ymax) ymax = yt2;

    xt1 = x1 - height * sinang;
    yt1 = y1 + height * cosang;
    if (xt1 < xmin) xmin = xt1;
    if (yt1 < ymin) ymin = yt1;
    if (xt1 > xmax) xmax = xt1;
    if (yt1 > ymax) ymax = yt1;

    xxsp = (xmax - xmin) / (CSW_F)(nc - 1);
    yysp = (ymax - ymin) / (CSW_F)(nr - 1);
    nnc = nc;
    nnr = nr;

/*
    Allocate memory for the rotated image and for a work array.
*/
    crot = (unsigned char *)csw_Calloc (dsize * nnc * nnr * sizeof(unsigned char));
    if (!crot) {
        return -1;
    }

    trow = (unsigned char *)csw_Calloc (dsize * (nnc + nnr) * sizeof(unsigned char));
    if (!trow) {
        return -1;
    }

/*
    Loop through the current image and rotate each point to
    its position in the new image.
*/
    for (i=0; i<nr; i++) {
        yt1 = i * ysp;
        yt1sang = yt1 * sinang;
        yt1cang = yt1 * cosang;
        offset = i * nc;
        for (j=0; j<nc; j++) {
            k = offset + j;
            xt1 = j * xsp;
            xt2 = xt1 * cosang - yt1sang;
            yt2 = xt1 * sinang + yt1cang;
            ii = (int)((yt2 + y1 - ymin) / yysp);
            if (ii < 0) ii = 0;
            if (ii > nr - 1) ii = nr - 1;
            jj = (int)((xt2 + x1 - xmin) / xxsp);
            if (jj < 0) jj = 0;
            if (jj > nc - 1) jj = nc - 1;
            kk = ii * nnc + jj;
            k *= dsize;
            kk *= dsize;
            crot[kk] = data[k];
            if (dsize == 3) {
                crot[kk+1] = data[k+1];
                crot[kk+2] = data[k+2];
            }
        }
    }
    
/*
    There may be small gaps in the image due to float roundoff 
    errors, so fill them in here.
*/
    for (i=0; i<nnr; i++) {
        offset = i * nnc;
        memcpy (trow, crot + offset * dsize, dsize * nnc * sizeof(unsigned char));
        for (j=1; j<nnc; j++) {
            k = offset + j;
            k *= dsize;
            kd = (j - 1) * dsize;
            if (crot[k] == 0) {
                crot[k] = trow[kd];
                if (dsize == 3) {
                    crot[k+1] = trow[kd+1];
                    crot[k+2] = trow[kd+2];
                }
            }
        }
        for (j=nnc-2; j>=0; j--) {
            k = offset + j;
            k *= dsize;
            kd = (j + 1) * dsize;
            if (crot[k] == 0) {
                crot[k] = trow[kd];
                if (dsize == 3) {
                    crot[k+1] = trow[kd+1];
                    crot[k+2] = trow[kd+2];
                }
            }
        }
        for (j=1; j<nnc; j++) {
            k = offset + j;
            k *= dsize;
            kd = (j - 1) * dsize;
            if (crot[k] == 0) {
                crot[k] = trow[kd];
                if (dsize == 3) {
                    crot[k+1] = trow[kd+1];
                    crot[k+2] = trow[kd+2];
                }
            }
        }
        for (j=nnc-2; j>=0; j--) {
            k = offset + j;
            k *= dsize;
            kd = (j + 1) * dsize;
            if (crot[k] == 0) {
                crot[k] = trow[kd];
                if (dsize == 3) {
                    crot[k+1] = trow[kd+1];
                    crot[k+2] = trow[kd+2];
                }
            }
        }
    }

    for (i=0; i<nc; i++) {
        for (j=0; j<nr; j++) {
            k = j * nnc + i;
            k *= dsize;
            kd = j * dsize;
            trow[kd] = crot[k];
            if (dsize == 3) {
                trow[kd+1] = crot[k+1];
                trow[kd+2] = crot[k+2];
            }
        }
        for (j=1; j<nr; j++) {
            k = j * nnc + i;
            k *= dsize;
            kd = (j - 1) * dsize;
            if (crot[k] == 0) {
                crot[k] = trow[kd];
                if (dsize == 3) {
                    crot[k+1] = trow[kd+1];
                    crot[k+2] = trow[kd+2];
                }
            }
        }
        for (j=nnr-2; j>=0; j--) {
            k = j * nnc + i;
            k *= dsize;
            kd = (j + 1) * dsize;
            if (crot[k] == 0) {
                crot[k] = trow[kd];
                if (dsize == 3) {
                    crot[k+1] = trow[kd+1];
                    crot[k+2] = trow[kd+2];
                }
            }
        }
        for (j=1; j<nr; j++) {
            k = j * nnc + i;
            k *= dsize;
            kd = (j - 1) * dsize;
            if (crot[k] == 0) {
                crot[k] = trow[kd];
                if (dsize == 3) {
                    crot[k+1] = trow[kd+1];
                    crot[k+2] = trow[kd+2];
                }
            }
        }
        for (j=nnr-2; j>=0; j--) {
            k = j * nnc + i;
            k *= dsize;
            kd = (j + 1) * dsize;
            if (crot[k] == 0) {
                crot[k] = trow[kd];
                if (dsize == 3) {
                    crot[k+1] = trow[kd+1];
                    crot[k+2] = trow[kd+2];
                }
            }
        }
    }

/*
    For a direct color image (dsize = 3), the 0,0,0 triples need to be
    replaced with 255,255,255 triples.
*/
    if (dsize == 3) {
        for (i=0; i<nnc*nnr; i++) {
            j = i * 3;
            if (crot[j] == 0  &&  crot[j+1] == 0  &&  crot[j+2] == 0) {
                crot[j] = 255;
                crot[j+1] = 255;
                crot[j+2] = 255;
            }
        }
    }

/*
    All done, return the rotated image stuff.
*/
    *rdata = crot;
    *nc2 = nnc;
    *nr2 = nnr;
    *x1out = xmin;
    *y1out = ymin;
    *xspout = xxsp;
    *yspout = yysp;

    bsuccess = true;

    return 1;

}  /*  end of function gpf_rotate_color_image  */




/*
  ****************************************************************

            g p f _ I n t e r p I m a g e 2 4 R o w

  ****************************************************************

    For each cell in a output image row, find the overlapping cell in the
  input image and assign that value to the output image cell.  This
  version is for 24 bit color image data.

    If the output image row extends beyond the input image boundaries, the
  specified limits of the output image row will be modified to the part that
  overlaps the input image.

*/

int GPFImage::gpf_InterpImage24Row (const char *datain, CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12, 
                          int nc1, int nr1,
                          char *dataout, CSW_F *x21, CSW_F *y21, CSW_F *x22, CSW_F *y22, 
                          int *nc2, int *nr2)
{
    int          jj, kk, it, jt, i1, istat;
    CSW_F        dx1, dy1, xt, yt, gx1, gy1, gx2;

/*
    check obvious errors
*/

    if (nc1 < 1  ||  nr1 < 1  ||  *nc2 < 1) {
        return -1;
    }

/*
    if the output image extends beyond the boundaries of
    the input image, clip it to the input image boundary
*/

    istat = ClipImageRow (x11, y11, x12, y12, 
                          x21, x22, nc2);
    if (istat == -1) {
        return -1;
    }

/*  suppress warnings  */

    y22 = y22;
    nr2 = nr2;

/*
    calculate image cell sizes and the offset of the output image
    origin relative to the input image origin
*/

    gx1 = (x12 - x11) / (CSW_F)(nc1 - 1);
    gy1 = (y12 - y11) / (CSW_F)(nr1 - 1);

    gx2 = (*x22 - *x21) / (CSW_F)(*nc2 - 1);
    dx1 = *x21 - x11;
    dy1 = *y21 - y11;

/*
    loop through the output image row, assigning values from 
    the input image.
*/

    yt = dy1;
    it = (int)(yt / gy1 + 0.5f);
    if (it > nr1 - 1) it = nr1 - 1;
    if (it < 0) it = 0;
    i1 = it * nc1 * 3;

    for (jj=0; jj<*nc2; jj++) {
        kk = jj * 3;
        xt = jj * gx2 + dx1;
        jt = (int)(xt / gx1 + 0.5f);
        jt *= 3;
        dataout[kk] = datain[i1+jt];
        dataout[kk+1] = datain[i1+jt+1];
        dataout[kk+2] = datain[i1+jt+2];
    }

    return 1;

}  /*  end of function gpf_InterpImage24Row  */




/*
  ****************************************************************

          g p f _ I m a g e R o t a t e C C W 2 4

  ****************************************************************

    Rotate an image 90 degrees CCW.  The first cell (lower left)
  rotates to the lower right of the new image.  This is used when
  a plot needs to be rotated to fit on a sheet of paper.  This function
  is only used for 24 bit color images.

*/

int GPFImage::gpf_ImageRotateCCW24 (const char *datain, int nc, int nr, int type,
                          char **dataout)
{
    int             i, j, k, i3, k3, inew, size;
    unsigned char   *ctmp = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            *dataout = NULL;
            csw_Free (ctmp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

    type = type;

/*
    allocate the output image
*/
    size = 3 * sizeof(unsigned char);

    ctmp = (unsigned char *)csw_Malloc (nc * nr * size);
    if (!ctmp) {
        return -1;
    }

/*
    rotate char or unsigned char image
*/
    for (i=0; i<nc; i++) {
        inew = (i + 1) * nr - 1;
        inew *= 3;
        i3 = i * 3;
        k = 0;
        for (j=0; j<nr; j++) {
            k3 = j * 3;
            ctmp[inew+k] = datain[nc*k3+i3];
            ctmp[inew+k+1] = datain[nc*k3+i3+1];
            ctmp[inew+k+2] = datain[nc*k3+i3+2];
            k-=3;
        }
    }
    *dataout = (char *)ctmp;
    bsuccess = true;

    return 1;

}  /*  end of function gpf_ImageRotateCCW24  */
