
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_arith.cc

    This file has CSWGrdArith class methods for one and two grid arithmetic
    operations.  The public functions are:

            grd_one_grid_arith
            grd_two_grid_arith

    Other private functions are used to support these public functions.
*/

#include <stdio.h>
#include <math.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/simulP.h"
#include "csw/utils/private_include/gpf_utils.h"

#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_arith.h"



/*
  ***********************************************************************

                g r d _ o n e _ g r i d _ a r i t h

  ***********************************************************************

  Manipulate a grid by doing arithmetic between each grid value and
  the specified constant.  If the gridout pointer is not null, it must
  point to memory large enough for at least ncol*nrow CSW_F numbers.
  If gout is NULL, the input grid will be overwritten with the results.

  The tag can be a predefined tag (listed in csw/surfaceworks/include/grid_api.h) or you can
  specify GRD_CALL_FUNCTION as the tag.  In this case, you must supply
  a function pointer to a void function with the folllowing parameters
  in the funcptr parameter.

        void (*funcptr)(GRidArithData *)

  The client_data pointer passed to this function is passed on to the
  work function.

*/

int CSWGrdArith::grd_one_grid_arith (CSW_F *grid, CSW_F *gridout,
                        int ncol, int nrow, int tag, CSW_F constant,
                        CSW_F nullvalue, void (*funcptr)(GRidArithData*),
                        void *client_data)
{
    int          i, j, offset;
    CSW_F        *gout, zin, zout, logmult;

/*
    Check for obvious errors.
*/
    if (grid == NULL) {
        grd_utils_ptr->grd_set_err (6);
        return -1;
    }

    if (ncol < 2  ||  nrow < 2) {
        grd_utils_ptr->grd_set_err (7);
        return -1;
    }

    if (tag < 1  ||  tag > 10) {
        if (tag != 999  &&  tag != 12) {
            grd_utils_ptr->grd_set_err (4);
            return -1;
        }
    }

    if (tag == GRD_DIVIDE) {
        if (constant == 0.0f) {
            grd_utils_ptr->grd_set_err (2);
            return -1;
        }
    }

    if (tag == GRD_POWER) {
        if (constant <= 0.0f) {
            grd_utils_ptr->grd_set_err (6);
            return -1;
        }
    }

    logmult = 1.0f;
    if (tag == GRD_LOG) {
        if (constant <= 1.0f) {
            grd_utils_ptr->grd_set_err (3);
            return -1;
        }
        logmult = (CSW_F)log ((double)constant);
    }

/*
    Make sure funcptr and tag agree.
*/
    if (tag == GRD_CALL_FUNC  &&  funcptr == NULL) {
        grd_utils_ptr->grd_set_err (5);
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
    Overwrite the input grid if gridout is NULL.
*/
    if (gridout) {
        gout = gridout;
    }
    else {
        gout = grid;
    }

/*
    Fill in the members of the ArithData structure that do  not
    change from node to node.  This structure is sent to the
    application supplied operation function if such a function
    is specified.
*/
    ArithData.type = GRD_ONE_GRID_ARITH;
    ArithData.grid1 = grid;
    ArithData.grid2 = NULL;
    ArithData.x = 1.e30f;
    ArithData.y = 1.e30f;
    ArithData.x1 = 1.e30f;
    ArithData.y1 = 1.e30f;
    ArithData.x2 = 1.e30f;
    ArithData.y2 = 1.e30f;
    ArithData.ncol = ncol;
    ArithData.nrow = nrow;
    ArithData.constant = constant;
    ArithData.value2 = 1.e30f;
    ArithData.nullvalue = nullvalue;
    ArithData.client_data = client_data;
    ArithData.firstnode = 1;
    ArithData.lastnode = 0;
    ArithData.abort_flag = 0;

/*
    Loop through the grid, applying the operation to
    each non null grid value.
*/
    for (i=0; i<nrow; i++) {

        ArithData.row = i;
        offset = i * ncol;

        for (j=0; j<ncol; j++) {

            zin = grid[offset+j];
            if (zin >= nullvalue  &&  tag != GRD_CALL_FUNC) {
                gout[offset+j] = nullvalue;
                continue;
            }
            if (i == nrow-1  &&  j == ncol-1) {
                ArithData.lastnode = 1;
            }

            switch (tag) {

                case GRD_CALL_FUNC:
                    ArithData.column = j;
                    ArithData.value1 = zin;
                    ArithData.value_output = nullvalue;
                    (*funcptr)(&ArithData);
                    if (ArithData.abort_flag == 1) {
                        grd_utils_ptr->grd_set_err (98);
                        return -1;
                    }
                    ArithData.firstnode = 0;
                    zout = ArithData.value_output;
                    break;

                case GRD_ADD:
                    zout = zin + constant;
                    break;

                case GRD_SUBTRACT:
                    zout = zin - constant;
                    break;

                case GRD_CONSTANT_MINUS:
                    zout = constant - zin;
                    break;

                case GRD_MULTIPLY:
                    zout = zin * constant;
                    break;

                case GRD_DIVIDE:
                    zout = zin / constant;
                    break;

                case GRD_RECIPROCAL:
                    if (zin != 0.0f) {
                        zout = constant / zin;
                    }
                    else {
                        zout = nullvalue;
                    }
                    break;

                case GRD_EXPONENT:
                    if (zin < 0.0f) {
                        zout = nullvalue;
                    }
                    else {
                        zout = (CSW_F)pow ((double)zin, (double)constant);
                    }
                    break;

                case GRD_POWER:
                    zout = (CSW_F)pow ((double)constant, (double)zin);
                    break;

                case GRD_LOG:
                    if (zin > 0.0f) {
                        zout = (CSW_F)log ((double)zin);
                        zout /= logmult;
                    }
                    else {
                        zout = nullvalue;
                    }
                    break;

                case GRD_MINIMUM:
                    zout = zin;
                    if (zin < constant) zout = constant;
                    break;

                case GRD_MAXIMUM:
                    zout = zin;
                    if (zin > constant) zout = constant;
                    break;

                default:
                    grd_utils_ptr->grd_set_err (4);
                    return -1;

            }

            gout[offset+j] = zout;

        }

    }

    return 1;

}  /*  end of function grd_one_grid_arith  */






/*
  ***********************************************************************************

                  g r d _ t w o _ g r i d _ a r i t h

  ***********************************************************************************

    Do an arithmetic operation between two grids.  Each grid is resampled into work
  grids corresponding to the rectangular intersection of the two grids.  The spacing
  in the resampled grid is the average of the two input grid spacings.  The operation
  can be a predefined operation or a work function can be specified that will do
  the operation.

    The work function is a void function with the following parameters.

        void (*funcptr) (GRidArithData *data_pointer)

    The client data specified in grd_two_grid_arith is passed on to the work
  function as a member of the GRidArithData structure.

*/

int CSWGrdArith::grd_two_grid_arith (CSW_F *grid1, char *mask1,
                        CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                        int ncol1, int nrow1,
                        FAultLineStruct *faults1, int nfaults1,
                        CSW_F *grid2, char *mask2,
                        CSW_F x21, CSW_F y21, CSW_F x22, CSW_F y22,
                        int ncol2, int nrow2,
                        FAultLineStruct *faults2, int nfaults2,
                        CSW_F nullvalue,
                        CSW_F **gridout, char **maskout,
                        CSW_F *x1out, CSW_F *y1out,
                        CSW_F *x2out, CSW_F *y2out, int *ncout, int *nrout,
                        int tag, void (*funcptr)(GRidArithData*),
                        void *client_data)
{
    int      istat, i, j, k, ncol, nrow, offset;
    CSW_F    xmin, ymin, xmax, ymax, z1, z2, zout, xspace, yspace;
    CSW_F    *gw1 = NULL, *gw2 = NULL, *gw3 = NULL;
    char     *mw1 = NULL, *mw2 = NULL, *mw3 = NULL;
    int      samegeom;


    auto fscope = [&]()
    {
        csw_Free (gw1);
        csw_Free (mw1);
        csw_Free (gw3);
        csw_Free (mw3);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Check for obvious errors in the input parameters.
 */
    if (grid1 == NULL  ||  grid2 == NULL) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (ncol1 < 2  ||  nrow1 < 2  ||  ncol2 < 2  ||  nrow2 < 2) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

    if (tag < 1  ||  tag > 10) {
        if (tag != 999) {
            grd_utils_ptr->grd_set_err (4);
            return -1;
        }
    }

    if (gridout == NULL  ||  x1out == NULL  ||  y1out == NULL  ||
        x2out == NULL  ||  y2out == NULL  ||  ncout == NULL  ||
        nrout == NULL) {
        grd_utils_ptr->grd_set_err (6);
        return -1;
    }

/*
    Make sure funcptr and tag agree.
*/
    if (tag == GRD_CALL_FUNC  &&  funcptr == NULL) {
        grd_utils_ptr->grd_set_err (8);
        return -1;
    }

/*
 * Check if the two input grid geometries are nearly identical.
 */
    samegeom =
      grd_utils_ptr->grd_compare_geoms (
        x11, y11, x12, y12, ncol1, nrow1,
        x21, y21, x22, y22, ncol2, nrow2);

/*
 *  Get the grid intersection if the input geometries are not identical.
 */
    if (samegeom == 0) {
        istat = IntersectGeometry (x11, y11, x12, y12, ncol1, nrow1,
                                   x21, y21, x22, y22, ncol2, nrow2,
                                   &xmin, &ymin, &xmax, &ymax, &ncol, &nrow);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (2);
            return -1;
        }
    }

/*
 * Arbitrarily use the grid 1 geometry if the input grids
 * have identical geometries.
 */
    else {
        xmin = x11;
        ymin = y11;
        xmax = x12;
        ymax = y12;
        ncol = ncol1;
        nrow = nrow1;
    }


/*
    Allocate workspace memory.
*/
    if (ncol < 2) ncol = 2;
    if (nrow < 2) nrow = 2;
    *gridout = NULL;
    *maskout = NULL;
    i = ncol * nrow;

MSL
    gw1 = (CSW_F *)csw_Malloc (i * 2 * sizeof(CSW_F));
    if (!gw1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    gw2 = gw1 + i;

MSL
    mw1 = (char *)csw_Malloc (i * 2 * sizeof(char));
    if (!mw1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    mw2 = mw1 + i;
    memset (mw1, 0, i*2*sizeof(char));

/*
    Allocate output memory.
*/
MSL
    gw3 = (CSW_F *)csw_Malloc (i * sizeof(CSW_F));
    if (!gw3) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
MSL
    mw3 = (char *)csw_Malloc (i * sizeof(char));
    if (!mw3) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    memset (mw3, 0, i*sizeof(char));

/*
    If the program is in parameter error simulation mode,
    return success (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

/*
    If the samegeom flag is zero,
    resample each input grid into the work grids.
*/
    if (samegeom == 0) {
        if (faults1  &&  nfaults1 > 0) {
            istat = grd_fault_ptr->grd_define_fault_vectors (faults1, nfaults1);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            grd_fault_ptr->grd_set_fault_resample_geometry (1, xmin, ymin, xmax, ymax,
                                             ncol, nrow);
            istat = grd_fault_ptr->grd_build_fault_indices (grid1, ncol1, nrow1,
                                             x11, y11, x12, y12);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            for (i=0; i<ncol*nrow; i++) {
                gw1[i] = 1.e30f;
            }
            istat = grd_fault_ptr->grd_resample_faulted_grid (gw1, ncol, nrow,
                                               xmin, ymin, xmax, ymax,
                                               GRD_BILINEAR, 1);
            grd_fault_ptr->grd_set_fault_resample_geometry (0, xmin, ymin, xmax, ymax,
                                             ncol, nrow);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            if (mask1  &&  mw1) {
                istat = grd_fault_ptr->grd_resample_faulted_mask (mask1, mw1, ncol, nrow,
                                                   xmin, ymin, xmax, ymax);
                if (istat == -1) {
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
            }
            istat = grd_fault_ptr->grd_define_fault_vectors (faults1, nfaults1);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            istat = grd_fault_ptr->grd_build_fault_indices (gw1, ncol, nrow,
                                             xmin, ymin, xmax, ymax);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            istat = grd_fault_ptr->grd_fill_faulted_nulls (1.e19f);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
        }
        else {
            ResampleGrid (grid1, mask1, x11, y11, x12, y12, ncol1, nrow1,
                          gw1, mw1, xmin, ymin, xmax, ymax, ncol, nrow,
                          GRD_BILINEAR);
        }

        if (faults2  &&  nfaults2 > 0) {
            istat = grd_fault_ptr->grd_define_fault_vectors (faults2, nfaults2);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            grd_fault_ptr->grd_set_fault_resample_geometry (1, xmin, ymin, xmax, ymax,
                                             ncol, nrow);
            istat = grd_fault_ptr->grd_build_fault_indices (grid2, ncol2, nrow2,
                                             x21, y21, x22, y22);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            for (i=0; i<ncol*nrow; i++) {
                gw2[i] = 1.e30f;
            }
            istat = grd_fault_ptr->grd_resample_faulted_grid (gw2, ncol, nrow,
                                               xmin, ymin, xmax, ymax,
                                               GRD_BILINEAR, 1);
            grd_fault_ptr->grd_set_fault_resample_geometry (0, xmin, ymin, xmax, ymax,
                                             ncol, nrow);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            if (mask1  &&  mw1) {
                istat = grd_fault_ptr->grd_resample_faulted_mask (mask2, mw2, ncol, nrow,
                                                   xmin, ymin, xmax, ymax);
                if (istat == -1) {
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
            }
            istat = grd_fault_ptr->grd_define_fault_vectors (faults2, nfaults2);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            istat = grd_fault_ptr->grd_build_fault_indices (gw2, ncol, nrow,
                                             xmin, ymin, xmax, ymax);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            istat = grd_fault_ptr->grd_fill_faulted_nulls (1.e19f);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
        }
        else {
            ResampleGrid (grid2, mask2, x21, y21, x22, y22, ncol2, nrow2,
                          gw2, mw2, xmin, ymin, xmax, ymax, ncol, nrow,
                          GRD_BILINEAR);
        }

    }

/*
 * The input grids have the same geometry, so copy them into the
 * work grids without resampling.
 */
    else {
        memcpy (gw1, grid1, ncol * nrow * sizeof(CSW_F));
        memcpy (gw2, grid2, ncol * nrow * sizeof(CSW_F));
        if (mw1  &&  mask1) {
            memcpy (mw1, mask1, ncol * nrow * sizeof(char));
        }
        if (mw2  &&  mask2) {
            memcpy (mw2, mask2, ncol * nrow * sizeof(char));
        }
    }

/*
    Fill in the members of the ArithData structure that do  not
    change from node to node.  This structure is sent to the
    application supplied operation function if such a function
    is specified.
*/
    ArithData.type = GRD_TWO_GRID_ARITH;
    ArithData.grid1 = gw1;
    ArithData.grid2 = gw2;
    ArithData.x1 = xmin;
    ArithData.y1 = ymin;
    ArithData.x2 = xmax;
    ArithData.y2 = ymax;
    ArithData.ncol = ncol;
    ArithData.nrow = nrow;
    ArithData.constant = 1.e30f;
    ArithData.nullvalue = nullvalue;
    ArithData.client_data = client_data;
    ArithData.firstnode = 1;
    ArithData.lastnode = 0;
    ArithData.abort_flag = 0;

    yspace = (ymax - ymin) / (nrow - 1);
    xspace = (xmax - xmin) / (ncol - 1);

/*
    Loop through the common grid geometry, applying the operation
    on each node.
*/
    for (i=0; i<nrow; i++) {

        ArithData.row = i;
        ArithData.y = ymin + i * yspace;
        offset = i * ncol;

        for (j=0; j<ncol; j++) {

            k = offset + j;

        /*
            If both input masks are valid at a node, then the output
            is valid.  If either node is invalid outside, then the
            output is invalid outside.  If one is valid and the other
            is invalid inside, the output is invalid inside.

            The check for non null mw1 is inserted to suppress a bogus lint warning.
        */
            mw3[k] = 0;
            if (mw1) {
                if (mw1[k] == 3  ||  mw2[k] == 3) {
                    mw3[k] = 3;
                }
                else if (mw1[k] == 1  ||  mw2[k] == 1) {
                    mw3[k] = 1;
                }
                else if (mw1[k] == 2  &&  mw2[k] == 2) {
                    mw3[k] = 2;
                }
                else if (mw1[k] == 4  &&  mw2[k] == 4) {
                    mw3[k] = 4;
                }
                else if (mw1[k] == 2  &&  mw2[k] == 0) {
                    mw3[k] = 2;
                }
                else if (mw1[k] == 4  &&  mw2[k] == 0) {
                    mw3[k] = 4;
                }
                else if (mw1[k] == 4  &&  mw2[k] == 2) {
                    mw3[k] = 4;
                }
                else if (mw1[k] == 0  &&  mw2[k] == 2) {
                    mw3[k] = 2;
                }
                else if (mw1[k] == 0  &&  mw2[k] == 4) {
                    mw3[k] = 4;
                }
                else if (mw1[k] == 2  &&  mw2[k] == 4) {
                    mw3[k] = 4;
                }
            }

            z1 = gw1[k];
            if (z1 >= nullvalue) {
                gw3[k] = nullvalue;
                continue;
            }
            z2 = gw2[k];
            if (z2 >= nullvalue) {
                gw3[k] = nullvalue;
                continue;
            }

            if (i == nrow-1  &&  j == ncol-1) {
                ArithData.lastnode = 1;
            }

            switch (tag) {

                case GRD_REPLACE_NULLS:
                    if (z1 >= nullvalue) {
                        zout = z2;
                    }
                    else {
                        zout = z1;
                    }
                    break;

                case GRD_CALL_FUNC:
                    ArithData.column = j;
                    ArithData.x = xmin + j * xspace;
                    ArithData.value1 = z1;
                    ArithData.value2 = z2;
                    ArithData.value_output = nullvalue;
                    (*funcptr)(&ArithData);
                    if (ArithData.abort_flag == 1) {
                        *gridout = NULL;
                        if (maskout) *maskout = NULL;
                        grd_utils_ptr->grd_set_err (98);
                        return -1;
                    }
                    zout = ArithData.value_output;
                    ArithData.firstnode = 0;
                    break;

                case GRD_ADD:
                    zout = z1 + z2;
                    break;

                case GRD_SUBTRACT:
                    zout = z1 - z2;
                    break;

                case GRD_MULTIPLY:
                    zout = z1 * z2;
                    break;

                case GRD_DIVIDE:
                    if (z2 == 0.0f) {
                        zout = nullvalue;
                    }
                    else {
                        zout = z1 / z2;
                    }
                    break;

                case GRD_EXPONENT:
                    if (z1 < 0.0f) {
                        zout = nullvalue;
                    }
                    else {
                        zout = (CSW_F)pow ((double)z1, (double)z2);
                    }
                    break;

                case GRD_MINIMUM:
                    zout = z1;
                    if (z2 < z1) zout = z2;
                    break;

                case GRD_MAXIMUM:
                    zout = z1;
                    if (z2 > z1) zout = z2;
                    break;

                default:
                    grd_utils_ptr->grd_set_err (4);
                    return -1;

            }

            gw3[k] = zout;

        }

    }

    *gridout = gw3;
    if (maskout) {
        *maskout = mw3;
    }
    else {
        csw_Free (mw3);
    }
    *x1out = xmin;
    *y1out = ymin;
    *x2out = xmax;
    *y2out = ymax;
    *ncout = ncol;
    *nrout = nrow;

    return 1;

}  /*  end of function grd_two_grid_arith  */




/*
  ****************************************************************************

                  I n t e r s e c t G e o m e t r y

  ****************************************************************************

    Calculate the bounds and number of columns and rows for the intersection
  of two grids.  If there is no intersecting area, -1 is returned.  Otherwise,
  the results are returned in the xmin, ymin, xmax, ymax, ncol and nrow
  pointers.

*/

int CSWGrdArith::IntersectGeometry (CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                              int ncol1, int nrow1,
                              CSW_F x21, CSW_F y21, CSW_F x22, CSW_F y22,
                              int ncol2, int nrow2,
                              CSW_F *xmin, CSW_F *ymin, CSW_F *xmax, CSW_F *ymax,
                              int *ncol, int *nrow)
{
    CSW_F    xsp1, ysp1, xsp2, ysp2, xt1, yt1, xt2, yt2;

/*
    Find bounds of intersection.
*/
    xt1 = x11;
    if (x21 > x11) xt1 = x21;
    yt1 = y11;
    if (y21 > y11) yt1 = y21;
    xt2 = x12;
    if (x22 < x12) xt2 = x22;
    yt2 = y12;
    if (y22 < y12) yt2 = y22;

    if (xt1 >= xt2  ||  yt1 >= yt2) {
        return -1;
    }

/*
    Calculate average spacings.
*/
    xsp1 = (x12 - x11) / (CSW_F)(ncol1 - 1);
    ysp1 = (y12 - y11) / (CSW_F)(nrow1 - 1);
    xsp2 = (x22 - x21) / (CSW_F)(ncol2 - 1);
    ysp2 = (y22 - y21) / (CSW_F)(nrow2 - 1);

    xsp1 = (xsp1 + xsp2) / 2.0f;
    ysp1 = (ysp1 + ysp2) / 2.0f;

/*
    determine output columns and rows
*/
    *ncol = (int) ((xt2 - xt1) / xsp1 + 1.5f);
    *nrow = (int) ((yt2 - yt1) / ysp1 + 1.5f);
    if (*ncol < 2) *ncol = 2;
    if (*nrow < 2) *nrow = 2;

    *xmin = xt1;
    *ymin = yt1;
    *xmax = xt2;
    *ymax = yt2;

    return 1;

}  /*  end of private IntersectGeometry function  */




/*
  ******************************************************************

                   R e s a m p l e G r i d

  ******************************************************************

    Create a new grid by interpolating the old grid at the new grid's
  node locations.

*/

int CSWGrdArith::ResampleGrid (CSW_F *grid, char *mask, CSW_F x1, CSW_F y1,
                         CSW_F x2, CSW_F y2, int ncol, int nrow,
                         CSW_F *gout, char *maskout, CSW_F x1out, CSW_F y1out,
                         CSW_F x2out, CSW_F y2out,
                         int ncout, int nrout, int flag)
{
    int        istat, i, j, im, jm, offset, koff;
    int        expandflag, km;
    CSW_F      *zw = NULL, y0, ysp, xsp, ymsp, xmsp, tiny, eps;
    CSW_F      *xw = NULL, *yw = NULL;

    xw = x_maxc;
    yw = y_maxc;

/*
    Shift the input and output x and y limits of the grids
    to be relative to the lower left corner of the input grid.
    This improves the precision in the bicubic or bilinear
    interpolation used for resampling.
*/
    x2 -= x1;
    y2 -= y1;
    x1out -= x1;
    y1out -= y1;
    x2out -= x1;
    y2out -= y1;
    x1 = 0.0f;
    y1 = 0.0f;

/*
    If the input and output grid geometries are identical,
    copy the input grid and mask to the output and return.
*/
    istat = grd_utils_ptr->grd_compare_geoms (
        x1, y1, x2, y2, ncol, nrow,
        x1out, y1out, x2out, y2out, ncout, nrout);
    if (istat == 1) {
        csw_memcpy ((char *)gout, (char *)grid, ncol * nrow * sizeof(CSW_F));
        if (mask &&  maskout) {
            csw_memcpy (maskout, mask, ncol * nrow * sizeof(char));
        }
        return 1;
    }

/*
    Node spacing of output grid.
*/
    ysp = (y2out - y1out) / (CSW_F)(nrout - 1);
    xsp = (x2out - x1out) / (CSW_F)(ncout - 1);
    ymsp = (y2 - y1) / (CSW_F)(nrow - 1);
    xmsp = (x2 - x1) / (CSW_F)(ncol - 1);
    tiny = (xmsp + ymsp) / 2000.0f;

    eps = (x2 - x1 + y2 - y1) / 200000.0f;

    im = 0;
    jm = 0;
    offset = 0;
    koff = 0;
    expandflag = 0;
    if (x1out < x1-eps  ||  x2out > x2+eps  ||
        y1out < y1-eps  ||  y2out > y2+eps) {
        expandflag = 1;
    }

/*
    Interpolate a row at a time.
*/
    for (i=0; i<nrout; i++) {

        y0 = y1out + i * ysp;
        zw = gout + i * ncout;
        if (mask &&  maskout) {
            im = (int) ((y0 + tiny) / ymsp);
            offset = im * ncol;
            koff = i * ncout;
        }

        for (j=0; j<ncout; j++) {
            yw[j] = y0;
            xw[j] = x1out + j * xsp;
            if (mask  &&  maskout) {
                if (y0 < y1  ||  y0 > y2) {
                    maskout[koff+j] = 1;
                }
                else {
                    if (xw[j] < x1  ||  xw[j] > x2) {
                        maskout[koff+j] = 1;
                    }
                    else {
                        jm = (int) ((xw[j] + tiny) / xmsp);
                        km = offset + jm;
                        maskout[koff+j] = mask[km];
                    }
                }
            }
        }

        istat = 1;
        if (flag == GRD_BILINEAR) {
            istat = grd_utils_ptr->grd_bilin_interp (xw, yw, zw, ncout,
                              grid, ncol, nrow, 1,
                              x1, y1, x2, y2);
        }
        else if (flag == GRD_BICUBIC) {
            istat = grd_utils_ptr->grd_bicub_interp (xw, yw, zw, ncout, (CSW_F)1.e19f,
                              grid, ncol, nrow, 1,
                              x1, y1, x2, y2, -1, -1);
        }
        else {
            grd_utils_ptr->grd_step_interp (xw, yw, zw, ncout,
                             grid, ncol, nrow,
                             x1, y1, x2, y2);
        }

        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

    }

    if (expandflag == 0) {
        return 1;
    }

    return istat;

}  /*  end of private ResampleGrid function  */






/*
  ****************************************************************

              g r d _ r e s a m p l e _ g r i d

  ****************************************************************

    This is the public grid resampling function.  All this function
  does is a bunch of parameter error checking and calls the private
  ResampleGrid function.

    This is not a documented part of the SurfaceWorks API.  The actual
  API function is grd_ResampleGrid, located in grd_api.c.  That file
  has detailed documentation of the function parameters.

*/

int CSWGrdArith::grd_resample_grid (CSW_F *grid, char *mask, int ncol, int nrow,
                       CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                       FAultLineStruct *faults, int nfaults,
                       CSW_F *newgrid, char *newmaskin, int newncol, int newnrow,
                       CSW_F newx1, CSW_F newy1, CSW_F newx2, CSW_F newy2,
                       int flag)
{
    char        *newmask;
    int         i, istat, samegeom;

    faults = faults;
    nfaults = nfaults;

/*
    check for parameter errors
*/
    if (!grid  ||  !newgrid) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (mask &&  !newmaskin) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    newmask = newmaskin;
    if (!mask) {
        if (newmaskin) {
            memset ((char *)newmaskin, 0, newncol * newnrow * sizeof(char));
        }
        newmask = NULL;
    }

    if (x1 >= x2  ||  y1 >= y2  ||
        newx1 >= newx2  ||  newy1 >= newy2) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    if (ncol < 2  ||  nrow < 2  ||
        newncol < 2  ||  newnrow < 2) {
        grd_utils_ptr->grd_set_err (6);
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
 * Check if the two input grid geometries are nearly identical.
 */
    samegeom =
      grd_utils_ptr->grd_compare_geoms (
        x1, y1, x2, y2, ncol, nrow,
        newx1, newy1, newx2, newy2, newncol, newnrow);

    if (samegeom == 0) {
        if (faults  &&  nfaults > 0) {
            istat = grd_fault_ptr->grd_define_fault_vectors (faults, nfaults);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            grd_fault_ptr->grd_set_fault_resample_geometry (1, newx1, newy1, newx2, newy2,
                                             newncol, newnrow);
            istat = grd_fault_ptr->grd_build_fault_indices (grid, ncol, nrow,
                                             x1, y1, x2, y2);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            for (i=0; i<newncol*newnrow; i++) {
                newgrid[i] = 1.e30f;
            }
            istat = grd_fault_ptr->grd_resample_faulted_grid (newgrid, newncol, newnrow,
                                               newx1, newy1, newx2, newy2,
                                               flag, 1);
            if (istat == -1) {
                grd_fault_ptr->grd_set_fault_resample_geometry (0, newx1, newy1, newx2, newy2,
                                                 newncol, newnrow);
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            if (mask  &&  newmask) {
                istat = grd_fault_ptr->grd_build_fault_indices (grid, ncol, nrow,
                                                 x1, y1, x2, y2);
                if (istat == -1) {
                    grd_fault_ptr->grd_set_fault_resample_geometry (0, newx1, newy1, newx2, newy2,
                                                     newncol, newnrow);
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
                istat = grd_fault_ptr->grd_resample_faulted_mask (mask, newmask, newncol, newnrow,
                                                   newx1, newy1, newx2, newy2);
                if (istat == -1) {
                    grd_fault_ptr->grd_set_fault_resample_geometry (0, newx1, newy1, newx2, newy2,
                                                     newncol, newnrow);
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
            }
            grd_fault_ptr->grd_set_fault_resample_geometry (0, newx1, newy1, newx2, newy2,
                                             newncol, newnrow);
            return 1;
        }
    }

    ResampleGrid (grid, mask, x1, y1, x2, y2, ncol, nrow,
                  newgrid, newmask, newx1, newy1, newx2, newy2,
                  newncol, newnrow, flag);

    return 1;

}  /*  end of function grd_resample_grid  */






/*
  ****************************************************************

            g r d _ b a c k _ i n t e r p o l a t e

  ****************************************************************

    Estimate the elevations at a set of x,y points based on
  bicubic or bilinear interpolation of the grid specified.
  If a point is outside the grid or if any of the nodes nearest
  the point are greater than 1.e20 or less than -1.e20, then
  the z value of the point is returned as 1.e30.

    This is not the SurfaceWorks API function.  An application
  should always call grd_BackInterpolate (from grd_api.c) rather
  than calling this directly.  The header for the API function
  documents all of the parameters.

*/

int CSWGrdArith::grd_back_interpolate (CSW_F *grid, int ncol, int nrow,
                          CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                          FAultLineStruct *faults, int nfaults,
                          CSW_F *x, CSW_F *y, CSW_F *z, int npts,
                          int flag)
{
    int             faultflag, istat;

/*
    check for parameter errors
*/
    if (!grid  ||  !x  ||  !y  ||  !z) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (ncol < 2  ||  nrow < 2) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (x1 >= x2  ||  y1 >= y2) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    if (npts < 1) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return success (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

    faultflag = 0;
    if (flag < 0) {
        faultflag = 1;
        flag = -flag;
    }

/*
    Set up the faults if needed.
*/
    if (faults  &&  nfaults > 0) {
        istat = grd_fault_ptr->grd_define_fault_vectors (faults, nfaults);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        istat = grd_fault_ptr->grd_build_fault_indices (grid, ncol, nrow,
                                         x1, y1, x2, y2);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        faultflag = 1;
    }

/*
    do either bilinear or bicubic interpolation.
*/
    istat = 1;
    if (flag == GRD_BILINEAR) {
        if (faultflag == 1) {
            istat = grd_fault_ptr->con_faulted_bilin_interp_2 (grid, ncol, nrow, (CSW_F)1.e20, -1,
                                        x, y, z, npts);
        }
        else {
            istat = grd_utils_ptr->grd_bilin_interp (x, y, z, npts,
                              grid, ncol, nrow, 1,
                              x1, y1, x2, y2);
        }
    }
    else if (flag == GRD_BICUBIC) {
        if (faultflag == 1) {
            istat = grd_fault_ptr->con_faulted_bicub_interp_2 (grid, ncol, nrow, (CSW_F)1.e20, -1,
                                        x, y, z, npts);
        }
        else {
            istat = grd_utils_ptr->grd_bicub_interp (x, y, z, npts, (CSW_F)1.e20f,
                              grid, ncol, nrow, 1,
                              x1, y1, x2, y2, -1, -1);
        }
    }
    else {
        grd_utils_ptr->grd_step_interp (x, y, z, npts,
                         grid, ncol, nrow,
                         x1, y1, x2, y2);
    }

    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
    }
    return istat;

}  /*  end of function grd_back_interpolate  */






/*
  ****************************************************************

          g r d _ h o r i z o n t a l _ g r a d i e n t

  ****************************************************************

    Calculate the slope and uphill direction at each node in a grid.
  The slope is based on the elevation chage over the average x and
  y grid spacing.  The direction grid is in degrees ccw from the
  positive x axis.  All directions are between 0 and 360 degrees.

*/

int CSWGrdArith::grd_horizontal_gradient (CSW_F *grid, int ncol, int nrow,
                             CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                             FAultLineStruct *faults, int nfaults,
                             CSW_F *slope_grid, CSW_F *direction_grid)
{
    int              istat, i, j, i1, i2, j1, j2, off0, off1, off2;
    CSW_F            dx, dy, dt, rad2deg, xspace, yspace,
                     savg, xfact, yfact;

    faults = faults;
    nfaults = nfaults;

/*
    Check for obvious errors.
*/
    if (!grid  ||  !slope_grid  ||  !direction_grid) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }
    if (ncol < 2  ||  nrow < 2) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }
    if (x1 >= x2  ||  y1 >= y2) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

/*
    If any grid nodes are > 1.e20 or < -1.e20,
    I assume they are hard nulls and abort the calculation.
*/
    for (i=0; i<ncol*nrow; i++) {
        if (grid[i] > 1.e20f  ||  grid[i] < -1.e20f) {
            grd_utils_ptr->grd_set_err (5);
            return -1;
        }
    }

/*
    If the program is in parameter error simulation mode,
    return success (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

/*
    Call the faulted version if needed.
*/
    if (faults  &&  nfaults > 0) {
        istat = grd_fault_ptr->grd_define_fault_vectors (faults, nfaults);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        istat = grd_fault_ptr->grd_build_fault_indices (grid, ncol, nrow,
                                         x1, y1, x2, y2);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        istat = grd_fault_ptr->grd_faulted_hgrad (slope_grid, direction_grid);
        return istat;
    }

/*
    First, get x and y spacing and the ratio between
    each and the average spacing.
*/
    xspace = (x2 - x1) / (ncol - 1);
    yspace = (y2 - y1) / (nrow - 1);
    savg = xspace + yspace;
    xfact = xspace / savg;
    yfact = yspace / savg;

    rad2deg = 180.0f / 3.141926f;

/*
    For each node, use the nodes above, below, left and
    right of it to calculate the slope and direction.
*/
    for (i=0; i<nrow; i++) {

        i1 = i - 1;
        i2 = i + 1;
        if (i1 < 0) i1 = 0;
        if (i2 > nrow-1) i2 = nrow-1;

        off0 = i * ncol;
        off1 = i1 * ncol;
        off2 = i2 * ncol;

        for (j=0; j<ncol; j++) {

            j1 = j - 1;
            j2 = j + 1;
            if (j1 < 0) j1 = 0;
            if (j2 > ncol-1) j2 = ncol-1;

            dx = grid[off0+j2] - grid[off0+j1];
            if (j1 == j) dx *= 2.0f;
            if (j2 == j) dx *= 2.0f;
            dy = grid[off2+j] - grid[off1+j];
            if (i1 == i) dy *= 2.0f;
            if (i2 == i) dy *= 2.0f;

            dx *= xfact;
            dy *= yfact;

            dt = dx * dx + dy * dy;
            slope_grid[off0+j] = (CSW_F)sqrt ((double)dt);

            dt = (CSW_F)atan2 ((double)dy, (double)dx);
            if (dt < 0.0f) dt += 6.2831852f;
            dt *= rad2deg;
            direction_grid[off0+j] = dt;

        }

    }

    return 1;

}  /*  end of function grd_horizontal_gradient  */
