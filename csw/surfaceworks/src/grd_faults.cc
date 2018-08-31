/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_faults.cc

    This file has functions used to deal with faults in grids.
    The fault vector data are kept here and various functions that
    need to use this data are also here.  The application should
    not call these functions directly.  The API for faulted grids
    is in the file named grd_api.c, along with the rest of the
    SurfaceWorks gridding API.

    Some functions used for contouring faulted grids are also
    located here.  The common functionality of faulting is more
    important than the gridding/contouring separation so I did not
    choose to put the faulted contour functions in a separate file.

*/

#include <assert.h>
#include <memory>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"
#include "csw/utils/private_include/simulP.h"

#include "csw/utils/private_include/gpf_calcdraw.h"
#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/ply_protoP.h"
#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_graph.h"
#include "csw/utils/private_include/csw_fileio.h"

#include "csw/surfaceworks/include/con_shared_structs.h"

#include "csw/surfaceworks/private_include/grd_fault.h"




void CSWGrdFault::grd_set_debug_color_val (double val)
{
    val = val;
}


int CSWGrdFault::grd_print_checks (void)
{
    printf ("\n");
    printf ("Ncheck1 = %d    Ncheck2 = %d\n", Ncheck1, Ncheck2);
    printf ("Ncheck3 = %d    Ncheck4 = %d\n", Ncheck3, Ncheck4);
    printf ("Ncheck5 = %d    Ncheck6 = %d\n", Ncheck5, Ncheck6);
    printf ("Ncheck7 = %d    Ncheck8 = %d\n", Ncheck7, Ncheck8);
    printf ("\n");
    Ncheck1 = 0;
    Ncheck2 = 0;
    Ncheck3 = 0;
    Ncheck4 = 0;
    Ncheck5 = 0;
    Ncheck6 = 0;
    Ncheck7 = 0;
    Ncheck8 = 0;

    return 1;
}

int CSWGrdFault::grd_set_faulted_contour_level (CSW_F value)
{
    value = value;
    return 1;
}


void CSWGrdFault::grd_set_fix_fault_crossing_flag (int ival)
{
    FixCrossingFlag = ival;
}




/*
  ****************************************************************************

                 g r d _ s e t _ f a u l t _ o p t i o n

  ****************************************************************************

*/

int CSWGrdFault::grd_set_fault_option (int tag, int ival, CSW_F fval, char *cval)
{

    fval = fval;
    cval = cval;

    switch (tag) {

        case CON_CONTOUR_IN_FAULTS:
            NvContourFlag = ival;
            break;

        default:
            break;

    }

    return 1;

}  /*  end of function grd_set_fault_option  */





/*
  ****************************************************************

          g r d _ d e f i n e _ f a u l t _ v e c t o r s

  ****************************************************************

    Copy the fault line structures into the local arrays for them.
  When the fault indices are built, the faults are clipped to the
  area of interest, polygon faults are unioned and other needed
  processing is done.

*/

int CSWGrdFault::grd_define_fault_vectors (FAultLineStruct *faults, int nfaults)
{
    int                  istat;
    int                  do_write;
    CSW_F                *x, *y;
    int                  i, nlines, *npts, *linetypes;
    int                  *nc = p_i2000;

    do_write = csw_GetDoWrite ();
    if (do_write) {
        istat =
            grd_fault_structs_to_arrays (faults, nfaults,
                                         &x, &y, NULL,
                                         &npts, &linetypes,
                                         &nlines);
        if (istat == -1) {
            return -1;
        }
        if (nlines > 1000) nlines = 1000;
        for (i=0; i<nlines; i++) {
            nc[i] = 1;
        }
        grd_fileio_ptr->grd_write_float_lines (x, y, NULL,
                        nlines, nc, npts,
                        (char *)"defined_faults.xyz");
        csw_Free (x);
        csw_Free (npts);
    }

    XFaultShift = 0.0;
    YFaultShift = 0.0;
    grd_free_faults ();
    istat = CopyFaultLinesToOrig (faults, nfaults);

    return istat;

}  /*  end of function grd_define_fault_vectors  */





/*
  ****************************************************************

                  g r d _ f r e e _ f a u l t s

  ****************************************************************

    Free the static pointers used to store fault data.

*/

int CSWGrdFault::grd_free_faults (void)
{

    if (FaultVectors) {
        csw_Free (FaultVectors);
        FaultVectors = NULL;
    }

    if (FaultVectorsUsed) {
        csw_Free (FaultVectorsUsed);
        FaultVectorsUsed = NULL;
    }

    grd_free_fault_indices ();

    grd_free_fault_line_structs (OrigFaultLines, NumOrigFaultLines);
    OrigFaultLines = NULL;
    NumOrigFaultLines = 0;

    Ncol = 0;
    Nrow = 0;
    NumFaultVectors = 0;
    MaxFaultVectors = 0;

    return 1;

}  /*  end of function grd_free_faults  */




/*
  ****************************************************************

          g r d _ f r e e _ f a u l t _ i n d i c e s

  ****************************************************************

    Free the static pointers used to store fault indexes.

*/

int CSWGrdFault::grd_free_fault_indices (void)
{

    if (ColumnCrossings)
        csw_Free (ColumnCrossings);

    if (ClosestFault)
        csw_Free (ClosestFault);

    if (IndexLink)
        csw_Free (IndexLink);

    if (IndexGrid1)
        csw_Free (IndexGrid1);

    if (EndPointList)
        csw_Free (EndPointList);

    FreeCrossingElevations ();

    ColumnCrossings = NULL;
    RowCrossings = NULL;
    NodeGrazing = NULL;
    ClosestFault = NULL;

    IndexLink = NULL;
    IndexGrid1 = NULL;
    EndPointList = NULL;

    Ncol = 0;
    Nrow = 0;

    return 1;

}  /*  end of function grd_free_fault_indices  */





/*
  ****************************************************************

          g r d _ c h e c k _ f a u l t _ b l o c k i n g

  ****************************************************************

    Check if the specified line segment intersects any fault vectors.
  If it does, return a status of 1 and set the weight accordingly.
  If it does not, return a status of zero and set the weight to 1.0f.

*/

int CSWGrdFault::grd_check_fault_blocking (int irow, int jcol, CSW_F x2, CSW_F y2,
                              CSW_F *weight)
{
    int            i, j, k, istat, level, offset;
    int            r1, r2, c1, c2, c2sav, itmp;
    CSW_F          x1, y1, d1, d2;
    double         slope, bint, x, y;
    CSW_F          xint, yint;

Ncheck1++;

/*
    Set the returned weight to 1.0 in case no fault blocking is found.
*/
    if (weight) {
        *weight = 1.0f;
    }

/*
    First, calculate the node coordinates and the "level"
    of distance between the node and the point.
*/
    x1 = Xmin + jcol * Xspace;
    y1 = Ymin + irow * Yspace;

    if (x2 >= x1)
        d1 = (x2 - x1) / Xspace + .01f;
    else
        d1 = (x1 - x2) / Xspace + .01f;
    if (y2 >= y1)
        d2 = (y2 - y1) / Yspace + .01f;
    else
        d2 = (y1 - y2) / Yspace + .01f;

    if (d1 > d2) {
        level = (int)d1;
    }
    else {
        level = (int)d2;
    }
/*
    level++;
*/

    ncalls++;

    k = irow * Ncol + jcol;
    if (level < ClosestFault[k]) {
        ntimes++;
        if (ntimes % nmod == 0) {
            ntimes = ntimes;
            ncalls = ncalls;
        }
        return 0;
    }

    if (NodeGrazing[k] == 1) {
        AdjustForFaultGrazing (&x1, &y1);
    }

/*
    Find the intersection between the line and any faults in the area.
*/
    if (weight) {
        *weight = 0.0f;
    }
/*
    Calculate the starting and ending rows and columns
    traversed by this vector.
*/
    r1 = (int)((y1 - Ymin) / Yspace);
    r2 = (int)((y2 - Ymin) / Yspace);
    if (r1 < 0) r1 = 0;
    if (r2 > Nrow-1) r2 = Nrow - 1;
    if (r1 > r2) {
        itmp = r1;
        r1 = r2;
        r2 = itmp;
    }

    c1 = (int)((x1 - Xmin) / Xspace);
    c2 = (int)((x2 - Xmin) / Xspace);
    if (c1 < 0) c1 = 0;
    if (c2 > Ncol-1) c2 = Ncol - 1;

    memset (FaultVectorsUsed, 0, NumFaultVectors * sizeof(char));
    FaultVectorUsedValue = 1;

/*
    Nearly horizontal vector with both endpoints in
    the same row.
*/
    if (r1 == r2) {
        if (c1 > c2) {
            itmp = c1;
            c1 = c2;
            c2 = itmp;
        }

        offset = r1 * Ncol;
        for (j=c1; j<=c2; j++) {
            k = offset + j;
            istat = con_find_fault_intersection (x1, y1, x2, y2, k, 1,
                                                 &xint, &yint);
            if (istat >= 1) {
                memset (FaultVectorsUsed, 0, NumFaultVectors * sizeof(char));
                FaultVectorUsedValue = 0;
                return 1;
            }
        }
    }

/*
    Nearly vertical vector all in same column.
*/
    else if (c1 == c2) {
        if (r1 > r2) {
            itmp = r1;
            r1 = r2;
            r2 = itmp;
        }

        for (j=r1; j<=r2; j++) {
            k = j * Ncol + c1;
            istat = con_find_fault_intersection (x1, y1, x2, y2, k, 1,
                                                 &xint, &yint);
            if (istat >= 1) {
                FaultVectorUsedValue = 0;
                memset (FaultVectorsUsed, 0, NumFaultVectors * sizeof(char));
                return 1;
            }
        }
    }

/*
    general case traversing multiple columns and rows
*/
    else {

        slope = (y2 - y1) / (x2 - x1);
        bint = y1 - slope * x1;

        y = (double)y1;
        if (y2 < y1) y = (double)y2;
        x = (y - bint) / slope;
        c1 = (int)((x - Xmin) / Xspace);

        for (i=r1; i<=r2; i++) {
            y = Ymin + (i + 1) * Yspace;
            if (i == r2) {
                y = y2;
                if (y1 > y2) y = y1;
            }
            x = (y - bint) / slope;
            c2 = (int)((x - Xmin) / Xspace);
            c2sav = c2;
            if (c1 > c2) {
                itmp = c1;
                c1 = c2;
                c2 = itmp;
            }
            if (c1 < 0) c1 = 0;
            if (c2 >= Ncol) c2 = Ncol - 1;
            offset = i * Ncol;

            for (j=c1; j<=c2; j++) {
                k = offset + j;
                istat = con_find_fault_intersection (x1, y1, x2, y2, k, 1,
                                                     &xint, &yint);
                if (istat >= 1) {
                    FaultVectorUsedValue = 0;
                    memset (FaultVectorsUsed, 0, NumFaultVectors * sizeof(char));
                    return 1;
                }
            }
            c1 = c2sav;
        }

    }  /* end of general case if block  */

    if (weight) {
        *weight = 1.0f;
    }

    memset (FaultVectorsUsed, 0, NumFaultVectors * sizeof(char));
    FaultVectorUsedValue = 0;

    return 0;

}  /*  end of function grd_check_fault_blocking  */







/*
  ****************************************************************

     g r d _ c h e c k _ g r i d _ f a u l t _ b l o c k i n g

  ****************************************************************

    Given the column and row of a grid node and the column and row
  of a second grid node, check if the grid cell edges connecting
  the two nodes are intersected by any fault vectors.  If there
  is no intersection, return zero and weight = 1.0.  If there is
  an intersection, return 1 and weight = 0.0.

*/

int CSWGrdFault::grd_check_grid_fault_blocking (int jcol, int irow,
                                   int jcol2, int irow2, CSW_F *weight)
{
    int           istat, k, d1, d2;
    CSW_F         xt, yt;

Ncheck2++;

/*
    Always return zero if the start and end nodes
    are identical.
*/
    if (jcol == jcol2  &&  irow == irow2) {
        *weight = 1.0f;
        return 0;
    }

/*
    Make sure both nodes are inside the grid.
*/
    *weight = 0.0f;
    if (jcol < 0  ||  jcol >= Ncol)
        return -1;
    if (jcol2 < 0  ||  jcol2 >= Ncol)
        return -1;
    if (irow < 0  ||  irow >= Nrow)
        return -1;
    if (irow2 < 0  ||  irow2 >= Nrow)
        return -1;

/*
 * If the second node grazes a fault, return 2.
 */
    k = irow2 * Ncol + jcol2;
    if (NodeGrazing[k] == 1) {
        return 2;
    }

/*
    If the "distance" between the nodes is smaller
    than the closest fault distance, return 0 and weight = 1.0.
*/
    d1 = irow2 - irow;
    if (d1 < 0)
        d1 = -d1;

    d2 = jcol2 - jcol;
    if (d2 < 0)
        d2 = -d2;

    d1++;
    d2++;

    k = irow * Ncol + jcol;
    if (ClosestFault[k] > d1  &&  ClosestFault[k] > d2) {
        *weight = 1.0f;
        return 0;
    }

    k = irow2 * Ncol + jcol2;
    if (ClosestFault[k] > d1  &&  ClosestFault[k] > d2) {
        *weight = 1.0f;
        return 0;
    }

/*
    Set the weight to zero in case there is a fault
    between the two nodes.
*/
    *weight = 0.0f;

/*
 * Bug 9255
 *  Always check the line connecting the nodes for a fault crossing.
 *  The row and column crossing indices are not reliable enough in
 *  grazing cases.
 */
    xt = jcol2 * Xspace + Xmin;
    yt = irow2 * Yspace + Ymin;
    istat = grd_check_fault_blocking (irow, jcol, xt, yt, NULL);
    if (istat != 0) {
        return 1;
    }

/*
    No fault intersection was found, so return zero and weight = 1.0f.
*/
    *weight = 1.0f;

    return 0;

}  /*  end of function grd_check_grid_fault_blocking  */





/*
  ****************************************************************************

              g r d _ s e t _ f a u l t _ z l i m i t s

  ****************************************************************************

*/

int CSWGrdFault::grd_set_fault_zlimits (CSW_F z1, CSW_F z2)
{

    Zmin = z1;
    Zmax = z2;

    return 1;

}  /*  end of function grd_set_fault_zlimits  */




/*
 *****************************************************************

   g r d _ s e t _ f a u l t _ r e s a m p l e _ g e o m e t r y

 *****************************************************************

  If faulted resampling of a grid is done, the output geometry is
  defined here.  This is needed to make sure that the faults do
  not graze nodes in either the previous or new grids.

  To completely disable the resample checking, set flag to zero.
  To enable the checking and use the specified geometry, set
  flag to non zero.

  This should be called prior to calling grd_build_fault_indices
  for it to have the desired effect.

*/

int CSWGrdFault::grd_set_fault_resample_geometry (int flag,
                                     CSW_F x1, CSW_F y1,
                                     CSW_F x2, CSW_F y2,
                                     int nc, int nr)
{
    if (flag == 0) {
        ResampleFlag = 0;
        NewXmin = 1.e30f;
        NewYmin = 1.e30f;
        NewXmax = -1.e30f;
        NewYmax = -1.e30f;
        NewNcol = -1;
        NewNrow = -1;
        NewXspace = -1.0f;
        NewYspace = -1.0f;
        return 1;
    }

    ResampleFlag = 1;
    NewXmin = x1;
    NewYmin = y1;
    NewXmax = x2;
    NewYmax = y2;
    NewNcol = nc;
    NewNrow = nr;
    NewXspace = (x2 - x1) / (CSW_F)(nc - 1);
    NewYspace = (y2 - y1) / (CSW_F)(nr - 1);

    NewXmax = NewXmax;
    NewYmax = NewYmax;
    NewNcol = NewNcol;
    NewNrow = NewNrow;

    return 1;

}  /* end of function grd_set_fault_resample_geometry */



void CSWGrdFault::grd_set_fault_index_extent (int extent)
{
    FaultIndexExtent = 2 * extent;
    if (FaultIndexExtent < 3) FaultIndexExtent = 3;
}


/*
  ****************************************************************

          g r d _ b u i l d _ f a u l t _ i n d i c e s

  ****************************************************************

    Calculate the nearest fault distance table, the column crossing table,
  and the row crossing table for fault vectors.

*/

int CSWGrdFault::grd_build_fault_indices (CSW_F *grid, int ncol, int nrow,
                             CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2)
{
    int            istat, i, j, k, offset;
    CSW_F          xt, yt;
    double         dsav;

    bool           bsuccess = false;    

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            grd_free_faults ();
            csw_Free (ColumnCrossings);
            ColumnCrossings = NULL;
            RowCrossings = NULL;
            NodeGrazing = NULL;
            csw_Free (ClosestFault);
            ClosestFault = NULL;
            csw_Free (CellCrossings);
            CellCrossings = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);



/*
    Clean up memory from any previous fault indices.
*/
    grd_free_fault_indices ();

/*
    Save the grid geometry in static file variables.
*/
    Grid = grid;
    Ncol = ncol;
    Nrow = nrow;
    Xmin = x1;
    Ymin = y1;
    Xmax = x2;
    Ymax = y2;

    if (Grid) {
        Zmin = 1.e30f;
        Zmax = -1.e30f;
        for (i=0; i<ncol*nrow; i++) {
            if (Grid[i] < Zmin) Zmin = Grid[i];
            if (Grid[i] > Zmax) Zmax = Grid[i];
        }
    }

    Xspace = (x2 - x1) / (CSW_F)(ncol - 1);
    Yspace = (y2 - y1) / (CSW_F)(nrow - 1);
    Tiny = (Xspace + Yspace) / 500.0f;
    Tiny2 = (Xspace + Yspace) / 200.0f;
    Tiny2 *= Tiny2;
    CellSize = Xspace + Yspace;

    EqualTiny = Tiny / 10.0f;
    ExtendTiny = Tiny / 10.0f;

    NodeOffset = (double)(x2 - x1 + y2 - y1) / 2000000.0;

    if (FaultVectors == NULL) {

    /*
        Union the original fault polygons if needed.
    */
        UnionFaults ();

    /*
        Break the faults into individual vectors.
    */
        SeparateFaultVectors ();

    /*
     * Save the fault vectors before extending and clipping.
     */
        SaveOriginalFaultVectors ();

        grd_extend_faults_to_margins (
            OrigXmin,
            OrigYmin,
            OrigXmax,
            OrigYmax,
            MarginXmin,
            MarginYmin,
            MarginXmax,
            MarginYmax);
    }

/*
    Clip the fault vectors to the grid that is being calculated
    or contoured.
*/
    ClipFaultsToGrid ();

/*
    If any index arrays are not NULL, csw_Free them.
*/
    if (ColumnCrossings) {
        csw_Free (ColumnCrossings);
        ColumnCrossings = NULL;
        RowCrossings = NULL;
        NodeGrazing = NULL;
    }
    if (ClosestFault) {
        csw_Free (ClosestFault);
        ClosestFault = NULL;
    }

/*
    Allocate space for fault index grids.
*/
    ColumnCrossings = (char *)csw_Calloc (3*ncol*nrow*sizeof(char));
    if (!ColumnCrossings) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    RowCrossings = ColumnCrossings + ncol * nrow;
    NodeGrazing = RowCrossings + ncol * nrow;

    ClosestFault = (int *)csw_Calloc (ncol*nrow*sizeof(int));
    if (!ClosestFault) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Make sure horizontal and vertical fault vectors
    are not very nearly on grid lines.
*/
    AdjustProblemFaults ();

/*
    Build the various index grids.
*/
    istat = BuildCellIndex ();
    if (istat == -1) {
        return -1;
    }
    BuildColumnIndex ();
    BuildRowIndex ();
    BuildDistanceTable ();

    if (CellCrossings) csw_Free (CellCrossings);
    CellCrossings = (char *)csw_Calloc (ncol*nrow*sizeof(char));
    if (!CellCrossings) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    for (i=0; i<ncol*nrow; i++) {
        if (IndexGrid1[i] != -1) {
            CellCrossings[i] = (char)1;
        }
        else {
            CellCrossings[i] = (char)0;
        }
    }

/*
    If any node is almost directly on top of a fault,
    set the NodeGrazing array and make sure all 4 cells
    surrounding the node have the CellCrossings flag set
    to 1.
*/
    dsav = NodeOffset;
    NodeOffset *= 10.0;
    for (i=0; i<ncol*nrow; i++) {
        if (ClosestFault[i] > 0) {
            NodeGrazing[i] = 0;
        }
        else {
            xt = (i % ncol) * Xspace + Xmin;
            yt = (i / nrow) * Yspace + Ymin; /*lint !e653*/
            istat = CheckForFaultGrazing (xt, yt);
            if (istat >= 1) {
                NodeGrazing[i] = 1;
            }
            else {
                NodeGrazing[i] = 0;
            }
        }
    }
    NodeOffset = dsav;

    for (i=0; i<nrow-1; i++) {
        offset = ncol * i;
        for (j=0; j<ncol-1; j++) {
            k = offset + j;
            if (NodeGrazing[k] == 0) continue;
            if (j > 0) {
                if (CellCrossings[k-1] == 0) {
                    CellCrossings[k-1] = -1;
                }
            }
            if (j < ncol-1) {
                if (CellCrossings[k+1] == 0) {
                    CellCrossings[k+1] = -1;
                }
            }
            if (i > 0  &&  j > 0) {
                if (CellCrossings[k-ncol-1] == 0) {
                    CellCrossings[k-ncol-1] = -1;
                }
            }
            if (i > 0  &&  j < ncol-1) {
                if (CellCrossings[k-ncol+1] == 0) {
                    CellCrossings[k-ncol+1] = -1;
                }
            }
        }
    }

