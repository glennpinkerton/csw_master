
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_fileio.cc

    Implement the methods for the CSWGrdFileio class.

    This file has functions to write and read grid data to portable
    binary files.  The files can be transferred to machines with different
    byte order (e.g. big endian and little endian) but the bit order
    on each system must be the same.  

    These should be considered private functions.  The interfaces here
    may change in the future.  An application should always call the
    grd_WriteFile and grd_ReadFile functions (found in grd_api.cc) rather
    than calling the functions in this file directly.
*/


#include <stdio.h>
#include <string.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"
#include "csw/utils/private_include/simulP.h"
#include "csw/utils/private_include/gpf_utils.h"

#include "csw/surfaceworks/private_include/grd_fileio.h"




/*
  ****************************************************************

                   g r d _ w r i t e _ f i l e

  ****************************************************************

    Write a grid and its mask to a file.  The file consists of a header
  with ascii representation of the grid geometry followed by a list of
  portable binary numbers with the actual grid values.  The grid values
  are scaled to fit as unsigned integers from zero to ten million.  If
  a node is outside and uncontrolled, 100 million is added to the value.
  If inside and uncontrolled, 200 million is added.  Thus, the final
  number is a combination of the value and the mask at the node.

    The grid cannot have hard coded null values (i.e. 1.e30) embedded
  in it for this scaling scheme to work.  An error is returned if any
  grid value is greater than 1.e20f or less than -1.e20f.

*/