/*
    Calculate fault elevations where each fault crosses a row
    or column.  These are used when interpolating the surface
    in cells intersected by faults to insure that the same
    interpolation scheme is used regardless of how a contour
    enters or exits the grid cell.
*/
    if (ElevIndex) csw_Free (ElevIndex);
    ElevIndex = (int *)csw_Malloc (3 * ncol * nrow * sizeof(int));
    if (ElevIndex == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    ElevIndex2 = ElevIndex + ncol * nrow;
    EndPointIndex = ElevIndex2 + ncol * nrow;

    for (i=0; i<ncol*nrow; i++) {
        ElevIndex[i] = -1;
        ElevIndex2[i] = -1;
        EndPointIndex[i] = -1;
    }

    MaxElevList = INDEX_CHUNK;
    NumElevList = 0;
    ElevList = (CRossingElevation **)csw_Malloc
               (MaxElevList * sizeof(CRossingElevation*));
    if (ElevList == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    istat = SetupRowCrossingElevations ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = SetupColumnCrossingElevations ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    istat = BuildEndPointIndex ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    bsuccess = true;

    return 1;

}  /*  end of function grd_build_fault_indices  */




/*
  ****************************************************************

                B u i l d D i s t a n c e T a b l e

  ****************************************************************

  Fill in the ClosestFault array with the l1 distance to the
  nearest fault.  The distance is based on a square of cells
  around each grid point.  If the nearest point is in the square
  that lies 5 grid cells away, the distance assigned is 5.  This
  is not a euclidean distance.  The distance is only used to
  speed up searching for fault vectors.

  This is only called from grd_build_fault_indices.  The row and
  column crossing arrays must have been built prior to calling
  this function.

*/

int CSWGrdFault::BuildDistanceTable (void)
{
    int           i, j, k, ii, kk, ncm1, nrm1, done, istat,
                  offset, offset2, start, maxii, i1, i2, j0, j2;
    CSW_F         xt, yt;

/*
    The maximum distance is from the FaultIndexExtent
*/
    maxii = FaultIndexExtent + 1;
    ncm1 = Ncol - 1;
    nrm1 = Nrow - 1;

/*
    loop through each row and column
*/
    start = 0;
    offset = 0;

    for (i=0; i<Nrow; i++) {

    /*
        for all but the first row, use the value of the
        first column in the previous row as a starting point
    */
        if (i > 0)     {
            start = *(ClosestFault + offset) - 1;
            if (start < 0)
                start = 0;
        }
        offset = i * Ncol;

        for (j=0; j<Ncol; j++) {

        /*
            Initialize the distance at this node to the
            maximum in case no faults are found near the node.
        */
            ClosestFault[offset+j] = maxii;

        /*
            check if there are any points in the square of cells
            start distance from the grid node.  Increment the distance
            until a point is found or until the maximum distance is reached.
        */
            done = 0;
            if (start < 0) start = 0;
            for (ii=start; ii<maxii; ii++) {


            /*
                indices of the corners of a square ii grid cells
                from the node
            */
                j0 = j - ii - 1;
                if (j0 < 0) j0 = 0;
                i1 = i - ii - 1;
                if (i1 < 0) i1 = 0;
                j2 = ii+j+1;
                if (j2 > ncm1) j2 = ncm1;
                i2 = ii+i+1;
                if (i2 > nrm1) i2 = nrm1;

            /*
                check the top row of the square
            */
                offset2 = i2 * Ncol;
                for (k=j0; k<=j2; k++) {
                    kk = offset2 + k;
                    if (RowCrossings[kk] == 1  ||  ColumnCrossings[kk] == 1) {
                        ClosestFault[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;

            /*
                check the bottom row of the square
            */
                offset2 = i1 * Ncol;
                for (k=j0; k<=j2; k++) {
                    kk = offset2 + k;
                    if (RowCrossings[kk] == 1  ||  ColumnCrossings[kk] == 1) {
                        ClosestFault[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;

            /*
                check the left side of the square
            */
                for (k=i1+1; k<i2; k++) {
                    kk = k * Ncol + j0;
                    if (RowCrossings[kk] == 1  ||  ColumnCrossings[kk] == 1) {
                        ClosestFault[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;

            /*
                check the right side of the square
            */
                for (k=i1+1; k<i2; k++) {
                    kk = k * Ncol + j2;
                    if (RowCrossings[kk] == 1  ||  ColumnCrossings[kk] == 1) {
                        ClosestFault[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;

                if (ii > 1) continue;

            /*
                Check for node grazing at this level.
            */
                j2 = j + ii + 1;
                if (j2 > ncm1) j2 = ncm1;
                i2 = i + ii + 1;
                if (i2 > nrm1) i2 = nrm1;

                yt = i1 * Yspace + Ymin;
                for (k=j0; k<=j2; k++) {
                    xt = k * Xspace + Xmin;
                    istat = CheckForFaultGrazing (xt, yt);
                    if (istat > 0) {
                        ClosestFault[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;

                yt = i2 * Yspace + Ymin;
                for (k=j0; k<=j2; k++) {
                    xt = k * Xspace + Xmin;
                    istat = CheckForFaultGrazing (xt, yt);
                    if (istat > 0) {
                        ClosestFault[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;

                xt = j0 * Xspace + Xmin;
                for (k=i1; k<=i2; k++) {
                    yt = k * Yspace + Ymin;
                    istat = CheckForFaultGrazing (xt, yt);
                    if (istat > 0) {
                        ClosestFault[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;

                xt = j2 * Xspace + Xmin;
                for (k=i1; k<=i2; k++) {
                    yt = k * Yspace + Ymin;
                    istat = CheckForFaultGrazing (xt, yt);
                    if (istat > 0) {
                        ClosestFault[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;
            }
        }
    }

    return 1;

}  /*  end of private BuildDistanceTable function  */






/*
  ****************************************************************

               B u i l d C o l u m n I n d e x

  ****************************************************************

   Build a char array representing the vertical grid cell edges
  in the grid.  If an edge is crossed by a fault vector, it is set
  to a non zero value.  If not crossed, it is set to zero.

*/

int CSWGrdFault::BuildColumnIndex (void)
{
    int              i, k, jj, r1, r2, c1, c2;
    CSW_F            x1, y1, x2, y2;

/*
    Loop through each fault line.
*/
    for (i=0; i<NumFaultVectors; i++) {

        if (FaultVectors[i].flag == OUTSIDE_GRID_FLAG) {
            continue;
        }

        x1 = FaultVectors[i].x1;
        y1 = FaultVectors[i].y1;
        x2 = FaultVectors[i].x2;
        y2 = FaultVectors[i].y2;

        ExtendVector (&x1, &y1, &x2, &y2, -1.0f);

        if (x2 - x1 == 0.0)
            x2 += Tiny;
        if (y2 - y1 == 0.0)
            y2 += Tiny;

        c1 = (int)((x1 - Xmin) / Xspace);
        if (x1-Xmin < 0.0) c1--;
        c2 = (int)((x2 - Xmin) / Xspace);
        if (x2-Xmin < 0.0) c2--;

    /*
        If both end points of the vector are in the same column,
        there are no vertical edge intersections with this vector.
    */
        if (c1 == c2  &&  c1 != 0  &&  c1 != Ncol-1) {
            continue;
        }

        r1 = (int)((y1 - Ymin) / Yspace);
        if (y1-Ymin < 0.0) r1--;
        r2 = (int)((y2 - Ymin) / Yspace);
        if (y2-Ymin < 0.0) r2--;

    /*
        If both end points are in the same row, simply set all
        the columns in that row between the end points.
    */
        if (r1 == r2) {
            if (c1 > c2) {
                k = c1;
                c1 = c2;
                c2 = k;
            }
            c1++;
            if (c1 < 0) c1 = 0;
            if (c1 > Ncol-1) c1 = Ncol - 1;
            if (c2 < 0) c2 = 0;
            if (c2 > Ncol-1) c2 = Ncol - 1;
            k = r1 * Ncol;
            for (jj=c1; jj<=c2; jj++) {
                ColumnCrossings[k+jj] = 1;
            }
            continue;
        }

    /*
        The general case traverses both rows and columns and
        is done here.
    */
        SetColumnCrossingForVector (x1, y1, x2, y2, i);

    }

    return 1;

}  /*  end of private BuildColumnIndex function  */






/*
  ****************************************************************

       S e t C o l u m n C r o s s i n g F o r V e c t o r

  ****************************************************************

    Set the vertical grid cell edge intersections for a non horizontal
  and non vertical vector.  This is only called from BuildColumnIndex.

*/

int CSWGrdFault::SetColumnCrossingForVector (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                                       int n)
{
    int              c1, c2, i, j, k;
    CSW_F            slope, bint, xt1, yt1;

/*
    The n parameter is for future use.
*/
    n = n;

/*
    Calculate the slope and y intercept of the line.
*/
    slope = (y2 - y1) / (x2 - x1);
    bint = y1 - slope * x1;

    c1 = (int)((x1 - Xmin) / Xspace);
    if (x1 < Xmin) c1--;
    c2 = (int)((x2 - Xmin) / Xspace);
    if (x2 < Xmin) c2--;
    if (c1 > c2) {
        k = c1;
        c1 = c2;
        c2 = k;
    }
    c1++;
    if (c1 < 0) c1 = 0;
    if (c2 > Ncol - 1) c2 = Ncol - 1;

    for (i=c1; i<=c2; i++) {
        xt1 = i * Xspace + Xmin;
        yt1 = slope * xt1 + bint;
        j = (int)((yt1 - Ymin) / Yspace);
        if (j < 0  ||  j >= Nrow) continue;
        k = j * Ncol + i;
        ColumnCrossings[k] = 1;
    }

    return 1;

}  /*  end of private SetColumnCrossingForVector function  */






/*
  ****************************************************************

                    B u i l d R o w I n d e x

  ****************************************************************

   Build a char array representing the horizontal grid cell edges
  in the grid.  If an edge is crossed by a fault vector, it is set
  to a non zero value.  If not crossed, it is set to zero.

*/

int CSWGrdFault::BuildRowIndex (void)
{
    int              i, k, jj, r1, r2, c1, c2;
    CSW_F            x1, y1, x2, y2;

/*
    Loop through each fault line.
*/
    for (i=0; i<NumFaultVectors; i++) {

        if (FaultVectors[i].flag == OUTSIDE_GRID_FLAG) {
            continue;
        }

        x1 = FaultVectors[i].x1;
        y1 = FaultVectors[i].y1;
        x2 = FaultVectors[i].x2;
        y2 = FaultVectors[i].y2;

        ExtendVector (&x1, &y1, &x2, &y2, -1.0f);

        if (x2 - x1 == 0.0)
            x2 += Tiny;
        if (y2 - y1 == 0.0)
            y2 += Tiny;

        r1 = (int)((y1 - Ymin) / Yspace);
        if (y1-Ymin < 0.0) r1--;
        r2 = (int)((y2 - Ymin) / Yspace);
        if (y2-Ymin < 0.0) r2--;

        if (r1 > r2) {
            k = r1;
            r1 = r2;
            r2 = k;
        }

    /*
     * bug 9132
     *  The r1++ that was here should not have been.  It was only needed
     * if the entire vector was in a single column and it screws things
     * up if it is not used only for that purpose.
     */
        if (r1 < 0)
            r1 = 0;
        if (r1 > Nrow - 1)
            r1 = Nrow - 1;
        if (r2 < 0)
            r2 = 0;
        if (r2 > Nrow - 1)
            r2 = Nrow - 1;

    /*
        If both end points of the vector are in the same row,
        there are no horizontal edge intersections with this vector.
    */
        if (r1 == r2  &&  r1 != 0  &&  r1 != Nrow-1) {
            continue;
        }

        c1 = (int)((x1 - Xmin) / Xspace);
        if (x1-Xmin < 0.0) c1--;
        c2 = (int)((x2 - Xmin) / Xspace);
        if (x2-Xmin < 0.0) c2--;

    /*
        If both end points are in the same column, simply set all
        the rows in that column between the end points.
    */
        if (c1 == c2) {

        /*
         * bug 9132
         *  The r1++ that used to be above here should have been in
         *  this new position.
         */
            r1++;
            for (jj=r1; jj<=r2; jj++) {
                k = jj * Ncol;
                RowCrossings[k+c1] = 1;
            }
            continue;
        }

    /*
        The general case traverses both rows and columns and
        is done here.
    */
        SetRowCrossingForVector (x1, y1, x2, y2);

    }

    return 1;

}  /*  end of private BuildRowIndex function  */






/*
  ****************************************************************

           S e t R o w C r o s s i n g F o r V e c t o r

  ****************************************************************

    Set the horizontal grid cell edge intersections for a non horizontal
  and non vertical vector.  This is only called from BuildRowIndex.

*/

int CSWGrdFault::SetRowCrossingForVector (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2)
{
    int              r1, r2, i, j, k;
    CSW_F            slope, bint, xt1, yt1;

/*
    Calculate the slope and y intercept of the line.
*/
    slope = (y2 - y1) / (x2 - x1);
    bint = y1 - slope * x1;

    r1 = (int)((y1 - Ymin) / Yspace);
    if (y1 < Ymin) r1--;
    r2 = (int)((y2 - Ymin) / Yspace);
    if (y2 < Ymin) r2--;
    if (r1 > r2) {
        k = r1;
        r1 = r2;
        r2 = k;
    }
    r1++;
    if (r1 < 0) r1 = 0;
    if (r2 > Nrow - 1) r2 = Nrow - 1;

    for (i=r1; i<=r2; i++) {
        yt1 = i * Yspace + Ymin;
        xt1 = (yt1 - bint) / slope;
        j = (int)((xt1 - Xmin) / Xspace);
        if (j < 0  ||  j >= Ncol) continue;
        k = i * Ncol + j;
        RowCrossings[k] = 1;
    }

    return 1;

}  /*  end of private SetRowCrossingForVector function  */






/*
  ****************************************************************

           g r d _ f a u l t _ c h e c k _ n e e d e d

  ****************************************************************

    Check the closest fault array in all directions nc nodes from the
  specified row and column.  If any of the closest points are greater
  than twice the nc value, then no points in the area can be faulted
  out from any point or node in the area.

*/

int CSWGrdFault::grd_fault_check_needed (int i, int j, int nc)
{

    if (ClosestFault[i*Ncol+j] > nc) {
        return 0;
    }

    return 1;

}  /*  end of function grd_fault_check_needed  */






/*
  ****************************************************************

       g r d _ i n t e r p _ i n _ f a u l t e d _ a r e a

  ****************************************************************

    Estimate the elevation at points near a fault by doing a
  weighted average of nodes surrounding the point and on the
  same side of the fault as the point.

*/

int CSWGrdFault::grd_interp_in_faulted_area (CSW_F *x, CSW_F *y, CSW_F *z,
                                int irow, int jcol,
                                int nlist, CSW_F *grid, int nc)
{
    int           k;

    k = irow * Ncol + jcol;

    Ncoarse = nc;

    if (ClosestFault[k] > nc-1) {
        grd_faulted_bicub_interp (grid, Ncol, Nrow, 1.e20f,
                                  x[0], y[0], irow, jcol,
                                  x, y, z, nlist);
    }
    else {
        grd_faulted_bicub_interp_2 (grid, Ncol, Nrow, 1.e20f, -1,
                                    x, y, z, nlist);
    }

    return 1;

}  /*  end of function grd_interp_in_faulted_area  */







/*
  ****************************************************************

       g r d _ g e t _ c l o s e s t _ f a u l t _ p t r

  ****************************************************************

    Return the closest fault pointer to the grid or contour
  calculation functions so the first fault block check (is the
  point close enough to a fault to bother) is made there rather
  than via a function call.

*/

int CSWGrdFault::grd_get_closest_fault_ptr (int **ptr)
{
    *ptr = ClosestFault;
    return 1;

}  /*  end of function grd_get_closest_fault_ptr  */






/*
  ****************************************************************

              A d j u s t P r o b l e m F a u l t s

  ****************************************************************

  If a vertical or horizontal fault vector lies almost exactly
  on top of a vertical or horizontal grid line, adjust one or more
  end points to make it farther away from the grid line.

*/

int CSWGrdFault::AdjustProblemFaults (void)
{
    int              i, r1, c1;
    CSW_F            x1, y1, x2, y2, et2;

    et2 = EqualTiny * 4.0f;

/*
    Loop through each fault line.
*/
    for (i=0; i<NumFaultVectors; i++) {

        if (FaultVectors[i].flag == OUTSIDE_GRID_FLAG) {
            continue;
        }

        x1 = FaultVectors[i].x1;
        y1 = FaultVectors[i].y1;
        x2 = FaultVectors[i].x2;
        y2 = FaultVectors[i].y2;

        if (csw_NearEqual (y1, y2, EqualTiny)) {
            r1 = (int)((y1 - Ymin) / Yspace);
            if (csw_NearEqual (y1-Ymin, r1*Yspace, EqualTiny)) {
                FaultVectors[i].y1 += et2;
                FaultVectors[i].y2 += et2;
            }
            r1++;
            if (csw_NearEqual (y1-Ymin, r1*Yspace, EqualTiny)) {
                FaultVectors[i].y1 += et2;
                FaultVectors[i].y2 += et2;
            }
        }

        if (csw_NearEqual (x1, x2, EqualTiny)) {
            c1 = (int)((x1 - Xmin) / Xspace);
            if (csw_NearEqual (x1-Xmin, c1*Xspace, EqualTiny)) {
                FaultVectors[i].x1 += et2;
                FaultVectors[i].x2 += et2;
            }
            c1++;
            if (csw_NearEqual (x1-Xmin, c1*Xspace, EqualTiny)) {
                FaultVectors[i].x1 += et2;
                FaultVectors[i].x2 += et2;
            }
        }

    }

    if (ResampleFlag == 0) return 1;

/*
 * Loop through the adjusted fault vectors and make sure
 * they do not graze resampled nodes either.
 */
    for (i=0; i<NumFaultVectors; i++) {

        x1 = FaultVectors[i].x1;
        y1 = FaultVectors[i].y1;
        x2 = FaultVectors[i].x2;
        y2 = FaultVectors[i].y2;

        if (csw_NearEqual (y1, y2, EqualTiny)) {
            r1 = (int)((y1 - NewYmin) / NewYspace);
            if (csw_NearEqual (y1-NewYmin, r1*NewYspace, EqualTiny)) {
                FaultVectors[i].y1 += et2;
                FaultVectors[i].y2 += et2;
            }
            r1++;
            if (csw_NearEqual (y1-NewYmin, r1*NewYspace, EqualTiny)) {
                FaultVectors[i].y1 += et2;
                FaultVectors[i].y2 += et2;
            }
        }

        if (csw_NearEqual (x1, x2, EqualTiny)) {
            c1 = (int)((x1 - NewXmin) / NewXspace);
            if (csw_NearEqual (x1-NewXmin, c1*NewXspace, EqualTiny)) {
                FaultVectors[i].x1 += et2;
                FaultVectors[i].x2 += et2;
            }
            c1++;
            if (csw_NearEqual (x1-NewXmin, c1*NewXspace, EqualTiny)) {
                FaultVectors[i].x1 += et2;
                FaultVectors[i].x2 += et2;
            }
        }

    }

/*
 *  There still may be fault grazings from diagonal lines.
 */
    ZigZagToAvoidNodes ();

    return 1;

}  /*  end of private AdjustProblemFaults function  */




/*
  ****************************************************************

        c o n _ f a u l t e d _ b i c u b _ i n t e r p

  ****************************************************************

    Do bicubic interpolation of a faulted grid.  If the specified
  row and column is not near any fault, treat the interpolation as
  if it is being done on a non faulted grid.  If a fault is near,
  create a 4 by 4 work grid with all nodes not blocked by the
  fault from the xref, yref point.  Any nodes in the 4 by 4 work
  grid not assigned are calculated by the null extension function
  and then the interpolation is done using the 4 by 4 work grid.

    This function is only used for grid resampling, back interpolation
  and contour smoothing.  Thus, the skip factor is always 1.  The
  x and y limits of the grid must have been specified by a call to
  grd_build_fault_indices prior to calling this function.

*/

int CSWGrdFault::con_faulted_bicub_interp (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                              CSW_F xref, CSW_F yref, int irow, int jcol,
                              CSW_F *x, CSW_F *y, CSW_F *z, int npts)
{
    int              i, j, i0, k, k2, i1, i2, j1, j2,
                     node, istat, ng, nb, nofaultflag;
    CSW_F            bwork[16], wgt, x1, y1, x2, y2, xt, yt;

    NvContourFlag = NvContourFlag;

/*
    If there are less than 4 columns and 4 rows in the grid,
    no bicubic interpolation is possible.
*/
    nofaultflag = 0;
    if (ncol < 4  ||  nrow < 4)
        nofaultflag = 1;

/*
    make sure the null value is a large positive number.
*/
    if (nullval < 1.e20)
        nullval = 1.e20f;

/*
    If there is no fault nearby, use the nonfaulted interpolation function.
*/
    node = irow * Ncol + jcol;
    if (node < 0  ||  node >= Ncol * Nrow) {
        assert (0);
    }
    if (ClosestFault[node] > 2  ||  nofaultflag == 1) {
        istat = grd_utils_ptr->grd_bicub_interp (x, y, z, npts, nullval,
                                  grid, ncol, nrow, 1,
                                  Xmin, Ymin, Xmax, Ymax,
                                  -1, -1);
        return istat;
    }

/*
    Calculate rows and columns for the 4 by 4 area near irow, jcol.
*/
    i1 = irow - 1;
    i2 = irow + 2;
    j1 = jcol - 1;
    j2 = jcol + 2;

/*
    Find nodes on the same side of the fault as xref, yref.
    This is done by determining if xref, yref is on the same
    fault block as irow, jcol.  If this is true, all nodes
    on the same fault block as irow, jcol are also on the same
    fault block as xref, yref.  If this is false, all nodes
    on the same fault block as irow, jcol are not on the same fault
    block as xref, yref.
*/
    ng = 0;
    nb = 0;
    k2 = 0;
    for (i=i1; i<=i2; i++) {
        if (i < 0  ||  i > Nrow-1) {
            bwork[k2] = nullval * 10.0f;
            k2++;
            bwork[k2] = nullval * 10.0f;
            k2++;
            bwork[k2] = nullval * 10.0f;
            k2++;
            bwork[k2] = nullval * 10.0f;
            k2++;
            nb += 4;
            continue;
        }
        i0 = Ncol * i;
        yt = i * Yspace + Ymin;
        for (j=j1; j<=j2; j++) {
            if (j < 0  ||  j > Ncol-1) {
                bwork[k2] = nullval * 10.0f;
                k2++;
                nb++;
                continue;
            }
            xt = j * Xspace + Xmin;
            istat = CheckForFaultGrazing (xt, yt);
            if (istat >= 1) {
                bwork[k2] = nullval * 10.0f;
                nb++;
                k2++;
                continue;
            }
            k = i0+j;
            istat = grd_check_fault_blocking (i, j, xref, yref, &wgt);
            if (istat == 0) {
                bwork[k2] = grid[k];
                ng++;
            }
            else {
                bwork[k2] = nullval * 10.0f;
                nb++;
            }
            k2++;
        }
    }

/*
 * BUGFIX !!!!
 *
 * The bilin interpolation should be faulted here.
 */
    if (ng < 6) {
        istat = grd_faulted_bilin_interp (
                                  grid, ncol, nrow, 1.e30f,
                                  xref, yref, irow, jcol,
                                  x, y, z, npts);
        return istat;
    }

/*
    Use the fault crossing info to fill in the nodes
    in the center cell of the 4 by 4 subgrid.
*/
    FillFromCrossings (i1, i2, j1, j2, bwork);

/*
    Calculate the corners of the work grid.
*/
    x1 = j1 * Xspace + Xmin;
    y1 = i1 * Yspace + Ymin;
    x2 = j2 * Xspace + Xmin;
    y2 = i2 * Yspace + Ymin;

/*
    fill in the null and faulted out values in the work grid.
*/
    if (nb > 0) {
        istat = FillInNulls (bwork, 4, 4);
        if (istat == -1) {
            return -1;
        }
    }

/*
    The work grid can now be treated as a non faulted grid
    for interpolation purposes.  Note that the center cell
    (cell 1,1) is always used for interpolation.
*/
    istat = grd_utils_ptr->grd_bicub_interp (x, y, z, npts, nullval,
                              bwork, 4, 4, 1,
                              x1, y1, x2, y2,
                              -1, -1);

    return istat;

}  /*  end of function con_faulted_bicub_interp  */






/*
  ****************************************************************

        c o n _ f a u l t e d _ b i l i n _ i n t e r p

  ****************************************************************

    Do bilinear interpolation of a faulted grid.  If the specified
  row and column is not near any fault, treat the interpolation as
  if it is being done on a non faulted grid.  If a fault is near,
  create a 4 by 4 work grid with all nodes not blocked by the
  fault from the xref, yref point.  Any nodes in the 4 by 4 work
  grid not assigned are calculated by the null extension function
  and then the interpolation is done using the 4 by 4 work grid.

    This function is only used for grid resampling, back interpolation
  and contour smoothing.  Thus, the skip factor is always 1.  The
  x and y limits of the grid must have been specified by a call to
  grd_build_fault_indices prior to calling this function.

*/

int CSWGrdFault::con_faulted_bilin_interp (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                              CSW_F xref, CSW_F yref, int irow, int jcol,
                              CSW_F *x, CSW_F *y, CSW_F *z, int npts)
{
    int              i, j, i0, k, k2, i1, i2, j1, j2,
                     node, istat, ng, nb, nofaultflag;
    CSW_F            bwork[16], wgt, x1, y1, x2, y2, xt, yt;
    CSW_F            bw1[16], bw2[16];

    memset (bwork, 0, 16 * sizeof(CSW_F));

/*
    If there are less than 4 columns and 4 rows in the grid,
    no faulted interpolation is possible.
*/
    nofaultflag = 0;
    if (ncol < 4  ||  nrow < 4)
        nofaultflag = 1;

/*
    make sure the null value is a large positive number.
*/
    if (nullval < 1.e20)
        nullval = 1.e20f;

/*
    If there is no fault nearby, use the nonfaulted interpolation function.
*/
    node = irow * Ncol + jcol;
    if (node < 0  ||  node >= Ncol * Nrow) {
        assert (0);
    }
    if (ClosestFault[node] > 1  ||  nofaultflag == 1) {
        istat = grd_utils_ptr->grd_bilin_interp (x, y, z, npts,
                                  grid, ncol, nrow, 1,
                                  Xmin, Ymin, Xmax, Ymax);
        return istat;
    }

/*
    Calculate rows and columns for the 4 by 4 area near irow, jcol.
*/
    i1 = irow - 1;
    if (i1 < 0)
        i1 = 0;
    if (i1 > Nrow - 4)
        i1 = Nrow - 4;

    i2 = irow + 2;
    if (i2 > Nrow - 1)
        i2 = Nrow - 1;
    if (i2 < 3)
        i2 = 3;

    j1 = jcol - 1;
    if (j1 < 0)
        j1 = 0;
    if (j1 > Ncol - 4)
        j1 = Ncol - 4;

    j2 = jcol + 2;
    if (j2 > Ncol - 1)
        j2 = Ncol - 1;
    if (j2 < 3)
        j2 = 3;

/*
    Find nodes on the same side of the fault as xref, yref.
*/
    ng = 0;
    nb = 0;
    k2 = 0;
    ExtendEndPointFlag = 0;
    for (i=i1; i<i2; i++) {
        i0 = Ncol * i;
        for (j=j1; j<j2; j++) {
            k = i0 + j;
            if (EndPointIndex[k] != -1) {
                ExtendEndPointFlag = 1;
                break;
            }
        }
        if (ExtendEndPointFlag) break;
    }

    for (i=i1; i<=i2; i++) {
        i0 = Ncol * i;
        yt = i * Yspace + Ymin;
        for (j=j1; j<=j2; j++) {
            xt = j * Xspace + Xmin;
            istat = CheckForFaultGrazing (xt, yt);
            if (istat >= 1) {
                bwork[k2] = nullval * 10.0f;
                nb++;
                k2++;
                continue;
            }
            k = i0+j;
            istat = grd_check_fault_blocking (i, j, xref, yref, &wgt);
            if (istat == 0) {
                bwork[k2] = grid[k];
                ng++;
            }
            else {
                bwork[k2] = nullval * 10.0f;
                nb++;
            }
            k2++;
        }
    }
    ExtendEndPointFlag = 0;

    if (ng < 1) {
        istat = grd_utils_ptr->grd_bilin_interp (x, y, z, npts,
                                  grid, ncol, nrow, 1,
                                  Xmin, Ymin, Xmax, Ymax);
        return istat;
    }

    memcpy (bw1, bwork, 16 * sizeof(CSW_F));

/*
    Use the fault crossing info to fill in the nodes
    in the center cell of the 4 by 4 subgrid.
*/
    FillFromCrossings (i1, i2, j1, j2, bwork);

/*
    Calculate the corners of the work grid.
*/
    x1 = j1 * Xspace + Xmin;
    y1 = i1 * Yspace + Ymin;
    x2 = j2 * Xspace + Xmin;
    y2 = i2 * Yspace + Ymin;

    memcpy (bw2, bwork, 16 * sizeof(CSW_F));

/*
    fill in the null and faulted out values in the work grid.
*/
    if (nb > 0) {
        istat = FillInNulls (bwork, 4, 4);
        if (istat == -1) {
            return -1;
        }
    }


/*
    The work grid can now be treated as a non faulted grid
    for interpolation purposes.
*/
    istat = grd_utils_ptr->grd_bilin_interp (x, y, z, npts,
                              bwork, 4, 4, 1,
                              x1, y1, x2, y2);

    return istat;

}  /*  end of function con_faulted_bilin_interp  */





/*
  ****************************************************************

       g r d _ s e t _ f a u l t _ d a t a _ g r a z i n g

  ****************************************************************

    Determine if a data point is very near a fault.  In this case,
  it is impossible to determine which side of the fault the point
  lies on, so the point cannot be used in the surface calculation.
  The grazing points have their x coordinate set to 1.e33, and the
  surface calculation subsequently ignores the points.

*/

int CSWGrdFault::grd_set_fault_data_grazing (CSW_F *x, CSW_F *y, int npts)

{
    int            istat, i, ngraze, col, row, k;
    CSW_F          xt, yt;
    double         nosav;

    ngraze = 0;
    nosav = NodeOffset;

    for (i=0; i<npts; i++) {

        xt = x[i];
        yt = y[i];

        if (xt < Xmin  ||  xt > Xmax  ||
            yt < Ymin  ||  yt > Ymax) {
            continue;
        }

    /*
        If the closest fault index is greater than 2, there
        is no possibility of a grazing data point.
    */
        col = (int)((xt - Xmin) / Xspace);
        if (xt - Xmax == 0.0)
            col--;
        if (col < 0)
            continue;
        row = (int)((yt - Ymin) / Yspace);
        if (yt - Ymax == 0.0)
            row--;
        if (row < 0)
            continue;
        k = row * Ncol + col;
        if (ClosestFault[k] > 2)
            continue;

    /*
        Check each fault for grazing with the point.
    */
        istat = CheckForFaultGrazing (xt, yt);
        if (istat >= 1) {
            GrazedPoints[ngraze].x = xt;
            GrazedPoints[ngraze].n = i;
            x[i] = FAULTED_OUT_VALUE;
            ngraze++;
            if (ngraze > MAX_GRAZED_POINTS - 1) {
                ngraze = MAX_GRAZED_POINTS - 1;
            }
        }

    /*
        If there are too many grazing points, return an error.
    */
        if (ngraze >= MAX_GRAZED_POINTS) {
            NodeOffset = nosav;
            return -1;
        }

    }

    NumGrazedPoints = ngraze;
    NodeOffset = nosav;

    return 1;

}  /*  end of function grd_set_fault_data_grazing  */




/*
  ****************************************************************

     g r d _ r e s e t _ f a u l t _ d a t a _ g r a z i n g

  ****************************************************************

    Reset the original x coordinates of the grazed points.  If the
  list pointer is not null, return the indices of each point reset.
  The list is allocated by this function.

*/

int CSWGrdFault::grd_reset_fault_data_grazing (CSW_F *x, int **list, int *nlist)
{
    int          i, n, *loc = NULL;

    bool      bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (loc);
            if (nlist) {
                *nlist = 0;
            }
            if (list) {
                *list = NULL;
            }
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (NumGrazedPoints < 1) {
        return 1;
    }

    loc = NULL;
    if (list) {
        *list = NULL;
        loc = (int *)csw_Malloc (NumGrazedPoints * sizeof(int));
        if (!loc) {
            return -1;
        }
        *list = loc;
    }

    if (nlist) {
        *nlist = NumGrazedPoints;
    }

    for (i=0; i<NumGrazedPoints; i++) {
        n = GrazedPoints[i].n;
        x[n] = GrazedPoints[i].x;
        if (loc) {
            loc[i] = n;
        }
    }

    bsuccess = true;

    return 1;

}  /*  end of function grd_reset_fault_data_grazing  */





/*
  ****************************************************************************

                     C l i p F a u l t s T o G r i d

  ****************************************************************************

    Clip the FaultVector array to the grid limits currently being used.

*/

int CSWGrdFault::ClipFaultsToGrid (void)
{
    int           i, n2, i2;
    CSW_F         xy1[10], xy2[20], tiny;

    GPFCalcdraw     gpf_calcdraw_obj;

    tiny = (Xspace + Yspace) / 2000.0f;

    for (i=0; i<NumFaultVectors; i++) {
        xy1[0] = FaultVectors[i].x1;
        xy1[1] = FaultVectors[i].y1;
        xy1[2] = FaultVectors[i].x2;
        xy1[3] = FaultVectors[i].y2;
        gpf_calcdraw_obj.gpf_cliplineprim (xy1, 2,
                          Xmin-tiny, Ymin-tiny, Xmax+tiny, Ymax+tiny,
                          xy2, &n2, &i2);
        if (n2 < 1) {
            FaultVectors[i].flag = OUTSIDE_GRID_FLAG;
            continue;
        }
        FaultVectors[i].x1 = xy2[0];
        FaultVectors[i].y1 = xy2[1];
        FaultVectors[i].x2 = xy2[2];
        FaultVectors[i].y2 = xy2[3];
    }

    return 1;

}  /*  end of private ClipFaultsToGrid function  */





/*
  ****************************************************************************

               S a v e O r i g i n a l F a u l t V e c t o r s

  ****************************************************************************

  Copy the current fault vectors into the OriginalFaultVectors array.
*/

int CSWGrdFault::SaveOriginalFaultVectors (void)

{

    bool    bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (OriginalFaultVectors);
            OriginalFaultVectors = NULL;
            NumOriginalFaultVectors = 0;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    csw_Free (OriginalFaultVectors);
    OriginalFaultVectors = NULL;
    NumOriginalFaultVectors = 0;

    if (FaultVectors == NULL) {
        return 1;
    }

    OriginalFaultVectors = (FAultVector *)csw_Malloc
        (NumFaultVectors * sizeof(FAultVector));
    if (OriginalFaultVectors == NULL) {
        return -1;
    }

    memcpy (OriginalFaultVectors, FaultVectors,
            NumFaultVectors * sizeof(FAultVector));
    NumOriginalFaultVectors = NumFaultVectors;

    bsuccess = true;

    return 1;

}  /*  end of private ClipFaultsToGrid function  */





/*
  ****************************************************************************

                        B u i l d C e l l I n d e x

  ****************************************************************************

    Build the indices of which fault vectors cross grid cells.  The index
  is a grid with the start offset in the IndexLink array for the start of
  a link list of vectors crossing the cell.  There is also an end index
  grid used to quickly add a new element to the link list associated with
  each cell.

    The first vector found crossing a cell will be put into the next available
  IndexLink element.  The offset of that IndexLink element will be put into
  the IndexGrid1 element for the cell and into the IndexGrid2 element for the
  cell.  Subsequent vectors crossing the cell will be put into the next
  IndexLink element and that element will be put into the next member of the
  IndexGrid2 element.  Then, the IndexGrid2 element is changed to the offset
  of the IndexLink element just added.

*/

int CSWGrdFault::BuildCellIndex (void)
{
    int          ivec, i, j, k, n, c1, c2, r1, r2, itmp,
                 offset, c2sav;
    CSW_F        x1, y1, x2, y2;
    double       slope, bint, x, y;

    bool    bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (IndexLink);
            IndexLink = NULL;
            MaxIndexLink = 0;
            NumIndexLink = 0;
            csw_Free (IndexGrid1);
            IndexGrid1 = NULL;
            IndexGrid2 = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Allocate the initial chunk of the IndexLink array.
*/
    MaxIndexLink = 0;
    NumIndexLink = 0;
    csw_Free (IndexLink);
    IndexLink = NULL;
    csw_Free (IndexGrid1);
    IndexGrid1 = NULL;
    IndexGrid2 = NULL;

    MaxIndexLink += INDEX_CHUNK;
    IndexLink = (FAultIndexLink *)csw_Malloc (MaxIndexLink * sizeof(FAultIndexLink));
    if (!IndexLink) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Allocate space for the IndexGrid arrays.  Initialize
    all to -1, indicating no vectors are yet in the cells.
*/
    n = Ncol * Nrow;
    IndexGrid1 = (int *)csw_Malloc (n * 2 * sizeof(int));
    if (!IndexGrid1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    IndexGrid2 = IndexGrid1 + n;
    for (i=0; i<n; i++) {
        IndexGrid1[i] = -1;
        IndexGrid2[i] = -1;
    }

/*
    Loop through the fault vectors and index each of them.
*/
    for (ivec=0; ivec<NumFaultVectors; ivec++) {

        if (FaultVectors[ivec].flag == OUTSIDE_GRID_FLAG) {
            continue;
        }

        x1 = FaultVectors[ivec].x1;
        y1 = FaultVectors[ivec].y1;
        x2 = FaultVectors[ivec].x2;
        y2 = FaultVectors[ivec].y2;

        ExtendVector (&x1, &y1, &x2, &y2, -1.0f);

    /*
        Calculate the starting and ending rows and columns
        traversed by this vector.
    */
        r1 = (int)((y1 - Ymin) / Yspace);
        r2 = (int)((y2 - Ymin) / Yspace);
        if (r1 > r2) {
            itmp = r1;
            r1 = r2;
            r2 = itmp;
        }

        c1 = (int)((x1 - Xmin) / Xspace);
        c2 = (int)((x2 - Xmin) / Xspace);
        if (c1 > c2) {
            itmp = c1;
            c1 = c2;
            c2 = itmp;
        }

    /*
        Nearly horizontal vector with both endpoints in
        the same row.
    */
        if (r1 == r2  &&  r1 >= 0  &&  r1 < Nrow) {

            offset = r1 * Ncol;
            if (c1 < 0) c1 = 0;
            if (c2 > Ncol-1) c2 = Ncol - 1;
            for (j=c1; j<=c2; j++) {
                if (j < 0  ||  j >= Ncol) continue;
                k = offset + j;
                if (IndexGrid1[k] == -1) {
                    IndexGrid1[k] = NumIndexLink;
                    IndexGrid2[k] = NumIndexLink;
                }
                else {
                    IndexLink[IndexGrid2[k]].next = NumIndexLink;
                    IndexGrid2[k] = NumIndexLink;
                }
                IndexLink[NumIndexLink].findex = ivec;
                IndexLink[NumIndexLink].next = -1;
                NumIndexLink++;
                if (NumIndexLink >= MaxIndexLink) {
                    MaxIndexLink += INDEX_CHUNK;
                    IndexLink = (FAultIndexLink *)csw_Realloc
                                (IndexLink, MaxIndexLink * sizeof(FAultIndexLink));
                    if (!IndexLink) {
                        grd_utils_ptr->grd_set_err (1);
                        return -1;
                    }
                }
            }
        }

    /*
        Nearly vertical vector all in same column.
    */
        else if (c1 == c2  &&  c1 >= 0  &&  c1 < Ncol) {

            if (r1 < 0) r1 = 0;
            if (r2 > Nrow-1) r2 = Nrow - 1;
            for (j=r1; j<=r2; j++) {
                if (j < 0  ||  j >= Nrow) continue;
                k = j * Ncol + c1;
                if (IndexGrid1[k] == -1) {
                    IndexGrid1[k] = NumIndexLink;
                    IndexGrid2[k] = NumIndexLink;
                }
                else {
                    IndexLink[IndexGrid2[k]].next = NumIndexLink;
                    IndexGrid2[k] = NumIndexLink;
                }
                IndexLink[NumIndexLink].findex = ivec;
                IndexLink[NumIndexLink].next = -1;
                NumIndexLink++;
                if (NumIndexLink >= MaxIndexLink) {
                    MaxIndexLink += INDEX_CHUNK;
                    IndexLink = (FAultIndexLink *)csw_Realloc
                                (IndexLink, MaxIndexLink * sizeof(FAultIndexLink));
                    if (!IndexLink) {
                        grd_utils_ptr->grd_set_err (1);
                        return -1;
                    }
                }
            }
        }

    /*
        general case traversing multiple columns and rows
    */
        else {

            slope = (y2 - y1) / (x2 - x1);
            bint = y1 - slope * x1;

            y = (double)y1;
            if (y2 < y1) y = (double)y2;
            x = (y - bint) / slope;
            c1 = (int)((x - Xmin) / Xspace);

            for (i=r1; i<=r2; i++) {
                if (i < 0  ||  i >= Nrow) continue;
                y = Ymin + (i + 1) * Yspace;
                if (i == r2) {
                    y = y2;
                    if (y1 > y2) y = y1;
                }
                x = (y - bint) / slope;
                c2 = (int)((x - Xmin) / Xspace);
                c2sav = c2;
                if (c1 > c2) {
                    itmp = c1;
                    c1 = c2;
                    c2 = itmp;
                }
                if (c1 < 0) c1 = 0;
                if (c2 >= Ncol) c2 = Ncol - 1;
                offset = i * Ncol;

                for (j=c1; j<=c2; j++) {
                    if (j < 0  ||  j >= Ncol) continue;
                    k = offset + j;
                    if (IndexGrid1[k] == -1) {
                        IndexGrid1[k] = NumIndexLink;
                        IndexGrid2[k] = NumIndexLink;
                    }
                    else {
                        IndexLink[IndexGrid2[k]].next = NumIndexLink;
                        IndexGrid2[k] = NumIndexLink;
                    }
                    IndexLink[NumIndexLink].findex = ivec;
                    IndexLink[NumIndexLink].next = -1;
                    NumIndexLink++;
                    if (NumIndexLink >= MaxIndexLink) {
                        MaxIndexLink += INDEX_CHUNK;
                        IndexLink = (FAultIndexLink *)csw_Realloc
                                    (IndexLink, MaxIndexLink * sizeof(FAultIndexLink));
                        if (!IndexLink) {
                            grd_utils_ptr->grd_set_err (1);
                            return -1;
                        }
                    }
                }
                c1 = c2sav;
            }

        }  /* end of general case if block  */

    }  /*  end of loop through vectors  */

    bsuccess = true;

    return 1;

}  /*  end of private BuildCellIndex function  */





/*
  ****************************************************************************

                           G e t V e c t o r s

  ****************************************************************************

    Return a list of fault vector numbers that intersect the specified
  range of rows and columns.  If there are no
  vectors crossing the area, nvec is set to zero and vecnums is set to NULL.

  The only possible error is a memory allocation failure.  The VecNumWork
  array is allocated here, and it is reused whenever possible.  DO NOT FREE
  THE vecnums POINTER RETURNED!!!!
*/

int CSWGrdFault::GetVectors (int r1, int r2, int c1, int c2,
                       int **vecnums, int *nvec)

{
    int             i, j, k, offset, n, *vnum = NULL, maxv, inum;
    FAultIndexLink  *fiptr = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            if (vnum == VecNumWork) VecNumWork = NULL;
            csw_Free (vnum);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    *nvec = 0;
    *vecnums = NULL;

    if (r1 < 0) r1 = 0;
    if (r2 >= Nrow) r2 = Nrow - 1;
    if (c1 < 0) c1 = 0;
    if (c2 >= Ncol) c2 = Ncol;

    if (MaxFaultVectors < 1) {
        return 0;
    }

    if (VecNumWork == NULL) {
        VecNumWork = (int *)csw_Malloc (MaxFaultVectors * sizeof(int));
        if (VecNumWork == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }
    maxv = MaxFaultVectors;
    vnum = VecNumWork;

    n = 0;
    for (i=r1; i<=r2; i++) {
        offset = i * Ncol;
        for (j=c1; j<=c2; j++) {
            k = offset + j;
            if (IndexGrid1[k] == -1) continue;
            inum = IndexGrid1[k];
            for (;;) {
                fiptr = IndexLink + inum;

                if (FaultVectors[fiptr->findex].used == 0) {
                    vnum[n] = fiptr->findex;
                    FaultVectors[fiptr->findex].used = 1;
                    n++;
                }
                if (n >= maxv) {
                    maxv += 100;
                    vnum = (int *)csw_Realloc (vnum, maxv * sizeof(int));
                    VecNumWork = vnum;
                    if (!vnum) {
                        grd_utils_ptr->grd_set_err (1);
                        return -1;
                    }
                }
                inum = fiptr->next;
                if (inum == -1) break;
            }
        }
    }

    if (n < 1) {
        return 0;
    }

    for (i=0; i<n; i++) {
        FaultVectors[vnum[i]].used = 0;
    }

    *nvec = n;
    *vecnums = vnum;

    bsuccess = true;

    return 1;

}  /*  end of private GetVectors function  */





/*
  ****************************************************************************

             c o n _ f i n d _ f a u l t _ i n t e r s e c t i o n

  ****************************************************************************

    Find the intersection between the specified contour line segment and the
  closest fault in the specified grid cell.  If no intersection is found,
  zero is returned and xint, yint are set to 1.e30.  If an intersection is
  found, it is put in xint, yint and the number of the fault + 1 is returned.
  If a memory allocation error occurs, -1 is returned.

    If flag is set to 1, no x,y or fault number is returned.  In this case,
  a return status of 1 indicates that an intersection exists and a return
  status of zero indicates that no fault intersection exists.

*/

int CSWGrdFault::con_find_fault_intersection (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                                 int kcell, int flag,
                                 CSW_F *xint, CSW_F *yint)
{
    int            r1, r2, c1, c2, ivec, i, istat, inum;
    int            *veclist, nvec;
    CSW_F          xt1, yt1, xt2, yt2, dist, dmax, xit, yit;
    double         dx, dy, dxint, dyint, dx1, dy1, dx2, dy2;

    CSWPolyUtils   ply_utils_obj;

    *xint = 1.e30f;
    *yint = 1.e30f;
    if (FaultVectors == NULL  ||  NumFaultVectors < 1) {
        return 0;
    }

    if (kcell >= 0) {
        r1 = kcell / Ncol;
        r2 = r1+1;
        c1 = kcell % Ncol;
        c2 = c1+1;
        r1--;
        c1--;
        if (r1 < 0) r1 = 0;
        if (c1 < 0) c1 = 0;
        if (r2 > Nrow - 1) r2 = Nrow - 1;
        if (c2 > Ncol - 1) c2 = Ncol - 1;
    }
    else {
        r1 = (int)((y1 - Ymin) / Yspace);
        r2 = (int)((y2 - Ymin) / Yspace);
        if (r1 > r2) {
            i = r1;
            r1 = r2;
            r2 = i;
        }
        r2++;
        c1 = (int)((x1 - Xmin) / Xspace);
        c2 = (int)((x2 - Xmin) / Xspace);
        if (c1 > c2) {
            i = c1;
            c1 = c2;
            c2 = i;
        }
        c2++;
        if (r1 < 0) r1 = 0;
        if (c1 < 0) c1 = 0;
        if (r2 > Nrow - 1) r2 = Nrow - 1;
        if (c2 > Ncol - 1) c2 = Ncol - 1;
    }

    veclist = NULL;
    nvec = 0;
    istat = GetVectors (r1, r2, c1, c2,
                        &veclist, &nvec);
    if (istat == -1) {
        return -1;
    }
    if (nvec < 1) {
        return 0;
    }

    dx1 = x1;
    dy1 = y1;
    dx2 = x2;
    dy2 = y2;
    if (dx1 < Xmin) dx1 = Xmin;
    if (dx2 < Xmin) dx2 = Xmin;
    if (dx1 > Xmax) dx1 = Xmax;
    if (dx2 > Xmax) dx2 = Xmax;
    if (dy1 < Ymin) dy1 = Ymin;
    if (dy2 < Ymin) dy2 = Ymin;
    if (dy1 > Ymax) dy1 = Ymax;
    if (dy2 > Ymax) dy2 = Ymax;

    inum = -1;
    dmax = 1.e30f;
    for (ivec=0; ivec<nvec; ivec++) {

        i = veclist[ivec];

        if (FaultVectorsUsed[i] == 1) {
            continue;
        }

        FaultVectorsUsed[i] = (char)FaultVectorUsedValue;

        if (FaultVectors[i].flag == OUTSIDE_GRID_FLAG) {
            continue;
        }

        xt1 = FaultVectors[i].x1;
        yt1 = FaultVectors[i].y1;
        xt2 = FaultVectors[i].x2;
        yt2 = FaultVectors[i].y2;

        if (EndFlag) {
            if (FaultVectors[i].endflag1 != 0  ||
                FaultVectors[i].endflag2 != 0) {
                if (ExtendEndPointFlag == 1) {
                    ExtendVector (&xt1, &yt1, &xt2, &yt2, CellSize);
                    if (FaultVectors[i].endflag1 == 0) {
                        xt1 = FaultVectors[i].x1;
                        yt1 = FaultVectors[i].y1;
                    }
                    if (FaultVectors[i].endflag2 == 0) {
                        xt2 = FaultVectors[i].x2;
                        yt2 = FaultVectors[i].y2;
                    }
                }
            }
        }

        dxint = dx1;
        dyint = dy1;
        istat = ply_utils_obj.ply_segint ((double)xt1, (double)yt1,
                            (double)xt2, (double)yt2,
                            dx1, dy1, dx2, dy2,
                            &dxint, &dyint);

        if (!(istat == 0  ||  istat == 2  ||
              istat == 3  ||  istat == 4  ||  istat == 6)) {
            continue;
        }

        if (flag == 1) {
            return 1;
        }

        xit = (CSW_F)dxint;
        yit = (CSW_F)dyint;

        dx = x1 - xit;
        dy = y1 - yit;
        dist = (CSW_F)(dx * dx + dy * dy);
        if (dist < dmax) {
            inum = i;
            dmax = dist;
            *xint = xit;
            *yint = yit;
        }
    }

    if (inum == -1) {
        return 0;
    }

    return inum + 1;

}  /*  end of function con_find_fault_intersection  */






/*
  ****************************************************************************

            c o n _ g e t _ f a u l t _ c e l l _ c r o s s i n g s

  ****************************************************************************

    Return the pointers for the cell crossing, column crossing and
  row crossing arrays.  These are used by the contour calculations
  to make decisions about whether faulted contour handling is needed
  in a particular grid cell.

*/

int CSWGrdFault::con_get_fault_cell_crossings (char **cell, char **col, char **row,
                                  int **closest, char **nodegraze)
{

    *cell = CellCrossings;
    *col = ColumnCrossings;
    *row = RowCrossings;
    *closest = ClosestFault;
    *nodegraze = NodeGrazing;

    return 1;

}  /*  end of function con_get_fault_cell_crossings  */




/*
  ****************************************************************************

                          F i l l I n N u l l s

  ****************************************************************************

    Fill in the null values from a work grid used in the vicinity of a fault.
  When bilinear or bicubic interpolation is needed near a fault, the nodes
  on the correct side of the fault are used to fill in the rest of the work
  grid and then the interpolation is done using the filled in work grid.

*/

int CSWGrdFault::FillInNulls (CSW_F *grid, int ncol, int nrow)
{
    CSW_F              gtmp1[100], gtmp2[100], fnull, sum, z1, z2, zt;
    int                i, j, k, offset, i1, j1, n, ndone, nsize, numbad;
    CSW_F              softchk;

    fnull = 1.e19f;
    nsize = ncol * nrow * sizeof(CSW_F);

/*
    Reset any soft nulls found in the work grid to hard nulls.
*/
    softchk = (CSW_F)(GRD_SOFT_NULL_VALUE / 100.0);
    for (i=0; i<ncol*nrow; i++) {
        if (grid[i] < softchk) grid[i] = 1.e30f;
    }

    memcpy (gtmp1, grid, nsize);
    memcpy (gtmp2, grid, nsize);

/*
    First try to extrapolate gradients to fill in the null values.
*/
    for (;;) {

        ndone = 0;
        for (i=0; i<nrow; i++) {
            offset = i * ncol;
            for (j=0; j<ncol; j++) {
                k = offset + j;
                if (gtmp1[k] < fnull) continue;
                n = 0;
                sum = 0.0f;
                j1 = j - 2;
                if (j1 >= 0) {
                    z1 = gtmp1[offset+j1];
                    j1++;
                    z2 = gtmp1[offset+j1];
                    if (z1 < fnull  &&  z2 < fnull) {
                        zt = z2 - (z1 - z2);
                        sum += zt;
                        n++;
                    }
                }
                j1 = j + 2;
                if (j1 < ncol) {
                    z1 = gtmp1[k+2];
                    z2 = gtmp1[k+1];
                    if (z1 < fnull  &&  z2 < fnull) {
                        zt = z2 - (z1 - z2);
                        sum += zt;
                        n++;
                    }
                }
                i1 = i - 2;
                if (i1 >= 0) {
                    z1 = gtmp1[i1*ncol+j];
                    i1++;
                    z2 = gtmp1[i1*ncol+j];
                    if (z1 < fnull  &&  z2 < fnull) {
                        zt = z2 - (z1 - z2);
                        sum += zt;
                        n++;
                    }
                }
                i1 = i + 2;
                if (i1 < nrow) {
                    z1 = gtmp1[k+ncol*2];
                    z2 = gtmp1[k+ncol];
                    if (z1 < fnull  &&  z2 < fnull) {
                        zt = z2 - (z1 - z2);
                        sum += zt;
                        n++;
                    }
                }
                i1 = i + 2;
                j1 = j - 2;
                if (i1 < nrow  &&  j1 >= 0) {
                    z1 = gtmp1[k+ncol*2-2];
                    z2 = gtmp1[k+ncol-1];
                    if (z1 < fnull  &&  z2 < fnull) {
                        zt = z2 - (z1 - z2);
                        sum += zt;
                        n++;
                    }
                }
                i1 = i + 2;
                j1 = j + 2;
                if (i1 < nrow  &&  j1 < ncol) {
                    z1 = gtmp1[k+ncol*2+2];
                    z2 = gtmp1[k+ncol+1];
                    if (z1 < fnull  &&  z2 < fnull) {
                        zt = z2 - (z1 - z2);
                        sum += zt;
                        n++;
                    }
                }
                i1 = i - 2;
                j1 = j - 2;
                if (i1 >= 0  &&  j1 >= 0) {
                    z1 = gtmp1[i1*ncol+j1];
                    i1++;
                    j1++;
                    z2 = gtmp1[i1*ncol+j1];
                    if (z1 < fnull  &&  z2 < fnull) {
                        zt = z2 - (z1 - z2);
                        sum += zt;
                        n++;
                    }
                }
                i1 = i - 2;
                j1 = j + 2;
                if (i1 >= 0  &&  j1 < ncol && k >= ncol*2-2) {
                    z1 = gtmp1[k-ncol*2+2];
                    z2 = gtmp1[k-ncol+1];
                    if (z1 < fnull  &&  z2 < fnull) {
                        zt = z2 - (z1 - z2);
                        sum += zt;
                        n++;
                    }
                }
                if (n > 0) {
                    gtmp2[k] = sum / (CSW_F)n;
                    ndone++;
                }
            }
        }

        memcpy (gtmp1, gtmp2, nsize);

        if (ndone == 0) break;

    }

/*
    If all nodes are filled in, return.
*/
    n = 0;
    for (i=0; i<ncol*nrow; i++) {
        if (gtmp1[i] >= fnull) {
            n++;
        }
    }

    if (n == 0) {
        memcpy (grid, gtmp1, nsize);
        return 1;
    }

    numbad = n;
    if (numbad >= ncol*nrow / 2) {
        numbad = 1;
    }
    else {
        numbad = 0;
    }

/*
    Use the nearest non null neighbors for any
    nodes still not filled in.
*/
    for (;;) {

        ndone = 0;
        for (i=0; i<nrow; i++) {
            offset = i * ncol;
            for (j=0; j<ncol; j++) {
                k = offset + j;
                if (gtmp1[k] < fnull) continue;
                n = 0;
                sum = 0.0f;
                j1 = j - 1;
                if (j1 >= 0  &&  k >= 1) {
                    z2 = gtmp1[k-1];
                    if (z2 < fnull) {
                        sum += z2;
                        n++;
                    }
                }
                j1 = j + 1;
                if (j1 < ncol) {
                    z2 = gtmp1[k+1];
                    if (z2 < fnull) {
                        sum += z2;
                        n++;
                    }
                }
                i1 = i - 1;
                if (i1 >= 0  &&  k-ncol >= 0) {
                    z2 = gtmp1[k-ncol]; /*lint !e676*/ /* PC-lint 7.50y bug */
                    if (z2 < fnull) {
                        sum += z2;
                        n++;
                    }
                }
                i1 = i + 1;
                if (i1 < nrow) {
                    z2 = gtmp1[k+ncol];
                    if (z2 < fnull) {
                        sum += z2;
                        n++;
                    }
                }

            /*
                If there is a solution just using horizontal and
                vertical neighbors, do not check diagonal neighbors.
            */
                if (numbad == 1  &&  n > 0) {
                    gtmp2[k] = sum / (CSW_F)n;
                    ndone++;
                    continue;
                }
                i1 = i + 1;
                j1 = j - 1;
                if (i1 < nrow  &&  j1 >= 0) {
                    z2 = gtmp1[i1*ncol+j1];
                    if (z2 < fnull) {
                        sum += z2;
                        n++;
                    }
                }
                i1 = i + 1;
                j1 = j + 1;
                if (i1 < nrow  &&  j1 < ncol) {
                    z2 = gtmp1[i1*ncol+j1];
                    if (z2 < fnull) {
                        sum += z2;
                        n++;
                    }
                }
                i1 = i - 1;
                j1 = j - 1;
                if (i1 >= 0  &&  j1 >= 0) {
                    z2 = gtmp1[i1*ncol+j1];
                    if (z2 < fnull) {
                        sum += z2;
                        n++;
                    }
                }
                i1 = i - 1;
                j1 = j + 1;
                if (i1 >= 0  &&  j1 < ncol) {
                    z2 = gtmp1[i1*ncol+j1];
                    if (z2 < fnull) {
                        sum += z2;
                        n++;
                    }
                }
                if (n > 0) {
                    gtmp2[k] = sum / (CSW_F)n;
                    ndone++;
                }
            }
        }

        memcpy (gtmp1, gtmp2, ncol*nrow*sizeof(CSW_F));

        if (ndone == 0) break;

    }

/*
    All nodes must be filled in now so return.
*/
    memcpy (grid, gtmp1, nsize);

    return 1;

}  /*  end of private FillInNulls function  */




/*
  ****************************************************************************

                          E x t e n d V e c t o r

  ****************************************************************************

*/

int CSWGrdFault::ExtendVector (CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2,
                         CSW_F length)
{
    double            ang, cosang, sinang, dx, dy;
    double            xt, yt, dt;

    dt = (double)length;
    if (dt < ExtendTiny) {
        dt = (double)ExtendTiny;
    }

    dx = (double)(*x2 - *x1);
    dy = (double)(*y2 - *y1);

    ang = atan2 (dy, dx);
    cosang = cos (ang);
    sinang = sin (ang);

    xt = (double)(*x1);
    yt = (double)(*y1);

    xt -= dt * cosang;
    yt -= dt * sinang;
    *x1 = (CSW_F)xt;
    *y1 = (CSW_F)yt;

    xt = (double)(*x2);
    yt = (double)(*y2);

    xt += dt * cosang;
    yt += dt * sinang;
    *x2 = (CSW_F)xt;
    *y2 = (CSW_F)yt;

    return 1;

}  /*  end of private ExtendVector function  */





/*
  ****************************************************************************

             S e t u p R o w C r o s s i n g E l e v a t i o n s

  ****************************************************************************

    Calculate FAultCrossingElevation structures for each spot where a fault
  crosses a row in the grid.  These elevations are indexed to the nodes to
  the left and right of the fault crossing, a separate elevation for each
  node.  The two nodes on the same side of the fault are used to extrapolate
  an elevation at the fault.  If only one node exists on a fault side, that
  nodes elevation is used.

    The reason for doing this is to establish one set of elevations at faults
  that can be used for any contours crossing the cell that has the fault,
  regardless of the entrance and exit sides of the contour through the cell.
  In other words, the faulted cell will be interpolated in exactly the same
  fashion no matter what contour crosses it.  If the exact same interpolation
  is not used, then contours can cross each other near the faults.

*/

int CSWGrdFault::SetupRowCrossingElevations (void)
{
    CRossingElevation        estruct, *eptr;
    int                      i, j, k, offset, backnode, istat;
    CSW_F                    zt, z1, z2, x1, y1, x2, y2,
                             tiny, xint, yint, pct;

    if (Grid == NULL) return 1;

    eptr = &estruct;

/*
    Loop through each row.  If the RowCrossings array for a cell side
    is true, then calculate crossing elevations for the side and the
    fault.
*/
    tiny = (Xspace + Yspace) / 2000.0f;
    for (i=0; i<Nrow; i++) {
        offset = i * Ncol;
        backnode = offset;
        y1 = i * Yspace + Ymin;
        y2 = y1;
        for (j=0; j<Ncol-1; j++) {
            k = offset + j;
            if (RowCrossings[k] == 0) {
                backnode = k;
                continue;
            }
            z1 = Grid[backnode];
            z2 = Grid[k];
            x1 = j * Xspace + Xmin;
            x2 = x1 + Xspace;
            istat = con_find_fault_intersection (x1-tiny, y1, x2+tiny, y2,
                                                 k, 0, &xint, &yint);
            if (istat == -1) {
                FreeCrossingElevations ();
                return -1;
            }
            if (istat == 0) {
                backnode = k;
                continue;
            }
            if (z2 >= NullTest  ||  z1 >= NullTest) {
                zt = NullValue;
            }
            else {
                pct = (xint - x1) / Xspace;
                zt = z2 + pct * (z2 - z1);
            }
            eptr->x = xint;
            eptr->y = yint;
            eptr->z = zt;
            eptr->node = k;
            eptr->backnode = backnode;
            eptr->flag = 0;
            eptr->fault_num = istat - 1;

            istat = AddCrossingElevation (k, eptr);
            if (istat == -1) {
                FreeCrossingElevations ();
                return -1;
            }
            backnode = k + 1;
        }

        backnode = offset + Ncol - 1;
        for (j=Ncol-1; j>0; j--) {
            k = offset + j;
            if (RowCrossings[k-1] == 0) {
                backnode = k;
                continue;
            }
            z1 = Grid[backnode];
            z2 = Grid[k];
            x1 = j * Xspace + Xmin;
            x2 = x1 - Xspace;
            istat = con_find_fault_intersection (x1+tiny, y1, x2-tiny, y2,
                                                 k, 0, &xint, &yint);
            if (istat == -1) {
                FreeCrossingElevations ();
                return -1;
            }
            if (istat == 0) {
                backnode = k;
                continue;
            }
            if (z2 >= NullTest  ||  z1 >= NullTest) {
                zt = NullValue;
            }
            else {
                pct = (x1 - xint) / Xspace;
                zt = z2 + pct * (z2 - z1);
            }
            eptr->x = xint;
            eptr->y = yint;
            eptr->z = zt;
            eptr->node = k;
            eptr->backnode = backnode;
            eptr->flag = 0;
            eptr->fault_num = istat - 1;

            istat = AddCrossingElevation (k, eptr);
            if (istat == -1) {
                FreeCrossingElevations ();
                return -1;
            }
            backnode = k - 1;
        }
    }

    return 1;

}  /*  end of private SetupRowCrossingElevations function  */





/*
  ****************************************************************************

          S e t u p C o l u m n C r o s s i n g E l e v a t i o n s

  ****************************************************************************

    Calculate FAultCrossingElevation structures for each spot where a fault
  crosses a column in the grid.  These elevations are indexed to the nodes to
  the top and bottom of the fault crossing, a separate elevation for each
  node.  The two nodes on the same side of the fault are used to extrapolate
  an elevation at the fault.  If only one node exists on a fault side, that
  nodes elevation is used.

    The reason for doing this is to establish one set of elevations at faults
  that can be used for any contours crossing the cell that has the fault,
  regardless of the entrance and exit sides of the contour through the cell.
  In other words, the faulted cell will be interpolated in exactly the same
  fashion no matter what contour crosses it.  If the exact same interpolation
  is not used, then contours can cross each other near the faults.

*/

int CSWGrdFault::SetupColumnCrossingElevations (void)
{
    CRossingElevation        estruct, *eptr;
    int                      i, j, k, backnode, istat;
    CSW_F                    zt, z1, z2, x1, y1, x2, y2,
                             tiny, xint, yint, pct;

    if (Grid == NULL) return 1;

    eptr = &estruct;

/*
    Loop through each column of cells.  If the ColumnCrossings array for
    a cell side is true, then calculate crossing elevations for the side
    and the fault.
*/
    tiny = (Xspace + Yspace) / 2000.0f;
    for (j=0; j<Ncol; j++) {
        backnode = j;
        for (i=0; i<Nrow-1; i++) {
            k = i * Ncol + j;
            if (ColumnCrossings[k] == 0) {
                backnode = k;
                continue;
            }
            z1 = Grid[backnode];
            z2 = Grid[k];
            y1 = i * Yspace + Ymin;
            y2 = y1 + Yspace;
            x1 = j * Xspace + Xmin;
            x2 = x1;
            istat = con_find_fault_intersection (x1, y1-tiny, x2, y2+tiny,
                                                 k, 0, &xint, &yint);
            if (istat == -1) {
                FreeCrossingElevations ();
                return -1;
            }
            if (istat == 0) {
                backnode = k;
                continue;
            }
            if (z1 >= NullTest  ||  z2 >= NullTest) {
                zt = NullValue;
            }
            else {
                pct = (yint - y1) / Yspace;
                zt = z2 + pct * (z2 - z1);
            }
            eptr->x = xint;
            eptr->y = yint;
            eptr->z = zt;
            eptr->node = k;
            eptr->backnode = backnode;
            eptr->flag = 1;
            eptr->fault_num = istat - 1;

            istat = AddCrossingElevation (k, eptr);
            if (istat == -1) {
                FreeCrossingElevations ();
                return -1;
            }
            backnode = k + Ncol;
        }

        backnode = (Nrow - 1) * Ncol + j;
        for (i=Nrow-1; i>0; i--) {
            k = i * Ncol + j;
            if (ColumnCrossings[k-Ncol] == 0) {
                backnode = k;
                continue;
            }
            z1 = Grid[backnode];
            z2 = Grid[k];
            x1 = j * Xspace + Xmin;
            x2 = x1;
            y1 = i * Yspace + Ymin;
            y2 = y1 - Yspace;
            istat = con_find_fault_intersection (x1, y1+tiny, x2, y2-tiny,
                                                 k, 0, &xint, &yint);
            if (istat == -1) {
                FreeCrossingElevations ();
                return -1;
            }
            if (istat == 0) {
                backnode = k;
                continue;
            }
            if (z1 >= NullTest  ||  z2 >= NullTest) {
                zt = NullValue;
            }
            else {
                pct = (y1 - yint) / Yspace;
                zt = z2 + pct * (z2 - z1);
            }
            eptr->x = xint;
            eptr->y = yint;
            eptr->z = zt;
            eptr->node = k;
            eptr->backnode = backnode;
            eptr->flag = 1;
            eptr->fault_num = istat - 1;

            istat = AddCrossingElevation (k, eptr);
            if (istat == -1) {
                FreeCrossingElevations ();
                return -1;
            }
            backnode = k - Ncol;
        }
    }

    return 1;

}  /*  end of private SetupColumnCrossingElevations function  */





/*
  ****************************************************************************

                 F r e e C r o s s i n g E l e v a t i o n s

  ****************************************************************************

    Free the memory associated with the fault crossing elevations.

*/

int CSWGrdFault::FreeCrossingElevations (void)
{
    int           i;

    if (ElevList) {
        for (i=0; i<NumElevList; i++) {
            if (ElevList[i] != NULL) {
                csw_Free (ElevList[i]);
            }
        }
    }

    if (ElevList) csw_Free (ElevList);
    if (ElevIndex) csw_Free (ElevIndex);

    ElevList = NULL;
    ElevIndex = NULL;
    MaxElevList = 0;
    NumElevList = 0;

    return 1;

}  /*  end of private FreeCrossingElevations function  */




/*
  ****************************************************************************

               A d d C r o s s i n g E l e v a t i o n

  ****************************************************************************

*/

int CSWGrdFault::AddCrossingElevation (int kcell, CRossingElevation *eptr)
{
    int                 i;
    CRossingElevation   *epstore = NULL;
    CRossingElevation   **cetmp = NULL;

    bool    bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (epstore);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (NumElevList >= MaxElevList - 1) {
        MaxElevList += INDEX_CHUNK;
        cetmp = (CRossingElevation **)csw_Realloc
                (ElevList, MaxElevList * sizeof(CRossingElevation*));
        if (cetmp == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        ElevList = cetmp;
    }

    epstore = (CRossingElevation *)csw_Malloc (sizeof(CRossingElevation));
    if (epstore == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    memcpy (epstore, eptr, sizeof(CRossingElevation));

    if (ElevIndex[kcell] == -1) {
        ElevIndex[kcell] = NumElevList;
    }
    else {
        i = ElevIndex2[kcell];
        ElevList[i]->next = NumElevList;
    }

    epstore->next = -1;
    ElevList[NumElevList] = epstore;
    ElevIndex2[kcell] = NumElevList;

    NumElevList++;

    bsuccess = true;

    return 1;

}  /*  end of private AddCrossingElevation function  */




/*
  ****************************************************************************

                     F i l l F r o m C r o s s i n g s

  ****************************************************************************

    Use the fault crossing information for the center cell of the bwork
  array to fill in all 4 corners of the cell if needed.  This is only
  called from con_faulted_bilin_interp and con_faulted_bicub_interp.

    This is strictly based on bwork being a 4 by 4 subgrid of the original
  grid and bwork spans the i1, i2  j1, j2 ranges of the original grid.

*/

int CSWGrdFault::FillFromCrossings (int i1, int i2, int j1, int j2, CSW_F *bwork)
{
    int             i, k, n, istat;
    CSW_F           nulltest, bw2[100], zta[10], sum;

    memset (zta, 0, 10 * sizeof(CSW_F));

    i2 = i2;
    j2 = j2;

    nulltest = (CSW_F)(GRD_SOFT_NULL_VALUE / 100.0);
    for (i=0; i<16; i++) {
        if (bwork[i] < nulltest) {
            bwork[i] = 1.e30f;
        }
    }

    nulltest = 1.e19f;
    memcpy (bw2, bwork, 16 * sizeof(CSW_F));

/*
    If the lower left corner of the center cell is null, use the
    crossing info from non null horizontal and vertical neighbors
    to fill in the value of the node.
*/
    if (bwork[5] >= nulltest) {
        n = 0;
        if (bwork[4] < nulltest) {
            k = (i1 + 1) * Ncol + j1;
            istat = EvaluateCrossing (k, k+1, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[6] < nulltest) {
            k = (i1 + 1) * Ncol + j1 + 2;
            istat = EvaluateCrossing (k, k-1, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[1] < nulltest) {
            k = i1 * Ncol + j1 + 1;
            istat = EvaluateCrossing (k, k+Ncol, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[9] < nulltest) {
            k = (i1 + 2) * Ncol + j1 + 1;
            istat = EvaluateCrossing (k, k-Ncol, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (n > 0) {
            sum = 0.0f;
            for (i=0; i<n; i++) {
                sum += zta[i];
            }
            bw2[5] = sum / (CSW_F)n;
        }
    }

/*
    If the lower right corner of the center cell is null, use the
    crossing info from non null horizontal and vertical neighbors
    to fill in the value of the node.
*/
    if (bwork[6] >= nulltest) {
        n = 0;
        if (bwork[5] < nulltest) {
            k = (i1 + 1) * Ncol + j1 + 1;
            istat = EvaluateCrossing (k, k+1, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[7] < nulltest) {
            k = (i1 + 1) * Ncol + j1 + 3;
            istat = EvaluateCrossing (k, k-1, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[2] < nulltest) {
            k = i1 * Ncol + j1 + 2;
            istat = EvaluateCrossing (k, k+Ncol, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[10] < nulltest) {
            k = (i1 + 2) * Ncol + j1 + 2;
            istat = EvaluateCrossing (k, k-Ncol, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (n > 0) {
            sum = 0.0f;
            for (i=0; i<n; i++) {
                sum += zta[i];
            }
            bw2[6] = sum / (CSW_F)n;
        }
    }

/*
    If the upper left corner of the center cell is null, use the
    crossing info from non null horizontal and vertical neighbors
    to fill in the value of the node.
*/
    if (bwork[9] >= nulltest) {
        n = 0;
        if (bwork[8] < nulltest) {
            k = (i1 + 2) * Ncol + j1;
            istat = EvaluateCrossing (k, k+1, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[10] < nulltest) {
            k = (i1 + 2) * Ncol + j1 + 2;
            istat = EvaluateCrossing (k, k-1, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[5] < nulltest) {
            k = (i1 + 1) * Ncol + j1 + 1;
            istat = EvaluateCrossing (k, k+Ncol, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[13] < nulltest) {
            k = (i1 + 3) * Ncol + j1 + 1;
            istat = EvaluateCrossing (k, k-Ncol, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (n > 0) {
            sum = 0.0f;
            for (i=0; i<n; i++) {
                sum += zta[i];
            }
            bw2[9] = sum / (CSW_F)n;
        }
    }

/*
    If the upper right corner of the center cell is null, use the
    crossing info from non null horizontal and vertical neighbors
    to fill in the value of the node.
*/
    if (bwork[10] >= nulltest) {
        n = 0;
        if (bwork[9] < nulltest) {
            k = (i1 + 2) * Ncol + j1 + 1;
            istat = EvaluateCrossing (k, k+1, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[11] < nulltest) {
            k = (i1 + 2) * Ncol + j1 + 3;
            istat = EvaluateCrossing (k, k-1, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[6] < nulltest) {
            k = (i1 + 1) * Ncol + j1 + 2;
            istat = EvaluateCrossing (k, k+Ncol, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (bwork[14] < nulltest) {
            k = (i1 + 3) * Ncol + j1 + 2;
            istat = EvaluateCrossing (k, k-Ncol, zta+n);
            if (istat == 1) {
                n++;
            }
        }
        if (n > 0) {
            sum = 0.0f;
            for (i=0; i<n; i++) {
                sum += zta[i];
            }
            bw2[10] = sum / (CSW_F)n;
        }
    }

/*
    Copy bw2 into bwork.
*/
    memcpy (bwork, bw2, 16 * sizeof(CSW_F));

/*
    If any of the central nodes are still null, reflect the gradients
    in the non null central nodes to fill in the null values.
*/
    if (bwork[5] >= nulltest) {
        if (bwork[6] < nulltest  &&
            bwork[10] < nulltest  &&
            bwork[9] < nulltest) {
            bw2[5] = bwork[9] + bwork[6] - bwork[10];
        }
        else if (bwork[9] < nulltest  &&
                 bwork[6] < nulltest) {
            bw2[5] = (bwork[9] + bwork[6]) / 2.0f;
        }
        else if (bwork[10] < nulltest  &&
                 bwork[6] < nulltest) {
            bw2[5] = bwork[6];
        }
        else if (bwork[9] < nulltest  &&
                 bwork[10] < nulltest) {
            bw2[5] = bwork[9];
        }
    }

    if (bwork[6] >= nulltest) {
        if (bwork[5] < nulltest  &&
            bwork[10] < nulltest  &&
            bwork[9] < nulltest) {
            bw2[6] = bwork[10] + bwork[5] - bwork[9];
        }
        else if (bwork[10] < nulltest  &&
                 bwork[5] < nulltest) {
            bw2[6] = (bwork[10] + bwork[5]) / 2.0f;
        }
        else if (bwork[9] < nulltest  &&
                 bwork[5] < nulltest) {
            bw2[6] = bwork[5];
        }
        else if (bwork[9] < nulltest  &&
                 bwork[10] < nulltest) {
            bw2[6] = bwork[10];
        }
    }

    if (bwork[9] >= nulltest) {
        if (bwork[6] < nulltest  &&
            bwork[10] < nulltest  &&
            bwork[5] < nulltest) {
            bw2[9] = bwork[5] + bwork[10] - bwork[6];
        }
        else if (bwork[10] < nulltest  &&
                 bwork[5] < nulltest) {
            bw2[9] = (bwork[10] + bwork[5]) / 2.0f;
        }
        else if (bwork[10] < nulltest  &&
                 bwork[6] < nulltest) {
            bw2[9] = bwork[10];
        }
        else if (bwork[5] < nulltest  &&
                 bwork[6] < nulltest) {
            bw2[9] = bwork[5];
        }
    }

    if (bwork[10] >= nulltest) {
        if (bwork[6] < nulltest  &&
            bwork[5] < nulltest  &&
            bwork[9] < nulltest) {
            bw2[10] = bwork[9] + bwork[6] - bwork[5];
        }
        else if (bwork[9] < nulltest  &&
                 bwork[6] < nulltest) {
            bw2[10] = (bwork[9] + bwork[6]) / 2.0f;
        }
        else if (bwork[9] < nulltest  &&
                 bwork[5] < nulltest) {
            bw2[10] = bwork[9];
        }
        else if (bwork[6] < nulltest  &&
                 bwork[5] < nulltest) {
            bw2[10] = bwork[6];
        }
    }

/*
    Copy the final bw2 into bwork and return.
*/
    memcpy (bwork, bw2, 16 * sizeof(CSW_F));

    return 1;

}  /*  end of private FillFromCrossings function  */






/*
  ****************************************************************************

                     E v a l u a t e C r o s s i n g

  ****************************************************************************

    Given a node with a value, calculate the value at the neighbor node
  from the fault crossing info.

*/

int CSWGrdFault::EvaluateCrossing (int node, int neighbor, CSW_F *value)
{
    CRossingElevation    *ceptr, *cesave;
    int                  start, n, flagcheck;
    CSW_F                z1, z2, zt;

/*
    Determine if this is a horizontal or vertical side.
*/
    n = neighbor - node;
    if (n == 1  ||  n == -1) {
        flagcheck = 0;
    }
    else {
        flagcheck = 1;
    }

    *value = 1.e30f;

    if (Grid == NULL) return 0;

/*
    Loop through all crossing elevation structures linked to the
    specified node.  Ignore any which do not match in the vertical
    or horizontal checks.  Also, if the backnode value in the
    crossing structure is the same as neighbor, do not use it.
    The first crossing structure with correct flag and backnode
    is used.
*/
    start = ElevIndex[node];
    if (start == -1) {
        return 0;
    }
    ceptr = ElevList[start];
    cesave = NULL;

    for (;;) {
        if (ceptr->flag == flagcheck  &&  ceptr->backnode != neighbor) {
            cesave = ceptr;
            break;
        }
        start = ceptr->next;
        if (start == -1) {
            break;
        }
        ceptr = ElevList[start];
    }

/*
    Return failure if no structure was found.
*/
    if (cesave == NULL) {
        return 0;
    }

/*
    Use the backnode and node to extrapolate to neighbor.
*/
    z1 = Grid[node];
    z2 = Grid[cesave->backnode];

    if (z1 > 1.e19f  ||  z2 > 1.e19f) {
        return 0;
    }

    zt = z1 - (z2 - z1);

    *value = zt;

    return 1;

}  /*  end of private EvaluateCrossing function  */



/*
  ****************************************************************

        c o n _ f a u l t e d _ b i c u b _ i n t e r p _ 2

  ****************************************************************

    This function calls con_faulted_bicub_interp for each point in
  the specified array.  The x and y coordinates of the array points
  are used as the reference points for the interpolation.  In other
  words, no grid nodes across faults from the point will be used in
  interpolating the surface at the point.

*/

int CSWGrdFault::con_faulted_bicub_interp_2 (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                                int kcell,
                                CSW_F *x, CSW_F *y, CSW_F *z, int npts)
{
    int              i, ir, jc, istat, iref, n;
    CSW_F            xt, yt, xref, yref;
    CSW_F            *xa = NULL, *ya = NULL, *za = NULL;
    int              *idone = NULL, *ia = NULL;


    auto fscope = [&]()
    {
        csw_Free (xa);
        csw_Free (idone);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Initialize the z values as nulls (1.e30)
*/
    for (i=0; i<npts; i++) {
        z[i] = 1.e30f;
    }

/*
    If no valid cell number has been specified, do the interpolation
    the hard slow way.
*/
    if (kcell < 0) {
        for (i=0; i<npts; i++) {

            if (x[i] >= 1.e20) {
                z[i] = 1.e30f;
                continue;
            }

            ir = (int)((y[i] - Ymin) / Yspace);
            jc = (int)((x[i] - Xmin) / Xspace);

            if (ir < 0  ||  jc < 0  ||  ir > nrow-1  ||  jc > Ncol-1) {
                z[i] = 1.e30f;
                continue;
            }

            xt = x[i];
            yt = y[i];
            istat = con_faulted_bicub_interp (grid, ncol, nrow, nullval,
                                              xt, yt, ir, jc,
                                              &xt, &yt, z+i, 1);
            if (istat == -1) {
                return -1;
            }
        }
        return 1;
    }

/*
    If a cell number is passed, all of the points are
    inside or on the edge of that cell.  In this case,
    groups of points on the same fault side are interpolated
    together to speed up the process.
*/
    n = npts + 1;
    if (n < 100) n = 100;
    xa = (CSW_F *)csw_Malloc (n * 3 * sizeof(CSW_F));
    if (xa == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    ya = xa + n;
    za = ya + n;
    idone = (int *)csw_Calloc (n * 2 * sizeof(int));
    if (idone == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    ia = idone + n;

    ir = kcell / Ncol;
    jc = kcell % Ncol;

    for (i=0; i<npts; i++) {
        if (x[i] >= 1.e20) {
            z[i] = 1.e30f;
            idone[i] = 1;
            continue;
        }
    }

    for (;;) {

    /*
     * BUGFIX !!!!
     *
     * The FaultVectorsUsed array should not be used in this loop.
     */
        memset (FaultVectorsUsed, 0, NumFaultVectors * sizeof(char));
        FaultVectorUsedValue = 0;

        iref = -1;
        for (i=0; i<npts; i++) {
            if (idone[i] == 1) continue;
            iref = i;
            break;
        }
        if (iref == -1) break;

        xref = x[iref];
        yref = y[iref];
/*
        AdjustForFaultGrazing (&xref, &yref);
*/
        xa[0] = xref;
        ya[0] = yref;
        ia[0] = iref;
        n = 1;

        for (i=iref+1; i<npts; i++) {
            if (idone[i] == 1) continue;
            xt = x[i];
            yt = y[i];
            istat = CheckForFaultGrazing (xt, yt);
            if (istat == 1) {
                ShortenVector (&xt, &yt, &xref, &yref);
            }
            istat = con_find_fault_intersection (xt, yt, xref, yref,
                                                 kcell, 1, &xt, &yt);
            if (istat == 0) {
                xa[n] = x[i];
                ya[n] = y[i];
                ia[n] = i;
                n++;
            }
        }

        if (n < 1) {
            idone[iref] = 1;
            continue;
        }

        istat = con_faulted_bicub_interp (grid, ncol, nrow, nullval,
                                          xref, yref, ir, jc,
                                          xa, ya, za, n);
        if (istat == -1) {
            return -1;
        }

        for (i=0; i<n; i++) {
            z[ia[i]] = za[i];
            idone[ia[i]] = 1;
        }

    }

    return 1;

}  /*  end of function con_faulted_bicub_interp_2  */





/*
  ****************************************************************************

                   C h e c k F o r F a u l t G r a z i n g

  ****************************************************************************

    Check if the specified x,y point is almost exactly on top of a fault.
  Return the fault number + 1 if it is or zero if it isn't.

*/

int CSWGrdFault::CheckForFaultGrazing (CSW_F x, CSW_F y)
{
    int               i, j, r1, r2, c1, c2, istat;
    int               *vecnums, nvec;
    CSW_F             xy[10], dist;

    r1 = (int)((y - Ymin) / Yspace);
    r2 = r1;
    c1 = (int)((x - Xmin) / Xspace);
    c2 = c1;

    r1--;
    c1--;
    if (r1 < 0) r1 = 0;
    if (c1 < 0) c1 = 0;

    istat = GetVectors (r1, r2, c1, c2, &vecnums, &nvec);
    if (istat == -1) {
        return -1;
    }
    if (nvec < 1  ||  vecnums == NULL) {
        return 0;
    }

    for (i=0; i<nvec; i++) {
        j = vecnums[i];
        if (FaultVectors[j].flag == OUTSIDE_GRID_FLAG) {
            continue;
        }
        xy[0] = FaultVectors[j].x1;
        xy[1] = FaultVectors[j].y1;
        xy[2] = FaultVectors[j].x2;
        xy[3] = FaultVectors[j].y2;
        gpf_calclinedist1 (xy, 2, x, y, &dist);
        if (dist <= NodeOffset) {
            return j + 1;
        }
    }

    return 0;

}  /*  end of private CheckForFaultGrazing function  */






/*
  ****************************************************************************

                A d j u s t F o r F a u l t G r a z i n g

  ****************************************************************************

    If the specified x,y point is almost exactly on a fault, move it a
  slight bit perpendicular to the fault vector it grazes.

*/

int CSWGrdFault::AdjustForFaultGrazing (CSW_F *xin, CSW_F *yin)
{
    int                   istat;
    FAultVector           *fptr;
    CSW_F                 x, y, xout, yout;

    x = *xin;
    y = *yin;

    istat = CheckForFaultGrazing (x, y);
    if (istat == 0) {
        return 0;
    }

    fptr = FaultVectors + istat - 1;

    PerpOffset (fptr->x1, fptr->y1,
                fptr->x2, fptr->y2,
                x, y, (CSW_F)(NodeOffset * 20.0),
                &xout, &yout);

    *xin = xout;
    *yin = yout;

    return 1;

}  /*  end of private AdjustForFaultGrazing function  */




/*
  ****************************************************************************

                           P e r p O f f s e t

  ****************************************************************************

    Calculate a point perpendicular from the specified line at the specified
  point and by the specified distance.

*/

int CSWGrdFault::PerpOffset (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                       CSW_F xt, CSW_F yt, CSW_F distance,
                       CSW_F *xout, CSW_F *yout)
{
    double           xx, yy, dx, dy, angle;

    dx = x2 - x1;
    dy = y2 - y1;
    angle = atan2 (dy, dx);
    angle -= HALFPI;
    dx = (double)distance * cos (angle);
    dy = (double)distance * sin (angle);
    xx = xt + dx;
    yy = yt + dy;

    if (xx < (double)Xmin  ||  xx > (double)Xmax  ||
        yy < (double)Ymin  ||  yy > (double)Ymax) {
        angle += HALFPI * 2.0;
        dx = (double)distance * cos (angle);
        dy = (double)distance * sin (angle);
        xx = xt + dx;
        yy = yt + dy;
    }

    *xout = (CSW_F)(xx);
    *yout = (CSW_F)(yy);

    return 1;

}  /*  end of private PerpOffset function  */




/*
  ****************************************************************************

                 c o n _ f a u l t e d _ s m o o t h i n g

  ****************************************************************************

    This function does light smoothing on a faulted grid.  Neighbor nodes
  adjacent to a center node and not across a fault are averaged for the
  smoothed value at the center node.

*/

int CSWGrdFault::con_faulted_smoothing (int level)
{
    int                  ndo, ido, i, j, k, ii, jj, kk,
                         offset, offset2, faultflag,
                         istat, i1, i2, j1, j2;
    CSW_F                *gwork = NULL, sum, sum2, xref, yref, wdum;
    CSW_F                nullflag, xt, yt;
    char                 sfile[200], *cenv = NULL;


    auto fscope = [&]()
    {
        csw_Free (gwork);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (Grid == NULL) {
        return 0;
    }

    ndo = level;
    gwork = (CSW_F *)csw_Malloc (Ncol * Nrow * sizeof(CSW_F));
    if (!gwork) {
        return -1;
    }

    nullflag = 1.e19f;

/*
    Use multiple smoothing passes for higher smoothing levels.
*/
    for (ido=0; ido<ndo; ido++) {

        for (i=0; i<Nrow; i++) {

            offset = i * Ncol;
            yref = Ymin + i * Yspace;

            for (j=0; j<Ncol; j++) {

                k = offset + j;

                if (Grid[k] > nullflag  ||  Grid[k] < -nullflag) {
                    gwork[k] = Grid[k];
                    continue;
                }

                i1 = i - 1;
                i2 = i + 1;
                j1 = j - 1;
                j2 = j + 1;

                if (i1 < 0) i1 = 0;
                if (i2 > Nrow - 1) i2 = Nrow - 1;
                if (j1 < 0) j1 = 0;
                if (j2 > Ncol - 1) j2 = Ncol - 1;

                faultflag = 0;
                if (ClosestFault[k] < 2) {
                    faultflag = 1;
                }
                xref = Xmin + j * Xspace;

                if (faultflag) {
                    istat = CheckForFaultGrazing (xref, yref);
                    if (istat != 0) {
                        gwork[k] = Grid[k];
                        continue;
                    }
                }

            /*
                Average the non null neighbors of node k if they are
                on the same side of faults as node k.
            */
                sum = 0.0f;
                sum2 = 0.0f;
                for (ii=i1; ii<=i2; ii++) {
                    offset2 = ii * Ncol;
                    yt = Ymin + ii * Yspace;
                    for (jj=j1; jj<=j2; jj++) {
                        xt = Xmin + ii * Xspace;
                        if (faultflag) {
                            istat = CheckForFaultGrazing (xt, yt);
                            if (istat != 0) {
                                continue;
                            }
                        }
                        kk = offset2 + jj;
                        if (Grid[kk] > nullflag  ||  Grid[kk] < -nullflag) {
                            continue;
                        }
                        if (faultflag) {
                            istat = grd_check_fault_blocking (ii, jj, xref, yref, &wdum);
                            if (istat == 0) {
                                sum += Grid[kk];
                                sum2 += 1.0f;
                            }
                        }
                        else {
                            sum += Grid[kk];
                            sum2 += 1.0f;
                        }
                    }
                }

                if (sum2 > 0.0f) {
                    gwork[k] = sum / sum2;
                }
                else {
                    gwork[k] = Grid[k];
                }
            }
        }

    /*
        Copy the work grid into the original grid and go to the next
        smoothing pass.
    */
        memcpy (Grid, gwork, Ncol*Nrow*sizeof(CSW_F));

    }

    cenv = csw_getenv ("CSW_RESAMPLED_FILE");
    if (cenv) {
        sprintf (sfile, "s_%s", cenv);
        grd_fileio_ptr->grd_write_file (sfile, NULL,
                        Grid, NULL, NULL,
                        Ncol, Nrow,
                        (double)Xmin, (double)Ymin, (double)Xmax, (double)Ymax,
                        GRD_NORMAL_GRID_FILE, NULL, 0);
    }

    return 1;

}  /*  end of function con_faulted_smoothing  */





/*
  ****************************************************************************

             g r d _ r e s a m p l e _ f a u l t e d _ g r i d

  ****************************************************************************

    Resample a grid with faults.  The faults must have been indexed via a
  call to grd_build_fault_indices prior to calling this function.  The source
  grid geometry has also been defined by that function.

*/

int CSWGrdFault::grd_resample_faulted_grid (CSW_F *gout, int ncout, int nrout,
                               CSW_F x1out, CSW_F y1out, CSW_F x2out, CSW_F y2out,
                               int flag, int nskipin)
{
    int               i, j, k, ii, jj, kk, offset, offset2,
                      i1, i2, j1, j2, nskip, n, efsave, istat;
    CSW_F             *x = NULL, *y = NULL, *z = NULL,
                      xt, yt, x1, y1, x2, y2,
                      xsp2, ysp2, tiny;
    int               start_row, end_row, start_col, end_col;


    auto fscope = [&]()
    {
        csw_Free (x);
    };
    CSWScopeGuard func_scope_guard (fscope);


    nskipin = nskipin;
    nskip = 1;

    efsave = EndFlag;
    EndFlag = 0;

/*
    Calculate the spacings in the new resampled grid.
*/
    xsp2 = (x2out - x1out) / (CSW_F)(ncout - 1);
    ysp2 = (y2out - y1out) / (CSW_F)(nrout - 1);

/*
    Allocate x, y and z arrays for the maximum sized subgrid.
*/
    n = (int)((Yspace / ysp2 + 2.0f) * (Xspace / xsp2 + 2.0f));
    if (n < 100) n = 100;
    if (n > ncout * nrout) n = ncout * nrout;
    x = (CSW_F *)csw_Malloc (3 * n * sizeof(CSW_F));
    if (!x) {
        return -1;
    }
    y = x + n;
    z = y + n;

/*
    tiny is used to insure that no nodes get left out
    due to precision problems.
*/
    tiny = (xsp2 + ysp2) / 40.0f;

    int do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_triangle_ptr->grd_WriteXYZGridFile (
                (char *)"ResampBeforeLoop.xyz",
				Grid, Ncol, Nrow, 1,
				Xmin, Ymin, Xmax, Ymax);
    }

    for (i=0; i<ncout*nrout; i++) {
        gout[i] = 1.e30;
    }

/*
    Loop through the original grid, defined in static variables
    Grid, Ncol, Nrow, Xmin, Ymin, Xmax, Ymax.  For each cell in
    the original grid, calculate x and y coordinates for all nodes
    in the new grid that lie inside the original grid cell.
    The original grid is then interpolated using appropriate
    faulting versions of the interpolation functions to get
    z values at each x,y location.  These z values are then put
    back into the new grid array.
*/
    start_row = (int)((y1out - Ymin) / Yspace);
    end_row = (int)((y2out - Ymin) / Yspace);
    end_row+=nskip;
    if (start_row < 0) start_row = 0;
    if (end_row > Nrow-1) end_row = Nrow - 1;

    start_col = (int)((x1out - Xmin) / Xspace);
    end_col = (int)((x2out - Xmin) / Xspace);
    end_col+=nskip;
    if (start_col < 0) start_col = 0;
    if (end_col > Ncol-1) end_col = Ncol - 1;

    for (i=start_row; i<end_row; i+=nskip) {

        offset = i * Ncol;
        y1 = i * Yspace + Ymin;
        y2 = y1 + Yspace;
        y2 += tiny;
        i1 = (int)((y1 - y1out) / ysp2);
        i2 = (int)((y2 - y1out) / ysp2);
        if (i1 < 0) i1 = 0;
        if (i2 > nrout-1) i2 = nrout - 1;

        if (i1 > i2) continue;

        for (j=start_col; j<end_col; j+=nskip) {

            k = offset + j;
            x1 = j * Xspace + Xmin;
            x2 = x1 + Xspace * nskip;
            x2 += tiny;
            j1 = (int)((x1 - x1out) / xsp2);
            j2 = (int)((x2 - x1out) / xsp2);
            if (j1 < 0) j1 = 0;
            if (j2 > ncout-1) j2 = ncout - 1;

            if (j1 > j2) continue;

            n = 0;
            for (ii=i1; ii<=i2; ii++) {
                if (ii < 0  ||  ii >= nrout) continue;
                yt = y1out + ii * ysp2;
                if (yt < y1  ||  yt > y2) continue;
                for (jj=j1; jj<=j2; jj++) {
                    if (jj < 0  ||  jj >= ncout) continue;
                    xt = x1out + jj * xsp2;
                    if (xt < x1  ||  xt > x2) continue;
                    x[n] = xt;
                    y[n] = yt;
                    n++;
                }
            }

            if (flag == GRD_BICUBIC) {
                if (ClosestFault[k] > nskip * 2) {
                    grd_utils_ptr->grd_bicub_interp (x, y, z, n, 1.e20f,
                                      Grid, Ncol, Nrow, nskip,
                                      Xmin, Ymin, Xmax, Ymax,
                                      i, j);
                }
                else if (ClosestFault[k] > nskip * 2 -1  &&
                         ClosestFault[k+nskip] > nskip * 2 -1  &&
                         ClosestFault[k+nskip*Ncol] > nskip * 2 -1  &&
                         ClosestFault[k+nskip*Ncol+nskip] > nskip * 2 -1) {
                    grd_utils_ptr->grd_bicub_interp (x, y, z, n, 1.e20f,
                                      Grid, Ncol, Nrow, nskip,
                                      Xmin, Ymin, Xmax, Ymax,
                                      i, j);
                }
                else if (ClosestFault[k] > nskip * 2 -1) {
                    con_faulted_bicub_interp (Grid, Ncol, Nrow, 1.e20f,
                                              x[0], y[0], i, j,
                                              x, y, z, n);
                }
                else {
                    con_faulted_bicub_interp_2 (Grid, Ncol, Nrow, 1.e20f, k,
                                                x, y, z, n);
                }
            }
            else {
                if (ClosestFault[k] > nskip * 2) {
                    grd_utils_ptr->grd_bilin_interp (x, y, z, n,
                                      Grid, Ncol, Nrow, nskip,
                                      Xmin, Ymin, Xmax, Ymax);
                }
                else {
                    con_faulted_bilin_interp_2 (Grid, Ncol, Nrow, 1.e20f,
                                                k,
                                                x, y, z, n);
                }
            }

            for (ii=0; ii<n; ii++) {
                if (x[ii] < Xmin-tiny  ||  x[ii] > Xmax+tiny  ||
                    y[ii] < Ymin-tiny  ||  y[ii] > Ymax+tiny) {
                    z[ii] = 1.e30f;
                }
            }

            n = 0;
            for (ii=i1; ii<=i2; ii++) {
                if (ii < 0  ||  ii >= nrout) continue;
                yt = y1out + ii * ysp2;
                if (yt < y1  ||  yt > y2) continue;
                offset2 = ii * ncout;
                for (jj=j1; jj<=j2; jj++) {
                    if (jj < 0  ||  jj >= ncout) continue;
                    xt = x1out + jj * xsp2;
                    if (xt < x1  ||  xt > x2) continue;
                    kk = offset2 + jj;
                    gout[kk] = z[n];
                    n++;
                }
            }

        }

    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_triangle_ptr->grd_WriteXYZGridFile (
                (char *)"ResampBeforeNullFill.xyz",
                gout, ncout, nrout, 1,
                x1out, y1out, x2out, y2out);
    }

    EndFlag = efsave;

    if (NullFillFlag) {
        istat = grd_build_fault_indices (gout, ncout, nrout,
                                         x1out, y1out, x2out, y2out);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        istat =  grd_fill_faulted_nulls (1.e19f);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }


    return 1;

}  /*  end of function grd_resample_faulted_grid  */




/*
 **************************************************************************************

      g r d _ s e t _ f a u l t _ r e s a m p l e _ n u l l _ f i l l _ f l a g

 **************************************************************************************

*/

int CSWGrdFault::grd_set_fault_resample_null_fill_flag (int value)
{
    NullFillFlag = value;
    return 1;
}




/*
  ****************************************************************************

                      S h o r t e n V e c t o r

  ****************************************************************************

*/

int CSWGrdFault::ShortenVector (CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2)
{
    double            ang, cosang, sinang, dx, dy;
    double            xt, yt, dt;

    dt = (double)ExtendTiny;

    dx = (double)(*x2 - *x1);
    dy = (double)(*y2 - *y1);

    ang = atan2 (dy, dx);
    cosang = cos (ang);
    sinang = sin (ang);

    xt = (double)(*x1);
    yt = (double)(*y1);

    xt += dt * cosang;
    yt += dt * sinang;
    *x1 = (CSW_F)xt;
    *y1 = (CSW_F)yt;

    xt = (double)(*x2);
    yt = (double)(*y2);

    xt -= dt * cosang;
    yt -= dt * sinang;
    *x2 = (CSW_F)xt;
    *y2 = (CSW_F)yt;

    return 1;

}  /*  end of private ShortenVector function  */




/*
  ****************************************************************

        g r d _ f a u l t e d _ b i c u b _ i n t e r p

  ****************************************************************

    Do bicubic interpolation of a faulted grid.  If the specified
  row and column is not near any fault, treat the interpolation as
  if it is being done on a non faulted grid.  If a fault is near,
  create a 4 by 4 work grid with all nodes not blocked by the
  fault from the xref, yref point.  Any nodes in the 4 by 4 work
  grid not assigned are calculated by the null extension function
  and then the interpolation is done using the 4 by 4 work grid.

    This function is only used for grid resampling, back interpolation
  and contour smoothing.  Thus, the skip factor is always 1.  The
  x and y limits of the grid must have been specified by a call to
  grd_build_fault_indices prior to calling this function.

*/

int CSWGrdFault::grd_faulted_bicub_interp (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                              CSW_F xref, CSW_F yref, int irow, int jcol,
                              CSW_F *x, CSW_F *y, CSW_F *z, int npts)
{
    int              i, j, i0, k, k2, i1, i2, j1, j2,
                     node, istat, ng, nb, nofaultflag;
    CSW_F            bwork[16], wgt, x1, y1, x2, y2, xt, yt;

/*
    If there are less than 4 columns and 4 rows in the grid,
    no faulted bicubic interpolation is possible.
*/
    nofaultflag = 0;
    if (ncol < 4  ||  nrow < 4)
        nofaultflag = 1;

/*
    make sure the null value is a large positive number.
*/
    if (nullval < 1.e20)
        nullval = 1.e20f;

/*
    If there is no fault nearby, use the nonfaulted interpolation function.
*/
    node = irow * Ncol + jcol;
    if (ClosestFault[node] > Ncoarse * 2  ||  nofaultflag == 1) {
        istat = grd_utils_ptr->grd_bicub_interp (x, y, z, npts, nullval,
                                  grid, ncol, nrow, Ncoarse,
                                  Xmin, Ymin, Xmax, Ymax,
                                  -1, -1);
        return istat;
    }

/*
    Calculate rows and columns for the 4 by 4 area near irow, jcol.
*/
    i1 = irow - Ncoarse;
    if (i1 < 0)
        i1 = 0;
    if (i1 > Nrow - 3 * Ncoarse - 1)
        i1 = Nrow - 3 * Ncoarse - 1;

    i2 = irow + 2 * Ncoarse;
    if (i2 > Nrow - 1)
        i2 = Nrow - 1;
    if (i2 < 3 * Ncoarse)
        i2 = 3 * Ncoarse;

    j1 = jcol - Ncoarse;
    if (j1 < 0)
        j1 = 0;
    if (j1 > Ncol - 3 * Ncoarse - 1)
        j1 = Ncol - 3 * Ncoarse - 1;

    j2 = jcol + 2 * Ncoarse;
    if (j2 > Ncol - 1)
        j2 = Ncol - 1;
    if (j2 < 3 * Ncoarse)
        j2 = 3 * Ncoarse;

/*
    Find nodes on the same side of the fault as xref, yref.
    This is done by determining if xref, yref is on the same
    fault block as irow, jcol.  If this is true, all nodes
    on the same fault block as irow, jcol are also on the same
    fault block as xref, yref.  If this is false, all nodes
    on the same fault block as irow, jcol are not on the same fault
    block as xref, yref.
*/
    ng = 0;
    nb = 0;
    k2 = 0;
    for (i=i1; i<=i2; i+=Ncoarse) {
        i0 = Ncol * i;
        yt = i * Yspace + Ymin;
        for (j=j1; j<=j2; j+=Ncoarse) {
            xt = j * Xspace + Xmin;
            istat = CheckForFaultGrazing (xt, yt);
            if (istat >= 1) {
                bwork[k2] = nullval * 10.0f;
                nb++;
                k2++;
                continue;
            }
            k = i0+j;
            istat = grd_check_fault_blocking (i, j, xref, yref, &wgt);
            if (istat == 0) {
                bwork[k2] = grid[k];
                ng++;
            }
            else {
                bwork[k2] = nullval * 10.0f;
                nb++;
            }
            k2++;
        }
    }

    if (ng < 1) {
        istat = grd_utils_ptr->grd_bilin_interp (x, y, z, npts,
                                  grid, ncol, nrow, Ncoarse,
                                  Xmin, Ymin, Xmax, Ymax);
/*
        for (i=0; i<npts; i++) {
            z[i] = 1.e30f;
        }
*/
        return istat;
    }

/*
    Calculate the corners of the work grid.
*/
    x1 = j1 * Xspace + Xmin;
    y1 = i1 * Yspace + Ymin;
    x2 = j2 * Xspace + Xmin;
    y2 = i2 * Yspace + Ymin;

/*
    fill in the null and faulted out values in the work grid.
*/
    if (nb > 0) {
        istat = FillInNulls (bwork, 4, 4);
        if (istat == -1) {
            return -1;
        }
    }

/*
    The work grid can now be treated as a non faulted grid
    for interpolation purposes.  Note that the center cell
    (cell 1,1) is always used for interpolation.
*/
    istat = grd_utils_ptr->grd_bicub_interp (x, y, z, npts, nullval,
                              bwork, 4, 4, 1,
                              x1, y1, x2, y2,
                              -1, -1);

    return istat;

}  /*  end of function grd_faulted_bicub_interp  */






/*
  ****************************************************************

        g r d _ f a u l t e d _ b i l i n _ i n t e r p

  ****************************************************************

    Do bilinear interpolation of a faulted grid.  If the specified
  row and column is not near any fault, treat the interpolation as
  if it is being done on a non faulted grid.  If a fault is near,
  create a 4 by 4 work grid with all nodes not blocked by the
  fault from the xref, yref point.  Any nodes in the 4 by 4 work
  grid not assigned are calculated by the null extension function
  and then the interpolation is done using the 4 by 4 work grid.

    This function is only used for grid resampling, back interpolation
  and contour smoothing.  Thus, the skip factor is always 1.  The
  x and y limits of the grid must have been specified by a call to
  grd_build_fault_indices prior to calling this function.

*/

int CSWGrdFault::grd_faulted_bilin_interp (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                              CSW_F xref, CSW_F yref, int irow, int jcol,
                              CSW_F *x, CSW_F *y, CSW_F *z, int npts)
{
    int              i, j, i0, k, k2, i1, i2, j1, j2,
                     node, istat, ng, nb, nofaultflag;
    CSW_F            bwork[16], wgt, x1, y1, x2, y2, xt, yt;

/*
    If there are less than 4 columns and 4 rows in the grid,
    no faulted interpolation is possible.
*/
    nofaultflag = 0;
    if (ncol < 4  ||  nrow < 4)
        nofaultflag = 1;

/*
    make sure the null value is a large positive number.
*/
    if (nullval < 1.e20)
        nullval = 1.e20f;

/*
    If there is no fault nearby, use the nonfaulted interpolation function.
*/
    node = irow * Ncol + jcol;
    if (ClosestFault[node] > 0  ||  nofaultflag == 1) {
        istat = grd_utils_ptr->grd_bilin_interp (x, y, z, npts,
                                  grid, ncol, nrow, 1,
                                  Xmin, Ymin, Xmax, Ymax);
        return istat;
    }

/*
    Calculate rows and columns for the 4 by 4 area near irow, jcol.
*/
    i1 = irow - Ncoarse;
    if (i1 < 0)
        i1 = 0;
    if (i1 > Nrow - 3 * Ncoarse - 1)
        i1 = Nrow - 3 * Ncoarse - 1;

    i2 = irow + 2 * Ncoarse;
    if (i2 > Nrow - 1)
        i2 = Nrow - 1;
    if (i2 < 3 * Ncoarse)
        i2 = 3 * Ncoarse;

    j1 = jcol - Ncoarse;
    if (j1 < 0)
        j1 = 0;
    if (j1 > Ncol - 3 * Ncoarse - 1)
        j1 = Ncol - 3 * Ncoarse - 1;

    j2 = jcol + 2 * Ncoarse;
    if (j2 > Ncol - 1)
        j2 = Ncol - 1;
    if (j2 < 3 * Ncoarse)
        j2 = 3 * Ncoarse;

/*
    Find nodes on the same side of the fault as xref, yref.
*/
    ng = 0;
    nb = 0;
    k2 = 0;
    for (i=i1; i<=i2; i++) {
        i0 = Ncol * i;
        yt = i * Yspace + Ymin;
        for (j=j1; j<=j2; j++) {
            xt = j * Xspace + Xmin;
            istat = CheckForFaultGrazing (xt, yt);
            if (istat >= 1) {
                bwork[k2] = nullval * 10.0f;
                nb++;
                k2++;
                continue;
            }
            k = i0+j;
            istat = grd_check_fault_blocking (i, j, xref, yref, &wgt);
            if (istat == 0) {
                bwork[k2] = grid[k];
                ng++;
            }
            else {
                bwork[k2] = nullval * 10.0f;
                nb++;
            }
            k2++;
        }
    }

    if (ng < 1) {
        istat = grd_utils_ptr->grd_bilin_interp (x, y, z, npts,
                                  grid, ncol, nrow, 1,
                                  Xmin, Ymin, Xmax, Ymax);
        return istat;
    }

/*
    Calculate the corners of the work grid.
*/
    x1 = j1 * Xspace + Xmin;
    y1 = i1 * Yspace + Ymin;
    x2 = j2 * Xspace + Xmin;
    y2 = i2 * Yspace + Ymin;

/*
    fill in the null and faulted out values in the work grid.
*/
    if (nb > 0) {
        istat = FillInNulls (bwork, 4, 4);
        if (istat == -1) {
            return -1;
        }
    }


/*
    The work grid can now be treated as a non faulted grid
    for interpolation purposes.
*/
    istat = grd_utils_ptr->grd_bilin_interp (x, y, z, npts,
                              bwork, 4, 4, 1,
                              x1, y1, x2, y2);

    return istat;

}  /*  end of function grd_faulted_bilin_interp  */



/*
  ****************************************************************

        g r d _ f a u l t e d _ b i c u b _ i n t e r p _ 2

  ****************************************************************

    This function calls grd_faulted_bicub_interp for each point in
  the specified array.  The x and y coordinates of the array points
  are used as the reference points for the interpolation.  In other
  words, no grid nodes across faults from the point will be used in
  interpolating the surface at the point.

*/

int CSWGrdFault::grd_faulted_bicub_interp_2 (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                                int kcell,
                                CSW_F *x, CSW_F *y, CSW_F *z, int npts)
{
    int              i, ir, jc, istat, iref, n;
    CSW_F            xt, yt, xref, yref;
    CSW_F            *xa = p_d2000, *ya = p_d2001, *za = p_d2002;
    int              *idone = p_i2000, *ia = p_i2001;

/*
    If no valid cell number has been specified, do the interpolation
    the hard slow way.
*/
    if (kcell < 0) {
        for (i=0; i<npts; i++) {


            if (x[i] > 1.e20) {
                z[i] = 1.e30f;
                continue;
            }

            ir = (int)((y[i] - Ymin) / (Yspace * Ncoarse));
            jc = (int)((x[i] - Xmin) / (Xspace * Ncoarse));
            ir *= Ncoarse;
            jc *= Ncoarse;

            if (ir < 0  ||  jc < 0  ||  ir > nrow-1  ||  jc > Ncol-1) {
                z[i] = 1.e30f;
                continue;
            }

            xt = x[i];
            yt = y[i];
            AdjustForFaultGrazing (&xt, &yt);
            istat = grd_faulted_bicub_interp (grid, ncol, nrow, nullval,
                                              xt, yt, ir, jc,
                                              &xt, &yt, z+i, 1);
            if (istat == -1) {
                return -1;
            }
        }
        return 1;
    }

/*
    If a cell number is passed, all of the points are
    inside or on the edge of that cell.  In this case,
    groups af points on the same fault side are interpolated
    together to speed up the process.
*/
    memset (idone, 0, 2000 * sizeof(int));
    ir = kcell / Ncol;
    jc = kcell % Ncol;

    for (i=0; i<npts; i++) {
        if (x[i] >= 1.e20) {
            z[i] = 1.e30f;
            idone[i] = 1;
            continue;
        }
    }

    for (;;) {

        iref = -1;
        for (i=0; i<npts; i++) {
            if (idone[i] == 1) continue;
            iref = i;
            break;
        }
        if (iref == -1) break;

        xref = x[iref];
        yref = y[iref];
        AdjustForFaultGrazing (&xref, &yref);
        xa[0] = xref;
        ya[0] = yref;
        ia[0] = iref;
        n = 1;

        for (i=iref+1; i<npts; i++) {
            if (idone[i] == 1) continue;
            xt = x[i];
            yt = y[i];
            istat = CheckForFaultGrazing (xt, yt);
            if (istat == 1) {
                ShortenVector (&xt, &yt, &xref, &yref);
            }
            istat = con_find_fault_intersection (xt, yt, xref, yref,
                                                 kcell, 1, &xt, &yt);
            if (istat == 0) {
                xa[n] = x[i];
                ya[n] = y[i];
                ia[n] = i;
                n++;
            }
        }

        istat = grd_faulted_bicub_interp (grid, ncol, nrow, nullval,
                                          xref, yref, ir, jc,
                                          xa, ya, za, n);
        if (istat == -1) {
            return -1;
        }

        for (i=0; i<n; i++) {
            z[ia[i]] = za[i];
            idone[ia[i]] = 1;
        }

    }

    return 1;

}  /*  end of function grd_faulted_bicub_interp_2  */






/*
  ****************************************************************

        g r d _ f a u l t e d _ b i l i n _ i n t e r p _ 2

  ****************************************************************

    This function calls grd_faulted_bilin_interp for each point in
  the specified array.  The x and y coordinates of the array points
  are used as the reference points for the interpolation.  In other
  words, no grid nodes across faults from the point will be used in
  interpolating the surface at the point.

*/

int CSWGrdFault::grd_faulted_bilin_interp_2 (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                                CSW_F *x, CSW_F *y, CSW_F *z, int npts)
{
    int              i, ir, jc, istat;
    CSW_F            xt, yt;

    for (i=0; i<npts; i++) {

        if (x[i] > 1.e20f) {
            z[i] = 1.e30f;
            continue;
        }

        ir = (int)((y[i] - Ymin) / (Yspace * Ncoarse));
        jc = (int)((x[i] - Xmin) / (Xspace * Ncoarse));
        ir *= Ncoarse;
        jc *= Ncoarse;

        if (ir < 0  ||  jc < 0  ||  ir > nrow-1  ||  jc > Ncol-1) {
            z[i] = 1.e30f;
            continue;
        }

        xt = x[i];
        yt = y[i];
        AdjustForFaultGrazing (&xt, &yt);
        istat = grd_faulted_bilin_interp (grid, ncol, nrow, nullval,
                                          xt, yt, ir, jc,
                                          &xt, &yt, z+i, 1);
        if (istat == -1) {
            return -1;
        }
    }

    return 1;

}  /*  end of function grd_faulted_bilin_interp_2  */




/*
  ****************************************************************************

        g r d _ b a c k _ i n t e r p o l a t e _ f a u l t e d

  ****************************************************************************

*/

int CSWGrdFault::grd_back_interpolate_faulted (CSW_F *grid, int ncol, int nrow,
                                  CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                                  CSW_F *x, CSW_F *y, CSW_F *z,
                                  int npts, int flag)
{
    int                    istat, iflag;

/*
    If there are less than 4 columns or less than 4 rows,
    a faulted interpolation is not possible.  In this case,
    do a non faulted interpolation instead.
*/
    iflag = -flag;
    if (ncol < 4  ||  nrow < 4) iflag = flag;

    istat = grd_arith_ptr->grd_back_interpolate (grid, ncol, nrow,
                                  x1, y1, x2, y2,
                                  NULL, 0,
                                  x, y, z, npts,
                                  iflag);

    return istat;

}  /*  end of function grd_back_interpolate_faulted  */




/*
  ****************************************************************************

               g r d _ s e t _ f a u l t _ e n d _ f l a g

  ****************************************************************************

*/

int CSWGrdFault::grd_set_fault_end_flag (int val)
{
    EndFlag = val;

    return 1;

}  /*  end of function grd_set_fault_end_flag  */




/*
  ****************************************************************************

                    B u i l d E n d P o i n t I n d e x

  ****************************************************************************

*/

int CSWGrdFault::BuildEndPointIndex (void)
{
    int               i, ir, jc, k, istat, start;
    CSW_F             x, y;

    if (EndPointList) csw_Free (EndPointList);

    MaxEndPoints = 0;
    NumEndPoints = 0;
    EndPointList = NULL;

    for (i=0; i<NumFaultVectors; i++) {

        if (FaultVectors[i].flag == OUTSIDE_GRID_FLAG) {
            continue;
        }

        if (FaultVectors[i].endflag1) {
            x = FaultVectors[i].x1;
            y = FaultVectors[i].y1;
            ir = (int)((y - Ymin) / Yspace);
            jc = (int)((x - Xmin) / Xspace);
            k = ir * Ncol + jc;
            start = IndexGrid1[k];
            if (start == -1) {
                istat = AddEndPoint (k, x, y, i, 1);
                if (istat == -1) {
                    return -1;
                }
            }
            else if (IndexLink[start].next == -1) {
                istat = AddEndPoint (k, x, y, i, 1);
                if (istat == -1) {
                    return -1;
                }
            }
        }
        if (FaultVectors[i].endflag2) {
            x = FaultVectors[i].x2;
            y = FaultVectors[i].y2;
            ir = (int)((y - Ymin) / Yspace);
            jc = (int)((x - Xmin) / Xspace);
            k = ir * Ncol + jc;
            start = IndexGrid1[k];
            if (start == -1) {
                istat = AddEndPoint (k, x, y, i, 2);
                if (istat == -1) {
                    return -1;
                }
            }
            else if (IndexLink[start].next == -1) {
                istat = AddEndPoint (k, x, y, i, 2);
                if (istat == -1) {
                    return -1;
                }
            }
        }

    }

    return 1;

}  /*  end of private BuildEndPointIndex function  */






/*
  ****************************************************************************

                           A d d E n d P o i n t

  ****************************************************************************

*/

int CSWGrdFault::AddEndPoint (int k, CSW_F x, CSW_F y, int i, int endnum)
{
    ENdPoint          *eptr;

    if (EndPointIndex[k] != -1) return 1;

    if (NumEndPoints >= MaxEndPoints) {
        MaxEndPoints += INDEX_CHUNK;
        EndPointList = (ENdPoint *)csw_Realloc (EndPointList,
                                                MaxEndPoints * sizeof(ENdPoint));
        if (!EndPointList) {
            return -1;
        }
    }

    EndPointIndex[k] = NumEndPoints;
    eptr = EndPointList + NumEndPoints;
    eptr->x = x;
    eptr->y = y;
    eptr->endnum = endnum;
    eptr->vecnum = i;
    NumEndPoints++;

    return 1;

}  /*  end of private AddEndPoint function  */





/*
  ****************************************************************************

              g r d _ g e t _ f a u l t _ e n d _ p o i n t

  ****************************************************************************

*/

int CSWGrdFault::grd_get_fault_end_point (int k, CSW_F *x, CSW_F *y)
{
    int             i;

    *x = 1.e30f;
    *y = 1.e30f;

    if (EndPointIndex[k] == -1) {
        return 0;
    }

    i = EndPointIndex[k];
    *x = EndPointList[i].x;
    *y = EndPointList[i].y;

    return 1;

}  /*  end of function grd_get_fault_end_point  */




/*
  ****************************************************************************

              c o n _ c o r r e c t _ n a r r o w _ g r i d

  ****************************************************************************

    The bicubic and bilinear interpolations used to contour next to faults
   need at least 4 by 4 grid cells.  If the grid has less than 4 rows or
   less than 4 columns, a special interpolation is done to make the grid
   have at least 4 columns and 4 rows.

    This is a crude interpolation.  Calculating the grid using more columns
   and/or rows will produce better results.

*/

int CSWGrdFault::con_correct_narrow_grid (CSW_F **newgrid, int *newncol, int *newnrow)
{
    int           i, j, k, iold, jold, kold,
                  offset, oldoffset, newcol, newrow;
    CSW_F         xnew, ynew, zt,
                  xspnew, yspnew, *ngrid = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (ngrid);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    *newgrid = NULL;
    *newncol = 0;
    *newnrow = 0;

    newcol = Ncol;
    newrow = Nrow;
    if (Ncol < 4) {
        newcol = 4;
    }
    if (Nrow < 4) {
        newrow = 4;
    }

    xspnew = (Xmax - Xmin) / (CSW_F)(newcol - 1);
    yspnew = (Ymax - Ymin) / (CSW_F)(newrow - 1);

    ngrid = (CSW_F *)csw_Malloc (newcol * newrow * sizeof(CSW_F));
    if (!ngrid) {
        return -1;
    }

    for (i=0; i<newrow; i++) {
        ynew = Ymin + yspnew * i;
        offset = i * newcol;
        iold = (int)((ynew - Ymin + Yspace / 2.0f) / Yspace);
        oldoffset = iold * Ncol;
        for (j=0; j<newcol; j++) {
            xnew = Xmin + xspnew * j;
            k = offset + j;
            jold = (int)((xnew - Xmin + Xspace / 2.0f) / Xspace);
            kold = oldoffset + jold;
            AverageNeighbors (kold, xnew, ynew, &zt);
            ngrid[k] = zt;
        }
    }

    *newgrid = ngrid;
    *newncol = newcol;
    *newnrow = newrow;

    bsuccess = true;

    return 1;

}  /*  end of function con_correct_narrow_grid  */




/*
  ****************************************************************************

                   A v e r a g e N e i g h b o r s

  ****************************************************************************

    Do an inverse distance weighted average of any cells in the grid
  that are neighbors of the specified point.  This is only called from
  the con_correct_narrow_grid function.

*/

int CSWGrdFault::AverageNeighbors (int kcell, CSW_F x, CSW_F y, CSW_F *zout)
{
    int             istat, i, j, n, i1, i2, j1, j2, offset, k;
    CSW_F           za[20], zt, tiny, da[20], dt, dx, dy,
                    xt, yt, wdum;

    memset (za, 0, 20 * sizeof(CSW_F));
    memset (da, 0, 20 * sizeof(CSW_F));

    tiny = (Xspace + Yspace) / 20.0f;
    tiny *= tiny;

    i = kcell / Ncol;
    j = kcell % Ncol;

    i1 = i - 1;
    i2 = i + 1;
    j1 = j - 1;
    j2 = j + 1;

    if (i1 < 0) i1 = 0;
    if (i2 > Nrow - 1) i2 = Nrow - 1;
    if (j1 < 0) j1 = 0;
    if (j2 > Ncol - 1) j2 = Ncol - 1;

    n = 0;
    for (i=i1; i<=i2; i++) {
        offset = i * Ncol;
        yt = i * Yspace + Ymin;
        for (j=j1; j<=j2; j++) {
            k = offset + j;
            istat = grd_check_fault_blocking (i, j, x, y, &wdum);
            if (istat == 0) {
                za[n] = Grid[k];
                xt = j * Xspace + Xmin;
                dx = x - xt;
                dy = y - yt;
                dt = dx * dx + dy * dy;
                if (dt < tiny) {
                    *zout = Grid[k];
                    return 1;
                }
                da[n] = dt;
                n++;
            }
        }
    }

    if (n == 0) {
        *zout = Grid[kcell];
        return 1;
    }

    zt = 0.0f;
    dt = 0.0f;
    for (i=0; i<n; i++) {
        zt += za[i] / da[i];
        dt += 1.0f / da[i];
    }

    zt /= dt;
    *zout = zt;

    return 1;

}  /*  end of private AverageNeighbors function  */




/*
  ****************************************************************************

           g r d _ g e t _ c u r r e n t _ f a u l t _ s t r u c t s

  ****************************************************************************

*/

int CSWGrdFault::grd_get_current_fault_structs (FAultLineStruct **fault_lines,
                                   int *num_fault_lines)
{
    int                    i, j, n, nout, noutsav, np;
    FAultLineStruct        *fptr = NULL, *fptr2 = NULL, *flist = NULL;
    POint3D                *pptr;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            grd_free_fault_line_structs (flist, noutsav);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    *num_fault_lines = 0;
    *fault_lines = NULL;

    if (NumOrigFaultLines < 1) {
        return 0;
    }

    nout = 0;
    for (i=0; i<NumOrigFaultLines; i++) {
        if (OrigFaultLines[i].comp_points == NULL) {
            nout++;
        }
        else {
            nout += OrigFaultLines[i].ncomp;
        }
    }

    flist = (FAultLineStruct *)csw_Calloc (nout * sizeof(FAultLineStruct));
    if (!flist) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    noutsav = nout;

    nout = 0;
    for (i=0; i<NumOrigFaultLines; i++) {
        fptr = OrigFaultLines + i;
        if (fptr->comp_points == NULL) {
            fptr2 = flist + nout;
            memcpy (fptr2, fptr, sizeof(FAultLineStruct));
            n = fptr->num_points;
            fptr2->points = (POint3D *)csw_Malloc (n * sizeof(POint3D));
            if (!fptr2->points) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            memcpy (fptr2->points, fptr->points,
                    fptr->num_points*sizeof(POint3D));
            for (j=0; j<fptr2->num_points; j++) {
                pptr = fptr2->points + j;
                pptr->x -= XFaultShift;
                pptr->y -= YFaultShift;
            }
            nout++;
        }
        else {
            n = 0;
            for (j=0; j<fptr->ncomp; j++) {
                fptr2 = flist + nout;
                fptr2->ncomp = 0;
                fptr2->comp_points = NULL;
                np = fptr->comp_points[j];
                fptr2->num_points = np;
                fptr2->points = (POint3D *)csw_Malloc (np*sizeof(POint3D));
                if (!fptr2->points) {
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
                memcpy (fptr2->points, fptr->points+n,
                        np*sizeof(POint3D));
                for (j=0; j<fptr2->num_points; j++) {
                    pptr = fptr2->points + j;
                    pptr->x -= XFaultShift;
                    pptr->y -= YFaultShift;
                }
                n += np;
                nout++;
            }
        }
    }

    *num_fault_lines = nout;
    *fault_lines = flist;

    bsuccess = true;

    return 1;

}  /*  end of function grd_get_current_fault_structs  */



/*
  ****************************************************************************

            g r d _ g e t _ c u r r e n t _ f a u l t _ l i n e s

  ****************************************************************************

  Reconstruct the continuous fault lines from the fault vectors.  The points in
  the lines will still reflect any tweaking neeed to avoid grid nodes, etc,
  but they will be in a continuous line rather than individual vectors.

  The output is allocated here and ownership is passed back to the calling
  function.  The xlineout, ylineout and zlineout are all part of a single
  allocated block addressed at xlineout.  The ilineout block is allocated
  separately.  The calling function should csw_Free xlineout and ilineout and
  it should not csw_Free ylineout and zlineout.

*/

int CSWGrdFault::grd_get_current_fault_lines (
    double       **xlineout,
    double       **ylineout,
    double       **zlineout,
    int          **ilineout,
    int          *nlineout,
    double       avspace)
{
    int          i, n, n2, nlast, ntot, istat, id;
    double       *xline = NULL, *yline = NULL, *zline = NULL;
    int          *iline = NULL;
    FAultVector  *fvp = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xline);
            csw_Free (iline);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * initialize output in case of error
 */
    *xlineout = NULL;
    *ylineout = NULL;
    *zlineout = NULL;
    *ilineout = NULL;
    *nlineout = 0;

    if (NumOriginalFaultVectors < 1  ||
        OriginalFaultVectors == NULL) {
        return 0;
    }

/*
 * Allocate space for output.  The x,y and z are all in a single block.
 * The iline is in a separate block.  The calling function takes ownership
 * of these blocks and needs to csw_Free them when appropriate.
 */
    ntot = NumOriginalFaultVectors * 2 + 1;
    xline = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (xline == NULL) {
        return -1;
    }
    yline = xline + ntot;
    zline = yline + ntot;

    iline = (int *)csw_Malloc (ntot * sizeof(int));
    if (iline == NULL) {
        return -1;
    }

/*
 * Put coordinates into the output.
 */
    n = 0;
    n2 = 0;
    id = -1;
    nlast = 0;

    for (i=0; i<NumOriginalFaultVectors; i++) {
        fvp = OriginalFaultVectors + i;
        if (fvp->id != id) {
            if (n > 0) {
                iline[n2] = n - nlast;
                n2++;
                nlast = n;
            }

            xline[n] = fvp->x1;
            yline[n] = fvp->y1;
            zline[n] = 1.e30;
            n++;
            id = fvp->id;
        }
        xline[n] = fvp->x2;
        yline[n] = fvp->y2;
        zline[n] = 1.e30;
        n++;
    }

    iline[n2] = n - nlast;
    n2++;

/*
 * Resample if avspace is reasonable.
 */
    if (avspace > 0.0  &&  avspace < 1.e20) {
        istat =
        ResampleConstraintLines (
            &xline,
            &yline,
            &zline,
            iline,
            NULL,
            n2,
            avspace);
        if (istat == -1) {
            return -1;
        }
    }

/*
 * transfer to output pointers and return.
 */
    *xlineout = xline;
    *ylineout = yline;
    *zlineout = zline;
    *ilineout = iline;
    *nlineout = n2;

    bsuccess = true;

    return 1;

}



/*
  ****************************************************************************

             g r d _ f r e e _ f a u l t _ l i n e _ s t r u c t s

  ****************************************************************************

*/

int CSWGrdFault::grd_free_fault_line_structs (FAultLineStruct *flist, int nlist)
{
    int                    i;

    if (flist == NULL) return 1;

    for (i=0; i<nlist; i++) {
        csw_Free (flist[i].comp_points);
        csw_Free (flist[i].points);
        flist[i].comp_points = NULL;
        flist[i].points = NULL;
    }

    csw_Free (flist);

    return 1;

}  /*  end of function grd_free_fault_line_structs  */



/*
  ****************************************************************

           S e p a r a t e F a u l t V e c t o r s

  ****************************************************************

*/

int CSWGrdFault::SeparateFaultVectors (void)
{
    int           istat, n, i, j, nf, i1, i2;
    int           extend_first, extend_last;
    CSW_F         *x = NULL, *y = NULL;
    int           *npts = NULL, *linetypes = NULL, nlines;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (x);
        csw_Free (y);
        csw_Free (npts);
        csw_Free (linetypes);
        if (bsuccess == false) {
            grd_free_faults ();
            csw_Free (FaultVectors);
            FaultVectors = NULL;
            csw_Free (FaultVectorsUsed);
            FaultVectorsUsed = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    istat = grd_fault_structs_to_arrays (OrigFaultLines,
                                         NumOrigFaultLines,
                                         &x, &y, NULL,
                                         &npts, &linetypes,
                                         &nlines);
    if (istat == -1) {
        return -1;
    }

/*
 * Make sure no end points lie on a column or
 * row line of either grid geometry.
 */
    AdjustFaultsForGridGeometry (x, y, npts, nlines);

/*
    Count the points in the fault arrays
*/
    n = 0;
    for (i=0; i<nlines; i++) {
        n += npts[i];
    }

/*
    All of the static pointers should be NULL.  If this
    is not the case, then probably this function has been
    called more than once.  If this happens, the last call
    will define the faults.  I csw_Free all the pointers if
    they are not NULL in preparation for this.
*/
    if (FaultVectors) {
        grd_free_faults ();
    }

/*
    Allocate the fault data space.
*/
    NumFaultVectors = 0;
    MaxFaultVectors = n * 2;
    if (MaxFaultVectors < 100) MaxFaultVectors = 100;
MSL
    FaultVectors = (FAultVector *)csw_Malloc (MaxFaultVectors * sizeof(FAultVector));
    if (!FaultVectors) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    csw_Free (FaultVectorsUsed);
    FaultVectorsUsed = NULL;
    FaultVectorsUsed = (char *)csw_Calloc (MaxFaultVectors * sizeof(char));
    if (FaultVectorsUsed == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Fill in the FaultVectors array.
*/
    n = 0;
    nf = 0;
    for (i=0; i<nlines; i++) {
        i1 = n;
        i2 = n + npts[i] - 1;
        extend_first = 0;
        extend_last = 0;
        istat = CheckIfLineIsExtendable (x+i1, y+i1, npts[i],
                                         &extend_first, &extend_last);
        for (j=i1; j<i2; j++) {
            FaultVectors[nf].x1 = (CSW_F)(x[j] - XFaultShift);
            FaultVectors[nf].y1 = (CSW_F)(y[j] - YFaultShift);
            FaultVectors[nf].x2 = (CSW_F)(x[j+1] - XFaultShift);
            FaultVectors[nf].y2 = (CSW_F)(y[j+1] - YFaultShift);
            FaultVectors[nf].flag = 0;
            FaultVectors[nf].id = i;
            FaultVectors[nf].type = linetypes[i];
            FaultVectors[nf].endflag1 = 0;
            FaultVectors[nf].endflag2 = 0;
            FaultVectors[nf].used = 0;
            if (j == i1  &&  extend_first == 1) {
                FaultVectors[nf].endflag1 = 1;
            }
            if (j == i2-1  &&  extend_last == 1) {
                FaultVectors[nf].endflag2 = 1;
            }
            nf++;
        }
        n += npts[i];
    }

    NumFaultVectors = nf;

    bsuccess = true;

    return 1;

}  /*  end of private SeparateFaultVectors function  */





/*
  ****************************************************************************

                  C o p y F a u l t L i n e s T o O r i g

  ****************************************************************************

*/

int CSWGrdFault::CopyFaultLinesToOrig (FAultLineStruct *faults, int nfaults)
{
    FAultLineStruct      *fptr = NULL, *fptr2 = NULL;
    int                  i, i2, n, fsize, istat;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            grd_free_fault_line_structs (OrigFaultLines, nfaults);
            OrigFaultLines = NULL;
            NumOrigFaultLines = 0;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    I need to make sure that the new OrigFaultLines pointer is
    different from the old.  Thus, I allocate a temp pointer,
    then I csw_Free the old Orig pointer.
*/
    fsize = nfaults * sizeof(FAultLineStruct);
    fptr = (FAultLineStruct *)csw_Calloc (fsize);
    if (!fptr) {
        return -1;
    }

    if (OrigFaultLines) {
        grd_free_fault_line_structs (OrigFaultLines, NumOrigFaultLines);
        OrigFaultLines = NULL;
        NumOrigFaultLines = 0;
    }

    OrigFaultLines = fptr;
    fptr = NULL;

/*
 * only copy faults that have the type set to
 * GRD_DISCONTINUITY_CONSTRAINT
 */
    i2 = 0;
    for (i=0; i<nfaults; i++) {
        if (i == i2) continue;
        fptr = faults + i;
        if (fptr->lclass != GRD_DISCONTINUITY_CONSTRAINT) {
            continue;
        }
        fptr2 = OrigFaultLines + i2;
        n = fptr->num_points;
        fptr2->num_points = n;
        fptr2->id = fptr->id;
        fptr2->type = fptr->type;
        fptr2->lclass = fptr->lclass;
        fsize = n * sizeof(POint3D);
        fptr2->points = (POint3D *)csw_Malloc (fsize);
        if (!fptr2->points) {
            return -1;
        }
        memcpy (fptr2->points, fptr->points, fsize);
        fptr2->ncomp = fptr->ncomp;
        if (fptr->ncomp > 0) {
            fptr2->comp_points = (int *)csw_Malloc (fptr->ncomp * sizeof(int));
            if (!fptr2->comp_points) {
                return -1;
            }
            memcpy (fptr2->comp_points, fptr->comp_points, fptr->ncomp*sizeof(int));
        }
        else {
            fptr2->comp_points = NULL;
        }
        i2++;
    }

    NumOrigFaultLines = i2;

/*
 * If there are crossing faults, separate them into non crossing
 * subsets of the faults.
 */
    istat =
    FixCrossingFaults ();
    if (istat == -1) {
        return -1;
    }

    bsuccess = true;

    return 1;

}  /*  end of private CopyFaultLinesToOrig function  */





/*
  ****************************************************************

     g r d _ f a u l t _ s t r u c t s _ t o _ a r r a y s

  ****************************************************************

*/

int CSWGrdFault::grd_fault_structs_to_arrays (FAultLineStruct *fault_list, int nlist,
                                 CSW_F **xfault, CSW_F **yfault, CSW_F **zfault,
                                 int **fault_points, int **linetypes,
                                 int *nfault)
{
    int                 i, n2, nt, j, *fpts = NULL, *lpts = NULL;
    CSW_F               *xout = NULL, *yout = NULL, *zout = NULL;
    FAultLineStruct     *fptr = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xout);
            csw_Free (yout);
            csw_Free (zout);
            csw_Free (fpts);
            csw_Free (lpts);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (fault_list == NULL  ||  nlist < 1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (xfault == NULL  ||  yfault == NULL  ||
        fault_points == NULL  ||  nfault == NULL) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    Initialize the output stuff in case an error occurs.
*/
    *xfault = NULL;
    *yfault = NULL;
    *fault_points = NULL;
    *linetypes = NULL;
    *nfault = 0;

/*
    Count and allocate the points needed.
*/
    n2 = 0;
    for (i=0; i<nlist; i++) {
        n2 += fault_list[i].num_points;
    }

    xout = (CSW_F *)csw_Malloc (n2 * sizeof(CSW_F));
    if (!xout) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    yout = (CSW_F *)csw_Malloc (n2 * sizeof(CSW_F));
    if (!yout) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    zout = (CSW_F *)csw_Malloc (n2 * sizeof(CSW_F));
    if (!zout) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    fpts = (int *)csw_Malloc (nlist * sizeof(int));
    if (!fpts) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    lpts = (int *)csw_Malloc (nlist * sizeof(int));
    if (!lpts) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Transfer points to the arrays.
*/
    n2 = 0;
    for (i=0; i<nlist; i++) {
        fptr = fault_list + i;
        nt = fptr->num_points;
        fpts[i] = nt;
        lpts[i] = fptr->lclass;
        for (j=0; j<nt; j++) {
            xout[n2] = (CSW_F)fptr->points[j].x;
            yout[n2] = (CSW_F)fptr->points[j].y;
            zout[n2] = (CSW_F)fptr->points[j].z;
            n2++;
        }
    }

    *xfault = xout;
    *yfault = yout;
    if (zfault != NULL) {
        *zfault = zout;
    }
    else {
        csw_Free (zout);
        zout = NULL;
    }
    *fault_points = fpts;
    *linetypes = lpts;
    *nfault = nlist;

    bsuccess = true;

    return 1;

}  /*  end of function grd_fault_structs_to_arrays  */





/*
  ****************************************************************

        g r d _ f a u l t _ a r r a y s _ t o _ s t r u c t s

  ****************************************************************

*/

int CSWGrdFault::grd_fault_arrays_to_structs (CSW_F *xfault, CSW_F *yfault, CSW_F *zfault,
                                 int *fault_points, int *linetypes, int nfault,
                                 FAultLineStruct **fault_list, int *nlist)
{
    FAultLineStruct    *flist = NULL, *fptr = NULL;
    int                i, j, n, nt;
    int                more_than_one_index;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            grd_free_fault_line_structs (flist, nfault);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Check obvious errors.
*/
    if (xfault == NULL  ||  yfault == NULL  ||
        fault_points == NULL  ||  nfault < 1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (fault_list == NULL  ||  nlist == NULL) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    *fault_list = NULL;
    *nlist = 0;

/*
    Allocate an empty fault line list.  Calloc is
    used to insure that all points pointers are
    initially NULL.
*/
    flist = (FAultLineStruct *)csw_Calloc (nfault * sizeof(FAultLineStruct));
    if (!flist) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Allocate each POint3D pointer and fill in each structure.
*/
    n = 0;
    more_than_one_index = 0;
    for (i=0; i<nfault; i++) {
        nt = fault_points[i];

    /*
     *  do not use a fault line with less than 2 points.
     */
        if (nt < 2) {
            continue;
        }
        fptr = flist + more_than_one_index;
        fptr->lclass = GRD_DISCONTINUITY_CONSTRAINT;
        if (linetypes != NULL) {
            fptr->lclass = linetypes[i];
        }
        fptr->points = (POint3D *)csw_Malloc (nt * sizeof(POint3D));
        if (fptr->points == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        for (j=0; j<nt; j++) {
            fptr->points[j].x = (double)xfault[n];
            fptr->points[j].y = (double)yfault[n];
            if (zfault != NULL) {
                fptr->points[j].z = (double)zfault[n];
            }
            else {
                fptr->points[j].z = 1.e30;
            }
            n++;
        }
        fptr->num_points = nt;
        more_than_one_index++;
    }

    *fault_list = flist;
    *nlist = more_than_one_index;

    bsuccess = true;

    return 1;

}  /*  end of function grd_fault_arrays_to_structs  */





/*
  ****************************************************************************

                          F a u l t C l o s e d

  ****************************************************************************

*/

int CSWGrdFault::FaultClosed (FAultLineStruct *fptr)
{
    int                n;
    double             x1, y1, x2, y2;

    n = fptr->num_points - 1;
    x1 = fptr->points[0].x;
    y1 = fptr->points[0].y;
    x2 = fptr->points[n].x;
    y2 = fptr->points[n].y;

    if (x1 - x2 == 0.0  &&  y1 - y2 == 0.0) {
        fptr->type = GRD_NON_VERTICAL_FAULT;
        return 1;
    }

    return 0;

}  /*  end of private FaultClosed function  */




/*
  ****************************************************************************

                        U n i o n F a u l t s

  ****************************************************************************

    Union any polygon faults and rewrite the original fault line data to
  reflect the results of the union operation.

*/

int CSWGrdFault::UnionFaults (void)
{
    int                i, j, k, n, n2, ndo, istat,
                       maxpts, maxcomp, nstruct;
    int                norig, nc, nk, nk2, nj, nt, nv, nfw;
    FAultLineStruct    *fptr = NULL, *fwork = NULL;
    double             *xf1 = NULL, *yf1 = NULL,
                       *xf2 = NULL, *yf2 = NULL,
                       *xout = NULL, *yout = NULL;
    int                *npf1 = NULL, *npf2 = NULL, nf1, nf2,
                       *nvf1 = NULL, *nvf2 = NULL,
                       *npout = NULL, *nvout = NULL, nfout;
    char               *cenv = NULL;

    std::unique_ptr <CSWPolyGraph>
        ply_graph_obj {new CSWPolyGraph ()};


    auto fscope = [&]()
    {
        grd_free_fault_line_structs (fwork, nfw);
        csw_Free (xf1);
        csw_Free (npf1);
    };
    CSWScopeGuard func_scope_guard (fscope);

    if (NumOrigFaultLines < 1) {
        return 1;
    }

/*
    Allocate memory for the fault line structures that are closed polygons or
    that are flagged as non vertical faults.
*/
    fwork = (FAultLineStruct *)csw_Calloc (NumOrigFaultLines * sizeof(FAultLineStruct));
    if (!fwork) {
        return -1;
    }

/*
    Separate the polygon faults from the original fault lines.
*/
    n2 = 0;
    n = 0;
    norig = 0;
    for (i=0; i<NumOrigFaultLines; i++) {
        fptr = OrigFaultLines + i;
        if (fptr->type < GRD_VERTICAL_FAULT  ||
            fptr->type > GRD_BLOCK_POLYGON_FAULT) {
            fptr->type = GRD_UNKNOWN_FAULT;
        }

    /*
     * Bug 9255
     * Ignore faults with less than 2 points
     */
        if (fptr->num_points < 2) {
            norig++;
            continue;
        }

    /*
     * The fwork array here has shallow copies of the original structures.
     * Do not use the grd_free_fault_line_structs function to csw_Free these
     * shallow copies since that will csw_Free the points arrays while the
     * original structures still reference the points arrays.
     */
        if (fptr->type == GRD_NON_VERTICAL_FAULT  ||
            (fptr->type == GRD_UNKNOWN_FAULT  &&  FaultClosed (fptr))) {
            memcpy (fwork+n, fptr, sizeof(FAultLineStruct));
            n++;
            n2 += fptr->num_points;
        }
        else {
            norig++;
        }
    }

/*
    If there are less than 2 polygon faults, don't do anything.  The
    original fault line structures will be used for the remaining
    preprocessing of faults.
*/
    if (n < 2) {
        return 0;
    }
    ndo = n;
    n += 10;

/*
    Allocate memory for the polygon boolean operation.  I allocate
    extra memory here because really garbage polygons can require it.
*/
    nf1 = n;
    n2 += n;
    n2 += 10;

    xf1 = (double *)csw_Malloc (n2 * 16 * sizeof(double));
    if (!xf1) {
        return -1;
    }
    npf1 = (int *)csw_Malloc (n * 16 * sizeof(int));
    if (!npf1) {
        return -1;
    }

    maxpts = 6 * n2;
    maxcomp = 6 * n;
    yf1 = xf1 + n2;
    xf2 = yf1 + n2;
    yf2 = xf2 + n2;
    xout = yf2 + n2;
    yout = xout + maxpts;

    npf2 = npf1 + n;
    nvf1 = npf2 + n;
    nvf2 = nvf1 + n;
    npout = nvf2 + n;
    nvout = npout + maxcomp;

/*
    Build the input arrays for the polygon boolean operation.
    The polygons are arbitrarily separated into two groups and
    the groups are then unioned together.
*/
    nf1 = 0;
    nf2 = 0;
    n = 0;
    for (i=0; i<ndo/2; i++) {
        fptr = fwork + i;
        for (j=0; j<fptr->num_points; j++) {
            xf1[n] = fptr->points[j].x;
            yf1[n] = fptr->points[j].y;
            n++;
        }
        nvf1[nf1] = fptr->num_points;
        npf1[nf1] = 1;
        nf1++;
    }

    n = 0;
    for (i=ndo/2; i<ndo; i++) {
        fptr = fwork + i;
        for (j=0; j<fptr->num_points; j++) {
            xf2[n] = fptr->points[j].x;
            yf2[n] = fptr->points[j].y;
            n++;
        }
        nvf2[nf2] = fptr->num_points;
        npf2[nf2] = 1;
        nf2++;
    }

    cenv = csw_getenv ("POLYGON_CLIP_FILE");
    if (cenv) {
        WritePolygons (cenv,
                       xf1, yf1, nf1, npf1, nvf1,
                       xf2, yf2, nf2, npf2, nvf2);
    }

/*
    Do the union operation.
*/
    istat = ply_graph_obj->ply_boolean (xf1, yf1, NULL, nf1, npf1, nvf1,
                         xf2, yf2, NULL, nf2, npf2, nvf2,
                         PLY_UNION,
                         xout, yout, NULL, &nfout, npout, nvout,
                         maxpts, maxcomp);
    if (istat == -1) {
        return -1;
    }

/*
    Allocate another array of fault line structures
    that has enough structures for all the original
    non polygon faults and for the union results.

*/
    n = norig + nfout;
    nfw = n;
    csw_Free (fwork);
    fwork = NULL;
    fwork = (FAultLineStruct *)csw_Calloc (n * sizeof(FAultLineStruct));
    if (!fwork) {
        return -1;
    }

/*
    Copy all of the vertical and block polygon faults
    into the new fwork array.
*/

/*
 * The fwork array here has complete copies of the original structures.
 * The points arrays are copied and therefore the grd_free_fault_line_structs
 * function should be used to clean up if needed.
 */
    n = 0;
    for (i=0; i<NumOrigFaultLines; i++) {
        fptr = OrigFaultLines + i;
        if (fptr->type != GRD_NON_VERTICAL_FAULT) {
            memcpy (fwork+n, fptr, sizeof(FAultLineStruct));
            fwork[n].points = (POint3D *)csw_Malloc (fwork[n].num_points * sizeof(POint3D));
            fwork[n].comp_points = NULL;
            fwork[n].ncomp = 0;
            if (!fwork[n].points) {
                return -1;
            }
            memcpy (fwork[n].points, fptr->points, fptr->num_points*sizeof(POint3D));
            n++;
        }
    }
    nstruct = n;

/*
    Transfer the union results into the fwork array.
*/
    nk = 0;
    nj = 0;
    n2 = 0;
    for (i=0; i<nfout; i++) {
        nc = npout[i];
        fptr = fwork + nstruct;
        fptr->ncomp = nc;
        fptr->comp_points = (int *)csw_Malloc (nc * sizeof(int));
        if (!fptr->comp_points) {
            return -1;
        }
        nt = 0;
        for (j=0; j<nc; j++) {
            fptr->comp_points[j] = nvout[nj];
            nt += nvout[nj];
            nj++;
        }
        fptr->points = (POint3D *)csw_Malloc (nt * sizeof(POint3D));
        if (!fptr->points) {
            return -1;
        }
        nk2 = 0;
        for (j=0; j<nc; j++) {
            nv = nvout[n2];
            n2++;
            for (k=0; k<nv; k++) {
                fptr->points[nk2].x = xout[nk];
                fptr->points[nk2].y = yout[nk];
                fptr->points[nk2].z = 1.e30;
                nk++;
                nk2++;
            }
        }
        fptr->num_points = nk2;
        fptr->id = 0;
        fptr->type = GRD_NON_VERTICAL_FAULT;
        fptr->lclass = GRD_DISCONTINUITY_CONSTRAINT;
        nstruct++;
    }

    CopyFaultLinesToOrig (fwork, nstruct);

    return 1;

}  /*  end of private UnionFaults function  */





/*
  ****************************************************************************

                c o n _ n u l l _ i n s i d e _ f a u l t s

  ****************************************************************************

    Set any grid nodes that are inside a non vertical polygon fault to the
  FAULTED_OUT_VALUE.  The grd_build_fault_indices function must have been
  called prior to calling this function.  On success, 1 is returned.  A
  memory allocation failure returns -1.

*/

int CSWGrdFault::con_null_inside_faults (void)
{
    int                 istat, npts, i, j, k, offset, npoly,
                        *packpoly = NULL;
    int                 *inside = NULL, *inside2 = NULL;
    int                 *linetypes = NULL;
    double              yt, *xpts = NULL, *ypts = NULL,
                        *xpoly = NULL, *ypoly = NULL;
    FAultLineStruct     *fptr = NULL;

    CSWPolyUtils   ply_utils_obj;


    auto fscope = [&]()
    {
        csw_Free (xpts);
        csw_Free (inside);
        csw_Free (linetypes);
        csw_Free (xpoly);
        csw_Free (ypoly);
        csw_Free (packpoly);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Do nothing if the Grid hasn't been set up.
*/
    if (Grid == NULL  ||  Ncol < 1  ||  Nrow < 1) {
        return 1;
    }

/*
    Allocate memory for grid points and inside arrays
*/
    npts = Ncol * Nrow;
    xpts = (double *)csw_Malloc (2 * npts * sizeof(double));
    if (!xpts) {
        return -1;
    }
    inside = (int *)csw_Malloc (2 * npts * sizeof(int));
    if (!inside) {
        return -1;
    }

    ypts = xpts + npts;
    inside2 = inside + npts;

/*
    Put the grid node coordinates into the points arrays.
*/
    for (i=0; i<Nrow; i++) {
        offset = i * Ncol;
        yt = Ymin + i * Yspace;
        for (j=0; j<Ncol; j++) {
            k = offset + j;
            ypts[k] = yt;
            xpts[k] = Xmin + j * Xspace;
        }
    }

/*
    Initialize the inside array to a weird number that will not
    be returned from the polygon inside/outside function.
*/
    for (i=0; i<npts; i++) {
        inside[i] = 99;
    }

/*
    For each closed polygon in the OrigFaultLines list, set the inside
    array for the nodes inside that polygon to 1.
*/
    for (i=0; i<NumOrigFaultLines; i++) {
        fptr = OrigFaultLines + i;
        if (fptr->type != GRD_NON_VERTICAL_FAULT) {
            continue;
        }
        istat = grd_fault_structs_to_arrays_2 (fptr, 1,
                                               &xpoly, &ypoly, NULL,
                                               &packpoly, &linetypes, &npoly);
        if (istat == -1) {
            return -1;
        }

        memset (inside2, 0, npts*sizeof(int));

        istat = ply_utils_obj.ply_points (xpoly, ypoly, packpoly, npoly,
                            xpts, ypts, npts, inside2);
        if (istat == -1) {
            return -1;
        }

        for (j=0; j<npts; j++) {
            if (inside2[j] == 1) {
                inside[j] = 1;
            }
        }
    }

    for (i=0; i<npts; i++) {
        if (inside[i] == 1) {
            Grid[i] = FAULTED_OUT_VALUE;
        }
    }

    return 1;

}  /*  end of function con_null_inside_faults  */





/*
  ****************************************************************************

          c o n _ n u l l _ p o i n t s _ i n _ f a u l t s

  ****************************************************************************

    If a specified point is inside a fault, set its z value to 1.e33.

*/

int CSWGrdFault::con_null_points_in_faults (CSW_F *xin, CSW_F *yin, CSW_F *z, int npts)
{
    int                 istat, i, j, npoly, *packpoly = NULL;
    int                 *inside = NULL, *inside2 = NULL, *linetypes = NULL;
    double              *xpts = NULL, *ypts = NULL, *xpoly = NULL, *ypoly = NULL;
    FAultLineStruct     *fptr = NULL;

    CSWPolyUtils   ply_utils_obj;


    auto fscope = [&]()
    {
        csw_Free (xpts);
        csw_Free (inside);
        csw_Free (xpoly);
        csw_Free (ypoly);
        csw_Free (packpoly);
        csw_Free (linetypes);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Allocate memory for grid points and inside arrays
*/
    xpts = (double *)csw_Malloc (2 * npts * sizeof(double));
    if (!xpts) {
        return -1;
    }
    inside = (int *)csw_Malloc (2 * npts * sizeof(int));
    if (!inside) {
        return -1;
    }

    ypts = xpts + npts;
    inside2 = inside + npts;

/*
    Transfer the coordinates to the double arrays.
*/
    for (i=0; i<npts; i++) {
        xpts[i] = (double)xin[i];
        ypts[i] = (double)yin[i];
    }

/*
    Initialize the inside array to a weird number that will not
    be returned from the polygon inside/outside function.
*/
    for (i=0; i<npts; i++) {
        inside[i] = 99;
    }

/*
    For each closed polygon in the OrigFaultLines list, set the inside
    array for the points inside that polygon to 1.
*/
    for (i=0; i<NumOrigFaultLines; i++) {
        fptr = OrigFaultLines + i;
        if (fptr->type != GRD_NON_VERTICAL_FAULT) {
            continue;
        }
        istat = grd_fault_structs_to_arrays_2 (fptr, 1,
                                               &xpoly, &ypoly, NULL,
                                               &packpoly, &linetypes, &npoly);
        if (istat == -1) {
            return -1;
        }

        memset (inside2, 0, npts*sizeof(int));

        istat = ply_utils_obj.ply_points (xpoly, ypoly, packpoly, npoly,
                            xpts, ypts, npts, inside2);
        if (istat == -1) {
            return -1;
        }

        for (j=0; j<npts; j++) {
            if (inside2[j] == 1) {
                inside[j] = 1;
            }
        }
    }

    for (i=0; i<npts; i++) {
        if (inside[i] == 1) {
            z[i] = FAULTED_OUT_VALUE;
        }
    }

    return 1;

}  /*  end of function con_null_points_in_faults  */





/*
  ****************************************************************************

                c o n _ m a s k _ i n s i d e _ f a u l t s

  ****************************************************************************

    Return a mask where nodes inside non vertical faults are 1, on edge are zero
  and outside are -1.

*/

int CSWGrdFault::con_mask_inside_faults (char *mask)
{
    int                 istat, npts, i, j, k, offset, npoly,
                        *packpoly = NULL;
    int                 *inside = NULL, *inside2 = NULL,
                        *linetypes = NULL;
    double              yt, *xpts = NULL, *ypts = NULL,
                        *xpoly = NULL, *ypoly = NULL;
    FAultLineStruct     *fptr = NULL;

    CSWPolyUtils   ply_utils_obj;


    auto fscope = [&]()
    {
        csw_Free (xpts);
        csw_Free (inside);
        csw_Free (xpoly);
        csw_Free (ypoly);
        csw_Free (packpoly);
        csw_Free (linetypes);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Allocate memory for grid points and inside arrays
*/
    npts = Ncol * Nrow;
    xpts = (double *)csw_Malloc (2 * npts * sizeof(double));
    if (!xpts) {
        return -1;
    }
    inside = (int *)csw_Malloc (2 * npts * sizeof(int));
    if (!inside) {
        return -1;
    }

    ypts = xpts + npts;
    inside2 = inside + npts;

/*
    Put the grid node coordinates into the points arrays.
*/
    for (i=0; i<Nrow; i++) {
        offset = i * Ncol;
        yt = Ymin + i * Yspace;
        for (j=0; j<Ncol; j++) {
            k = offset + j;
            ypts[k] = yt;
            xpts[k] = Xmin + j * Xspace;
        }
    }

/*
    Initialize the inside array to a weird number that will not
    be returned from the polygon inside/outside function.
*/
    for (i=0; i<npts; i++) {
        inside[i] = 99;
    }

/*
    For each closed polygon in the OrigFaultLines list, set the inside
    array for the nodes inside that polygon to 1.
*/
    for (i=0; i<NumOrigFaultLines; i++) {
        fptr = OrigFaultLines + i;
        if (fptr->type != GRD_NON_VERTICAL_FAULT) {
            continue;
        }
        istat = grd_fault_structs_to_arrays_2 (fptr, 1,
                                               &xpoly, &ypoly, NULL,
                                               &packpoly, &linetypes, &npoly);
        if (istat == -1) {
            return -1;
        }

        memset (inside2, 0, npts*sizeof(int));

        istat = ply_utils_obj.ply_points (xpoly, ypoly, packpoly, npoly,
                            xpts, ypts, npts, inside2);
        if (istat == -1) {
            return -1;
        }

        for (j=0; j<npts; j++) {
            if (inside2[j] == 1) {
                inside[j] = 1;
            }
        }
    }

    for (i=0; i<npts; i++) {
        mask[i] = (char)inside[i];
    }

    return 1;

}  /*  end of function con_mask_inside_faults  */





/*
  ****************************************************************************

             g r d _ r e a d _ f a u l t _ l i n e _ f i l e

  ****************************************************************************

    Read an array of fault line structures from a text file.

*/

int CSWGrdFault::grd_read_fault_line_file
     (char *filename, FAultLineStruct **faults, int *nfaults)
{
    int                  istat, fsize, maxlines, nlines, nf, nf2,
                         j0, id, type, i, j, n, filenum = -1;
    int                  *ifault = NULL, *idlist = NULL, *typelist = NULL;
    char                 line[1000], *cbuf = NULL, *ctmp = NULL,
                         **lines = NULL;
    CSW_F                *xf = NULL, *yf = NULL, xt, yt;
    FAultLineStruct      *flist = NULL;

    CSWFileioUtil        fileio_util_obj;

    bool    bfclose = false;
    bool    bsuccess = false;

    auto fscope = [&]()
    {
        if (bfclose) fileio_util_obj.csw_CloseFile (filenum);
        csw_Free (cbuf);
        csw_Free (lines);
        csw_Free (xf);
        csw_Free (ifault);
        if (bsuccess == false) {
            grd_free_fault_line_structs (flist, nf);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Open the file.
*/
    filenum = fileio_util_obj.csw_OpenFile (filename, "r");
    if (filenum == -1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }
    bfclose = true;

/*
    Allocate a buffer to hold all the characters in the file.
*/
    fsize = fileio_util_obj.csw_FileSize (filenum);
    if (fsize < 1) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    cbuf = (char *)csw_Malloc ((fsize+10) * sizeof(char));
    if (!cbuf) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    read from the file into the buffer.
*/
    istat = fileio_util_obj.csw_BinFileRead (cbuf, fsize, 1, filenum);
    fileio_util_obj.csw_CloseFile (filenum);
    bfclose = false;
    if (istat <= 0) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    Allocate an array of pointers for each line in the
    buffer and separate the buffer into these single lines.
*/
    cbuf[fsize] = '\0';
    maxlines = fsize / 3;
    if (maxlines < 1000) maxlines = 1000;
    lines = (char **)csw_Calloc (maxlines * sizeof(char*));
    if (!lines) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    istat = csw_SeparateLines (cbuf, lines, &nlines, maxlines);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    Allocate space for x, y points and for the array containing
    the number of points in each fault.  Lists with fault type
    and fault id for each fault are also allocated here.
*/
    xf = (CSW_F *)csw_Malloc (2 * maxlines * sizeof(CSW_F));
    if (!xf) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    yf = xf + maxlines;

    ifault = (int *)csw_Malloc (maxlines * 3 * sizeof(int));
    if (!ifault) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    idlist = ifault + maxlines;
    typelist = idlist + maxlines;

/*
    Parse each line, separating out individual faults.
    The faults are separated by a blank line.  Any other
    blank lines in the file will screw it up.
*/
    n = 0;
    nf = 0;
    id = 0;
    type = 0;
    j = 0;
    j0 = 0;

    for (i=0; i<nlines; i++) {

        strcpy (line, lines[i]);
        csw_StrLeftJust (line);
        csw_StrLowerCase (line);

        if (line[0] == '#') continue;
        ctmp = strchr (line, '#');
        if (ctmp) {
            *ctmp = '\0';
        }

        if (line[0] == '\0') {
            if (type == 2  &&  n < 3) {
                j = j0;
                continue;
            }
            else if (n < 2) {
                j = j0;
                continue;
            }
            else {
                if (type == 2) {
                    if (xf[j0] - xf[j-1] != 0.0  ||
                        yf[j0] - yf[j-1] != 0.0) {
                        xf[j] = xf[j0];
                        yf[j] = yf[j0];
                        j++;
                        n++;
                    }
                }
                ifault[nf] = n;
                idlist[nf] = id;
                typelist[nf] = type;
                nf++;
                n = 0;
                type = 0;
                id = 0;
                j0 = j;
                continue;
            }
        }

        if (line[0] == 't') {
            ctmp = strchr (line, '=');
            if (ctmp) {
                istat = sscanf (ctmp+1, "%d", &type);
                if (istat != 1) {
                    type = 0;
                }
            }
        }
        else if (line[0] == 'i') {
            ctmp = strchr (line, '=');
            if (ctmp) {
                istat = sscanf (ctmp+1, "%d", &id);
                if (istat != 1) {
                    id = 0;
                }
            }
        }
        else {
            istat = sscanf (line, "%lf %lf", &xt, &yt);
            if (istat == 2) {
                xf[j] = xt;
                yf[j] = yt;
                j++;
                n++;
            }
        }
    }

/*
    We are done with the raw characters so they can be csw_Freed now.
*/
    csw_Free (cbuf);
    cbuf = NULL;
    csw_Free (lines);
    lines = NULL;

/*
    Convert to fault line structures.
*/
    istat = grd_fault_arrays_to_structs (xf, yf, NULL, ifault, NULL, nf,
                                         &flist, &nf2);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    If the number of fault line structures is not the same as the
    number of faults read, there is a problem with the data.
*/
    if (nf != nf2) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    Assign the id and type members of each fault line structure.
*/
    for (i=0; i<nf; i++) {
        flist[i].id = idlist[i];
        flist[i].type = typelist[i];
    }

    *faults = flist;
    *nfaults = nf;

    bsuccess = true;

    return 1;

}  /*  end of function grd_read_fault_line_file  */




/*
  ****************************************************************************

                 g r d _ f a u l t e d _ s m o o t h i n g

  ****************************************************************************

    This function does variable smoothing on a faulted grid.  Neighbor nodes
  adjacent to a center node and not across a fault are averaged for the
  smoothed value at the center node.

    The grid must be in memory from a call to grd_build_fault_indices.

*/

int CSWGrdFault::grd_faulted_smoothing (CSW_F **sgrid, int level)
{
    int                  ndo, ido, i, j, k, ii, jj, kk,
                         offset, offset2, faultflag, skip,
                         residflag, istat, i1, i2, j1, j2;
    CSW_F                *gwork = NULL, *gorig = NULL,
                         sum, sum2, xref, yref, wdum;
    CSW_F                *smgrid = NULL;
    CSW_F                nullflag, xt, yt;

    bool    bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (gwork);
        csw_Free (gorig);
        if (bsuccess == false && sgrid != NULL) {
            csw_Free (smgrid);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (Grid == NULL) {
        return 0;
    }

    residflag = 0;
    if (level < 0) {
        residflag = 1;
        level = -level;
    }

    ndo = level;
    skip = 1;
    gwork = (CSW_F *)csw_Malloc (Ncol * Nrow * sizeof(CSW_F));
    if (!gwork) {
        return -1;
    }

    gorig = NULL;
    if (residflag) {
        gorig = (CSW_F *)csw_Malloc (Ncol * Nrow * sizeof(CSW_F));
        if (!gorig) {
            return -1;
        }
        memcpy (gorig, Grid, Ncol*Nrow*sizeof(CSW_F));
    }

    smgrid = Grid;
    if (sgrid) {
        smgrid = (CSW_F *)csw_Malloc (Ncol * Nrow * sizeof(CSW_F));
        if (!smgrid) {
            return -1;
        }
        memcpy (smgrid, Grid, Ncol*Nrow*sizeof(CSW_F));
    }

    nullflag = 1.e19f;

/*
    Use multiple smoothing passes for higher smoothing levels.
*/
    for (ido=0; ido<ndo; ido++) {

        for (i=0; i<Nrow; i++) {

            offset = i * Ncol;
            yref = Ymin + i * Yspace;

            for (j=0; j<Ncol; j++) {

                k = offset + j;

                if (smgrid[k] > nullflag  ||  smgrid[k] < -nullflag) {
                    gwork[k] = smgrid[k];
                    continue;
                }

                i1 = i - skip;
                j1 = j - skip;
                if (i1 < 0) i1 = 0;
                if (j1 < 0) j1 = 0;

                i2 = i + skip;
                j2 = j + skip;
                if (i2 > Nrow - 1) i2 = Nrow - 1;
                if (j2 > Ncol - 1) j2 = Ncol - 1;

                faultflag = 0;
                if (ClosestFault[k] < skip*2) {
                    faultflag = 1;
                }
                xref = Xmin + j * Xspace;

                if (faultflag) {
                    istat = CheckForFaultGrazing (xref, yref);
                    if (istat != 0) {
                        gwork[k] = smgrid[k];
                        continue;
                    }
                }

            /*
                Average the non null neighbors of node k if they are
                on the same side of faults as node k.
            */
                sum = 0.0f;
                sum2 = 0.0f;
                for (ii=i1; ii<=i2; ii+=skip) {
                    offset2 = ii * Ncol;
                    yt = Ymin + ii * Yspace;
                    for (jj=j1; jj<=j2; jj+=skip) {
                        xt = Xmin + ii * Xspace;
                        if (faultflag) {
                            istat = CheckForFaultGrazing (xt, yt);
                            if (istat != 0) {
                                continue;
                            }
                        }
                        kk = offset2 + jj;
                        if (smgrid[kk] > nullflag  ||  smgrid[kk] < -nullflag) {
                            continue;
                        }
                        if (faultflag) {
                            istat = grd_check_fault_blocking (ii, jj, xref, yref, &wdum);
                            if (istat == 0) {
                                sum += smgrid[kk];
                                sum2 += 1.0f;
                            }
                        }
                        else {
                            sum += smgrid[kk];
                            sum2 += 1.0f;
                        }
                    }
                }

                if (sum2 > 0.0f) {
                    gwork[k] = sum / sum2;
                }
                else {
                    gwork[k] = smgrid[k];
                }
            }
        }

    /*
        Copy the work grid into the original grid and go to the next
        smoothing pass.
    */
        memcpy (smgrid, gwork, Ncol*Nrow*sizeof(CSW_F));

    }

    if (gorig) {
        for (i=0; i<Ncol*Nrow; i++) {
            smgrid[i] = gorig[i] - smgrid[i];
        }
    }

    if (sgrid) {
        *sgrid = smgrid;
    }

    bsuccess = true;

    return 1;

}  /*  end of function grd_faulted_smoothing  */





/*
  ****************************************************************************

             g r d _ r e s a m p l e _ f a u l t e d _ m a s k

  ****************************************************************************

    Resample a mask with faults.  The faults must have been indexed via a
  call to grd_build_fault_indices prior to calling this function.  The source
  grid geometry has also been defined by that function.

*/

int CSWGrdFault::grd_resample_faulted_mask (char *maskin, char *maskout, int ncout, int nrout,
                               CSW_F x1out, CSW_F y1out, CSW_F x2out, CSW_F y2out)
{
    int               i, j, k, ii, jj, kk, offset, offset2, istat,
                      i1, i2, j1, j2, nskip, imoff, kmoff, im, jm;
    CSW_F             yt, x1, y1, x2, y2, xref, yref, xdum,
                      xsp2, ysp2, tiny, xmt, ymt;

    if (maskin == NULL  ||  maskout == NULL) {
        return 1;
    }

    nskip = 1;

/*
    Calculate the spacings in the new resampled grid.
*/
    xsp2 = (x2out - x1out) / (CSW_F)(ncout - 1);
    ysp2 = (y2out - y1out) / (CSW_F)(nrout - 1);

/*
    tiny is used to insure that no nodes get left out
    due to precision problems.
*/
    tiny = (xsp2 + ysp2) / 200.0f;

    for (i=0; i<ncout*nrout; i++) {
        maskout[i] = 99;
    }

/*
    Loop through the original grid, defined in static variables
    Grid, Ncol, Nrow, Xmin, Ymin, Xmax, Ymax.  For each cell in
    the original grid, calculate x and y coordinates for all nodes
    in the new grid that lie inside the original grid cell.  The
    new grid mask is assigned based on the mask of the original
    nodes that are on the same side of faults as the original node.
*/
    for (i=0; i<Nrow-1; i+=nskip) {

        offset = i * Ncol;
        y1 = i * Yspace + Ymin;
        y2 = y1 + Yspace;
        y2 += tiny;
        i1 = (int)((y1 - y1out) / ysp2);
        i2 = (int)((y2 - y1out) / ysp2);
        if (i1 < 0) i1 = 0;
        if (i2 > nrout-1) i2 = nrout - 1;
        yref = Ymin + i * Yspace;

        for (j=0; j<Ncol-1; j+=nskip) {

            k = offset + j;
            x1 = j * Xspace + Xmin;
            x2 = x1 + Xspace * nskip;
            x2 += tiny;
            j1 = (int)((x1 - x1out) / xsp2);
            j2 = (int)((x2 - x1out) / xsp2);
            if (j1 < 0) j1 = 0;
            if (j2 > ncout-1) j2 = ncout - 1;
            xref = Xmin + j * Xspace;

        /*
            All input mask corners are the same.
        */
            if (maskin[k] == maskin[k+1]  &&
                maskin[k] == maskin[k+Ncol]  &&
                maskin[k] == maskin[k+Ncol+1]) {
                for (ii=i1; ii<=i2; ii++) {
                    offset2 = ii * ncout;
                    for (jj=j1; jj<=j2; jj++) {
                        kk = offset2 + jj;
                        maskout[kk] = maskin[k];
                    }
                }
                continue;
            }

        /*
            The input cell does not have a fault in it.
        */
            if (ClosestFault[k] > nskip) {
                for (ii=i1; ii<=i2; ii++) {
                    yt = y1out + ii * ysp2;
                    im = (int)((yt + tiny) / Yspace);
                    if (im < 0) im = 0;
                    if (im > Nrow-1) im = Nrow - 1;
                    imoff = im * Ncol;
                    kmoff = ii * ncout;
                    for (jj=j1; jj<=j2; jj++) {
                        xmt = x1out + jj * xsp2;
                        jm = (int)((xmt + tiny) / Xspace);
                        if (jm < 0) jm = 0;
                        if (jm > Ncol-1) jm = Ncol - 1;
                        maskout[kmoff+jj] = maskin[imoff+jm];
                    }
                }
                continue;
            }

        /*
            The input cell does have a fault in it.
        */
            for (ii=i1; ii<=i2; ii++) {
                ymt = y1out + ii * ysp2;
                offset2 = ii * ncout;
                for (jj=j1; jj<=j2; jj++) {
                    xmt = x1out + jj * xsp2;
                    kk = offset2 + jj;
                    if (CheckForFaultGrazing (xmt, ymt)) {
                        maskout[kk] = maskin[k];
                        continue;
                    }
                    if (CheckForFaultGrazing (xref, yref) == 0) {
                        istat = con_find_fault_intersection (xref, yref, xmt, ymt,
                                                             k, 1, &xdum, &xdum);
                        if (istat == 0) {
                            maskout[kk] = maskin[k];
                            continue;
                        }
                    }
                    if (CheckForFaultGrazing (xref+Xspace, yref) == 0) {
                        istat = con_find_fault_intersection (xref+Xspace, yref, xmt, ymt,
                                                             k, 1, &xdum, &xdum);
                        if (istat == 0) {
                            maskout[kk] = maskin[k+1];
                            continue;
                        }
                    }
                    if (CheckForFaultGrazing (xref+Xspace, yref+Yspace) == 0) {
                        istat = con_find_fault_intersection (xref+Xspace, yref+Yspace, xmt, ymt,
                                                             k, 1, &xdum, &xdum);
                        if (istat == 0) {
                            maskout[kk] = maskin[k+Ncol+1];
                            continue;
                        }
                    }
                    if (CheckForFaultGrazing (xref, yref+Yspace) == 0) {
                        istat = con_find_fault_intersection (xref, yref+Yspace, xmt, ymt,
                                                             k, 1, &xdum, &xdum);
                        if (istat == 0) {
                            maskout[kk] = maskin[k+Ncol];
                            continue;
                        }
                    }
                    maskout[kk] = maskin[k];
                }
            }
        }
    }

    im = 0;
    for (;;) {
        im++;
        jm = 0;
        for (i=0; i<nrout; i++) {
            offset = i * ncout;
            for (j=0; j<ncout; j++) {
                k = offset + j;
                if (maskout[k] == 99) {
                    jm = 1;
                    if (im > 10) {
                        maskout[k] = 1;
                        continue;
                    }
                    if (j < ncout-1  &&  maskout[k+1] != 99) {
                        maskout[k] = maskout[k+1];
                    }
                    else if (j > 0) {
                        if (maskout[k-1] != 99) {
                            maskout[k] = maskout[k-1];
                        }
                    }
                }
            }
        }
        if (im > 10) break;
    }

    return 1;

}  /*  end of function grd_resample_faulted_mask  */








/*
  ****************************************************************

                   E x p a n d L e v e l

  ****************************************************************

    Average the neighbors of null nodes to assign a value to the
  null nodes.  If the eflag parameter is non zero, then an extrapolative
  average is combined with the simple average.  Only nodes on the same
  side of a fault as the center node are used.

*/

int CSWGrdFault::ExpandLevel (CSW_F *grid1, CSW_F *grid2, int ncol, int nrow,
                        CSW_F nval, int flag)
{
    int          i, j, ii, jj, i1, i2, k, k2, istat,
                 nt, ndone;
    CSW_F        sum, wdum;

    ndone = 0;
    sum = 0.0f;

    for (i=0; i<nrow; i++) {

        i1 = ncol * i;

        for (j=0; j<ncol; j++) {

            k = i1 + j;
            nt = 0;
            if (grid1[k] > nval) {
                sum = 0.0f;
                for (ii=i-1; ii<=i+1; ii++) {
                    if (ii < 0  ||  ii >= nrow) continue;
                    i2 = ii * ncol;
                    for (jj=j-1; jj<=j+1; jj++) {
                        if (jj < 0  ||  jj >= ncol) continue;
                        k2 = i2 + jj;
                        if (grid1[k2] < nval) {
                            if (ClosestFault[k2] > 1) {
                                nt++;
                                sum += grid1[k2];
                            }
                            else {
                                istat = grd_check_grid_fault_blocking (j, i, jj, ii, &wdum);
                                if (istat == 0) {
                                    nt++;
                                    sum += grid1[k2];
                                }
                            }
                        }
                    }
                }
            }

            if (flag == 1  &&  sum < 0.0f) {
                sum = -sum;
            }
            else if (flag == -1  &&  sum > 0.0f) {
                sum = -sum;
            }
            if (nt > 0) {
                grid2[k] = sum / (CSW_F)nt;
                ndone++;
            }
            else {
                grid2[k] = grid1[k];
            }
        }
    }

    return ndone;

}  /*  end of private ExpandLevel function  */






/*
  ****************************************************************

           g r d _ f i l l _ f a u l t e d _ n u l l s

  ****************************************************************

    Fill in nulls in a grid by averaging the nearest non null values.

*/

int CSWGrdFault::grd_fill_faulted_nulls (CSW_F nullval)
{
    int           i, istat, nstat;
    int           ncol, nrow, flag;
    CSW_F         *grid1 = NULL, *grid2 = NULL, nval;
    CSW_F         *grid = NULL, *gridout = NULL;


    auto fscope = [&]()
    {
        csw_Free (grid1);
    };
    CSWScopeGuard func_scope_guard (fscope);


    grid = Grid;
    gridout = Grid;

    ncol = Ncol;
    nrow = Nrow;
    flag = 0;

/*
    allocate work space memory
*/
MSL
    grid1 = (CSW_F *)csw_Malloc (ncol * nrow * 2 * sizeof(CSW_F));
    if (!grid1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    grid2 = grid1 + ncol * nrow;

/*
    copy input grid to workspace
*/
    csw_memcpy ((char *)grid1, (char *)grid, ncol*nrow*sizeof(CSW_F));

/*
    If the specified null value is a "reasonable" number,
    then set it to 1.e20.
*/
    if (nullval > -1.e10  &&  nullval < 1.e10) {
        nullval = 1.e30f;
    }

/*
    make nulls positive if needed
*/
    if (nullval < 0.0) {
        nval = nullval / 10.0f;
        for (i=0; i<ncol*nrow; i++) {
            if (grid1[i] < nval) {
                grid1[i] = -nullval;
            }
        }
        nval = -nval;
    }
    else {
        nval = nullval / 10.0f;
        for (i=0; i<ncol*nrow; i++) {
            if (grid1[i] < -nval) {
                grid1[i] = nullval;
            }
        }
    }

/*
    Make sure at least one non null is in the input grid and
    make sure all null values in the initial work grid are
    greater than 1.e20 (needed for resampling later).
*/
    nstat = 0;
    for (i=0; i< ncol*nrow; i++) {
        if (grid1[i] < nval) {
            nstat++;
        }
    }

    if (nstat == 0) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    Fill in the nulls near faults by averaging the non null nearest
    neighbors on the same side of the fault.  Do this by expanding
    one level at a time.
*/
    nstat = 0;
    for (i=0; i<=20; i++) {

        istat = ExpandLevel (grid1, grid2, ncol, nrow, nval, flag);
        if (istat == 0) {
            nstat = 1;
            break;
        }

        istat = ExpandLevel (grid2, grid1, ncol, nrow, nval, flag);
        if (istat == 0) {
            nstat = 2;
            break;
        }

    }

    if (nstat == 1) {
        csw_memcpy ((char *)grid1, (char *)grid2, ncol*nrow*sizeof(CSW_F));
    }

/*
    Use the non faulted null filling to fill in the remaining nulls
    that are not near faults.
*/
    istat = grd_utils_ptr->grd_fill_nulls_new (grid1, ncol, nrow, nullval,
                                grid2, NULL, 0);
    if (istat == -1) {
        return -1;
    }

/*
    copy the grid2 work grid to gridout and return
*/
    csw_memcpy ((char *)gridout, (char *)grid2, ncol*nrow*sizeof(CSW_F));

    return 1;

}  /*  end of function grd_fill_faulted_nulls  */




/*
  ****************************************************************************

                   g r d _ c o m b i n e _ f a u l t s

  ****************************************************************************

    Combine two fault line arrays into a third array.

*/

int CSWGrdFault::grd_combine_faults (FAultLineStruct *faults1, int nfaults1,
                        FAultLineStruct *faults2, int nfaults2,
                        FAultLineStruct **faultsout, int *nfaultsout)
{
    int                istat, ntot, nfmax;
    FAultLineStruct    *flist, *ftmp;


    auto fscope = [&]()
    {
        grd_free_fault_line_structs (flist, nfmax);
    };
    CSWScopeGuard func_scope_guard (fscope);


    *faultsout = NULL;
    *nfaultsout = 0;

    if (faults1 == NULL  ||  faults2 == NULL  ||
        nfaults1 < 1  ||  nfaults2 < 1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (faultsout == NULL  ||  nfaultsout == NULL) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    ntot = nfaults1 + nfaults2;
    nfmax = ntot;
    flist = (FAultLineStruct *)csw_Calloc (ntot * sizeof(FAultLineStruct));
    if (!flist) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    istat = CopyFaultLines (faults1, nfaults1, flist);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    ftmp = flist + nfaults1;
    istat = CopyFaultLines (faults2, nfaults2, ftmp);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    istat = RemoveDuplicateFaults (flist, ntot,
                                   faultsout, nfaultsout);

    return istat;

}  /*  end of function grd_combine_faults  */



/*
 ****************************************************************************

          g r d _ c o p y _ f a u l t _ l i n e _ s t r u c t s

 ****************************************************************************

*/

FAultLineStruct* CSWGrdFault::grd_copy_fault_line_structs (FAultLineStruct *in, int n)
{
    FAultLineStruct    *out = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (out);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (in == NULL  ||  n < 1) {
        return NULL;
    }

    out = (FAultLineStruct *)csw_Calloc (n * sizeof(FAultLineStruct));
    if (out == NULL) {
        return NULL;
    }

    CopyFaultLines (in, n, out);

    bsuccess = true;

    return out;

}  /* end of grd_copy_fault_line_structs function */




/*
  ****************************************************************************

                        C o p y F a u l t L i n e s

  ****************************************************************************

*/

int CSWGrdFault::CopyFaultLines (FAultLineStruct *faults, int nfaults,
                           FAultLineStruct *fout)
{
    FAultLineStruct      *fptr = NULL, *fptr2 = NULL;
    int                  i, n, fsize;

    fsize = nfaults * sizeof(FAultLineStruct);

    for (i=0; i<nfaults; i++) {
        fptr = faults + i;
        fptr2 = fout + i;
        n = fptr->num_points;
        fptr2->num_points = n;
        fptr2->id = fptr->id;
        fptr2->type = fptr->type;
        fptr2->lclass = fptr->lclass;
        fsize = n * sizeof(POint3D);
        fptr2->points = (POint3D *)csw_Malloc (fsize);
        if (!fptr2->points) {
            return -1;
        }
        memcpy (fptr2->points, fptr->points, fsize);
        fptr2->ncomp = fptr->ncomp;
        if (fptr->ncomp > 0) {
            fptr2->comp_points = (int *)csw_Malloc (fptr->ncomp * sizeof(int));
            if (!fptr2->comp_points) {
                return -1;
            }
            memcpy (fptr2->comp_points, fptr->comp_points, fptr->ncomp*sizeof(int));
        }
        else {
            fptr2->comp_points = NULL;
        }
    }

    return 1;

}  /*  end of private CopyFaultLines function  */






/*
  ****************************************************************************

                         W r i t e P o l y g o n s

  ****************************************************************************

*/

int CSWGrdFault::WritePolygons (char *fname,
                          double *xp1, double *yp1, int np1, int *nc1, int *nv1,
                          double *xp2, double *yp2, int np2, int *nc2, int *nv2)
{
    int                i, j, k, n, np;
    FILE               *fileptr;

    fileptr = fopen (fname, "w+");
    if (!fileptr) {
        return -1;
    }

    np = 0;
    n = 0;
    for (i=0; i<np1; i++) {
        for (j=0; j<nc1[i]; j++) {
            for (k=0; k<nv1[n]; k++) {
                fprintf (fileptr, "%15.10g %15.10g\n", xp1[np], yp1[np]);
                np++;
            }
            if (j < nc1[i]-1) {
                fprintf (fileptr, "c\n");
            }
            n++;
        }
        fprintf (fileptr, "\n");
    }

    fprintf (fileptr, "\n");

    np = 0;
    n = 0;
    for (i=0; i<np2; i++) {
        for (j=0; j<nc2[i]; j++) {
            for (k=0; k<nv2[n]; k++) {
                fprintf (fileptr, "%15.10g %15.10g\n", xp2[np], yp2[np]);
                np++;
            }
            if (j < nc2[i]-1) {
                fprintf (fileptr, "c\n");
            }
            n++;
        }
        fprintf (fileptr, "\n");
    }

    fclose (fileptr);

    return 1;

}  /*  end of private WritePolygons function  */




/*
  ****************************************************************************

                    g r d _ f a u l t e d _ h g r a d

  ****************************************************************************

    Calculate the horizontal gradient slope and direction grids for a faulted
  surface.  The fault vectors and indices to them must have been set up via
  calls to grd_define_fault_vectors and grd_build_fault_indices prior to
  calling this function.

*/

int CSWGrdFault::grd_faulted_hgrad (CSW_F *slope_grid, CSW_F *direction_grid)
{
    int                i, j, i1, i2, j1, j2, off0, off1, off2;
    int                k, istat, if1, if2, jf1, jf2;
    CSW_F              rad2deg, savg, xfact, yfact, dx, dy, dt, wdum;


/*
    First, get x and y spacing and the ratio between
    each and the average spacing.
*/
    savg = Xspace + Yspace;
    xfact = Xspace / savg;
    yfact = Yspace / savg;

    rad2deg = 180.0f / 3.141926f;

/*
    For each node, use the nodes above, below, left and
    right of it to calculate the slope and direction.
*/
    for (i=0; i<Nrow; i++) {

        i1 = i - 1;
        i2 = i + 1;
        if (i1 < 0) i1 = 0;
        if (i2 > Nrow-1) i2 = Nrow-1;

        off0 = i * Ncol;
        off1 = i1 * Ncol;
        off2 = i2 * Ncol;

        for (j=0; j<Ncol; j++) {

            k = off0 + j;
            if (ClosestFault[k] > 1) {
                j1 = j - 1;
                j2 = j + 1;
                if (j1 < 0) j1 = 0;
                if (j2 > Ncol-1) j2 = Ncol-1;

                dx = Grid[off0+j2] - Grid[off0+j1];
                if (j1 == j) dx *= 2.0f;
                if (j2 == j) dx *= 2.0f;
                dy = Grid[off2+j] - Grid[off1+j];
                if (i1 == i) dy *= 2.0f;
                if (i2 == i) dy *= 2.0f;

                dx *= xfact;
                dy *= yfact;

                dt = dx * dx + dy * dy;
                slope_grid[k] = (CSW_F)sqrt ((double)dt);

                if (dt > 0.0) {
                    dt = (CSW_F)atan2 ((double)dy, (double)dx);
                }
                if (dt < 0.0f) dt += 6.2831852f;
                dt *= rad2deg;
                direction_grid[k] = dt;
            }
            else {
                istat = grd_check_grid_fault_blocking (j-1, i, j, i, &wdum);
                if (istat == 0) {
                    jf1 = j-1;
                }
                else {
                    jf1 = j;
                }
                istat = grd_check_grid_fault_blocking (j+1, i, j, i, &wdum);
                if (istat == 0) {
                    jf2 = j+1;
                }
                else {
                    jf2 = j;
                }
                istat = grd_check_grid_fault_blocking (j, i-1, j, i, &wdum);
                if (istat == 0) {
                    if1 = i-1;
                }
                else {
                    if1 = i;
                }
                istat = grd_check_grid_fault_blocking (j, i+1, j, i, &wdum);
                if (istat == 0) {
                    if2 = i+1;
                }
                else {
                    if2 = i;
                }

                dx = Grid[off0+jf2] - Grid[off0+jf1];
                if (jf1 == j) dx *= 2.0f;
                if (jf2 == j) dx *= 2.0f;
                dy = Grid[if2*Ncol+j] - Grid[if1*Ncol+j];
                if (if1 == i) dy *= 2.0f;
                if (if2 == i) dy *= 2.0f;

                dx *= xfact;
                dy *= yfact;

                dt = dx * dx + dy * dy;
                slope_grid[k] = (CSW_F)sqrt ((double)dt);

                if (dt > 0.0) {
                    dt = (CSW_F)atan2 ((double)dy, (double)dx);
                }
                if (dt < 0.0f) dt += 6.2831852f;
                dt *= rad2deg;
                direction_grid[k] = dt;

            }

        }

    }

    return 1;

}  /*  end of function grd_faulted_hgrad  */







/*
  ****************************************************************

   g r d _ d e f i n e _ a n d _ s h i f t _ f a u l t _ v e c t o r s

  ****************************************************************

  Copy the fault line structures into the local arrays for them.
  When the fault indices are built, the faults are clipped to the
  area of interest, polygon faults are unioned and other needed
  processing is done.

*/

int CSWGrdFault::grd_define_and_shift_fault_vectors (FAultLineStruct *faults, int nfaults,
                                        double x, double y)
{
    int                  istat;
    int                  do_write;
    CSW_F                *fx, *fy;
    int                  i, nlines, *npts, *linetypes;
    int                  *nc = p_i2000;

    XFaultShift = x;
    YFaultShift = y;

    do_write = csw_GetDoWrite ();
    if (do_write) {
        istat =
            grd_fault_structs_to_arrays (faults, nfaults,
                                         &fx, &fy, NULL,
                                         &npts, &linetypes,
                                         &nlines);
        if (istat == -1) {
            return -1;
        }
        if (nlines > 1000) nlines = 1000;
        for (i=0; i<nlines; i++) {
            nc[i] = 1;
        }
        ShiftXY (fx, fy, nlines, npts);
        grd_fileio_ptr->grd_write_float_lines (fx, fy, NULL,
                        nlines, nc, npts,
                        (char *)"defined_faults.xyz");
        csw_Free (fx);
        csw_Free (npts);
    }

    grd_free_faults ();
    istat = CopyFaultLinesToOrig (faults, nfaults);

    return istat;

}  /*  end of function grd_define_fault_vectors  */



/*
  Add the specified x and y to fault coordinates and return
  a new set of fault structs reflecting the shift.
*/

int CSWGrdFault::grd_shift_fault_vectors (FAultLineStruct *faults, int nfaults,
                             double x, double y,
                             FAultLineStruct **fault_out, int *nfault_out)
{
    int                  istat;
    CSW_F                *fx, *fy, *fz;
    FAultLineStruct      *fout;
    int                  nfout;
    int                  nlines, *npts, *linetypes;
    double               xsav, ysav;

    istat =
        grd_fault_structs_to_arrays (faults, nfaults,
                                     &fx, &fy, &fz,
                                     &npts, &linetypes,
                                     &nlines);
    if (istat == -1) {
        return -1;
    }

    xsav = XFaultShift;
    ysav = YFaultShift;
    XFaultShift = x;
    YFaultShift = y;

    ShiftXY (fx, fy, nlines, npts);

    XFaultShift = xsav;
    YFaultShift = ysav;

    istat =
        grd_fault_arrays_to_structs (fx, fy, fz,
                                     npts, linetypes, nlines,
                                     &fout, &nfout);
    csw_Free (fx);
    csw_Free (fy);
    csw_Free (fz);
    csw_Free (npts);
    csw_Free (linetypes);

    if (istat == -1) {
        return -1;
    }

    *fault_out = fout;
    *nfault_out = nfout;

    return 1;

}  /*  end of function grd_shift_fault_vectors  */




/*
  ****************************************************************************

                 C h e c k F o r F a u l t G r a z i n g 2

  ****************************************************************************

  Check if the specified x,y point is almost exactly on top of a fault.
  Return the fault number + 1 if it is or zero if it isn't.

*/

int CSWGrdFault::CheckForFaultGrazing2 (double x, double y)
{
    int               i, j, r1, r2, c1, c2, istat;
    int               *vecnums, nvec;
    double            xy[10], dist;

    r1 = (int)((y - Ymin) / Yspace);
    r2 = r1;
    c1 = (int)((x - Xmin) / Xspace);
    c2 = c1;

    r1--;
    c1--;
    if (r1 < 0) r1 = 0;
    if (c1 < 0) c1 = 0;

    istat = GetVectors (r1, r2, c1, c2, &vecnums, &nvec);
    if (istat == -1) {
        return -1;
    }
    if (nvec < 1  ||  vecnums == NULL) {
        return 0;
    }

    for (i=0; i<nvec; i++) {
        j = vecnums[i];
        if (FaultVectors[j].flag == OUTSIDE_GRID_FLAG) {
            continue;
        }
        xy[0] = FaultVectors[j].x1;
        xy[1] = FaultVectors[j].y1;
        xy[2] = FaultVectors[j].x2;
        xy[3] = FaultVectors[j].y2;
        gpf_calclinedist2 (xy, 2, x, y, &dist);
        if (dist < NodeOffset) {
            return j + 1;
        }
    }

    return 0;

}  /*  end of private CheckForFaultGrazing2 function  */



/*
 *************************************************************************

        g r d _ s e t _ g r a z i n g _ g r i d _ n o d e s

 *************************************************************************

  You must call grd_build_fault_indices with the grid and geometry
  prior to calling this function.

*/

void CSWGrdFault::grd_set_grazing_grid_nodes (void)
{
    int                 i, j, k, offset, istat;
    double              dsav, hardchk, xt, yt;

    if (Grid == NULL  ||  ClosestFault == NULL) return;

    dsav = NodeOffset;
    NodeOffset *= 20.0;
    hardchk = GRD_HARD_NULL_VALUE / 100.0;

    for (i=0; i<Nrow; i++) {
        yt = Ymin + i * Yspace;
        offset = i * Ncol;
        for (j=0; j<Ncol; j++) {
            k = offset + j;
            if (Grid[k] > hardchk) continue;
            if (ClosestFault[k] > 0) continue;
            xt = Xmin + j * Xspace;
            istat = CheckForFaultGrazing2 (xt, yt);
            if (istat != 0) {
                Grid[k] = (CSW_F)GRD_SOFT_NULL_VALUE;
            }
        }
    }

    NodeOffset = dsav;

    return;

}  /* end of function grd_set_grazing_grid_nodes */





/*
 *************************************************************************

        g r d _ s e t _ g r a z i n g _ t r i m e s h _ n o d e s

 *************************************************************************

*/

void CSWGrdFault::grd_set_grazing_trimesh_nodes (NOdeStruct *nodes, int numnodes)
{
    int                 i, istat;
    double              dsav;
    //double              hardchk;

    dsav = NodeOffset;
    NodeOffset *= 20.0;
    //hardchk = GRD_HARD_NULL_VALUE / 100.0;

    for (i=0; i<numnodes; i++) {
        if (nodes[i].deleted) continue;
        //if (nodes[i].z >= hardchk) continue;
        istat = CheckForFaultGrazing2 (nodes[i].x, nodes[i].y);
        if (istat != 0) {
            nodes[i].z = GRD_SOFT_NULL_VALUE;
            nodes[i].shifted = 0;
        }
    }

    NodeOffset = dsav;

    return;

}  /* end of function grd_set_grazing_trimesh_nodes */





/*
  ****************************************************************

        c o n _ f a u l t e d _ b i l i n _ i n t e r p _ 2

  ****************************************************************

  This function calls con_faulted_bilin_interp for each point in
  the specified array.  The x and y coordinates of the array points
  are used as the reference points for the interpolation.  In other
  words, no grid nodes across faults from the point will be used in
  interpolating the surface at the point.

*/

int CSWGrdFault::con_faulted_bilin_interp_2 (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                                int kcell,
                                CSW_F *x, CSW_F *y, CSW_F *z, int npts)
{
    int              i, ir, jc, istat, iref, n;
    CSW_F            xt, yt, xref, yref;
    CSW_F            *xa = NULL, *ya = NULL, *za = NULL;
    int              *idone = NULL, *ia = NULL;
    double           nosav;


    auto fscope = [&]()
    {
        csw_Free (xa);
        csw_Free (idone);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Initialize the z values as nulls (1.e30)
*/
    for (i=0; i<npts; i++) {
        z[i] = 1.e30f;
    }

/*
    If no valid cell number has been specified, do the interpolation
    the hard slow way.
*/
    if (kcell < 0) {
        for (i=0; i<npts; i++) {

            if (x[i] >= 1.e20) {
                z[i] = 1.e30f;
                continue;
            }

            ir = (int)((y[i] - Ymin) / Yspace);
            jc = (int)((x[i] - Xmin) / Xspace);

            if (ir < 0  ||  jc < 0  ||  ir > nrow-1  ||  jc > Ncol-1) {
                z[i] = 1.e30f;
                continue;
            }

            xt = x[i];
            yt = y[i];
            AdjustForFaultGrazing (&xt, &yt);
            if (xt - x[i] != 0.0  ||  yt - y[i] != 0.0) {
                z[i] = (CSW_F)GRD_HARD_NULL_VALUE;
                continue;
            }
            istat = con_faulted_bilin_interp (grid, ncol, nrow, nullval,
                                              xt, yt, ir, jc,
                                              &xt, &yt, z+i, 1);
            if (istat == -1) {
                return -1;
            }
        }
        return 1;
    }

/*
    If a cell number is passed, all of the points are
    inside or on the edge of that cell.  In this case,
    groups of points on the same fault side are interpolated
    together to speed up the process.
*/
    n = npts + 1;
    if (n < 100) n = 100;
    xa = (CSW_F *)csw_Malloc (n * 3 * sizeof(CSW_F));
    if (xa == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    ya = xa + n;
    za = ya + n;
    idone = (int *)csw_Calloc (n * 2 * sizeof(int));
    if (idone == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    ia = idone + n;

    ir = kcell / Ncol;
    jc = kcell % Ncol;

    for (i=0; i<npts; i++) {
        if (x[i] >= 1.e20) {
            z[i] = 1.e30f;
            idone[i] = 1;
            continue;
        }
    }

    nosav = NodeOffset;
    NodeOffset *= 10.0;

    for (;;) {

        iref = -1;
        for (i=0; i<npts; i++) {
            if (idone[i] == 1) continue;
            xref = x[i];
            yref = y[i];
            istat = CheckForFaultGrazing (xref, yref);
            if (istat == 1) {
                continue;
            }
            iref = i;
            break;
        }
        if (iref == -1) break;

        xref = x[iref];
        yref = y[iref];
        xa[0] = xref;
        ya[0] = yref;
        ia[0] = iref;
        n = 1;

        for (i=iref+1; i<npts; i++) {
            if (idone[i] == 1) continue;
            xt = x[i];
            yt = y[i];
            istat = CheckForFaultGrazing (xt, yt);
            if (istat == 1) {
                ShortenVector (&xt, &yt, &xref, &yref);
            }
            istat = con_find_fault_intersection (xt, yt, xref, yref,
                                                 kcell, 1, &xt, &yt);
            if (istat == 0) {
                xa[n] = x[i];
                ya[n] = y[i];
                ia[n] = i;
                n++;
            }
        }

        if (n < 1) {
            idone[iref] = 1;
            continue;
        }

        istat = con_faulted_bilin_interp (grid, ncol, nrow, nullval,
                                          xref, yref, ir, jc,
                                          xa, ya, za, n);
        if (istat == -1) {
            return -1;
        }

        for (i=0; i<n; i++) {
            z[ia[i]] = za[i];
            idone[ia[i]] = 1;
        }

    }

    NodeOffset = nosav;

    return 1;

}  /*  end of function con_faulted_bilin_interp_2  */



/*
 **************************************************************************

               R e m o v e D u p l i c a t e F a u l t s

 **************************************************************************

  Remove duplicate faults (in terms of same x, y locations) from the specified
  list.  The list is overwritten with the non duplicated version.  The nlist
  parameter has the number of total faults on input and is passed back as
  the number of non duplicate faults.

*/

int CSWGrdFault::RemoveDuplicateFaults (FAultLineStruct *flist, int nlist,
                                  FAultLineStruct **fout, int *nfout)
{
    int                   i, j, n, n2, istat;
    FAultLineStruct       *f1 = NULL, *f2 = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (f1);
            csw_Free (f2);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    n = nlist;
    for (i=0; i<n; i++) {
        f1 = flist + i;
        if (f1->type == DUPLICATE_FAULT) continue;
        for (j=i+1; j<n; j++) {
            f2 = flist + j;
            if (f2->type == DUPLICATE_FAULT) continue;
            istat = CheckDuplicateFaults (f1, f2);
            if (istat == 1) {
                f2->type = DUPLICATE_FAULT;
            }
        }
    }

    n2 = 0;
    for (i=0; i<n; i++) {
        if (flist[i].type == DUPLICATE_FAULT) {
            continue;
        }
        n2++;
    }

    f1 = (FAultLineStruct *)csw_Calloc (n2 * sizeof(FAultLineStruct));
    if (f1 == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    n2 = 0;
    for (i=0; i<n; i++) {
        if (flist[i].type == DUPLICATE_FAULT) {
            continue;
        }
        f2 = grd_copy_fault_line_structs (flist+i, 1);
        csw_memcpy (f1+n2, f2, sizeof(FAultLineStruct));
        csw_Free (f2);
        f2 = NULL;
        n2++;
    }

    *fout = f1;
    *nfout = n2;

    bsuccess = true;

    return 1;

}  /* end of private RemoveDuplicatePoints function */




/*
 ***********************************************************************************

               C h e c k D u p l i c a t e F a u l t s

 ***********************************************************************************

  Return 1 if the x,y locations of both specified faults are nearly identical.
  Return zero if at least one point is different.

*/

int CSWGrdFault::CheckDuplicateFaults (FAultLineStruct *f1, FAultLineStruct *f2)
{
    int                i, n;
    POint3D            *p1, *p2;
    double             tiny, dx, dy, dist;

    tiny = NodeOffset * 1000.0;
    tiny *= tiny;

    if (f1->num_points != f2->num_points) {
        return 0;
    }

    n = f1->num_points;
    for (i=0; i<n; i++) {
        p1 = f1->points + i;
        p2 = f2->points + i;
        dx = p1->x - p2->x;
        dy = p1->y - p2->y;
        dist = dx * dx + dy * dy;
        if (dist > tiny) {
            return 0;
        }
    }

    return 1;

}  /* end of private CheckDuplicateFaults function */







/*
 *****************************************************************************************

               g r d _ c h e c k _ f o r _ f a u l t _ g r a z i n g

 *****************************************************************************************

  Return 1 if the specified point is very near a fault vector or return zero otherwise.

*/

int CSWGrdFault::grd_check_for_fault_grazing (double xt, double yt)
{
    int                     istat;

    if (NumFaultVectors < 1  ||  FaultVectors == NULL) {
        return 0;
    }

    istat = CheckForFaultGrazing2 (xt, yt);
    if (istat > 0) istat = 1;

    return istat;

}  /* end of function grd_check_for_fault_grazing */





/*
 ********************************************************************************************

                g r d _ g e t _ c u r r e n t _ f a u l t _ v e c t o r s

 ********************************************************************************************

*/

int CSWGrdFault::grd_get_current_fault_vectors (double **xline,
                                   double **yline,
                                   double **zline,
                                   int **iline,
                                   int *nline)
{
    int                  i, n;
    double               *xl = NULL, *yl = NULL, *zl = NULL;
    int                  *il = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (il);
            csw_Free (xl);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    *xline = NULL;
    *yline = NULL;
    *zline = NULL;
    *iline = NULL;
    *nline = 0;

    if (NumFaultVectors < 1) {
        return 1;
    }

    n = NumFaultVectors;
    il = (int *)csw_Malloc (n * sizeof(int));
    if (il == NULL) {
        return -1;
    }

    xl = (double *)csw_Malloc (n * 6 * sizeof(double));
    if (xl == NULL) {
        return -1;
    }
    yl = xl + 2 * n;
    zl = yl + 2 * n;

    n = 0;
    for (i=0; i<NumFaultVectors; i++) {
        il[i] = 2;
        xl[n] = FaultVectors[i].x1;
        yl[n] = FaultVectors[i].y1;
        zl[n] = GRD_SOFT_NULL_VALUE;
        n++;
        xl[n] = FaultVectors[i].x2;
        yl[n] = FaultVectors[i].y2;
        zl[n] = GRD_SOFT_NULL_VALUE;
        n++;
    }

    *nline = NumFaultVectors;
    *iline = il;
    *xline = xl;
    *yline = yl;
    *zline = zl;

    bsuccess = true;

    return 1;

}  /* end of function grd_get_current_fault_vectors */



/*
  ****************************************************************************

                         P e r p O f f s e t 2

  ****************************************************************************

    Calculate a point perpendicular from the specified line at the specified
  point and by the specified distance.

*/

int CSWGrdFault::PerpOffset2 (double x1, double y1, double x2, double y2,
                        double xt, double yt, double distance,
                        double *xout, double *yout)
{
    double           xx, yy, dx, dy, angle;

    dx = x2 - x1;
    dy = y2 - y1;
    angle = atan2 (dy, dx);
    angle -= HALFPI;
    dx = distance * cos (angle);
    dy = distance * sin (angle);
    xx = xt + dx;
    yy = yt + dy;

    if (xx < (double)Xmin  ||  xx > (double)Xmax  ||
        yy < (double)Ymin  ||  yy > (double)Ymax) {
        angle += HALFPI * 2.0;
        dx = distance * cos (angle);
        dy = distance * sin (angle);
        xx = xt + dx;
        yy = yt + dy;
    }

    *xout = xx;
    *yout = yy;

    return 1;

}  /*  end of private PerpOffset2 function  */




/*
 ************************************************************************************

                    A p p e n d F a u l t V e c t o r

 ************************************************************************************

  Append a fault vector to the FaultVector list.  If the ivec parameter is >= zero,
  use the existing FaultVectors[ivec] for type and flag information.  If not, use
  defaults for type and flag information.

*/

int CSWGrdFault::AppendFaultVector (int ivec, int endflag1, int endflag2,
                              double x1, double y1, double x2, double y2)
{
    int               nf;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (FaultVectorsUsed);
            FaultVectorsUsed = NULL;
            csw_Free (FaultVectors);
            FaultVectors = NULL;
            NumFaultVectors = 0;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (VecNumWork != NULL) {
        csw_Free (VecNumWork);
        VecNumWork = NULL;
    }

    if (NumFaultVectors >= MaxFaultVectors) {
        csw_Free (FaultVectorsUsed);
        FaultVectorsUsed = NULL;
        MaxFaultVectors += 100;
        FaultVectors = (FAultVector *)csw_Realloc
                                      (FaultVectors, MaxFaultVectors * sizeof(FAultVector));
        if (FaultVectors == NULL) {
            return -1;
        }
        FaultVectorsUsed = (char *)csw_Calloc (MaxFaultVectors * sizeof(char));
        if (FaultVectorsUsed == NULL) {
            return -1;
        }
    }

    nf = NumFaultVectors;
    FaultVectors[nf].x1 = (CSW_F)(x1);
    FaultVectors[nf].y1 = (CSW_F)(y1);
    FaultVectors[nf].x2 = (CSW_F)(x2);
    FaultVectors[nf].y2 = (CSW_F)(y2);

    FaultVectors[nf].endflag1 = endflag1;
    FaultVectors[nf].endflag2 = endflag2;

    if (ivec == -1) {
        FaultVectors[nf].flag = 0;
        FaultVectors[nf].id = nf;
        FaultVectors[nf].type = 0;
        FaultVectors[nf].used = 0;
    }
    else {
        FaultVectors[nf].flag = FaultVectors[ivec].flag;
        FaultVectors[nf].id = FaultVectors[ivec].id;
        FaultVectors[nf].type = FaultVectors[ivec].type;
        FaultVectors[nf].used = FaultVectors[ivec].used;
    }

    NumFaultVectors++;

    bsuccess = true;

    return 1;

}  /* end of private AppendFaultVector function */



/*
 ******************************************************************************

                     Z i g Z a g T o A v o i d N o d e s

 ******************************************************************************

  Where a diagonal fault vector grazes a grid node, break it by introducing two
  short vectors that detour around the grid node.  If a memory allocation fails,
  -1 is returned.  On success, 1 is returned.

*/

int CSWGrdFault::ZigZagToAvoidNodes (void)
{
    int               ido, ivec, i, r1, r2, c1, c2, itmp;
    CSW_F             x1, y1, x2, y2, xt, yt, slope, bint;
    int               num_vec;

    num_vec = NumFaultVectors;

/*
 * First, avoid the nodes in the original grid geometry.
 */
    for (ido=0; ido<num_vec; ido++) {

        x1 = FaultVectors[ido].x1;
        y1 = FaultVectors[ido].y1;
        x2 = FaultVectors[ido].x2;
        y2 = FaultVectors[ido].y2;

        if (csw_NearEqual (x1, x2, EqualTiny)) {
            continue;
        }
        if (csw_NearEqual (y1, y2, EqualTiny)) {
            continue;
        }

        if (y2 > y1) {
            r1 = (int)((y1 - Ymin) / Yspace);
            r2 = (int)((y2 - Ymin) / Yspace);
        }
        else {
            r1 = (int)((y2 - Ymin) / Yspace);
            r2 = (int)((y1 - Ymin) / Yspace);
        }

        if (x2 > x1) {
            c1 = (int)((x1 - Xmin) / Xspace);
            c2 = (int)((x2 - Xmin) / Xspace);
        }
        else {
            c1 = (int)((x2 - Xmin) / Xspace);
            c2 = (int)((x1 - Xmin) / Xspace);
        }

        if (r1 >= Nrow  ||  r2 < 0) continue;
        if (c1 >= Ncol  ||  c2 < 0) continue;

        ivec = ido;
        slope = (y2 - y1) / (x2 - x1);
        bint = y1 - slope * x1;

        if (r2 - r1 < c2 - c1) {
            for (i=r1; i<=r2; i++) {
                yt = i * Yspace + Ymin;
                if (yt < Ymin  ||  yt > Ymax) continue;
                xt = (yt - bint) / slope;
                if (xt < Xmin  ||  xt > Xmax) continue;
                itmp = (int)((xt - Xmin) / Xspace);
                if (csw_NearEqual (xt - Xmin, itmp * Xspace, EqualTiny)) {
                    ivec = SplitFaultVector (ivec, xt, yt);
                    if (ivec == -1) {
                        return -1;
                    }
                }
            }
        }
        else {
            for (i=c1; i<=c2; i++) {
                xt = i * Xspace + Xmin;
                if (xt < Xmin  ||  xt > Xmax) continue;
                yt = slope * xt + bint;
                if (yt < Ymin  ||  yt > Ymax) continue;
                itmp = (int)((yt - Ymin) / Yspace);
                if (csw_NearEqual (yt - Ymin, itmp * Yspace, EqualTiny)) {
                    ivec = SplitFaultVector (ivec, xt, yt);
                    if (ivec == -1) {
                        return -1;
                    }
                }
            }
        }
    }

    if (ResampleFlag == 0) {
        return 1;
    }

/*
 * Also avoid nodes in the resampled geometry if needed.
 */
    for (ido=0; ido<num_vec; ido++) {

        x1 = FaultVectors[ido].x1;
        y1 = FaultVectors[ido].y1;
        x2 = FaultVectors[ido].x2;
        y2 = FaultVectors[ido].y2;

        if (csw_NearEqual (x1, x2, EqualTiny)) {
            continue;
        }
        if (csw_NearEqual (y1, y2, EqualTiny)) {
            continue;
        }

        if (y2 > y1) {
            r1 = (int)((y1 - NewYmin) / NewYspace);
            r2 = (int)((y2 - NewYmin) / NewYspace);
        }
        else {
            r1 = (int)((y2 - NewYmin) / NewYspace);
            r2 = (int)((y1 - NewYmin) / NewYspace);
        }

        if (x2 > x1) {
            c1 = (int)((x1 - NewXmin) / NewXspace);
            c2 = (int)((x2 - NewXmin) / NewXspace);
        }
        else {
            c1 = (int)((x2 - NewXmin) / NewXspace);
            c2 = (int)((x1 - NewXmin) / NewXspace);
        }

        ivec = ido;
        slope = (y2 - y1) / (x2 - x1);
        bint = y1 - slope * x1;

        if (r2 - r1 < c2 - c1) {
            for (i=r1; i<=r2; i++) {
                yt = i * NewYspace + Ymin;
                if (yt < Ymin  ||  yt > Ymax) continue;
                xt = (yt - bint) / slope;
                if (xt < Xmin  ||  xt > Xmax) continue;
                itmp = (int)((xt - NewXmin) / NewXspace);
                if (csw_NearEqual (xt - NewXmin, itmp * NewXspace, EqualTiny)) {
                    ivec = SplitFaultVector (ivec, xt, yt);
                    if (ivec == -1) {
                        return -1;
                    }
                }
            }
        }
        else {
            for (i=c1; i<=c2; i++) {
                xt = i * NewXspace + Xmin;
                if (xt < Xmin  ||  xt > Xmax) continue;
                yt = slope * xt + bint;
                if (yt < Ymin  ||  yt > Ymax) continue;
                itmp = (int)((yt - NewYmin) / NewYspace);
                if (csw_NearEqual (yt - NewYmin, itmp * NewYspace, EqualTiny)) {
                    ivec = SplitFaultVector (ivec, xt, yt);
                    if (ivec == -1) {
                        return -1;
                    }
                }
            }
        }
    }

    return 1;

}  /* end of private ZigZagToAvoidNodes function */




/*
 *******************************************************************************************

                             S p l i t F a u l t V e c t o r

 *******************************************************************************************

  Split a vector to avoid the xt, yt spot.  This is only called from ZigZagToAvoidNodes.

*/

int CSWGrdFault::SplitFaultVector (int ivec, CSW_F xpin, CSW_F ypin)
{
    double            xperp, yperp, xp, yp, xt, yt,
                      x1, y1, x2, y2, dist, angle,
                      dlen, dx, dy;
    int               istat, ef2;

    xp = (double)xpin;
    yp = (double)ypin;

    dist = EqualTiny * 4.0;

    x1 = FaultVectors[ivec].x1;
    y1 = FaultVectors[ivec].y1;
    x2 = FaultVectors[ivec].x2;
    y2 = FaultVectors[ivec].y2;
    ef2 = FaultVectors[ivec].endflag2;

    dx = x2 - x1;
    dy = y2 - y1;
    dlen = dx * dx + dy * dy;
    dlen = sqrt (dlen);
    dlen /= 2.0;

    PerpOffset2 (x1, y1, x2, y2,
                 xp, yp, dist,
                 &xperp, &yperp);

    dist = (Xspace + Yspace) / 10.0;
    if (dist < EqualTiny * 10.0) dist = EqualTiny * 10.0;
    if (dist > dlen) dist = dlen;
    angle = atan2 (yp - y1, xp - x1);
    xt = xp - dist * cos(angle);
    yt = yp - dist * sin(angle);
    FaultVectors[ivec].x2 = (CSW_F)xt;
    FaultVectors[ivec].y2 = (CSW_F)yt;
    FaultVectors[ivec].endflag2 = 0;
    istat = AppendFaultVector (ivec, 0, 0,
                               xt, yt, xperp, yperp);
    if (istat == -1) {
        return -1;
    }

    angle = atan2 (yp - y2, xp - x2);
    xt = xp - dist * cos(angle);
    yt = yp - dist * sin(angle);
    istat = AppendFaultVector (ivec, 0, 0,
                               xperp, yperp, xt, yt);
    if (istat == -1) {
        return -1;
    }

    istat = AppendFaultVector (ivec, 0, ef2,
                               xt, yt, x2, y2);
    if (istat == -1) {
        return -1;
    }

    return (NumFaultVectors - 1);

}  /* end of private SplitFaultVector function */




/*
 **********************************************************************************

              A d j u s t F a u l t s F o r G r i d G e o m e t r y

 **********************************************************************************

  If a fault point lies on a row or column of either the original or resampled grid,
  adjust it so it does not lie on either grid geometry.

*/

void CSWGrdFault::AdjustFaultsForGridGeometry (CSW_F *xf, CSW_F *yf,
                                         int *npts, int nf)
{
    int             i, j, n, ido, jdo, np;
    CSW_F           xt, yt, xt2, yt2, fudge;

    fudge = (Xspace + Yspace) / 200.0f;

    n = 0;
    for (ido=0; ido<nf; ido++) {
        np = npts[ido];
        for (jdo=0; jdo<np; jdo++) {
            xt = xf[n] - Xmin;
            yt = yf[n] - Ymin;
            i = (int)(yt / Yspace);
            j = (int)(xt / Xspace);
            yt2 = i * Yspace;
            xt2 = j * Xspace;
            if (csw_NearEqual (xt, xt2, EqualTiny)) {
                xf[n] += fudge;
            }
            if (csw_NearEqual (yt, yt2, EqualTiny)) {
                yf[n] += fudge;
            }
            if (ResampleFlag == 0) continue;

            xt = xf[n] - NewXmin;
            yt = yf[n] - NewYmin;
            i = (int)(yt / NewYspace);
            j = (int)(xt / NewXspace);
            yt2 = i * NewYspace;
            xt2 = j * NewXspace;
            if (csw_NearEqual (xt, xt2, EqualTiny)) {
                xf[n] += fudge;
            }
            if (csw_NearEqual (yt, yt2, EqualTiny)) {
                yf[n] += fudge;
            }
        }
    }

    return;

}  /* end of private AdjustFaultsForGridGeometry function */





/*
  ****************************************************************

     g r d _ f a u l t _ s t r u c t s _ t o _ a r r a y s _ 2

  ****************************************************************

*/

int CSWGrdFault::grd_fault_structs_to_arrays_2 (FAultLineStruct *fault_list, int nlist,
                                   double **xfault, double **yfault, double **zfault,
                                   int **fault_points, int **linetypes,
                                   int *nfault)
{
    int                 i, n2, nt, j, *fpts = NULL, *lpts = NULL;
    double              *xout = NULL, *yout = NULL, *zout = NULL;
    FAultLineStruct     *fptr = NULL;

    bool    bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xout);
            csw_Free (yout);
            csw_Free (zout);
            csw_Free (fpts);
            csw_Free (lpts);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (fault_list == NULL  ||  nlist < 1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (xfault == NULL  ||  yfault == NULL  ||
        fault_points == NULL  ||  nfault == NULL) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    Initialize the output stuff in case an error occurs.
*/
    *xfault = NULL;
    *yfault = NULL;
    *fault_points = NULL;
    *linetypes = NULL;
    *nfault = 0;

/*
    Count and allocate the points needed.
*/
    n2 = 0;
    for (i=0; i<nlist; i++) {
        n2 += fault_list[i].num_points;
    }

    xout = (double *)csw_Malloc (n2 * sizeof(double));
    if (!xout) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    yout = (double *)csw_Malloc (n2 * sizeof(double));
    if (!yout) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    zout = (double *)csw_Malloc (n2 * sizeof(double));
    if (!zout) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    fpts = (int *)csw_Malloc (nlist * sizeof(int));
    if (!fpts) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    lpts = (int *)csw_Malloc (nlist * sizeof(int));
    if (!lpts) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Transfer points to the arrays.
*/
    n2 = 0;
    for (i=0; i<nlist; i++) {
        fptr = fault_list + i;
        nt = fptr->num_points;
        fpts[i] = nt;
        lpts[i] = fptr->lclass;
        for (j=0; j<nt; j++) {
            xout[n2] = (CSW_F)fptr->points[j].x;
            yout[n2] = (CSW_F)fptr->points[j].y;
            zout[n2] = (CSW_F)fptr->points[j].z;
            n2++;
        }
    }

    *xfault = xout;
    *yfault = yout;
    if (zfault != NULL) {
        *zfault = zout;
    }
    else {
        csw_Free (zout);
        zout = NULL;
    }
    *fault_points = fpts;
    *linetypes = lpts;
    *nfault = nlist;

    bsuccess = true;
    return 1;

}  /*  end of function grd_fault_structs_to_arrays  */





/*
  ****************************************************************

      g r d _ f a u l t _ a r r a y s _ t o _ s t r u c t s _ 2

  ****************************************************************

*/

int CSWGrdFault::grd_fault_arrays_to_structs_2 (double *xfault, double *yfault, double *zfault,
                                   int *fault_points, int *linetypes, int nfault,
                                   FAultLineStruct **fault_list, int *nlist)
{
    FAultLineStruct    *flist = NULL, *fptr = NULL;
    int                i, j, n, nt;

    bool    bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            grd_free_fault_line_structs (flist, nfault);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Check obvious errors.
*/
    if (xfault == NULL  ||  yfault == NULL  ||
        fault_points == NULL  ||  nfault < 1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (fault_list == NULL  ||  nlist == NULL) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    *fault_list = NULL;
    *nlist = 0;

/*
    Allocate an empty fault line list.  Calloc is
    used to insure that all points pointers are
    initially NULL.
*/
    flist = (FAultLineStruct *)csw_Calloc (nfault * sizeof(FAultLineStruct));
    if (!flist) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Allocate each POint3D pointer and fill in each structure.
*/
    n = 0;
    for (i=0; i<nfault; i++) {
        nt = fault_points[i];
        fptr = flist + i;
        fptr->lclass = GRD_DISCONTINUITY_CONSTRAINT;
        if (linetypes != NULL) {
            fptr->lclass = linetypes[i];
        }
        fptr->points = (POint3D *)csw_Malloc (nt * sizeof(POint3D));
        if (fptr->points == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        for (j=0; j<nt; j++) {
            fptr->points[j].x = (double)xfault[n];
            fptr->points[j].y = (double)yfault[n];
            if (zfault != NULL) {
                fptr->points[j].z = (double)zfault[n];
            }
            else {
                fptr->points[j].z = 1.e30;
            }
            n++;
        }
        fptr->num_points = nt;
    }

    *fault_list = flist;
    *nlist = nfault;

    bsuccess = true;

    return 1;

}  /*  end of function grd_fault_arrays_to_structs_2  */



/*
 ***********************************************************************************

              g r d _ e x t e n d _ f a u l t s _ t o _ m a r g i n s

 ***********************************************************************************

  If an end point of a fault is inside the margin (between the old and new limits
  specified) then extend it to outside the new limits.
*/

int CSWGrdFault::grd_extend_faults_to_margins (
    CSW_F    orig_xmin,
    CSW_F    orig_ymin,
    CSW_F    orig_xmax,
    CSW_F    orig_ymax,
    CSW_F    new_xmin,
    CSW_F    new_ymin,
    CSW_F    new_xmax,
    CSW_F    new_ymax)
{
    int              istat, i;
    CSW_F            extension_distance;
    CSW_F            x1, y1, x2, y2;
    FAultVector      *fptr;
    FAultLineStruct  *fline;
    int              id, nfline;

    if (orig_xmin >= orig_xmax  ||
        orig_ymin >= orig_ymax  ||
        new_xmin >= new_xmax  ||
        new_ymin >= new_ymax) {
        return 1;
    }

    OrigXmin = orig_xmin;
    OrigYmin = orig_ymin;
    OrigXmax = orig_xmax;
    OrigYmax = orig_ymax;
    MarginXmin = new_xmin;
    MarginYmin = new_ymin;
    MarginXmax = new_xmax;
    MarginYmax = new_ymax;

    if (FaultVectors == NULL) {
        return 1;
    }

  /*
   * Make the extension distance the sum of all the margin distances
   * to ensure extension outside the margin.
   */
    extension_distance = new_xmax - orig_xmax + orig_xmin - new_xmin +
                         new_ymax - orig_ymax + orig_ymin - new_ymin;

    for (i=0; i<NumFaultVectors; i++) {

        fptr = FaultVectors + i;

    /*
     * Extend any first points of vectors that are also first
     * endpoints of a fault polyline.
     */
        if (fptr->endflag1 == 1) {
            x1 = fptr->x1;
            y1 = fptr->y1;
            x2 = fptr->x2;
            y2 = fptr->y2;
            istat = InsideBox (x1, y1,
                               orig_xmin, orig_ymin,
                               orig_xmax, orig_ymax);
            if (istat == 0) {
                istat = InsideBox (x1, y1,
                                   new_xmin, new_ymin,
                                   new_xmax, new_ymax);
                if (istat == 1) {
                    ExtendVector (&x1, &y1, &x2, &y2, extension_distance);
                    fptr->x1 = x1;
                    fptr->y1 = y1;
                    id = fptr->id;
                    fline = OrigFaultLines + id;
                    fline->points[0].x = x1 + XFaultShift;
                    fline->points[0].y = y1 + YFaultShift;
                }
            }
        }

    /*
     * Extend any second points of vectors that are also last
     * endpoints of a fault polyline.
     */
        if (fptr->endflag2 == 1) {
            x1 = fptr->x1;
            y1 = fptr->y1;
            x2 = fptr->x2;
            y2 = fptr->y2;
            istat = InsideBox (x2, y2,
                               orig_xmin, orig_ymin,
                               orig_xmax, orig_ymax);
            if (istat == 0) {
                istat = InsideBox (x2, y2,
                                   new_xmin, new_ymin,
                                   new_xmax, new_ymax);
                if (istat == 1) {
                    ExtendVector (&x1, &y1, &x2, &y2, extension_distance);
                    fptr->x2 = x2;
                    fptr->y2 = y2;
                    id = fptr->id;
                    fline = OrigFaultLines + id;
                    nfline = fline->num_points - 1;
                    fline->points[nfline].x = x2 + XFaultShift;
                    fline->points[nfline].y = y2 + YFaultShift;
                }
            }
        }

    }

    return 1;

}  /* end of function grd_extend_faults_to_margins  */



/*
 ******************************************************************************

                             I n s i d e B o x

 ******************************************************************************

*/

int CSWGrdFault::InsideBox (CSW_F xt, CSW_F yt,
                      CSW_F xmin, CSW_F ymin,
                      CSW_F xmax, CSW_F ymax)

{
    if (xt >= xmin  &&  xt <= xmax  &&
        yt >= ymin  &&  yt <= ymax) {
        return 1;
    }

    return 0;
}



/*
 *********************************************************************************

                C h e c k I f L i n e I s E x t e n d a b l e

 *********************************************************************************

  Return 1 if it is ok to extend a fault line to make it go past the edge of
  the map.  Return zero if it is not ok.

  It is not ok to extend any line that loops onto itself, any line that has
  end points a long way from the map boundary or any line that closes in a
  polygon.

  If the line loops onto itself, the end points need to be fairly close to each
  other also.

  All this is based on my (Glenn's) judgement and will probably need to be adjusted
  with more experience from users.

*/

int CSWGrdFault::CheckIfLineIsExtendable (CSW_F *fxin, CSW_F *fyin, int nptsin,
                                    int *extend_first, int *extend_last)
{
    int            i, istat, ext1, ext2;
    double         *xin = NULL, *yin = NULL;
    double         xt1, yt1, xt2, yt2;
    double         dx, dy, dmax, dist, tiny;
    double         xmin, ymin, xmax, ymax;
    int            *seglist = NULL, nseg, nsegmax;


    auto fscope = [&]()
    {
        csw_Free (xin);
        csw_Free (seglist);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Preset to not extending.
 */
    *extend_first = 0;
    *extend_last = 0;

    ext1 = 0;
    ext2 = 0;

    if (fxin == NULL  ||  fyin == NULL  ||  nptsin < 2) {
        return 0;
    }

/*
 * If the line is a closed polygon, it is not extendable.
 */
    xt1 = fxin[0];
    yt1 = fyin[0];
    xt2 = fxin[nptsin-1];
    yt2 = fyin[nptsin-1];

    if (EqualTiny > 0.0) {
        dx = xt1 - xt2;
        dy = yt1 - yt2;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        if (dist <= EqualTiny) {
            return 0;
        }
    }
    else {
        xmin = ymin = 1.e30;
        xmax = ymax = 1.e30;
        for (i=0; i<nptsin; i++) {
            if (fxin[i] < xmin) xmin = fxin[i];
            if (fyin[i] < ymin) ymin = fyin[i];
            if (fxin[i] > xmax) xmax = fxin[i];
            if (fyin[i] > ymax) ymax = fyin[i];
        }
        if (xmin >= xmax  &&  ymin >= ymax) {
            return 0;
        }
        tiny = (xmax - xmin + ymax - ymin) / 200.0;
        dx = xt1 - xt2;
        dy = yt1 - yt2;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        if (dist <= tiny) {
            return 0;
        }
    }

    xin = (double *)csw_Malloc (nptsin * 2 * sizeof(double));
    if (xin == NULL) {
        return 0;
    }
    yin = xin + nptsin;

    for (i=0; i<nptsin; i++) {
        xin[i] = (double)fxin[i] - XFaultShift;
        yin[i] = (double)fyin[i] - YFaultShift;
    }

/*
 * Check for interior end points first
 */
    xt1 = xin[0];
    yt1 = yin[0];
    xt2 = xin[nptsin-1];
    yt2 = yin[nptsin-1];

    dx = (OrigXmax - OrigXmin) / 10.0;
    dy = (OrigYmax - OrigYmin) / 10.0;

    xmin = OrigXmin + dx;
    ymin = OrigYmin + dy;
    xmax = OrigXmax - dx;
    ymax = OrigYmax - dy;

    dmax = dx + dy;

/*
 * If both endpoints are interior (not close to boundary)
 * then neither endpoint should be flagged for extension.
 */
    if (xt1 > xmin  &&  xt1 < xmax  &&
        xt2 > xmin  &&  xt2 < xmax  &&
        yt1 > ymin  &&  yt1 < ymax  &&
        yt2 > ymin  &&  yt2 < ymax) {
        return 1;
    }

/*
 * If only one is interior, make the other extendable
 * pending further checks.
 */
    ext1 = 1;
    ext2 = 1;
    if (xt1 > xmin  &&  xt1 < xmax  &&
        yt1 > ymin  &&  yt1 < ymax) {
        ext1 = 0;
    }
    if (xt2 > xmin  &&  xt2 < xmax  &&
        yt2 > ymin  &&  yt2 < ymax) {
        ext2 = 0;
    }

/*
 * Check distance between end points.  If it is within 2 percent
 * of the average width and height of the area, no extension is done.
 */
    dx = xt1 - xt2;
    dy = yt1 - yt2;

    dist = dx * dx + dy * dy;
    dist = sqrt (dist);

    if (dist <= dmax / 10.0) {
        return 1;
    }

/*
 * Check if the path intersects itself.  The tiny value is
 * 1/1000th of the average dimension of the area.
 */
    nsegmax = nptsin;
    seglist = (int *)csw_Malloc (nptsin * sizeof(int));
    if (seglist == NULL) {
        return 0;
    }
    tiny = dmax / 200.0;

    istat = gpf_check_for_self_intersection (xin, yin, nptsin,
                                             seglist, &nseg, tiny, nsegmax);
    csw_Free (seglist);
    seglist = NULL;
    nseg = 0;

/*
 * If the path does intersect itself, then allow
 * a larger distance between endpoints to reject
 * the endpoints from extension.
 */
    if (istat == 1) {
        if (dist <= dmax / 5.0) {
            return 1;
        }
    }

    *extend_first = ext1;
    *extend_last = ext2;

    return 1;

}



/*--------------------------------------------------------------------------------*/


int CSWGrdFault::ResampleConstraintLines (
    double        **xline_io,
    double        **yline_io,
    double        **zline_io,
    int           *npline_io,
    int           *exact_flags,
    int           nline,
    double        avspace)
{
    int           i, j, n, n_new, npts, istat;
    double        *xa = NULL, *ya = NULL, *za = NULL,
                  *xnew = NULL, *ynew = NULL, *znew = NULL;
    double        *xline = NULL, *yline = NULL, *zline = NULL;

    typedef struct {
        double    *x;
        double    *y;
        double    *z;
        int       npts;
    } _templine;

    _templine     *tline;

    bool     bsuccess = false;
    bool     btline = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            if (btline) {
                for (j=0; j<nline; j++) {
                    csw_Free (tline[j].x);
                }
                csw_Free (tline);
            }
            csw_Free (xnew);
        }
        csw_Free (xline);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Allocate space for temporary storage of each resampled line.
 */
    tline = (_templine *)csw_Calloc (nline * sizeof(_templine));
    if (tline == NULL) {
        return -1;
    }

    xline = *xline_io;
    yline = *yline_io;
    zline = *zline_io;

/*
 * Rsample each line at the specified interval.  Put the
 * results for each line into the tline array.
 */
    n = 0;
    for (i=0; i<nline; i++) {

        xa = xline + n;
        ya = yline + n;
        za = zline + n;
        npts = npline_io[i];

        if (npts < 2) {
            tline[i].x = NULL;
            tline[i].y = NULL;
            tline[i].z = NULL;
            tline[i].npts = 0;
            continue;
        }

        n += npts;

        if (exact_flags == NULL) {
            istat =
            gpf_resample_line_segments (xa, ya, za, npts,
                               avspace,
                               &xnew, &ynew, &znew, &n_new);
            if (istat == -1) {
                btline = true;
                return -1;
            }
        }

        else {
            if (exact_flags[i] == 0) {
                istat =
                gpf_resample_line_segments (xa, ya, za, npts,
                                   avspace,
                                   &xnew, &ynew, &znew, &n_new);
                if (istat == -1) {
                    btline = true;
                    return -1;
                }
            }
            else {
                xnew = (double *)csw_Malloc (3 * npts * sizeof(double));
                if (xnew == NULL) {
                    btline = true;
                    return -1;
                }
                ynew = xnew + npts;
                znew = ynew + npts;
                memcpy (xnew, xa, npts * sizeof(double));
                memcpy (ynew, ya, npts * sizeof(double));
                memcpy (znew, za, npts * sizeof(double));
                n_new = npts;
            }
        }

        tline[i].x = xnew;
        xnew = NULL;
        tline[i].y = ynew;
        ynew = NULL;
        tline[i].z = znew;
        znew = NULL;
        tline[i].npts = n_new;

    }

/*
 * Free the original input.
 */
    csw_Free (xline);
    xline = NULL;

/*
 * Allocate space for output.
 */
    n = 0;
    for (i=0; i<nline; i++) {
        n += tline[i].npts;
    }

    xline = (double *)csw_Malloc (3 * n * sizeof(double));
    if (xline == NULL) {
        btline = true;
        return -1;
    }
    yline = xline + n;
    zline = yline + n;

    n = 0;
    for (i=0; i<nline; i++) {
        if (tline[i].npts == 0) {
            npline_io[i] = 0;
            continue;
        }
        memcpy (xline+n, tline[i].x, tline[i].npts * sizeof(double));
        memcpy (yline+n, tline[i].y, tline[i].npts * sizeof(double));
        memcpy (zline+n, tline[i].z, tline[i].npts * sizeof(double));
        npline_io[i] = tline[i].npts;
        n += tline[i].npts;
    }

    *xline_io = xline;
    *yline_io = yline;
    *zline_io = zline;

    for (j=0; j<nline; j++) {
        csw_Free (tline[j].x);
    }
    csw_Free (tline);
    tline = NULL;

    bsuccess = true;

    return 1;

}



/*---------------------------------------------------------------------------*/

/*
 * Insert intersection points into faults and
 * then decimate them to eliminate close points.
 */
int CSWGrdFault::FixCrossingFaults (void)
{
  double         *xfault = NULL, *yfault = NULL, *zfault = NULL;
  int            *fault_points = NULL, *linetypes = NULL, nfault;

  double         *xdec1 = NULL, *ydec1 = NULL, *zdec1 = NULL,
                 *xdec2 = NULL, *ydec2 = NULL, *zdec2 = NULL,
                 *xdec3 = NULL, *ydec3 = NULL, *zdec3 = NULL,
                 *xdec4 = NULL, *ydec4 = NULL, *zdec4 = NULL;
  int            *tdec3 = NULL, *tdec4 = NULL;
  int            ndec1, ndec2, ndec3, ndec4,
                 istat, *tag1 = NULL, *tag2 = NULL, *tag = NULL;
  int            i, j, n, n1, ntot, maxtag, nfp, size;
  double         **xsave = NULL, **ysave = NULL, **zsave = NULL;
  int            *nsave = NULL, maxsave, nlast, nsep;
  double         *xa = NULL, *ya = NULL, *za = NULL;
  FAultLineStruct  *fnew = NULL;
  int            numfnew;
  double         **xfp = NULL, **yfp = NULL, **zfp = NULL;
  int            **tfp = NULL;
  double         *xwork = NULL, *ywork = NULL, *zwork = NULL;
  double         *xout3 = NULL, *yout3 = NULL, *zout3 = NULL,
                 *xout4 = NULL, *yout4 = NULL, *zout4 = NULL;
  int            maxout, maxwork,
                 *tagout3 = NULL, *tagout4 = NULL, *tagwork = NULL;


  auto fscope = [&]()
  {
    FreeDoubleArrays (xfp, nfault);
    FreeDoubleArrays (xsave, n);
    csw_Free (xfault);
    csw_Free (yfault);
    csw_Free (zfault);
    csw_Free (fault_points);
    csw_Free (linetypes);
    csw_Free (tag1);
    csw_Free (xsave);
    csw_Free (nsave);
    csw_Free (xout3);
    csw_Free (xdec3);
  };
  CSWScopeGuard func_scope_guard (fscope);


  if (FixCrossingFlag == 0) {
    return 1;
  }

/*
 * There have to be at least 2 faults associated with the
 * surface to have a crossing.
 */
  if (NumOrigFaultLines < 2) {
    return 1;
  }

/*
 * If there are more than 1000 fault lines, I assume it is an error
 * in the calling parameters.
 */
  if (NumOrigFaultLines > 1000) {
    return -1;
  }

  istat = grd_fault_structs_to_arrays_2 (
    OrigFaultLines, NumOrigFaultLines,
    &xfault, &yfault, &zfault,
    &fault_points, &linetypes,
    &nfault);
  if (istat == -1) {
    return -1;
  }

  ntot = 0;
  for (i=0; i<nfault; i++) {
    ntot += fault_points[i];
  }

/*
 * Allocate several work space arrays.
 */
  maxtag = ntot * 10;
  if (maxtag < 10000) maxtag = 10000;
  tag1 = (int *)csw_Calloc (2 * maxtag * sizeof(int));
  if (tag1 == NULL) {
    return -1;
  }
  tag2 = tag1 + maxtag;

  maxsave = nfault * 100;
  if (maxsave < 10000) maxsave = 10000;
  xsave = (double **)csw_Calloc (3 * maxsave * sizeof(double *));
  if (xsave == NULL) {
    return -1;
  }
  ysave = xsave + maxsave;
  zsave = ysave + maxsave;
  nsave = (int *)csw_Calloc (maxsave * sizeof(int));
  if (nsave == NULL) {
    return -1;
  }

  xfp = (double **)csw_Calloc (nfault * 4 * sizeof(double *));
  if (xfp == NULL) {
    return -1;
  }
  yfp = xfp + nfault;
  zfp = yfp + nfault;

/*
 * BUGFIX
 * tfp array added to record tags for each line
 */
  tfp = (int **)(zfp + nfault);

/*
 * Put the fault lines into the fault pointer arrays.
 */
  n1 = 0;
  maxout = 0;
  for (i=0; i<nfault; i++) {
    n = fault_points[i];
    maxout += n;
    xfp[i] = (double *)csw_Calloc (n * 4 * sizeof(double));
    if (xfp[i] == NULL) {
      return -1;
    }
    yfp[i] = xfp[i] + n;
    zfp[i] = yfp[i] + n;
    tfp[i] = (int *)(zfp[i] + n);
    memcpy (xfp[i], xfault+n1, n * sizeof(double));
    memcpy (yfp[i], yfault+n1, n * sizeof(double));
    memcpy (zfp[i], zfault+n1, n * sizeof(double));
    n1 += n;
  }

/*
 * Allocate work space needed by grd_insert_crossing_points
 */
  maxout *= 2;
  maxwork = maxout;
  xout3 = (double *)csw_Malloc (11 * maxout * sizeof(double));
  if (xout3 == NULL) {
    return -1;
  }
  yout3 = xout3 + maxout;
  zout3 = yout3 + maxout;
  xout4 = zout3 + maxout;
  yout4 = xout4 + maxout;
  zout4 = yout4 + maxout;
  xwork = zout4 + maxout;
  ywork = xwork + maxout;
  zwork = ywork + maxout;
  tagout3 = (int *)(zwork + maxout);
  tagout4 = tagout3 + maxout;
  tagwork = tagout4 + maxout;

/*
 * Insert crossing points for each pair of crossing faults.
 */
  n = 0;
  for (i=0; i<nfault; i++) {

    xdec1 = xfp[i];
    ydec1 = yfp[i];
    zdec1 = zfp[i];
    ndec1 = fault_points[i];

  /*
   * Bug 9426
   * The tag arrays must be reset to zero when a new line is being processed.
   */
    memset (tag1, 0, 2 * maxtag * sizeof(int));

  /*
   * BUGFIX !!!!  10891
   * Set the tag1 values to those stored for this fault in
   * a previous iteration of this loop.
   */
    if (i > 0  &&  tfp[i] != NULL) {
        memcpy (tag1, tfp[i], ndec1 * sizeof(int));
    }

    for (j=i+1; j<nfault; j++) {

      xdec2 = xfp[j];
      ydec2 = yfp[j];
      zdec2 = zfp[j];
      ndec2 = fault_points[j];

    /*
     * BUGFIX !!!!  10891
     * Set the tag2 values to those stored for this fault in
     * a previous iteration of this loop.
     */
      if (i > 0  &&  tfp[j] != NULL) {
          memcpy (tag2, tfp[j], ndec2 * sizeof(int));
      }

      istat =
      grd_triangle_ptr->grd_insert_crossing_points (
        xdec1, ydec1, zdec1, tag1, ndec1,
        xdec2, ydec2, zdec2, tag2, ndec2,
        xout3, yout3, zout3, tagout3, &ndec3,
        xout4, yout4, zout4, tagout4, &ndec4,
        maxout,
        xwork, ywork, zwork, tagwork, maxwork);

    /*
     * If there were no intersections, the ith and jth
     * lines do not need changing.
      if (istat == 0) {
        continue;
      }
     */

      if (istat == -1) {
        return -1;
      }

      if (ndec3 > maxtag  ||  ndec4 > maxtag) {
        return -1;
      }

    /*
     * There is a change in the lines, so save the output
     * lines into their own space.  The updated lines are
     * used as input to the next crossing calculations.
     */
      xdec3 = (double *)csw_Calloc (ndec3 * 4 * sizeof(double));
      if (xdec3 == NULL) {
        return -1;
      }
      ydec3 = xdec3 + ndec3;
      zdec3 = ydec3 + ndec3;
      tdec3 = (int *)(zdec3 + ndec3);

      xdec4 = (double *)csw_Malloc (ndec4 * 4 * sizeof(double));
      if (xdec4 == NULL) {
        return -1;
      }
      ydec4 = xdec4 + ndec4;
      zdec4 = ydec4 + ndec4;
      tdec4 = (int *)(zdec4 + ndec4);

      memcpy (xdec3, xout3, ndec3 * sizeof(double));
      memcpy (ydec3, yout3, ndec3 * sizeof(double));
      memcpy (zdec3, zout3, ndec3 * sizeof(double));
      memcpy (xdec4, xout4, ndec4 * sizeof(double));
      memcpy (ydec4, yout4, ndec4 * sizeof(double));
      memcpy (zdec4, zout4, ndec4 * sizeof(double));

      memcpy (tag1, tagout3, ndec3 * sizeof(int));
      memcpy (tag2, tagout4, ndec4 * sizeof(int));

    /*
     * BUGFIX
     * Record the tags for each line.
     */
      memcpy (tdec3, tagout3, ndec3 * sizeof(int));
      memcpy (tdec4, tagout4, ndec4 * sizeof(int));

      csw_Free (xdec1);
      xdec1 = NULL;
      xfp[i] = xdec3;
      yfp[i] = ydec3;
      zfp[i] = zdec3;
      tfp[i] = tdec3;
      xdec1 = xdec3;
      ydec1 = ydec3;
      zdec1 = zdec3;
      ndec1 = ndec3;
      fault_points[i] = ndec3;

      csw_Free (xdec2);
      xdec2 = NULL;
      xfp[j] = xdec4;
      yfp[j] = ydec4;
      zfp[j] = zdec4;
      tfp[j] = tdec4;
      fault_points[j] = ndec4;

    }

  /*
   * All the crossings for the ith line are done.  Separate
   * this into uncrossed polylines.  The tag3 array is used
   * for the separation.  the tag3 value is zero if the point
   * is an original fault point or it is 1 if the point is an
   * inserted crossing point.  The points flagged as 1 are the
   * end point of a separate polyline and the start point of the
   * next separate polyline.
   */
    nlast = 0;

/*
 * BUGFIX
 * Use the tags recorded in tfp.
 */
    tag = tfp[i];
    for (j=0; j<ndec1; j++) {
      if (tag[j] == 1) {
        nsep = j - nlast + 1;
        xa = (double *)csw_Malloc (nsep * 3 * sizeof(double));
        if (xa == NULL) {
          return -1;
        }
        ya = xa + nsep;
        za = ya + nsep;
        memcpy (xa, xdec1 + nlast, nsep * sizeof(double));
        memcpy (ya, ydec1 + nlast, nsep * sizeof(double));
        memcpy (za, zdec1 + nlast, nsep * sizeof(double));
        xsave[n] = xa;
        ysave[n] = ya;
        zsave[n] = za;
        nsave[n] = nsep;
        n++;
        if (n >= maxsave) {
          return -1;
        }
        nlast = j;
      }
    }

    nsep = ndec1 - nlast;
    if (nsep > 1) {
      xa = (double *)csw_Malloc (nsep * 3 * sizeof(double));
      if (xa == NULL) {
        return -1;
      }
      ya = xa + nsep;
      za = ya + nsep;
      memcpy (xa, xdec1 + nlast, nsep * sizeof(double));
      memcpy (ya, ydec1 + nlast, nsep * sizeof(double));
      memcpy (za, zdec1 + nlast, nsep * sizeof(double));
      xsave[n] = xa;
      ysave[n] = ya;
      zsave[n] = za;
      nsave[n] = nsep;
      n++;
      if (n >= maxsave) {
        return -1;
      }
    }

  }  /* end of i loop through all faults. */

  csw_Free (xout3);
  xout3 = NULL;
  yout3 = NULL;
  zout3 = NULL;
  xout4 = NULL;
  yout4 = NULL;
  zout4 = NULL;
  xwork = NULL;
  ywork = NULL;
  zwork = NULL;
  tagout3 = NULL;
  tagout4 = NULL;
  tagwork = NULL;

  FreeDoubleArrays (xfp, nfault);
  xfp = NULL;
  yfp = NULL;
  zfp = NULL;

  csw_Free (tag1);
  tag1 = NULL;
  csw_Free (xfault);
  xfault = NULL;
  csw_Free (yfault);
  yfault = NULL;
  csw_Free (zfault);
  zfault = NULL;
  csw_Free (fault_points);
  fault_points = NULL;
  csw_Free (linetypes);
  linetypes = NULL;

/*
 * Combine the separately allocated lines into continuous
 * arrays of points.
 */
  ntot = 0;
  for (i=0; i<n; i++) {
    ntot += nsave[i];
  }
  xfault = (double *)csw_Malloc (ntot * 3 * sizeof(double));
  if (xfault == NULL) {
    return -1;
  }
  yfault = xfault + ntot;
  zfault = yfault + ntot;

  nfp = 0;
  for (i=0; i<n; i++) {
    size = nsave[i] * sizeof(double);
    memcpy (xfault+nfp, xsave[i], size);
    memcpy (yfault+nfp, ysave[i], size);
    memcpy (zfault+nfp, zsave[i], size);
    nfp += nsave[i];
  }

  FreeDoubleArrays (xsave, n);
  xsave = NULL;
  ysave = NULL;
  zsave = NULL;

/*
 * Convert the arrays to fault line structures.
 */
  istat =
  grd_fault_arrays_to_structs_2 (
    xfault, yfault, zfault,
    nsave, NULL, n,
    &fnew, &numfnew);
  csw_Free (xfault);
  xfault = NULL;
  yfault = NULL;
  zfault = NULL;
  csw_Free (nsave);
  nsave = NULL;

  if (istat == -1) {
    return -1;
  }

/*
 * Free the old orig fault lines and assign
 * the new lines to the static pointers.
 */
  grd_free_fault_line_structs (
    OrigFaultLines, NumOrigFaultLines);

  OrigFaultLines = fnew;
  NumOrigFaultLines = numfnew;

  return 1;

}

void CSWGrdFault::FreeDoubleArrays (double **alist, int nlist)
{
  int            i;

  if (alist == NULL) {
    return;
  }

  for (i=0; i<nlist; i++) {
    csw_Free (alist[i]);
  }
  csw_Free (alist);

  return;
}


void CSWGrdFault::ShiftXY (CSW_F *x, CSW_F *y, int nlines, int *npts)
{
    int        i, ntot;

    ntot = 0;
    for (i=0; i<nlines; i++) {
        ntot += npts[i];
    }

    for (i=0; i<ntot; i++) {
        x[i] -= (CSW_F)XFaultShift;
        y[i] -= (CSW_F)YFaultShift;
    }

    return;
}