int CSWGrdFileio::grd_write_file (const char *filename, const char *comment,
                    CSW_F *grid, char *mask, char *mask2,
                    int ncol, int nrow,
                    double xmin, double ymin, double xmax, double ymax,
                    int gridtype, FAultLineStruct *faults, int nfaults)
{
    int           istat, dlen, filenum;
    CSW_F         margin;

/*
    Check obvious errors.
*/
    if (ncol > WildInt  ||  ncol < -WildInt  ||
        nrow > WildInt  ||  nrow < -WildInt  ||
        xmin > WildFloat   ||  xmin < -WildFloat   ||
        ymin > WildFloat   ||  ymin < -WildFloat   ||
        xmax > WildFloat   ||  xmax < -WildFloat   ||
        ymax > WildFloat   ||  ymax < -WildFloat) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    if (!grid) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (!filename) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (filename[0] == '\0') {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return success (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

/*
    Open the output file if possible.  If a file already
    exists, it is overwritten.
*/
    filenum = CreateFile (filename, "b");
    if (filenum < 0) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    The first 10 bytes always have the version number.  Bytes 11-20 always have
    the number of bytes in the rest of the header and bytes 21-30 always have
    the number of bytes in the data area.  Bytes 31-40 always have the length
    of the comment.
*/
    dlen = ncol * nrow * 4;
    sprintf (Head1, "%d", FILE_VERSION);
    sprintf (Head2, "%d", HEADER_SIZE);
    sprintf (Head3, "%d", dlen);
    sprintf (Head4, "%d", COMMENT_SIZE);

/*
    Write the headers to the file.
*/
    istat = BinFileWrite ((void *)Head1, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileWrite ((void *)Head2, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileWrite ((void *)Head3, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileWrite ((void *)Head4, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Write the grid geometry and data to the file.
*/
    istat = WriteGrid (filenum, 0,
                       comment, grid, mask,
                       ncol, nrow,
                       xmin, ymin, xmax, ymax,
                       gridtype);

/*
    Write the number of faults to the file.
*/
    if (nfaults < 0) {
        nfaults = 0;
    }
    if (faults == NULL) {
        nfaults = 0;
    }
    sprintf (Head1, "%d", nfaults);
    istat = BinFileWrite ((void *)Head1, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    if (faults != NULL  &&  nfaults > 0) {
        Xmin = xmin;
        Ymin = ymin;
        Xmax = xmax;
        Ymax = ymax;
        margin = (CSW_F)((Xmax - Xmin + Ymax - Ymin) / MARGIN_DIVISOR);
        Xmin -= margin;
        Xmax += margin;
        Ymin -= margin;
        Ymax += margin;
        istat = WriteFaults (filenum, faults, nfaults);
        if (istat == -1) {
            CloseFile (filenum);
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

/*
 * If the mask2 array is not null, it needs to be 
 * written to the file also.
 */
    if (mask2 != NULL) {

        sprintf (Head1, "%d", ncol * nrow);
        istat = BinFileWrite ((void *)Head1, 10, 1, filenum);
        if (istat == -1) {
            CloseFile (filenum);
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

        istat = fileio_util_obj.csw_BinFileWrite ((void *)mask2,
                                  ncol * nrow * sizeof(char), 1,
                                  filenum);
    }
    else {
        sprintf (Head1, "0");
        istat = BinFileWrite ((void *)Head1, 10, 1, filenum);
        if (istat == -1) {
            CloseFile (filenum);
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

    CloseFile (filenum);

    return istat;

}  /*  end of function grd_write_file  */







/*
  ****************************************************************

                   g r d _ r e a d _ f i l e

  ****************************************************************

    Read a grid and its mask from a file.  The file consists of a header
  with ascii representation of the grid geometry followed by a list of
  portable binary numbers with the actual grid values.  The grid values
  are scaled to fit as unsigned integers from zero to ten million.  If
  a node is outside and uncontrolled, 100 million is added to the value.
  If inside and uncontrolled, 200 million is added.  Thus, the final
  number is a combination of the value and the mask at the node.

*/

int CSWGrdFileio::grd_read_file (const char *filename, char *comment,
                   CSW_F **grid, char **mask, char **mask2,
                   int *ncol, int *nrow,
                   double *xmin, double *ymin, double *xmax, double *ymax,
                   int *gridtype, FAultLineStruct **faults, int *nfaults)
{
    int            istat, dlen, filenum, version, hsize;
    int            csize;
    CSW_F          margin;

/*
    Check obvious errors.
*/
    if (!grid) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    Set the grid and (otionally) the mask to NULL
    in case of an error.
*/
    *grid = NULL;
    if (mask) *mask = NULL;

    if (!filename) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (filename[0] == '\0') {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (ncol == NULL  ||  nrow == NULL  ||
        xmin == NULL  ||  ymin == NULL  ||
        xmax == NULL  ||  ymax == NULL) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

/*
    Open the input file if possible.
*/
    filenum = OpenFile (filename, "r");
    if (filenum < 0) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    Do four reads for the 10 character blocks always present.
*/
    istat = BinFileRead ((void *)Head1, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileRead ((void *)Head2, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileRead ((void *)Head3, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileRead ((void *)Head4, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    sscanf (Head1, "%d", &version);
    sscanf (Head2, "%d", &hsize);
    sscanf (Head3, "%d", &dlen);
    sscanf (Head4, "%d", &csize);

/*
    An error is produced if this is a multiple grid file.
*/
    if (version > MULTI_GRID_BASE) {
        grd_utils_ptr->grd_set_err (8);
        CloseFile (filenum);
        return -1;
    }

    istat = ReadGrid (filenum, 0, 
                      comment, grid, mask,
                      ncol, nrow,
                      xmin, ymin, xmax, ymax, gridtype,
                      version, csize, dlen, hsize);


    if (version >= FAULT_VERSION) {
        Xmin = *xmin;
        Ymin = *ymin;
        Xmax = *xmax;
        Ymax = *ymax;
        margin = (CSW_F)((Xmax - Xmin + Ymax - Ymin) / MARGIN_DIVISOR);
        Xmin -= margin;
        Xmax += margin;
        Ymin -= margin;
        Ymax += margin;
        ReadFaults (version, filenum, faults, nfaults);
    }

    if (version >= EXPANDED_MASK_VERSION) {
        istat = ReadExpandedMask (mask2, filenum);
        if (istat == -1) {
            CloseFile (filenum);
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

    CloseFile (filenum);

    return istat;

}  /*  end of function grd_read_file  */





/*
  ****************************************************************

            g r d _ w r i t e _ b l e n d e d _ f i l e

  ****************************************************************

    Write a blended grid and its mask to a file.  The file has a header
  with ascii representation of the grid geometry followed by a list of
  portable binary numbers with the actual grid values.  If the mask
  exists, it is appended after the data.

*/

int CSWGrdFileio::grd_write_blended_file (const char *filename, const char *comment,
                            CSW_Blended *grid, char *mask,
                            int ncol, int nrow,
                            double xmin, double ymin, double xmax, double ymax,
                            FAultLineStruct *faults, int nfaults)
{
    int               istat, dlen, filenum;
    int               gridtype, uc8;
    double            zmin, zmax, scalezmin, scalezmax,
                      zscale;
    char              local_comment[COMMENT_SIZE];
    CSW_F             margin;

/*
    Check obvious errors.
*/
    if (ncol > WildInt  ||  ncol < -WildInt  ||
        nrow > WildInt  ||  nrow < -WildInt  ||
        xmin > WildFloat   ||  xmin < -WildFloat   ||
        ymin > WildFloat   ||  ymin < -WildFloat   ||
        xmax > WildFloat   ||  xmax < -WildFloat   ||
        ymax > WildFloat   ||  ymax < -WildFloat) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    if (!grid) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (!filename) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (filename[0] == '\0') {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

    uc8 = sizeof(CSW_Blended);

/*
    The first 10 bytes always have the version number.  Bytes 11-20 always have
    the number of bytes in the rest of the header and bytes 21-30 always have
    the number of bytes in the data area.  Bytes 31-40 always have the length
    of the comment.
*/
    dlen = ncol * nrow * uc8;
    if (mask) dlen += ncol * nrow * sizeof(char);
    sprintf (Head1, "%d", FILE_VERSION);
    sprintf (Head2, "%d", HEADER_SIZE);
    sprintf (Head3, "%d", dlen);
    sprintf (Head4, "%d", COMMENT_SIZE);

/*
    Fill in geometry and value scaling information in the 
    header.  The header may change format or get bigger in
    future versions, but it will not get smaller.
*/
    gridtype = GRD_BLENDED_GRID_FILE;
    zmin = 0.0;
    zmax = 0.0;
    scalezmin = 0.0;
    scalezmax = 0.0;
    zscale = 1.0;
    sprintf (Header, WRITE_FORMAT_1003,
             gridtype, ncol, nrow,
             xmin, ymin, xmax, ymax,
             zmin, zmax, scalezmin, scalezmax, zscale);

/*
    Open the output file if possible.  If a file already
    exists, it is overwritten.
*/
    filenum = CreateFile (filename, "b");
    if (filenum < 0) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    Write the headers to the file.
*/
    istat = BinFileWrite ((void *)Head1, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileWrite ((void *)Head2, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileWrite ((void *)Head3, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileWrite ((void *)Head4, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileWrite ((void *)Header, HEADER_SIZE, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    if (comment) {
        csw_StrTruncate (local_comment, comment, COMMENT_SIZE);
    }
    else {
        local_comment[0] = '\0';
    }
    istat = BinFileWrite ((void *)local_comment, COMMENT_SIZE, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Write the blended grid data to disk.
*/
    istat = BinFileWrite ((void *)grid, 1, ncol*nrow*uc8, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    If the mask exists, write it to the file.
*/
    if (mask) {
        istat = BinFileWrite ((void *)mask, 1, ncol*nrow*sizeof(char), filenum);
        if (istat == -1) {
            CloseFile (filenum);
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

/*
 * Write the fault lines if there are any.
 */
    if (faults != NULL  &&  nfaults > 0) {
        Xmin = xmin;
        Ymin = ymin;
        Xmax = xmax;
        Ymax = ymax;
        margin = (CSW_F)((Xmax - Xmin + Ymax - Ymin) / MARGIN_DIVISOR);
        Xmin -= margin;
        Xmax += margin;
        Ymin -= margin;
        Ymax += margin;
        istat = WriteFaults (filenum, faults, nfaults);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

    CloseFile (filenum);

    return 1;

}  /*  end of function grd_write_blended_file  */




/*
  ****************************************************************

            g r d _ r e a d _ b l e n d e d _ f i l e

  ****************************************************************

    Read a blended grid and its mask from a file.  The file has a header
  with ascii representation of the grid geometry followed by a list of
  portable binary numbers with the actual grid values.  The mask follows
  the data.  

    If there is no mask in the file, but a mask is desired by the call,
  a mask is allocated and filled with zeros.

*/

int CSWGrdFileio::grd_read_blended_file (const char *filename, char *comment,
                           CSW_Blended **grid, char **mask,
                           int *ncol, int *nrow,
                           double *xmin, double *ymin, double *xmax, double *ymax,
                           FAultLineStruct **faults, int *nfaults)
{
    int                istat, dlen, filenum = -1, version, hsize;
    int                nc, nr, csize, gridtype, uc8, s1, s2;
    double             zmin, zmax, scalezmin, scalezmax, zscale;
    CSW_Blended        *grloc = NULL;
    char               *maskloc = NULL;
    CSW_F              margin;

    bool     bfclose = true;
    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bfclose  &&  filenum >= 0) {
            CloseFile (filenum);
            filenum = -1;
        }
        if (bsuccess == false) {
            csw_Free (grloc);
            csw_Free (maskloc);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Check obvious errors.
*/
    if (!grid) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    Set grid and (optionally) mask to NULL
    in case of an error.
*/
    *grid = NULL;
    if (mask) *mask = NULL;

    if (faults) *faults = NULL;
    if (nfaults) *nfaults = 0;

    if (!filename) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (filename[0] == '\0') {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (ncol == NULL  ||  nrow == NULL  ||
        xmin == NULL  ||  ymin == NULL  ||
        xmax == NULL  ||  ymax == NULL) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

/*
    Open the input file if possible.
*/
    filenum = OpenFile (filename, "r");
    if (filenum < 0) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    uc8 = sizeof(CSW_Blended);

/*
    Do four reads for the 10 character blocks always present.
*/
    istat = BinFileRead ((void *)Head1, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileRead ((void *)Head2, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileRead ((void *)Head3, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = BinFileRead ((void *)Head4, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    sscanf (Head1, "%d", &version);
    sscanf (Head2, "%d", &hsize);
    sscanf (Head3, "%d", &dlen);
    sscanf (Head4, "%d", &csize);

/*
    An error is produced if this is a multiple grid file.
*/
    if (version > MULTI_GRID_BASE) {
        grd_utils_ptr->grd_set_err (8);
        return -1;
    }

/*
    Read the header (hsize bytes)
*/
    istat = BinFileRead ((void *)Header, hsize, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Read the comment (csize bytes) and copy it to
    the specified comment string if applicable.
*/
    istat = BinFileRead ((void *)Comment, csize, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    if (comment) {
        strcpy (comment, Comment);
    }

/*
    Decode the header based on the version number.
*/
    gridtype = -1;
    if (version == 1001) {
        sscanf (Header, READ_FORMAT_1001,
                ncol, nrow,
                xmin, ymin, xmax, ymax,
                &zmin, &zmax, &scalezmin, &scalezmax, &zscale);
        gridtype = GRD_NORMAL_GRID_FILE;
    }
    else if (version == 1002) {
        sscanf (Header, READ_FORMAT_1002,
                &gridtype, ncol, nrow,
                xmin, ymin, xmax, ymax,
                &zmin, &zmax, &scalezmin, &scalezmax, &zscale);
    }
    else if (version == 1003) {
        sscanf (Header, READ_FORMAT_1003,
                &gridtype, ncol, nrow,
                xmin, ymin, xmax, ymax,
                &zmin, &zmax, &scalezmin, &scalezmax, &zscale);
    }
    else {
        grd_utils_ptr->grd_set_err (6);
        return -1;
    }

    if (gridtype != GRD_BLENDED_GRID_FILE) {
        grd_utils_ptr->grd_set_err (7);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

    nc = *ncol;
    nr = *nrow;

/*
    Allocate space for the grid and mask.  The mask is also
    initialized to zero values in case there is no mask in the file.
*/
MSL
    grloc = (CSW_Blended *)csw_Malloc (nc*nr*uc8);
    if (!grloc) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

MSL
    maskloc = (char *)csw_Calloc (nc*nr*sizeof(char));
    if (!maskloc) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

/*
    Read the data into memory.
*/
    istat = BinFileRead ((void *)grloc, nc*nr*uc8, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    If a mask is desired, attempt to read it.
*/
    s1 = nc*nr*uc8;
    s2 = (int)nc*nr*sizeof(char);
    if (dlen == s1 + s2) {
        istat = BinFileRead ((void *)maskloc, nc*nr*sizeof(char), 1, filenum);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }
    if (mask == NULL) {
        csw_Free (maskloc);
        maskloc = NULL;
    }

/*
 * Attempt to read faults if the version is recent enough.
 */
    if (version >= BLENDED_FAULT_VERSION) {
        if (faults != NULL  &&  nfaults != NULL) {
            Xmin = *xmin;
            Ymin = *ymin;
            Xmax = *xmax;
            Ymax = *ymax;
            margin = (CSW_F)((Xmax - Xmin + Ymax - Ymin) / MARGIN_DIVISOR);
            Xmin -= margin;
            Xmax += margin;
            Ymin -= margin;
            Ymax += margin;
            ReadFaults (version, filenum, faults, nfaults);
        }
    }

    CloseFile (filenum);
    bfclose = false;
    *grid = grloc;
    if (mask) *mask = maskloc;

    bsuccess = true;

    return 1;

}  /*  end of function grd_read_blended_file  */






/*
  ****************************************************************

          g r d _ w r i t e _ m u l t i p l e _ f i l e

  ****************************************************************

    Write one or more grids into a multiple grid file.  Each grid 
  is identified with a unique tag string up to 80 characters long.
  The tags are written at the top of the file, along with the location
  in the file of the start of the grid represented by the tag.

    The file written here must be read using grd_ReadMultipleFile.

*/

int CSWGrdFileio::grd_write_multiple_file (char *name, GRidFileRec *list, int nlistin)
{
    int            nlist, scale_by_grid_flag;
    int            i, j, k, len, istat, filenum, nerr;
    int            ipos, tagpos, tagsize;
    GRidFileRec    *fptr, *fp2;
    char           localname[500], hpos[11], c10[10];
    FAultLineStruct  *faults;
    int            nfaults;
    CSW_F          margin, *gdata;
    char           *mask2;
    double         zmin, zmax;

/*
 * bug 8710
 *
 * Use all grids specified to calculate the scale for the file output
 * unless the nlistin parameter is less than zero.
 */
    nlist = nlistin;
    scale_by_grid_flag = 0;
    if (nlist < 0) {
        nlist = -nlist;
        scale_by_grid_flag = 1;
    }

/*
    check for "global" errors in the parameters
*/
    if (list == NULL) {
        grd_utils_ptr->grd_set_err (10);
        return -1;
    }

    if (nlist < 1  ||  nlist > GRD_MAX_MULTI_FILES) {
        grd_utils_ptr->grd_set_err (8);
        return -1;
    }

    len = strlen (name);
    if (len > 499) {
        grd_utils_ptr->grd_set_err (9);
        return -1;
    }

    strcpy (localname, name);
    csw_StrLeftJust (localname);
    if (localname[0] == '\0') {
        grd_utils_ptr->grd_set_err (9);
        return -1;
    }

/*
    check each structure in the list array for errors
*/
    nerr = 0;
    fptr = list;
    for (i=0; i<nlist; i++) {

        fptr->error = 0;

    /*
        Check wild values.
    */
        if (fptr->ncol > WildInt    ||  fptr->ncol < -WildInt    ||
            fptr->nrow > WildInt    ||  fptr->nrow < -WildInt    ||
            fptr->xmin > WildFloat  ||  fptr->xmin < -WildFloat  ||
            fptr->ymin > WildFloat  ||  fptr->ymin < -WildFloat  ||
            fptr->xmax > WildFloat  ||  fptr->xmax < -WildFloat  ||
            fptr->ymax > WildFloat  ||  fptr->ymax < -WildFloat  ||
            fptr->gridtype > WildInt  ||  fptr->gridtype < -WildInt) {
            fptr->error = 4;
            grd_utils_ptr->grd_set_err (4);
            nerr++;
            fptr++;
            continue;
        }

    /*
        Check for a null grid pointer
    */
        if (fptr->grid == NULL) {
            fptr->error = 3;
            grd_utils_ptr->grd_set_err (3);
            nerr++;
            fptr++;
            continue;
        }

    /*
        Check for less than 2 columns or rows.
    */
        if (fptr->ncol < 2  ||  fptr->nrow < 2) {
            fptr->error = 7;
            grd_utils_ptr->grd_set_err (7);
            nerr++;
            fptr++;
            continue;
        }

    /*
        Check if the tag has been used before.
    */
        fp2 = list;
        for (j=0; j<i; j++) {
            if (!strcmp (fp2->tag, fptr->tag)) {
                grd_utils_ptr->grd_set_err (6);
                fptr->error = 6;
                break;
            }
            fp2++;
        }
        if (fptr->error == 6) {
            fptr++;
            nerr++;
            continue;
        }

    /*
        Check for hard nulls in the grid.
    */
        for (k=0; k<fptr->ncol*fptr->nrow; k++) {
            if (fptr->grid[k] > WildFloat  ||
                fptr->grid[k] < -WildFloat) {
                fptr->error = 5;
                grd_utils_ptr->grd_set_err (5);
                break;
            }
        }
        if (fptr->error == 5) {
            fptr++;
            nerr++;
            continue;
        }

    /*
        Check for consistent corner coordinates.
    */
        if (fptr->xmin >= fptr->xmax  ||  fptr->ymin >= fptr->ymax) {
            grd_utils_ptr->grd_set_err (8);
            fptr->error = 8;
            nerr++;
            fptr++;
            continue;
        }

        fptr++;

    }

/*
    return -1 if an error occurred.
*/
    if (nerr > 0) {
        return -1;
    }

/*
    Create a new file.
*/
    filenum = CreateFile (localname, "b");
    if (filenum < 0) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    Write the "global headers" to the file
*/
    sprintf (Head1, "%d", FILE_VERSION + MULTI_GRID_BASE);
    sprintf (Head2, "%d", HEADER_SIZE);
    sprintf (Head3, "%d", nlist);
    sprintf (Head4, "%d", COMMENT_SIZE);

    istat = BinFileWrite ((void *)Head1, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileWrite ((void *)Head2, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileWrite ((void *)Head3, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileWrite ((void *)Head4, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        CloseFile (filenum);
        return -1;
    }

/*
    Write the tags and blanks for the file pointers.
*/
    tagpos = -1;
    tagsize = GRD_TAG_SIZE + 10;
    fptr = list;
    strcpy (hpos, "          ");
    for (i=0; i<nlist; i++) {
        
        istat = BinFileWrite ((void *)(fptr->tag), GRD_TAG_SIZE, 1, filenum);        
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            CloseFile (filenum);
            return -1;
        }
        if (i == 0) {
            tagpos = fileio_util_obj.csw_GetFilePosition (filenum);
            if (tagpos == -1) {
                grd_utils_ptr->grd_set_err (1);
                CloseFile (filenum);
                return -1;
            }
        }
                
        istat = BinFileWrite ((void *)hpos, 10, 1, filenum);        
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            CloseFile (filenum);
            return -1;
        }

        fptr++;

    }

/*
 * bug 8710
 *
 * If all grids are to be output using the same z scale, calculate
 * that common z scale now.
 */
    MultiZmin = 1.e30;
    MultiZmax = -1.e30;
    if (scale_by_grid_flag == 0) {

        zmin = 1.e30;
        zmax = -1.e30;
        for (i=0; i<nlist; i++) {

            fptr = list + i;
            gdata = fptr->grid;
            for (j=0; j<fptr->ncol * fptr->nrow; j++) {
                if (gdata[j] < zmin) zmin = gdata[j];
                if (gdata[j] > zmax) zmax = gdata[j];
            }
        }

        MultiZmin = zmin;
        MultiZmax = zmax;
    }

/*
    Write each grid to the file.
*/
    fptr = list;
    for (i=0; i<nlist; i++) {

        ipos = fileio_util_obj.csw_GetFilePosition (filenum);
        FilePos[i] = ipos;
        istat = WriteGrid (filenum, 0,
                           fptr->comment, 
                           fptr->grid,
                           fptr->mask,
                           fptr->ncol,
                           fptr->nrow,
                           fptr->xmin,
                           fptr->ymin,
                           fptr->xmax,
                           fptr->ymax,
                           fptr->gridtype);
        if (istat == -1) {
            CloseFile (filenum);
            MultiZmin = 1.e30;
            MultiZmax = -1.e30;
            return -1;
        }

        faults = fptr->faults;
        nfaults = fptr->nfaults;
        if (faults == NULL  ||  nfaults < 0) nfaults = 0;
        sprintf (c10, "%d", nfaults);
        istat = fileio_util_obj.csw_BinFileWrite ((void *)c10, 10, 1, filenum);
        if (istat == -1) {
            CloseFile (filenum);
            MultiZmin = 1.e30;
            MultiZmax = -1.e30;
            return -1;
        }

        if (faults != NULL  &&  nfaults > 0) {
            Xmin = fptr->xmin;
            Ymin = fptr->ymin;
            Xmax = fptr->xmax;
            Ymax = fptr->ymax;
            margin = (CSW_F)((Xmax - Xmin + Ymax - Ymin) / MARGIN_DIVISOR);
            Xmin -= margin;
            Xmax += margin;
            Ymin -= margin;
            Ymax += margin;
            istat = WriteFaults (filenum, faults, nfaults);
            if (istat == -1) {
                CloseFile (filenum);
                MultiZmin = 1.e30;
                MultiZmax = -1.e30;
                return -1;
            }
        }    

        mask2 = fptr->mask2;
        if (mask2) {
            sprintf (c10, "%d", fptr->ncol * fptr->nrow);
            istat = fileio_util_obj.csw_BinFileWrite (c10, 10, 1, filenum);
            istat = fileio_util_obj.csw_BinFileWrite (mask2, 
                                      fptr->ncol * fptr->nrow * sizeof(char),
                                      1, filenum);
        }
        else {
            sprintf (c10, "0");
            istat = fileio_util_obj.csw_BinFileWrite (c10, 10, 1, filenum);
        }
        if (istat == -1) {
            CloseFile (filenum);
            MultiZmin = 1.e30;
            MultiZmax = -1.e30;
            return -1;
        }

        fptr++;

    }

/*
    rewrite the file position data
*/
    for (i=0; i<nlist; i++) {

        istat = fileio_util_obj.csw_SetFilePosition (filenum, tagpos, SEEK_SET);
        if (istat == -1) {
            CloseFile (filenum);
            MultiZmin = 1.e30;
            MultiZmax = -1.e30;
            return -1;
        }
        sprintf (hpos, "%d", FilePos[i]);
        istat = BinFileWrite ((void *)hpos, 10, 1, filenum);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            CloseFile (filenum);
            MultiZmin = 1.e30;
            MultiZmax = -1.e30;
            return -1;
        }
        tagpos += tagsize;

    }

    CloseFile (filenum);
    MultiZmin = 1.e30;
    MultiZmax = -1.e30;

    return 1;

}  /*  end of function grd_write_multiple_file  */






/*
  ****************************************************************

          g r d _ r e a d _ m u l t i p l e _ f i l e

  ****************************************************************

    Read one or more grids from a multiple grid file.  Each grid 
  is identified with a unique tag string up to 80 characters long.
  The tags are written at the top of the file, along with the location
  in the file of the start of the grid represented by the tag.

    The file written here must have been written with grd_WriteMultipleFile.

*/

int CSWGrdFileio::grd_read_multiple_file (char *name, GRidFileRec *list, int nlist)
{
    int            i, j, len, istat, filenum, nfound;
    int            version, csize, hsize, listlen;
    GRidFileRec    *fptr;
    char           localname[500], ctag[GRD_TAG_SIZE], cpos[11];
    CSW_F          margin;

/*
    check for "global" errors in the parameters
*/
    if (list == NULL) {
        grd_utils_ptr->grd_set_err (10);
        return -1;
    }

    if (nlist < 1  ||  nlist > GRD_MAX_MULTI_FILES) {
        grd_utils_ptr->grd_set_err (8);
        return -1;
    }

    len = strlen (name);
    if (len > 499) {
        grd_utils_ptr->grd_set_err (9);
        return -1;
    }

    strcpy (localname, name);
    csw_StrLeftJust (localname);
    if (localname[0] == '\0') {
        grd_utils_ptr->grd_set_err (9);
        return -1;
    }

/*
    Open the file.
*/
    filenum = OpenFile (localname, "r");
    if (filenum < 0) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    Read the "global headers" from the file
*/
    istat = BinFileRead ((void *)Head1, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileRead ((void *)Head2, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileRead ((void *)Head3, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileRead ((void *)Head4, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        CloseFile (filenum);
        return -1;
    }

    sscanf (Head1, "%d", &version);
    sscanf (Head2, "%d", &hsize);
    sscanf (Head3, "%d", &listlen);
    sscanf (Head4, "%d", &csize);

    if (version < MULTI_GRID_BASE) {
        grd_utils_ptr->grd_set_err (10);
        CloseFile (filenum);
        return -1;
    }

    version -= MULTI_GRID_BASE;

/*
    In case a grid tag is not found, initialize the
    error numbers in the list to 11 ("tag not found")
    and initialize the FilePos array to -1.  Also 
    initialize the grid and mask pointers to NULL.
*/
    fptr = list;
    for (i=0; i<nlist; i++) {
        fptr->error = 11;
        fptr->grid = NULL;
        fptr->mask = NULL;
        fptr->mask2 = NULL;
        fptr++;
        FilePos[i] = -1;
    }

/*
    Read each tag in the file, and if it has a match
    in the tags specified in the list parameter, put
    the file pointer into the private FilePos array.
*/
    nfound = 0;
    for (i=0; i<listlen; i++) {
        
        istat = BinFileRead ((void *)ctag, GRD_TAG_SIZE, 1, filenum);        
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            CloseFile (filenum);
            return -1;
        }
        istat = BinFileRead ((void *)cpos, 10, 1, filenum);        
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            CloseFile (filenum);
            return -1;
        }

        fptr = list;
        for (j=0; j<nlist; j++) {
            if (!strcmp(ctag, fptr->tag)) {
                sscanf (cpos, "%d", FilePos+j);
                nfound++;
                break;
            }
            fptr++;
        }

        if (nfound >= nlist) {
            break;
        }

    }

/*
    Read each grid from the file.
*/
    fptr = list;
    for (i=0; i<nlist; i++) {

        if (FilePos[i] == -1) continue;

        istat = ReadGrid (filenum, FilePos[i],
                          fptr->comment, 
                          &(fptr->grid),
                          &(fptr->mask),
                          &(fptr->ncol),
                          &(fptr->nrow),
                          &(fptr->xmin),
                          &(fptr->ymin),
                          &(fptr->xmax),
                          &(fptr->ymax),
                          &(fptr->gridtype),
                          version, csize, 0, hsize);
        if (istat == -1) {
            CloseFile (filenum);
            FreeFileRecList (list, nlist);
            return -1;
        }

        if (version >= FAULT_VERSION) {
            Xmin = fptr->xmin;
            Ymin = fptr->ymin;
            Xmax = fptr->xmax;
            Ymax = fptr->ymax;
            margin = (CSW_F)((Xmax - Xmin + Ymax - Ymin) / MARGIN_DIVISOR);
            Xmin -= margin;
            Xmax += margin;
            Ymin -= margin;
            Ymax += margin;
            ReadFaults (version, filenum,
                        &fptr->faults,
                        &fptr->nfaults);
        }

        if (version >= EXPANDED_MASK_VERSION) {
            istat = ReadExpandedMask (&fptr->mask2, filenum);
            if (istat == -1) {
                CloseFile (filenum);
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
        }

        fptr->error = 0;

        fptr++;

    }

    CloseFile (filenum);

    return 1;

}  /*  end of function grd_read_multiple_file  */





/*
  ****************************************************************

                  F r e e F i l e R e c L i s t

  ****************************************************************

    Free any non null grid and mask members of the specified 
  GRidFileRec structure list.

*/

int CSWGrdFileio::FreeFileRecList (GRidFileRec *list, int nlist)
{
    int                  i;
    GRidFileRec          *fptr;

    fptr = list;
    for (i=0; i<nlist; i++) {
        if (fptr->grid) {
            csw_Free (fptr->grid);
            fptr->grid = NULL;
        }
        if (fptr->mask) {
            csw_Free (fptr->mask);
            fptr->mask = NULL;
        }
        if (fptr->mask2) {
            csw_Free (fptr->mask2);
            fptr->mask2 = NULL;
        }
        grd_fault_ptr->grd_free_fault_line_structs
            (fptr->faults, fptr->nfaults);
        fptr++;
    }

    return 1;

}  /*  end of private FreeFileRecList function  */


/*
 ***********************************************************************************
 
                 g r d _ c l e a n _ f i l e _ r e c _ l i s t

 ***********************************************************************************

*/

int CSWGrdFileio::grd_clean_file_rec_list (GRidFileRec *list, int nlist) 
{
    FreeFileRecList (list, nlist);
    return 1;
}




/*
  ****************************************************************

                       W r i t e G r i d

  ****************************************************************

    Write a grid to an open file, at the specified position, or 
  appended to the current position if fileptr is zero.

*/

int CSWGrdFileio::WriteGrid (int filenum, int fileptr, const char *comment,
                      CSW_F *grid, char *mask,
                      int ncol, int nrow,
                      double xmin, double ymin, double xmax, double ymax,
                      int gridtype)
{
    int           istat, tval, i, n;
    double        zmin, zmax, scalezmin, scalezmax,
                  zscale, zt;
    unsigned char *c4;
    char          local_comment[COMMENT_SIZE];
    char          tmask;
    unsigned char    crow[10000];

/*
    Find the non null grid limits.
*/
    zmin = 1.e20f;
    zmax = -1.e20f;
    for (i=0; i<ncol*nrow; i++) {
        zt = grid[i];
        if (zt > HARD_NULL  ||  zt < -HARD_NULL) {
            continue;
        }
        if (zt < zmin) zmin = zt;
        if (zt > zmax) zmax = zt;
    }

/*
 * bug 8710
 *
 * If all grids for a multi grid file need to be at the
 * same scale, then the MultZmin and MultiZmax variables
 * should have been set up prior to calling this function.
 * If they have been set up, use them.
 */
    if (MultiZmax > MultiZmin) {
        zmin = MultiZmin;
        zmax = MultiZmax;
    }

    Zmin = zmin;
    Zmax = zmax;

/*
    Expand the grid limits by one percent and use the 
    expanded values to scale from CSW_F to int values.
    This prevents any CSW_F to int roundoff from producing
    int values less than zero or greater than 10 million
    when the actual grid values are scaled.
*/
    zt = (zmax - zmin) / 100.0f;
    if (zt <= 0.0f) zt = 1.0f;
    scalezmin = zmin - zt;
    scalezmax = zmax + zt;
    zscale = (scalezmax - scalezmin) / 10000000.0f;
    
/*
    Fill in geometry and value scaling information in the 
    header.  The header may change format or get bigger in
    future versions, but it will not get smaller.
*/
    sprintf (Header, WRITE_FORMAT_1003,
             gridtype, ncol, nrow,
             xmin, ymin, xmax, ymax,
             zmin, zmax, scalezmin, scalezmax, zscale);

/*
    Position the file and start writing.
*/
    if (fileptr > 0)
        fileio_util_obj.csw_SetFilePosition (filenum, fileptr, 0);

    istat = BinFileWrite ((void *)Header, HEADER_SIZE, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    if (comment) {
        csw_StrTruncate (local_comment, comment, COMMENT_SIZE);
    }
    else {
        local_comment[0] = '\0';
    }
    istat = BinFileWrite ((void *)local_comment, COMMENT_SIZE, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    For each value, scale it to an unsigned integer, convert the
    integer to a portable 4 byte representation, and write the
    4 bytes to the file.  The output is written in 10000 byte
    chunks.
*/
    n = 0;
    c4 = crow;
    for (i=0; i<ncol*nrow; i++) {
        if (grid[i] > HARD_NULL  ||  grid[i] < -HARD_NULL) {
            tval = NULL_INT;
        }
        else {
            tval = (int) ((grid[i] - scalezmin) / zscale + 0.5f);
        }
        if (mask) {
            tmask = mask[i];
            if (tmask > 10) tmask = 0;
            tval += MASK_BASE * tmask;
        }
        PintToChar4 (tval, c4);
        n += 4;
        c4 += 4;
        if (n == 10000) {
            istat = BinFileWrite ((void *)crow, 10000, 1, filenum);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            n = 0;
            c4 = crow;
        }
    }

    if (n > 0) {
        istat = BinFileWrite ((void *)crow, n, 1, filenum);
    }

    return 1;

}  /*  end of private WriteGrid function  */






/*
  ****************************************************************

                        R e a d G r i d

  ****************************************************************

    Read a grid from an open file.  Read from the specified
  fileptr position or from the current position if fileptr is
  zero.

*/

int CSWGrdFileio::ReadGrid (int filenum, int fileptr, char *comment,
                     CSW_F **grid, char **mask,
                     int *ncol, int *nrow,
                     double *xmin, double *ymin,
                     double *xmax, double *ymax,
                     int *gridtype,
                     int version, int csize, int dlen, int hsize)
{
    int            istat, tval, i;
    int            nc, nr;
    double         zmin, zmax, scalezmin, scalezmax,
                   zscale;
    CSW_F          *grloc = NULL;
    unsigned char  *c4 = NULL, *cwork = NULL;
    char           *maskloc = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (cwork);
        if (bsuccess == false) {
            csw_Free (grloc);
            csw_Free (maskloc);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (fileptr > 0) {
        fileio_util_obj.csw_SetFilePosition (filenum, fileptr, SEEK_SET);
    }

/*
    Read the header (hsize bytes)
*/
    istat = BinFileRead ((void *)Header, hsize, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Read the comment (csize bytes) and copy it to
    the specified comment string if applicable.
*/
    istat = BinFileRead ((void *)Comment, csize, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    if (comment) {
        strcpy (comment, Comment);
    }

/*
    Decode the header based on the version number.
*/
    if (version == 1001) {
        sscanf (Header, READ_FORMAT_1001,
                ncol, nrow,
                xmin, ymin, xmax, ymax,
                &zmin, &zmax, &scalezmin, &scalezmax, &zscale);
        *gridtype = GRD_NORMAL_GRID_FILE;
    }
    else if (version == 1002) {
        sscanf (Header, READ_FORMAT_1002,
                gridtype, ncol, nrow,
                xmin, ymin, xmax, ymax,
                &zmin, &zmax, &scalezmin, &scalezmax, &zscale);
    }
    else if (version == 1003  ||  version >= 2001) {
        sscanf (Header, READ_FORMAT_1003,
                gridtype, ncol, nrow,
                xmin, ymin, xmax, ymax,
                &zmin, &zmax, &scalezmin, &scalezmax, &zscale);
    }
    else {
        grd_utils_ptr->grd_set_err (6);
        return -1;
    }

/*
    Error if this is a blended grid file.
*/
    if (*gridtype == GRD_BLENDED_GRID_FILE) {
        grd_utils_ptr->grd_set_err (7);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

    nc = *ncol;
    nr = *nrow;

    if (dlen == 0) {
        dlen = nc * nr * 4;
    }

/*
    Allocate space for the grid and mask.
*/
MSL
    grloc = (CSW_F *)csw_Malloc (nc*nr*sizeof(CSW_F));
    if (!grloc) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

    if (mask) {
MSL
        maskloc = (char *)csw_Malloc (nc*nr*sizeof(char));
        if (!maskloc) {
            grd_utils_ptr->grd_set_err (5);
            return -1;
        }
    }
    else {
        maskloc = NULL;
    }

MSL
    cwork = (unsigned char *)csw_Malloc (dlen * sizeof(char));
    if (!cwork) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

    TinyFloat = (CSW_F)(zmax - zmin) / 100000.0f;

/*
    Read all data into memory.
*/
    istat = BinFileRead ((void *)cwork, dlen, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    c4 = cwork;
    for (i=0; i<nc*nr; i++) {
        Char4ToPint (c4, &tval);
        if (maskloc) {
            maskloc[i] = (char) (tval / MASK_BASE);
        }
        tval %= MASK_BASE;
        if (tval >= NULL_INT_READ) {
            grloc[i] = 1.e30f;
        }
        else {
            grloc[i] = (CSW_F) (tval * zscale + scalezmin);
            if (grloc[i] < zmin)
                grloc[i] = (CSW_F)zmin;
            if (grloc[i] > zmax)
                grloc[i] = (CSW_F)zmax;
            if (NearZero(grloc[i])) grloc[i] = 0.0;
        }
        c4 += 4;
    }

    *grid = grloc;
    if (mask) *mask = maskloc;

    bsuccess = true;

    return 1;

}  /*  end of private ReadGrid function  */




/* 
  ****************************************************************************

                           N e a r Z e r o

  ****************************************************************************

    Return 1 if the value is very near zero, or return zero otherwise.

*/

int CSWGrdFileio::NearZero (CSW_F value)
{
    if (value > -TinyFloat  &&  value < TinyFloat) {
        return 1;
    }
    return 0;

}  /*  end of private NearZero function  */



/*
  ****************************************************************

                      I n t T o C h a r 4

  ****************************************************************

    Encode an integer into a 4 byte character string.

*/

void CSWGrdFileio::IntToChar4 (int inum, char *ucptr)
{
    int              sign;
    unsigned char    cptr[4];

    sign = 1;
    if (inum < 0) {
        sign = -1;
        inum = -inum;
    }

    cptr[0] = (unsigned char)(inum / IDIV3);
    inum %= IDIV3;
    cptr[1] = (unsigned char)(inum / IDIV2);
    inum %= IDIV2;
    cptr[2] = (unsigned char)(inum / IDIV1);
    inum %= IDIV1;
    cptr[3] = (unsigned char)(inum);

    if (sign < 0) {
        cptr[0] |= 128;
    }

    memcpy (ucptr, (char *)cptr, 4*sizeof(char));

    return;

}  /*  end of private IntToChar4 function  */



/*
  ****************************************************************

                     C h a r 4 T o I n t

  ****************************************************************

    Decode a 4 byte character string to an int.

*/

void CSWGrdFileio::Char4ToInt (char *ucptr, int *inum)
{
    int            sign;
    unsigned char  c1, cptr[4];

    memcpy ((char *)cptr, ucptr, 4*sizeof(char));

    c1 = cptr[0];
    sign = 1;
    if (c1 & 128) {
        sign = -1;
        c1 ^= 128;
    }

    *inum = c1*IDIV3 + cptr[1]*IDIV2 + cptr[2]*IDIV1 + cptr[3];
    *inum *= sign;

    return;

}  /*  end of private Char4ToInt function  */





/*
  ****************************************************************

                      P i n t T o C h a r 4

  ****************************************************************

    Encode an integer into a 4 byte character string.  This function
  assumes a positive integer.

*/

void CSWGrdFileio::PintToChar4 (int inum, unsigned char *cptr)
{

    cptr[0] = (unsigned char) (inum / IDIV3);
    inum %= IDIV3;
    cptr[1] = (unsigned char) (inum / IDIV2);
    inum %= IDIV2;
    cptr[2] = (unsigned char) (inum / IDIV1);
    inum %= IDIV1;
    cptr[3] = (unsigned char) inum;

    return;

}  /*  end of private PintToChar4 function  */




/*
  ****************************************************************

                     C h a r 4 T o P i n t

  ****************************************************************

    Decode a 4 byte character string to a positive int.
 
*/

void CSWGrdFileio::Char4ToPint (const unsigned char *cptr, int *inum)
{
 
    *inum = cptr[0]*IDIV3 + cptr[1]*IDIV2 + cptr[2]*IDIV1 + cptr[3];
 
    return;
 
}  /*  end of private Char4ToPint function  */














/*
    *** NOTE-ON-PORTABILITY ***

  ***************************************************************

    The following methods are private functions used to create, open,
    read, write and close files.  I have implemented them by calling
    similar public class functions in this class.  If you would rather
    use your own fileio functions for grid file reading and writing,
    these private functions are the only things you need to change.

  ***************************************************************

*/


/*
  ****************************************************************

                    C r e a t e F i l e

  ****************************************************************

*/

int CSWGrdFileio::CreateFile (const char *name, const char *mode)
{

    return fileio_util_obj.csw_CreateFile (name, mode);

}  /*  end of private CreateFile function  */


/*
  ****************************************************************

                      O p e n F i l e

  ****************************************************************

*/

int CSWGrdFileio::OpenFile (const char *name, const char *mode)
{

    return fileio_util_obj.csw_OpenFile (name, mode);

}  /*  end of private OpenFile function  */

/*
  ****************************************************************

                      A p p e n d F i l e

  ****************************************************************

*/

int CSWGrdFileio::AppendFile (const char *name, const char *mode, long *position)
{
    return fileio_util_obj.csw_AppendFile (name, mode, position);

}  /*  end of private AppendFile function  */

/*
  ****************************************************************

                    B i n F i l e W r i t e

  ****************************************************************

*/

int CSWGrdFileio::BinFileWrite (const void *buffer, int size, int count, int file)
{

    return fileio_util_obj.csw_BinFileWrite (buffer, size, count, file);

}  /*  end of private BinFileWrite function  */


/*
  ****************************************************************

                      B i n F i l e R e a d

  ****************************************************************

*/

int CSWGrdFileio::BinFileRead (void *buffer, int size, int count, int file)
{

    return fileio_util_obj.csw_BinFileRead (buffer, size, count, file);

}  /*  end of private BinFileRead function  */


/*
  ****************************************************************

                      C l o s e F i l e

  ****************************************************************

*/

int CSWGrdFileio::CloseFile (int file)
{

    return fileio_util_obj.csw_CloseFile (file);

}  /*  end of private CloseFile function  */





/* 
  ****************************************************************************

                       W r i t e F a u l t s

  ****************************************************************************

    Write the specified fault data to the current position in the specified
  file.  A return status of -1 indicates an error writing to the file.  A
  status of 1 indicates success.

*/

int CSWGrdFileio::WriteFaults (int filenum, FAultLineStruct *faults,
                        int nfaults)
{
    int               i, j, len, istat;
    double            xscale, zscale, xt, yt, zt;
    int               ix, iy, iz, blen;
    unsigned char     c4[4], cbuf[2000];
    char              c20[20];
    FAultLineStruct   *fptr;
    POint3D           *pptr;

    if (faults == NULL  ||  nfaults == 0) {
        return 1;
    }

/*
    Calculate the scaling factors for x, y and z and write
    them to the file.  Note that if the grid is flat (Zmin
    equals Zmax) then the z scaling factor is set to 1.0
    to avoid zero divides when reading the file.  If both
    x and y dimensions are zero, the x scale is also set
    to 1.0 for the same reason.
*/
    xscale = (Xmax - Xmin + Ymax - Ymin) / FAULT_SCALE_CONSTANT_XY;
    zscale = (Zmax - Zmin) / FAULT_SCALE_CONSTANT_Z;

    if (zscale <= FAULT_DOUBLE_MIN) {
        zscale = 1.0;
    }
    if (xscale <= FAULT_DOUBLE_MIN) {
        xscale = 1.0;
    }

    sprintf (c20, "%15.10g", xscale);
    istat = BinFileWrite (c20, 20, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    sprintf (c20, "%15.10g", zscale);
    istat = BinFileWrite (c20, 20, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    For each fault line structure, determine its length in bytes
    and write the length followed by the data for the fault line.
    The first 3 words after the length are the number of points,
    id and type.  These are followed by a list of the fault's
    x, y and z coordinates.  The coordinates are written as integers
    after appropriate scaling by xscale or zscale.
*/
    for (i=0; i<nfaults; i++) {
        
        fptr = faults + i;
        len = fptr->num_points * 12 + 12 + 4 + fptr->ncomp * 4;
        PintToChar4 (len, c4);
        istat = BinFileWrite (c4, 1, 4, filenum);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        PintToChar4 (fptr->num_points, c4);
        istat = BinFileWrite (c4, 1, 4, filenum);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        PintToChar4 (fptr->id, c4);
        istat = BinFileWrite (c4, 1, 4, filenum);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        PintToChar4 (fptr->type, c4);
        istat = BinFileWrite (c4, 1, 4, filenum);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        PintToChar4 (fptr->ncomp, c4);
        istat = BinFileWrite (c4, 1, 4, filenum);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        if (fptr->ncomp > 0) {
            for (j=0; j<fptr->ncomp; j++) {
                PintToChar4 (fptr->comp_points[j], c4);
                istat = BinFileWrite (c4, 1, 4, filenum);
                if (istat == -1) {
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
            }
        }
   
        blen = 0;
        cbuf[0] = '\0';
        for (j=0; j<fptr->num_points; j++) {
            pptr = fptr->points + j;
            xt = pptr->x;
            yt = pptr->y;
            zt = pptr->z;
            ix = (int)((xt - Xmin) / xscale);
            iy = (int)((yt - Ymin) / xscale);
            if (zt > Zmax  ||  zt < Zmin) {
                iz = FAULT_NULL_INT;
            }
            else {
                iz = (int)((zt - Zmin) / zscale);
            }
            PintToChar4 (ix, cbuf + blen); 
            blen += 4;
            PintToChar4 (iy, cbuf + blen); 
            blen += 4;
            PintToChar4 (iz, cbuf + blen); 
            blen += 4;
        /*
            The local buffer for points is 2000 bytes, so I
            write the buffer to the file when it is not quite
            filled to make sure the buffer doesn't overflow.
        */
            if (blen > 1980) {
                istat = BinFileWrite (cbuf, 1, blen, filenum);
                if (istat == -1) {
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
                blen = 0;
            }
        }
        if (blen > 0) {
            istat = BinFileWrite (cbuf, 1, blen, filenum);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
        }
    }

    return 1;

}  /*  end of private WriteFaults function  */





/* 
  ****************************************************************************

                            R e a d F a u l t s

  ****************************************************************************

  Read the fault data from the specified file.  Allocate  a FAultLineStruct
  array and POint3D arrays as needed.  If an error reading the file occurs, 
  the error is set to 1.  If a memory allocation error occurs, the error is
  set to 5.  On success, 1 is returned and faults and nfaults are non null
  and greater than zero respectively.  If an error occurs, -1 is returned
  and faults is NULL and nfaults is zero.  If no faults are present in the
  file, 0 is returned, but faults is NULL and nfaults is zero.

*/

int CSWGrdFileio::ReadFaults (int version, int filenum,
                       FAultLineStruct **faults, int *nfaults)
{
    FAultLineStruct        *fptr = NULL, *flist = NULL;
    POint3D                *pptr = NULL, *plist = NULL;

    int                    istat, j, i, len, nf, maxbuf, npts;
    int                    ix, iy, iz;
    double                 xscale, zscale;
    unsigned char          c4[4], *cptr = NULL;
    char                   *lbuf = NULL, c10[10], c20[20];

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (lbuf);
        if (bsuccess == false) {
            FreeFaultLines (flist, nf);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    version = version;

    if (faults) {
        *faults = NULL;
    }
    if (nfaults) {
        *nfaults = 0;
    }

/*
    First, read the number of faults from the file.
*/
    istat = BinFileRead (c10, 10, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    sscanf (c10, "%d", &nf);

    if (nf < 1) {
        return 1;
    }

/*
    read the scale factors for x,y and z
*/
    istat = BinFileRead (c20, 20, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    sscanf (c20, "%lg", &xscale);
    istat = BinFileRead (c20, 20, 1, filenum);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    sscanf (c20, "%lg", &zscale);

/*
    Allocate an array to read all the bytes for a particular
    fault line into.  This may need to be expanded later, 
    but for now a guess of 10000 bytes is made.
*/
    maxbuf = 10000;
    lbuf = (char *)csw_Malloc (maxbuf * sizeof(char));
    if (lbuf == NULL) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

/*
    Allocate a list of nf fault line structures and initialize
    them to all zero.  This insures that the points member of
    each structure is NULL.  If a memory allocation error occurs
    while filling in the fault line list, the structures not yet
    filled in need to have NULL points members to allow the 
    cleanup done in FreeFaultLines to work properly.
*/
    flist = (FAultLineStruct *)csw_Calloc (nf * sizeof(FAultLineStruct));
    if (flist == NULL) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

/*
    Read the data for a single fault line at a time
    and fill in the fault line structure.
*/
    for (i=0; i<nf; i++) {
        fptr = flist + i;
        istat = BinFileRead (c4, 4, 1, filenum);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        Char4ToPint (c4, &len);
        if (len > maxbuf) {
            maxbuf = len + 1000;
            lbuf = static_cast<char *> 
                (csw_Realloc ((void *)lbuf, maxbuf * sizeof(char)));
            if (!lbuf) {
                grd_utils_ptr->grd_set_err (5);
                return -1;
            }
        }
    /*
        Read all the bytes for this line into lbuf.
    */
        istat = BinFileRead (lbuf, len, 1, filenum);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    /*
        Decode the lbuf array 4 bytes at a time to get the
        num_points, id and type members of the fault line structure.
    */
        cptr = (unsigned char *)lbuf;
        Char4ToPint (cptr, &(fptr->num_points));
        cptr += 4;
        Char4ToPint (cptr, &(fptr->id));
        cptr += 4;
        Char4ToPint (cptr, &(fptr->type));
        cptr += 4;
        npts = fptr->num_points;
        if (version >= 2002) {
            Char4ToPint (cptr, &(fptr->ncomp));
            cptr += 4;
            if (fptr->ncomp > 0) {
                fptr->comp_points = (int *)csw_Malloc (fptr->ncomp * sizeof(int));
                if (fptr->comp_points == NULL) {
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
                for (j=0; j<fptr->ncomp; j++) {
                    Char4ToPint (cptr, fptr->comp_points+j);
                    cptr += 4;
                }
            }
        }
        else {
            fptr->ncomp = 0;
            fptr->comp_points = NULL;
        }
   
    /*
        Allocate a POint3D array for the fault line structure.
        Fill in the points array from the lbuf array.
    */
        plist = (POint3D *)csw_Malloc (npts * sizeof(POint3D));
        if (plist == NULL) {
            grd_utils_ptr->grd_set_err (5);
            return -1;
        }
        fptr->points = plist;
        for (j=0; j<npts; j++) {
            pptr = plist + j;
            Char4ToPint (cptr, &ix);
            cptr += 4;
            Char4ToPint (cptr, &iy);
            cptr += 4;
            Char4ToPint (cptr, &iz);
            cptr += 4;
            pptr->x = Xmin + (double)ix * xscale;    
            pptr->y = Ymin + (double)iy * xscale;    
            if (iz == FAULT_NULL_INT) {
                pptr->z = 1.e30;    
            }
            else {
                pptr->z = Zmin + (double)iz * zscale;    
            }
        }
    }

    if (faults) {
        *faults = flist;
    }
    else {
        FreeFaultLines (flist, nf);
        flist = NULL;
        nf = 0;
    }
    if (nfaults) {
        *nfaults = nf;
    }

    bsuccess = true;

    return 1;

}  /*  end of private ReadFaults function  */





/* 
  ****************************************************************************

                   F r e e F a u l t L i n e s

  ****************************************************************************

    Free the memory associated with a list of FAultLineStructs.

*/

void CSWGrdFileio::FreeFaultLines (FAultLineStruct *flist, int nf)
{
    int                  i;

    for (i=0; i<nf; i++) {
        if (flist[i].comp_points) {
            csw_Free (flist[i].comp_points);
        }
        if (flist[i].points) {
            csw_Free (flist[i].points);
        }
    }

    csw_Free (flist);

    return;

}  /*  end of private FreeFaultLines function  */




/*
 **************************************************************************

                      R e a d E x p a n d e d M a s k

 **************************************************************************

  Read the "crossing mask" information from the grid file.

*/

int CSWGrdFileio::ReadExpandedMask (char **mask2,
                             int filenum)
{
    int               nc, istat;
    char              header[20], *mask = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (mask);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (mask2 != NULL) {
        *mask2 = NULL;
    }

/*
 * First, read the number of nodes in the extended mask.
 */
    istat = fileio_util_obj.csw_BinFileRead (header, 10, 1, filenum);
    if (istat == -1) {
        return -1;
    }

/*
 * If there are no extended mask nodes, that is not an error,
 * but nothing else needs to be read.
 */
    sscanf (header, "%d", &nc);
    if (nc < 1) {
        return 1;
    }

/*
 * Allocate space to receive the mask.
 */
    mask = (char *)csw_Malloc (nc * sizeof(char));
    if (mask == NULL) {
        return -1;
    }

    istat = fileio_util_obj.csw_BinFileRead (mask, nc * sizeof(char), 1, filenum);
    if (istat == -1) {
        return -1;
    }

/*
 * Either return the input mask array or csw_Free it
 * if the mask2 parameter is NULL.
 */
    if (mask2 != NULL) {
        *mask2 = mask;
    }
    else {
        csw_Free (mask);
        mask = NULL;
    }

    bsuccess = true;

    return 1;

}


/*-------------------------------------------------------------------------*/

#define TMESH_VERSION          100
#define TMESH_XY_DIVISOR       10000000.0
#define TMESH_Z_DIVISOR        1000000.0


int CSWGrdFileio::grd_write_binary_trimesh_file (
    int               vused,
    double            *vbase,
    TRiangleStruct    *triangles,
    int               ntri,
    EDgeStruct        *edges,
    int               nedge,
    NOdeStruct        *nodes,
    int               nnode,
    char              *filename)
{
    int               filenum, dlen, i, istat;
    int               ix, iy, iz, n1, n2, t1, t2, e1, e2, e3;
    double            xmin, ymin, xmax, ymax, zmin, zmax;
    double            xt, yt, zt, dxy, dz;
    double            tiny;
    unsigned char     *uc4;
    unsigned char     uc10000[10100];
    char              *c4;
    char              c10000[10100];
    int               n;

/*
    Open the output file if possible.  If a file already
    exists, it is overwritten.
*/
    filenum = CreateFile (filename, "b");
    if (filenum < 0) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    The first 10 bytes always have "!BIN_TMESH".  The next 10 bytes have
    the version number.  The next 10 bytes have the header length and the 
    next 10 bytes have the data length.
*/
    dlen = ntri * 12 + nedge * 16 + nnode * 24;
    sprintf (Head1, "!BIN_TMESH");
    sprintf (Head2, "%d", TMESH_VERSION);
    sprintf (Head3, "%d", HEADER_SIZE);
    sprintf (Head4, "%d", dlen);

/*
    Write the constant headers to the file.
*/
    istat = BinFileWrite ((void *)Head1, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileWrite ((void *)Head2, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileWrite ((void *)Head3, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileWrite ((void *)Head4, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        return -1;
    }

/*
 * Calculate the scales and limits to convert the xy coordinates
 * into local 32 bit integer representations.
 */
    xmin = 1.e30;
    ymin = 1.e30;
    zmin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;
    zmax = -1.e30;
    
    for (i=0; i<nnode; i++) {
        if (nodes[i].x < xmin) xmin = nodes[i].x;
        if (nodes[i].y < ymin) ymin = nodes[i].y;
        if (nodes[i].z < zmin) zmin = nodes[i].z;
        if (nodes[i].x > xmax) xmax = nodes[i].x;
        if (nodes[i].y > ymax) ymax = nodes[i].y;
        if (nodes[i].z > zmax) zmax = nodes[i].z;
    }

    if (xmin >= xmax  ||  ymin >= ymax  ||  zmin >= zmax) {
        CloseFile (filenum);
        return -1;
    }

    tiny = (xmax - xmin + ymax - ymin) / 1000.0;
    xmin -= tiny;
    ymin -= tiny;
    xmax += tiny;
    ymax += tiny;
    tiny = (zmax - zmin) / 1000.0;
    zmin -= tiny;
    zmax += tiny;

    dxy = (xmax - xmin + ymax - ymin) / TMESH_XY_DIVISOR;
    dz = (zmax - zmin) / TMESH_Z_DIVISOR;

/*
 * Write the data info header to the file.
 */
    sprintf (Header,
             "%d %d %d %g %g %g %g %g %d %g %g %g %g %g %g",
             nnode, nedge, ntri,
             xmin, ymin, zmin, dxy, dz,
             vused, vbase[0], vbase[1], vbase[2], vbase[3], vbase[4], vbase[5]);
    istat = BinFileWrite ((void *)Header, HEADER_SIZE, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        return -1;
    }

/*
 * Write the node coordinates.
 */
    uc4 = uc10000;
    n = 0;
    for (i=0; i<nnode; i++) {
        xt = nodes[i].x - xmin;
        yt = nodes[i].y - ymin;
        zt = nodes[i].z - zmin;
        ix = (int)(xt / dxy + 0.5);
        iy = (int)(yt / dxy + 0.5);
        iz = (int)(zt / dz + 0.5);
        PintToChar4 (ix, uc4);
        uc4 += 4;
        n += 4;
        PintToChar4 (iy, uc4);
        uc4 += 4;
        n += 4;
        PintToChar4 (iz, uc4);
        uc4 += 4;
        n += 4;
        if (n > 10000) {
            istat = BinFileWrite ((void *)uc10000, n, 1, filenum);
            if (istat == -1) {
                CloseFile (filenum);
                return -1;
            }
            uc4 = uc10000;
            n = 0;
        }
    }
    if (n > 0) {
        istat = BinFileWrite ((void *)uc10000, n, 1, filenum);
        if (istat == -1) {
            CloseFile (filenum);
            return -1;
        }
    }

/*
 * Write the edge information.
 */
    c4 = c10000;
    n = 0;
    for (i=0; i<nedge; i++) {
        n1 = edges[i].node1;        
        n2 = edges[i].node2;        
        t1 = edges[i].tri1;
        t2 = edges[i].tri2;
        IntToChar4 (n1, c4);
        c4 += 4;
        n += 4;
        IntToChar4 (n2, c4);
        c4 += 4;
        n += 4;
        IntToChar4 (t1, c4);
        c4 += 4;
        n += 4;
        IntToChar4 (t2, c4);
        c4 += 4;
        n += 4;
        if (n > 10000) {
            istat = BinFileWrite ((void *)c10000, n, 1, filenum);
            if (istat == -1) {
                CloseFile (filenum);
                return -1;
            }
            c4 = c10000;
            n = 0;
        }
    }
    if (n > 0) {
        istat = BinFileWrite ((void *)c10000, n, 1, filenum);
        if (istat == -1) {
            CloseFile (filenum);
            return -1;
        }
    }

/*
 * Write the triangle information.
 */
    c4 = c10000;
    n = 0;
    for (i=0; i<ntri; i++) {
        e1 = triangles[i].edge1;
        e2 = triangles[i].edge2;
        e3 = triangles[i].edge3;
        IntToChar4 (e1, c4);
        c4 += 4;
        n += 4;
        IntToChar4 (e2, c4);
        c4 += 4;
        n += 4;
        IntToChar4 (e3, c4);
        c4 += 4;
        n += 4;
        if (n > 10000) {
            istat = BinFileWrite ((void *)c10000, n, 1, filenum);
            if (istat == -1) {
                CloseFile (filenum);
                return -1;
            }
            c4 = c10000;
            n = 0;
        }
    }
    if (n > 0) {
        istat = BinFileWrite ((void *)c10000, n, 1, filenum);
        if (istat == -1) {
            CloseFile (filenum);
            return -1;
        }
    }

    CloseFile (filenum);

    return 1;

}

long CSWGrdFileio::grd_append_binary_trimesh_file (
    int               vused,
    double            *vbase,
    TRiangleStruct    *triangles,
    int               ntri,
    EDgeStruct        *edges,
    int               nedge,
    NOdeStruct        *nodes,
    int               nnode,
    char              *filename)
{
    int               filenum, dlen, i, istat;
    int               ix, iy, iz, n1, n2, t1, t2, e1, e2, e3;
    double            xmin, ymin, xmax, ymax, zmin, zmax;
    double            xt, yt, zt, dxy, dz;
    double            tiny;
    unsigned char     *uc4;
    unsigned char     uc10000[10100];
    char              *c4;
    char              c10000[10100];
    int               n;
    long position = 0L;

/*
    Open the output file if possible.  If a file already
    exists, it is overwritten.
*/
    filenum = AppendFile(filename, "a", &position);
    if (filenum < 0) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    The first 10 bytes always have "!BIN_TMESH".  The next 10 bytes have
    the version number.  The next 10 bytes have the header length and the 
    next 10 bytes have the data length.
*/
    dlen = ntri * 12 + nedge * 16 + nnode * 24;
    sprintf (Head1, "!BIN_TMESH");
    sprintf (Head2, "%d", TMESH_VERSION);
    sprintf (Head3, "%d", HEADER_SIZE);
    sprintf (Head4, "%d", dlen);

/*
    Write the constant headers to the file.
*/
    istat = BinFileWrite ((void *)Head1, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileWrite ((void *)Head2, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileWrite ((void *)Head3, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        return -1;
    }
    istat = BinFileWrite ((void *)Head4, 10, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        return -1;
    }

/*
 * Calculate the scales and limits to convert the xy coordinates
 * into local 32 bit integer representations.
 */
    xmin = 1.e30;
    ymin = 1.e30;
    zmin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;
    zmax = -1.e30;
    
    for (i=0; i<nnode; i++) {
        if (nodes[i].x < xmin) xmin = nodes[i].x;
        if (nodes[i].y < ymin) ymin = nodes[i].y;
        if (nodes[i].z < zmin) zmin = nodes[i].z;
        if (nodes[i].x > xmax) xmax = nodes[i].x;
        if (nodes[i].y > ymax) ymax = nodes[i].y;
        if (nodes[i].z > zmax) zmax = nodes[i].z;
    }

    if (xmin >= xmax  ||  ymin >= ymax  ||  zmin >= zmax) {
        CloseFile (filenum);
        return -1;
    }

    tiny = (xmax - xmin + ymax - ymin) / 1000.0;
    xmin -= tiny;
    ymin -= tiny;
    xmax += tiny;
    ymax += tiny;
    tiny = (zmax - zmin) / 1000.0;
    zmin -= tiny;
    zmax += tiny;

    dxy = (xmax - xmin + ymax - ymin) / TMESH_XY_DIVISOR;
    dz = (zmax - zmin) / TMESH_Z_DIVISOR;

/*
 * Write the data info header to the file.
 */
    sprintf (Header,
             "%d %d %d %g %g %g %g %g %d %g %g %g %g %g %g",
             nnode, nedge, ntri,
             xmin, ymin, zmin, dxy, dz,
             vused, vbase[0], vbase[1], vbase[2], vbase[3], vbase[4], vbase[5]);
    istat = BinFileWrite ((void *)Header, HEADER_SIZE, 1, filenum);
    if (istat == -1) {
        CloseFile (filenum);
        return -1;
    }

/*
 * Write the node coordinates.
 */
    uc4 = uc10000;
    n = 0;
    for (i=0; i<nnode; i++) {
        xt = nodes[i].x - xmin;
        yt = nodes[i].y - ymin;
        zt = nodes[i].z - zmin;
        ix = (int)(xt / dxy + 0.5);
        iy = (int)(yt / dxy + 0.5);
        iz = (int)(zt / dz + 0.5);
        PintToChar4 (ix, uc4);
        uc4 += 4;
        n += 4;
        PintToChar4 (iy, uc4);
        uc4 += 4;
        n += 4;
        PintToChar4 (iz, uc4);
        uc4 += 4;
        n += 4;
        if (n > 10000) {
            istat = BinFileWrite ((void *)uc10000, n, 1, filenum);
            if (istat == -1) {
                CloseFile (filenum);
                return -1;
            }
            uc4 = uc10000;
            n = 0;
        }
    }
    if (n > 0) {
        istat = BinFileWrite ((void *)uc10000, n, 1, filenum);
        if (istat == -1) {
            CloseFile (filenum);
            return -1;
        }
    }

/*
 * Write the edge information.
 */
    c4 = c10000;
    n = 0;
    for (i=0; i<nedge; i++) {
        n1 = edges[i].node1;        
        n2 = edges[i].node2;        
        t1 = edges[i].tri1;
        t2 = edges[i].tri2;
        IntToChar4 (n1, c4);
        c4 += 4;
        n += 4;
        IntToChar4 (n2, c4);
        c4 += 4;
        n += 4;
        IntToChar4 (t1, c4);
        c4 += 4;
        n += 4;
        IntToChar4 (t2, c4);
        c4 += 4;
        n += 4;
        if (n > 10000) {
            istat = BinFileWrite ((void *)c10000, n, 1, filenum);
            if (istat == -1) {
                CloseFile (filenum);
                return -1;
            }
            c4 = c10000;
            n = 0;
        }
    }
    if (n > 0) {
        istat = BinFileWrite ((void *)c10000, n, 1, filenum);
        if (istat == -1) {
            CloseFile (filenum);
            return -1;
        }
    }

/*
 * Write the triangle information.
 */
    c4 = c10000;
    n = 0;
    for (i=0; i<ntri; i++) {
        e1 = triangles[i].edge1;
        e2 = triangles[i].edge2;
        e3 = triangles[i].edge3;
        IntToChar4 (e1, c4);
        c4 += 4;
        n += 4;
        IntToChar4 (e2, c4);
        c4 += 4;
        n += 4;
        IntToChar4 (e3, c4);
        c4 += 4;
        n += 4;
        if (n > 10000) {
            istat = BinFileWrite ((void *)c10000, n, 1, filenum);
            if (istat == -1) {
                CloseFile (filenum);
                return -1;
            }
            c4 = c10000;
            n = 0;
        }
    }
    if (n > 0) {
        istat = BinFileWrite ((void *)c10000, n, 1, filenum);
        if (istat == -1) {
            CloseFile (filenum);
            return -1;
        }
    }

    CloseFile(filenum);

    return(position);
}


/*----------------------------------------------------------------------------*/

int CSWGrdFileio::read_binary_trimesh_file (
    char              *filename,
    long              position,
    int               *vused_out,
    double            **vbase_out,
    TRiangleStruct    **triangles_out,
    int               *ntri_out,
    EDgeStruct        **edges_out,
    int               *nedge_out,
    NOdeStruct        **nodes_out,
    int               *nnode_out)
{
    int               filenum = -1, i, istat, hsize;
    int               ix, iy, iz, n1, n2, t1, t2, e1, e2, e3;
    double            xmin, ymin, zmin;
    double            xt, yt, zt, dxy, dz;
    unsigned char     *uc4 = NULL;
    unsigned char     *ucbuff = NULL;
    char              *c4 = NULL;
    char              *cbuff = NULL;
    int               n;
    TRiangleStruct    *triangles = NULL;
    EDgeStruct        *edges = NULL;
    NOdeStruct        *nodes = NULL;
    int               nnode, nedge, ntri;
    double            *vbase = NULL;
    int               vused;

    bool    bsuccess = false;
    bool    bfclose = true;

    auto fscope = [&]()
    {
        if (bfclose  &&  filenum >= 0) {
            CloseFile (filenum);
        }
        if (bsuccess == false) {
            csw_Free (vbase);
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
        }
        csw_Free (ucbuff);
        csw_Free (cbuff);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Initialize output in case of error.
 */
    *triangles_out = NULL;
    *ntri_out = 0;
    *edges_out = NULL;
    *nedge_out = 0;
    *nodes_out = NULL;
    *nnode_out = 0;
    *vused_out = -1;
    *vbase_out = NULL;

/*
    Open the input file.
*/
    filenum = OpenFile (filename, "r");
    if (filenum < 0) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }
    /* set position in file to read */
    fileio_util_obj.csw_SetFilePosition(filenum, position, SEEK_SET);

/*
    Do 4 reads for the 10 character blocks always present.
*/
    istat = BinFileRead ((void *)Head1, 10, 1, filenum);
    if (istat == -1) {
        return -1;
    }

/*
 * If the first 10 characters do no identify a proper file type,
 * return zero.
 */
    if (strcmp (Head1, "!BIN_TMESH") != 0) {
        return 0;
    }

    istat = BinFileRead ((void *)Head2, 10, 1, filenum);
    if (istat == -1) {
        return -1;
    }
    istat = BinFileRead ((void *)Head3, 10, 1, filenum);
    if (istat == -1) {
        return -1;
    }
    istat = BinFileRead ((void *)Head4, 10, 1, filenum);
    if (istat == -1) {
        return -1;
    }

    sscanf (Head3, "%d", &hsize);
    if (hsize < 10) {
        return -1;
    }

    vbase = (double *)csw_Malloc (6 * sizeof(double));
    if (vbase == NULL) {
        return -1;
    }

/*
 * Read the header info.
 */
    istat = BinFileRead ((void *)Header, hsize, 1, filenum);
    if (istat == -1) {
        return -1;
    }
    sscanf (Header,
            "%d %d %d %lf %lf %lf %lf %lf %d %lf %lf %lf %lf %lf %lf",
            &nnode, &nedge, &ntri,
            &xmin, &ymin, &zmin, &dxy, &dz,
            &vused, vbase, vbase+1, vbase+2, vbase+3, vbase+4, vbase+5);

    if (nnode < 3  ||  nedge < 3  ||  ntri < 1) {
        return -1;
    }

/*
 * Allocate trimesh structure space.
 */
    nodes = NULL;
    edges = NULL;
    triangles = NULL;

    nodes = (NOdeStruct *)csw_Calloc (nnode * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Calloc (nedge * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Calloc (ntri * sizeof(TRiangleStruct));
    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

/*
 * Read the node coordinates.
 */
    n = nnode * 12;
    ucbuff = (unsigned char *)csw_Malloc (n * sizeof(unsigned char));
    if (ucbuff == NULL) {
        return -1;
    }
    istat = BinFileRead ((void *)ucbuff, n * sizeof(unsigned char), 1, filenum);
    if (istat == -1) {
        return -1;
    }
    uc4 = ucbuff;
    for (i=0; i<nnode; i++) {
        Char4ToPint (uc4, &ix);
        uc4 += 4;
        Char4ToPint (uc4, &iy);
        uc4 += 4;
        Char4ToPint (uc4, &iz);
        uc4 += 4;
        xt = ix * dxy + xmin;
        yt = iy * dxy + ymin;
        zt = iz * dz + zmin;
        nodes[i].x = xt;
        nodes[i].y = yt;
        nodes[i].z = zt;
    }

    csw_Free (ucbuff);
    ucbuff = NULL;

/*
 * Read the edge information.
 */
    n = nedge * 16;
    cbuff = (char *)csw_Malloc (n * sizeof(char));
    if (cbuff == NULL) {
        return -1;
    }
    istat = BinFileRead ((void *)cbuff, n * sizeof(char), 1, filenum);
    if (istat == -1) {
        return -1;
    }
    c4 = cbuff;
    for (i=0; i<nedge; i++) {
        Char4ToInt (c4, &n1);
        c4+=4;
        Char4ToInt (c4, &n2);
        c4+=4;
        Char4ToInt (c4, &t1);
        c4+=4;
        Char4ToInt (c4, &t2);
        c4+=4;
        edges[i].node1 = n1;        
        edges[i].node2 = n2;        
        edges[i].tri1 = t1;
        edges[i].tri2 = t2;
    }

/*
 * Read the triangle information.
 */
    n = ntri * 12;
    cbuff = (char *)csw_Malloc (n * sizeof(char));
    if (cbuff == NULL) {
        return -1;
    }
    istat = BinFileRead ((void *)cbuff, n * sizeof(char), 1, filenum);
    if (istat == -1) {
        return -1;
    }
    c4 = cbuff;
    for (i=0; i<ntri; i++) {
        Char4ToInt (c4, &e1);
        c4+=4;
        Char4ToInt (c4, &e2);
        c4+=4;
        Char4ToInt (c4, &e3);
        c4+=4;
        triangles[i].edge1 = e1;
        triangles[i].edge2 = e2;
        triangles[i].edge3 = e3;
    }

    CloseFile (filenum);
    bfclose = false;

    *triangles_out = triangles;
    *edges_out = edges;
    *nodes_out = nodes;
    *nnode_out = nnode;
    *nedge_out = nedge;
    *ntri_out = ntri;
    *vused_out = vused;
    *vbase_out = vbase;

    bsuccess = true;

    return 1;

}

/*----------------------------------------------------------------------------*/

int CSWGrdFileio::grd_read_binary_trimesh_file (
    char              *filename,
    int               *vused_out,
    double            **vbase_out,
    TRiangleStruct    **triangles_out,
    int               *ntri_out,
    EDgeStruct        **edges_out,
    int               *nedge_out,
    NOdeStruct        **nodes_out,
    int               *nnode_out)
{
  return(
    read_binary_trimesh_file(
      filename,
      0L, /* position */
      vused_out,
      vbase_out,
      triangles_out,
      ntri_out,
      edges_out,
      nedge_out,
      nodes_out,
      nnode_out
    )
  );
}

/*----------------------------------------------------------------------------*/

int CSWGrdFileio::grd_read_binary_trimesh_from_multi_file (
    char              *filename,
    long              position,
    int               *vused_out,
    double            **vbase_out,
    TRiangleStruct    **triangles_out,
    int               *ntri_out,
    EDgeStruct        **edges_out,
    int               *nedge_out,
    NOdeStruct        **nodes_out,
    int               *nnode_out)
{
  return(
    read_binary_trimesh_file(
      filename,
      position,
      vused_out,
      vbase_out,
      triangles_out,
      ntri_out,
      edges_out,
      nedge_out,
      nodes_out,
      nnode_out
    )
  );
}






/*
 ************************************************************************************

                g r d _ W r i t e T e x t T r i M e s h F i l e

 ************************************************************************************

*/

#define TEXT_TMESH_VERSION            "1.00"

int CSWGrdFileio::grd_write_text_tri_mesh_file
                 (int vused,
                  double *vbasein,
                  TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  char *filename)

{
    FILE         *fptr = NULL;
    int          i;
    char         line[500];
    double       vb[6], *vbase;


    auto fscope = [&]()
    {
        if (fptr != NULL) fclose (fptr);
    };
    CSWScopeGuard func_scope_guard (fscope);


    fptr = fopen (filename, "w");
    if (fptr == NULL) {
        printf ("Cannot open the new tri mesh file\n");
        return -1;
    }

    vb[0] = 1.e30;
    vb[1] = 1.e30;
    vb[2] = 1.e30;
    vb[3] = 1.e30;
    vb[4] = 1.e30;
    vb[5] = 1.e30;

    vbase = vbasein;
    if (vbase == NULL) {
        vused = 0;
        vbase = vb;
    }

    sprintf (line, "!TXT_TMESH %s\n", TEXT_TMESH_VERSION);
    fputs (line, fptr);

    sprintf (line, "%d %g %g %g %g %g %g\n",
             vused,
             vbase[0],
             vbase[1],
             vbase[2],
             vbase[3],
             vbase[4],
             vbase[5]);
    fputs (line, fptr);

    sprintf (line, "%d %d %d\n", numnodes, nedges, ntriangles);
    fputs (line, fptr);

    for (i=0; i<numnodes; i++) {
        sprintf (line, "%g %g %g %d\n",
            nodes[i].x, nodes[i].y,
            nodes[i].z, nodes[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<nedges; i++) {
        sprintf (line, "%d %d %d %d %d\n",
            edges[i].node1, edges[i].node2, edges[i].tri1,
            edges[i].tri2, edges[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<ntriangles; i++) {
        sprintf (line, "%d %d %d %d\n",
            triangles[i].edge1, triangles[i].edge2,
            triangles[i].edge3, triangles[i].flag);
        fputs (line, fptr);
    }

    fclose (fptr);
    fptr = NULL;

    return 1;

}

/*
 * Returns position in file where we wrote the data, or -1 on i/o error.
 */
long CSWGrdFileio::grd_append_text_tri_mesh_file
                 (int vused,
                  double *vbase,
                  TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  char *filename)

{
    FILE         *fptr = NULL;
    int          i;
    char         line[500];
    long position = 0L;


    auto fscope = [&]()
    {
        if (fptr != NULL) fclose (fptr);
    };
    CSWScopeGuard func_scope_guard (fscope);


    fptr = fopen(filename, "a");
    if (fptr == NULL) {
        printf ("Cannot open the new tri mesh file\n");
        return -1;
    }
    /* get position in file where we will be appending */
    fseek(fptr, 0L, SEEK_END);
    position = ftell(fptr);

    sprintf (line, "!TXT_TMESH %s\n", TEXT_TMESH_VERSION);
    fputs (line, fptr);

    sprintf (line, "%d %g %g %g %g %g %g\n",
             vused,
             vbase[0],
             vbase[1],
             vbase[2],
             vbase[3],
             vbase[4],
             vbase[5]);
    fputs (line, fptr);

    sprintf (line, "%d %d %d\n", numnodes, nedges, ntriangles);
    fputs (line, fptr);

    for (i=0; i<numnodes; i++) {
        sprintf (line, "%g %g %g %d\n",
            nodes[i].x, nodes[i].y,
            nodes[i].z, nodes[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<nedges; i++) {
        sprintf (line, "%d %d %d %d %d\n",
            edges[i].node1, edges[i].node2, edges[i].tri1,
            edges[i].tri2, edges[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<ntriangles; i++) {
        sprintf (line, "%d %d %d %d\n",
            triangles[i].edge1, triangles[i].edge2,
            triangles[i].edge3, triangles[i].flag);
        fputs (line, fptr);
    }

    fclose (fptr);
    fptr = NULL;

    return position;
}




/*
  ****************************************************************

                g r d _ W r i t e T e x t F i l e

  ****************************************************************

*/

int CSWGrdFileio::grd_write_text_file
                   (CSW_F *grid, int ncol, int nrow,
                   double xmin, double ymin, double xmax, double ymax,
                   char *filename)

{
    int            istat;

    int            n, i, j, offset;
    double         yt, xspace, yspace;
    double         *x = NULL, *y = NULL, *z = NULL;


    auto fscope = [&]()
    {
        csw_Free (x);
    };
    CSWScopeGuard func_scope_guard (fscope);


    x = (double *)csw_Malloc (ncol * nrow * 3 * sizeof(double));
    if (x == NULL) {
        return -1;
    }
    y = x + ncol * nrow;
    z = y + ncol * nrow;

    xspace = (xmax - xmin) / (ncol - 1);
    yspace = (ymax - ymin) / (nrow - 1);

    n = 0;
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        yt = ymin + yspace * i;
        for (j=0; j<ncol; j++) {
            x[n] = xmin + j * xspace;
            y[n] = yt;
            z[n] = grid[offset+j];
            n++;
        }
    }

    istat = grd_write_points (x, y, z, n, filename);

    return istat;

}  /*  end of grd_WriteTextFile function  */



/*
 ************************************************************************************

            g r d _ W r i t e T r i M e s h F i l e

 ************************************************************************************

*/

int CSWGrdFileio::grd_write_tri_mesh_file
                 (TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  double xshift, double yshift,
                  const char *filename)
{
    FILE         *fptr;
    int          i;
    char         line[500];

    fptr = fopen (filename, "w");
    if (fptr == NULL) {
        printf ("Cannot open the new tri mesh file\n");
        return -1;
    }

	sprintf (line, "!TXT_TMESH\n");
	fputs (line, fptr);

	sprintf (line, "0 0.0 0.0 0.0 0.0 0.0 0.0\n");
	fputs (line, fptr);

    sprintf (line, "%d %d %d\n", numnodes, nedges, ntriangles);
    fputs (line, fptr);

    for (i=0; i<numnodes; i++) {
        sprintf (line, "%g %g %g %d\n",
            nodes[i].x + xshift, nodes[i].y + yshift,
            nodes[i].z, nodes[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<nedges; i++) {
        sprintf (line, "%d %d %d %d %d\n",
            edges[i].node1, edges[i].node2, edges[i].tri1,
            edges[i].tri2, edges[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<ntriangles; i++) {
        sprintf (line, "%d %d %d %d\n",
            triangles[i].edge1, triangles[i].edge2,
            triangles[i].edge3, triangles[i].flag);
        fputs (line, fptr);
    }

    fclose (fptr);

    return 1;

}



/*
 ************************************************************************************

            g r d _ W r i t e T r i M e s h F i l e 2

 ************************************************************************************

*/

int CSWGrdFileio::grd_write_tri_mesh_file_2
                 (TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  double xshift, double yshift,
                  double *xb, double *yb, int nb,
                  const char *filename)

{
    FILE         *fptr;
    int          i;
    char         line[500];

    fptr = fopen (filename, "w");
    if (fptr == NULL) {
        printf ("Cannot open the new tri mesh file\n");
        return -1;
    }

    if (xb != NULL  &&  yb != NULL  &&  nb > 3) {
        sprintf (line, "boundary\n");
        fputs (line, fptr);
        sprintf (line, "%d\n", nb);
        fputs (line, fptr);
        for (i=0; i<nb; i++) {
            sprintf (line, "%g %g\n", xb[i], yb[i]);
            fputs (line, fptr);
        }
    }

    sprintf (line, "%d %d %d\n", numnodes, nedges, ntriangles);
    fputs (line, fptr);

    for (i=0; i<numnodes; i++) {
        sprintf (line, "%g %g %g %d\n",
            nodes[i].x + xshift, nodes[i].y + yshift,
            nodes[i].z, nodes[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<nedges; i++) {
        sprintf (line, "%d %d %d %d %d\n",
            edges[i].node1, edges[i].node2, edges[i].tri1,
            edges[i].tri2, edges[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<ntriangles; i++) {
        sprintf (line, "%d %d %d %d\n",
            triangles[i].edge1, triangles[i].edge2,
            triangles[i].edge3, triangles[i].flag);
        fputs (line, fptr);
    }

    fclose (fptr);

    return 1;

}



/*
 *********************************************************************************

                     g r d _ W r i t e P o i n t s

 *********************************************************************************

  Write a set of xyz points out to the specified ascii file.  This is used
  almost exclusively for debugging purposes.

*/

int CSWGrdFileio::grd_write_points
                    (double *x,
                     double *y,
                     double *z,
                     int npts,
                     char *fname)
{
    FILE         *fptr;
    int          i;
    char         c200[200];

    if (x == NULL  ||  y == NULL  ||  z == NULL  ||
        npts < 1  ||  fname == NULL) {
        return -1;
    }

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    for (i=0; i<npts; i++) {
        sprintf (c200, "%g %g %g\n", x[i], y[i], z[i]);
        fputs (c200, fptr);
    }

    fclose (fptr);

    return 1;

}



/*
  ****************************************************************

            g r d _ W r i t e B l e n d e d G r i d

  ****************************************************************

*/

int CSWGrdFileio::grd_write_blended_grid
                   (CSW_Blended *grid, int ncol, int nrow, int nskip,
                   double xmin, double ymin, double xmax, double ymax,
                   char *filename)

{
    int            istat;

    int            n, i, j, offset;
    double         yt, xspace, yspace;
    double         *x = NULL, *y = NULL, *z = NULL;
    CSW_Blended    *bptr = NULL;
    double         sum1, sum2;


    auto fscope = [&]()
    {
        csw_Free (x);
    };
    CSWScopeGuard func_scope_guard (fscope);


    x = (double *)csw_Malloc (ncol * nrow * 3 * sizeof(double));
    if (x == NULL) {
        return -1;
    }
    y = x + ncol * nrow;
    z = y + ncol * nrow;

    xspace = (xmax - xmin) / (ncol - 1);
    yspace = (ymax - ymin) / (nrow - 1);

    n = 0;
    for (i=0; i<nrow; i+=nskip) {
        offset = i * ncol;
        yt = ymin + yspace * i;
        for (j=0; j<ncol; j+=nskip) {
            x[n] = xmin + j * xspace;
            y[n] = yt;
            bptr = grid + offset + j;
            sum1 = 0.0;
            sum2 = 0.0;
            if (bptr->v1 > 0) {
                sum1 += bptr->v1;
                sum2++;
            }
            if (bptr->v2 > 0) {
                sum1 += bptr->v2;
                sum2++;
            }
            if (bptr->v3 > 0) {
                sum1 += bptr->v3;
                sum2++;
            }
            if (bptr->v4 > 0) {
                sum1 += bptr->v4;
                sum2++;
            }
            if (sum2 > 0.0) {
                z[n] = sum1 / sum2;
            }
            else {
                z[n] = -1;
            }
            n++;
        }
    }

    istat = grd_write_points (x, y, z, n, filename);

    return istat;

}  /*  end of grd_WriteBlendedGrid function  */



/*
 *********************************************************************************

                     g r d _ W r i t e F l o a t P o i n t s

 *********************************************************************************

  Write a set of xyz points out to the specified ascii file.  This is used
  almost exclusively for debugging purposes.

*/

int CSWGrdFileio::grd_write_float_points
                         (CSW_F  *x,
                          CSW_F  *y,
                          CSW_F  *z,
                          int npts,
                          const char *fname)
{
    FILE         *fptr;
    int          i;
    char         c200[200];

    if (x == NULL  ||  y == NULL  ||  z == NULL  ||
        npts < 1  ||  fname == NULL) {
        return -1;
    }

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    for (i=0; i<npts; i++) {
        sprintf (c200, "%g %g %g\n", x[i], y[i], z[i]);
        fputs (c200, fptr);
    }

    fclose (fptr);

    return 1;

}



/*
 *********************************************************************************

                   g r d _ W r i t e F l o a t L i n e s

 *********************************************************************************

  Write a set of x, y and (optional) z coordinates to a file as individual
  polylines.

*/

int CSWGrdFileio::grd_write_float_lines (
                   CSW_F  *x,
                   CSW_F  *y,
                   CSW_F  *z,
                   int    np,
                   int    *nc,
                   int    *nv,
                   const char *fname)
{
    FILE         *fptr;
    int          i, j, k, n1, n2;
    char         c200[200];

    if (x == NULL  ||  y == NULL  ||
        nc == NULL  ||  nv == NULL  ||
        np < 1  ||  fname == NULL) {
        return -1;
    }

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    n1 = 0;
    for (i=0; i<np; i++) {
        n1 += nc[i];
    }
    sprintf (c200, "%d\n", n1);
    fputs (c200, fptr);

    n1 = 0;
    n2 = 0;
    for (i=0; i<np; i++) {
        for (j=0; j<nc[i]; j++) {
            sprintf (c200, "%d\n", nv[n1]);
            fputs (c200, fptr);
            for (k=0; k<nv[n1]; k++) {
                if (z != NULL) {
                    sprintf (c200, "%g %g %g\n", x[n2], y[n2], z[n2]);
                    fputs (c200, fptr);
                }
                else {
                    sprintf (c200, "%g %g 0.0\n", x[n2], y[n2]);
                    fputs (c200, fptr);
                }
                n2++;
            }
            n1++;
        }
    }

    fclose (fptr);

    return 1;

}
