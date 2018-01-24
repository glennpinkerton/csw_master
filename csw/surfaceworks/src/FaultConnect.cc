
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * Include system headers.
 */
#include <assert.h>
#include <math.h>
#include <stdlib.h>


/*
 * This define allows private csw functions to be used.
 */
#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif

/*
 * Include general csw headers.
 */
#include <csw/utils/include/csw_.h>

#include <csw/utils/private_include/csw_scope.h>
#include <csw/utils/private_include/gpf_utils.h>

/*
 * Local headers.
 */
#include "FaultConnect.h"
#include "PadSurfaceForSim.h"
#include "SealedModel.h"


#define _MAX_Z_NORMAL_       .95



/*-----------------------------------------------------------------------*/

/**
 * This is an empty constructor for a FaultConnect object.  The object is
 * only initialized to an empty state when this is used.  To make the object
 * useful, you need to add horizons and faults with the various versions of
 * the addInputHorizon and addInputFault methods.
 */
FaultConnect::FaultConnect ()
{
    init ();
}


/*-----------------------------------------------------------------------*/

/**
 * The destructor csw_Frees all memory that was allocated in using the object
 * and sets the object to its empty state.
 */
FaultConnect::~FaultConnect()
{
    free_mem ();
}



/*-----------------------------------------------------------------------*/

/*
 * Initialize the private variables appropriately.  This should only
 * be called from constructors.
 *
 * This is a private method.
 */
void FaultConnect::init (void)
{
    detach_surf = NULL;
    detach_lower_surf = NULL;
    detach_upper_surf = NULL;
    detach_calc_surf = NULL;
    pad_detach_surf = NULL;
    sealed_detach_surf = NULL;
    input_faults = NULL;
    pad_faults = NULL;
    seal_faults = NULL;

    num_input_faults = 0;
    num_pad_faults = 0;
    num_seal_faults = 0;
    max_input_faults = 0;
    max_pad_faults = 0;
    max_seal_faults = 0;

    fault_contacts = NULL;
    seal_fault_contacts = NULL;
    num_fault_contacts = 0;
    num_seal_fault_contacts = 0;

    dxmin = 1.e30;
    dymin = 1.e30;
    dzmin = 1.e30;
    dxmax = -1.e30;
    dymax = -1.e30;
    dzmax = -1.e30;

    tnxNorm = 0.0;
    tnyNorm = 0.0;
    tnzNorm = 0.0;

    dcoefs[0] = 1.e30;
    dcoefs[1] = 1.e30;
    dcoefs[2] = 1.e30;

    average_edge_length = -1.0;

    test_data_flag = 0;
}

void FaultConnect::setAverageSpacing (double avspace)
{
    if (avspace <= 0.0  ||  avspace > 1.e20) {
        avspace = -1.0;
    }

    average_edge_length = avspace;
}


/*----------------------------------------------------------------------*/

/*
 * Free the memory for the private data members and re initialize them
 * all to their empty values.
 *
 * This is a private method.
 */
void FaultConnect::free_mem (void)
{
    FreeSurf (detach_surf, 1);
    FreeSurf (sealed_detach_surf, 1);

    FreeSurf (input_faults, num_input_faults);
    FreeSurf (pad_faults, num_pad_faults);
    FreeSurf (seal_faults, num_seal_faults);

    FreeLine (fault_contacts, num_fault_contacts);
    FreeLine (seal_fault_contacts, num_seal_fault_contacts);

    init ();
}

/*----------------------------------------------------------------------*/

/*
 * Free the members of a _SUrfStruct_ list.
 *
 * This is a private method.
 */
void FaultConnect::FreeSurf (_SUrfStruct_ *list, int nlist)
{
    _SUrfStruct_  *sptr;
    int           i;

    if (list == NULL  ||  nlist < 1) {
        return;
    }

    for (i=0; i<nlist; i++) {
        sptr = list + i;
        csw_Free (sptr->nodes);
        csw_Free (sptr->edges);
        csw_Free (sptr->tris);
        csw_Free (sptr->x);
        if (sptr->sgp) {
            delete (sptr->sgp);
        }
    }

    csw_Free (list);

    return;
}


/*----------------------------------------------------------------------*/

/*
 * Free the members of a _SUrfStruct_ list.
 *
 * This is a private method.
 */
void FaultConnect::FreeLine (_ILineStruct_ *list, int nlist)
{
    _ILineStruct_ *lptr;
    int           i;

    if (list == NULL  ||  nlist < 1) {
        return;
    }

    for (i=0; i<nlist; i++) {
        lptr = list + i;
        csw_Free (lptr->x);
    }

    csw_Free (list);

    return;
}


/*----------------------------------------------------------------------*/

/*
 * Free the members of a SUrfStruct but not the structure itself.
 *
 * This is a private method.
 */
void FaultConnect::CleanSurf (_SUrfStruct_ *sptr)
{
    if (sptr == NULL) {
        return;
    }

    csw_Free (sptr->nodes);
    csw_Free (sptr->edges);
    csw_Free (sptr->tris);
    csw_Free (sptr->x);

    memset (sptr, 0, sizeof(_SUrfStruct_));

    return;
}


/*----------------------------------------------------------------------*/

/*
 * Free the members of a _SUrfStruct_ list.
 *
 * This is a private method.
 */
void FaultConnect::CleanLine (_ILineStruct_ *lptr)
{

    if (lptr == NULL) {
        return;
    }

    csw_Free (lptr->x);

    memset (lptr, 0, sizeof(_ILineStruct_));

    return;
}


/*-------------------------------------------------------------------------*/

/*
 * Set the detach_surf member of the object.  If a non NULL
 * detach_surf already exists it is deleted and then this one
 * is used.
 */
int FaultConnect::setDetachment (
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
)
{

    NOdeStruct     *nt = NULL;
    EDgeStruct     *et = NULL;
    TRiangleStruct *tt = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nt);
            csw_Free (et);
            csw_Free (tt);
            FreeSurf (detach_surf, 1);
            detach_surf = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    FreeSurf (detach_surf, 1);
    detach_surf = NULL;

    if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
        return 1;
    }
    if (num_nodes < 3  ||  num_edges < 3  ||  num_tris < 1) {
        return 1;
    }

    nt = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    et = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    tt = (TRiangleStruct *)csw_Malloc (num_tris * sizeof(TRiangleStruct));
    detach_surf = (_SUrfStruct_ *)csw_Calloc (sizeof(_SUrfStruct_));

    if (nt == NULL  ||  et == NULL  ||  tt == NULL  ||
        detach_surf == NULL) {
        return -1;
    }

    memcpy (nt, nodes, num_nodes * sizeof(NOdeStruct));
    memcpy (et, edges, num_edges * sizeof(EDgeStruct));
    memcpy (tt, tris, num_tris * sizeof(TRiangleStruct));

    detach_surf->nodes = nt;
    detach_surf->edges = et;
    detach_surf->tris = tt;
    detach_surf->num_nodes = num_nodes;
    detach_surf->num_edges = num_edges;
    detach_surf->num_tris = num_tris;

    bsuccess = true;

    return 1;

}



/*-------------------------------------------------------------------------*/

/*
 * Add the specified trimesh to the detach_surf member of the object.
 * If the detach_surf is currently NULL, this is exactly the same as
 * the setDetachment method.  If the current detach_surf is not NULL,
 * then all nodes, both current and those being added, are appended
 * to the x, y and z node lists and the recalc flag is set to 1.
 */
int FaultConnect::addToDetachment (
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
)
{
    int       ntot, start, size;
    double    *xa = NULL, *ya = NULL, *za = NULL;

    bool bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xa);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
        return 1;
    }
    if (num_nodes < 3  ||  num_edges < 3  ||  num_tris < 1) {
        return 1;
    }

    int    istat;

    if (detach_surf == NULL) {
        istat = setDetachment (
            nodes,
            num_nodes,
            edges,
            num_edges,
            tris,
            num_tris
        );
        return istat;
    }

/*
 * Grow the x, y and z lists if needed.
 */
    ntot = detach_surf->num_nodes + detach_surf->npts + num_nodes;
    xa = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + ntot;
    za = ya + ntot;

/*
 * Copy any existing x, y and z points into the new arrays.
 * Free the existing x, y and z after the copy is done.
 */
    start = 0;
    if (detach_surf->npts > 0) {
        size = detach_surf->npts * sizeof(double);
        memcpy (xa, detach_surf->x, size);
        memcpy (ya, detach_surf->y, size);
        memcpy (za, detach_surf->z, size);
        start = detach_surf->npts;
        csw_Free (detach_surf->x);
        detach_surf->x = NULL;
        detach_surf->y = NULL;
        detach_surf->z = NULL;
    }

    int        i, n;

/*
 * Copy the existing nodes into the x, y and z arrays.
 */
    n = start;
    for (i=0; i<detach_surf->num_nodes; i++) {
        xa[n] = detach_surf->nodes[i].x;
        ya[n] = detach_surf->nodes[i].y;
        za[n] = detach_surf->nodes[i].z;
        n++;
    }

/*
 * Once the node have been copied the first time, csw_Free and null
 * the trimesh data in the detach_surf object.
 */
    csw_Free (detach_surf->nodes);
    csw_Free (detach_surf->edges);
    csw_Free (detach_surf->tris);
    detach_surf->nodes = NULL;
    detach_surf->edges = NULL;
    detach_surf->tris = NULL;
    detach_surf->num_nodes = 0;
    detach_surf->num_edges = 0;
    detach_surf->num_tris = 0;

/*
 * Copy the new nodes into the x, y and z arrays.
 */
    for (i=0; i<num_nodes; i++) {
        xa[n] = nodes[i].x;
        ya[n] = nodes[i].y;
        za[n] = nodes[i].z;
        n++;
    }

/*
 * Put the new x, y and z arrays into the detach_surf object.
 */
    detach_surf->x = xa;
    detach_surf->y = ya;
    detach_surf->z = za;
    detach_surf->npts = n;

    detach_surf->recalc = 1;

    bsuccess = true;

    return 1;

}





/*-------------------------------------------------------------------------*/

/*
 * Add the specified points to the detach_surf member of the object.
 * If the detach_surf is currently NULL, a new one is created.
 */
int FaultConnect::addToDetachment (
    double            *x,
    double            *y,
    double            *z,
    int               npts
)
{
    int       ntot, start, size;
    double    *xa = NULL, *ya = NULL, *za = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xa);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (x == NULL  ||  y == NULL  ||  z == NULL  ||  npts < 1) {
        return 1;
    }

    if (detach_surf == NULL) {
        detach_surf = (_SUrfStruct_ *)csw_Calloc (sizeof(_SUrfStruct_));
    }

    if (detach_surf == NULL) {
        return -1;
    }

/*
 * Grow the x, y and z lists if needed.
 */

    ntot = detach_surf->num_nodes + detach_surf->npts + npts;
    xa = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + ntot;
    za = ya + ntot;

/*
 * Copy any existing x, y and z points into the new arrays.
 * Free the existing x, y and z after the copy is done.
 */
    start = 0;
    if (detach_surf->npts > 0) {
        size = detach_surf->npts * sizeof(double);
        memcpy (xa, detach_surf->x, size);
        memcpy (ya, detach_surf->y, size);
        memcpy (za, detach_surf->z, size);
        start = detach_surf->npts;
        csw_Free (detach_surf->x);
        detach_surf->x = NULL;
        detach_surf->y = NULL;
        detach_surf->z = NULL;
    }

    int        i, n;

/*
 * Copy the existing nodes into the x, y and z arrays.
 */
    n = start;
    for (i=0; i<detach_surf->num_nodes; i++) {
        xa[n] = detach_surf->nodes[i].x;
        ya[n] = detach_surf->nodes[i].y;
        za[n] = detach_surf->nodes[i].z;
        n++;
    }

/*
 * Once the nodes have been copied the first time, csw_Free and null
 * the trimesh data in the detach_surf object.
 */
    csw_Free (detach_surf->nodes);
    csw_Free (detach_surf->edges);
    csw_Free (detach_surf->tris);
    detach_surf->nodes = NULL;
    detach_surf->edges = NULL;
    detach_surf->tris = NULL;
    detach_surf->num_nodes = 0;
    detach_surf->num_edges = 0;
    detach_surf->num_tris = 0;

/*
 * Copy the new points into the x, y and z arrays.
 */
    for (i=0; i<npts; i++) {
        xa[n] = x[i];
        ya[n] = y[i];
        za[n] = z[i];
        n++;
    }

/*
 * Put the new x, y and z arrays into the detach_surf object.
 */
    detach_surf->x = xa;
    detach_surf->y = ya;
    detach_surf->z = za;
    detach_surf->npts = n;

    detach_surf->recalc = 1;

    bsuccess = true;

    return 1;

}



/*-------------------------------------------------------------------------*/

/*
 * Set a new fault to the specified trimesh.  If a fault with the
 * specified id already exists, it's trimesh and points are replaced
 * with the specified trimesh.
 */
int FaultConnect::setFault (
    int               id,
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
)
{
    _SUrfStruct_      *fptr = NULL;

    NOdeStruct     *nt = NULL;
    EDgeStruct     *et = NULL;
    TRiangleStruct *tt = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nt);
            csw_Free (et);
            csw_Free (tt);
            if (fptr) CleanSurf (fptr);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    fptr = FindInputFaultForID (id);
    if (fptr) {
        CleanSurf (fptr);
    }

    if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
        return 1;
    }
    if (num_nodes < 3  ||  num_edges < 3  ||  num_tris < 1) {
        return 1;
    }

    nt = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    et = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    tt = (TRiangleStruct *)csw_Malloc (num_tris * sizeof(TRiangleStruct));

    if (nt == NULL  ||  et == NULL  ||  tt == NULL) {
        return -1;
    }

    if (fptr == NULL) {
        fptr = NextInputFaultSurf ();
    }
    if (fptr == NULL) {
        return -1;
    }

    memcpy (nt, nodes, num_nodes * sizeof(NOdeStruct));
    memcpy (et, edges, num_edges * sizeof(EDgeStruct));
    memcpy (tt, tris, num_tris * sizeof(TRiangleStruct));

    fptr->nodes = nt;
    fptr->edges = et;
    fptr->tris = tt;
    fptr->num_nodes = num_nodes;
    fptr->num_edges = num_edges;
    fptr->num_tris = num_tris;
    fptr->id = id;

    bsuccess = true;

    return 1;

}



/*-------------------------------------------------------------------------*/

/*
 * Add the nodes from the specified trimesh top the specified fault id.
 * If the fault id doesn't exist yet, this method is identical to setFault.
 */
int FaultConnect::addToFault (
    int               id,
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
)

{
    _SUrfStruct_      *fptr = NULL;
    int               istat;

    int       ntot, start, size;
    double    *xa = NULL, *ya = NULL, *za = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xa);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    fptr = FindInputFaultForID (id);

    if (fptr == NULL) {
        istat = setFault (
            id,
            nodes,
            num_nodes,
            edges,
            num_edges,
            tris,
            num_tris
        );
        return istat;
    }

    if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
        return 1;
    }
    if (num_nodes < 3  ||  num_edges < 3  ||  num_tris < 1) {
        return 1;
    }

/*
 * Grow the x, y and z lists if needed.
 */

    ntot = fptr->num_nodes + fptr->npts + num_nodes;
    xa = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + ntot;
    za = ya + ntot;

/*
 * Copy any existing x, y and z points into the new arrays.
 * Free the existing x, y and z after the copy is done.
 */
    start = 0;
    if (fptr->npts > 0) {
        size = fptr->npts * sizeof(double);
        memcpy (xa, fptr->x, size);
        memcpy (ya, fptr->y, size);
        memcpy (za, fptr->z, size);
        start = fptr->npts;
        csw_Free (fptr->x);
        fptr->x = NULL;
        fptr->y = NULL;
        fptr->z = NULL;
    }

    int        i, n;

/*
 * Copy the existing nodes into the x, y and z arrays.
 */
    n = start;
    for (i=0; i<fptr->num_nodes; i++) {
        xa[n] = fptr->nodes[i].x;
        ya[n] = fptr->nodes[i].y;
        za[n] = fptr->nodes[i].z;
        n++;
    }

/*
 * Once the node have been copied the first time, csw_Free and null
 * the trimesh data in the fptr object.
 */
    csw_Free (fptr->nodes);
    csw_Free (fptr->edges);
    csw_Free (fptr->tris);
    fptr->nodes = NULL;
    fptr->edges = NULL;
    fptr->tris = NULL;
    fptr->num_nodes = 0;
    fptr->num_edges = 0;
    fptr->num_tris = 0;

/*
 * Copy the new nodes into the x, y and z arrays.
 */
    for (i=0; i<num_nodes; i++) {
        xa[n] = nodes[i].x;
        ya[n] = nodes[i].y;
        za[n] = nodes[i].z;
        n++;
    }

/*
 * Put the new x, y and z arrays into the fptr object.
 */
    fptr->x = xa;
    fptr->y = ya;
    fptr->z = za;
    fptr->npts = n;

    fptr->recalc = 1;

    bsuccess = true;

    return 1;

}





/*-------------------------------------------------------------------------*/

/*
 * Add the specified points to the fptr member of the object.
 * If the fptr is currently NULL, a new one is created.
 */
int FaultConnect::addToFault (
    int               id,
    double            *x,
    double            *y,
    double            *z,
    int               npts
)
{
    _SUrfStruct_    *fptr = NULL;

    int       ntot, start, size;
    double    *xa = NULL, *ya = NULL, *za = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xa);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (x == NULL  ||  y == NULL  ||  z == NULL  ||  npts < 1) {
        return 1;
    }

    fptr = FindInputFaultForID (id);
    if (fptr == NULL) {
        fptr = NextInputFaultSurf ();
    }

    if (fptr == NULL) {
        return -1;
    }

    fptr->id = id;

/*
 * Grow the x, y and z lists if needed.
 */
    ntot = fptr->num_nodes + fptr->npts + npts;
    xa = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + ntot;
    za = ya + ntot;

/*
 * Copy any existing x, y and z points into the new arrays.
 * Free the existing x, y and z after the copy is done.
 */
    start = 0;
    if (fptr->npts > 0) {
        size = fptr->npts * sizeof(double);
        memcpy (xa, fptr->x, size);
        memcpy (ya, fptr->y, size);
        memcpy (za, fptr->z, size);
        start = fptr->npts;
        csw_Free (fptr->x);
        fptr->x = NULL;
        fptr->y = NULL;
        fptr->z = NULL;
    }

    int        i, n;

/*
 * Copy the existing nodes into the x, y and z arrays.
 */
    n = start;
    for (i=0; i<fptr->num_nodes; i++) {
        xa[n] = fptr->nodes[i].x;
        ya[n] = fptr->nodes[i].y;
        za[n] = fptr->nodes[i].z;
        n++;
    }

/*
 * Once the nodes have been copied the first time, csw_Free and null
 * the trimesh data in the fptr object.
 */
    csw_Free (fptr->nodes);
    csw_Free (fptr->edges);
    csw_Free (fptr->tris);
    fptr->nodes = NULL;
    fptr->edges = NULL;
    fptr->tris = NULL;
    fptr->num_nodes = 0;
    fptr->num_edges = 0;
    fptr->num_tris = 0;

/*
 * Copy the new points into the x, y and z arrays.
 */
    for (i=0; i<npts; i++) {
        xa[n] = x[i];
        ya[n] = y[i];
        za[n] = z[i];
        n++;
    }

/*
 * Put the new x, y and z arrays into the fptr object.
 */
    fptr->x = xa;
    fptr->y = ya;
    fptr->z = za;
    fptr->npts = n;

    fptr->recalc = 1;

    bsuccess = true;

    return 1;

}


/*-------------------------------------------------------------------------*/

/*
 * Return the fault in the input fault list that has the specified id.
 */

_SUrfStruct_ *FaultConnect::FindInputFaultForID (int id)
{
    _SUrfStruct_    *fptr;
    int             i;

    if (input_faults == NULL) {
        return NULL;
    }

    for (i=0; i<num_input_faults; i++) {
        fptr = input_faults + i;
        if (fptr->id == id) {
            return fptr;
        }
    }

    return NULL;
}


/*-------------------------------------------------------------------------*/

/*
 * Return the fault in the pad fault list that has the specified id.
 */

_SUrfStruct_ *FaultConnect::FindPadFaultForID (int id)
{
    _SUrfStruct_    *fptr;
    int             i;

    if (pad_faults == NULL) {
        return NULL;
    }

    for (i=0; i<num_pad_faults; i++) {
        fptr = pad_faults + i;
        if (fptr->id == id) {
            return fptr;
        }
    }

    return NULL;
}


/*-------------------------------------------------------------------------*/

/*
 * Return the fault in the seal fault list that has the specified id.
 */

_SUrfStruct_ *FaultConnect::FindSealFaultForID (int id)
{
    _SUrfStruct_    *fptr;
    int             i;

    if (seal_faults == NULL) {
        return NULL;
    }

    for (i=0; i<num_seal_faults; i++) {
        fptr = seal_faults + i;
        if (fptr->id == id) {
            return fptr;
        }
    }

    return NULL;
}


/*------------------------------------------------------------------------*/

/*
 * Get the pointer to the next unused input fault in the list.
 */

_SUrfStruct_ *FaultConnect::NextInputFaultSurf (void)
{
    _SUrfStruct_    *fptr;

/*
 * Grow the list if needed.
 */
    if (num_input_faults >= max_input_faults) {
        max_input_faults += 10;
        input_faults = (_SUrfStruct_ *)csw_Realloc
            (input_faults, max_input_faults * sizeof (_SUrfStruct_));
    }

    if (input_faults == NULL) {
        return NULL;
    }

    fptr = input_faults + num_input_faults;
    memset (fptr, 0, sizeof(_SUrfStruct_));

    num_input_faults++;

    return fptr;

}


/*------------------------------------------------------------------------*/

/*
 * Get the pointer to the next unused pad fault in the list.
 */

_SUrfStruct_ *FaultConnect::NextPadFaultSurf (void)
{
    _SUrfStruct_    *fptr;

/*
 * Grow the list if needed.
 */
    if (num_pad_faults >= max_pad_faults) {
        max_pad_faults += 10;
        pad_faults = (_SUrfStruct_ *)csw_Realloc
            (pad_faults, max_pad_faults * sizeof (_SUrfStruct_));
    }

    if (pad_faults == NULL) {
        return NULL;
    }

    fptr = pad_faults + num_pad_faults;
    memset (fptr, 0, sizeof(_SUrfStruct_));

    num_pad_faults++;

    return fptr;

}


/*------------------------------------------------------------------------*/

/*
 * Get the pointer to the next unused seal fault in the list.
 */

_SUrfStruct_ *FaultConnect::NextSealFaultSurf (void)
{
    _SUrfStruct_    *fptr;

/*
 * Grow the list if needed.
 */
    if (num_seal_faults >= max_seal_faults) {
        max_seal_faults += 10;
        seal_faults = (_SUrfStruct_ *)csw_Realloc
            (seal_faults, max_seal_faults * sizeof (_SUrfStruct_));
    }

    if (seal_faults == NULL) {
        return NULL;
    }

    fptr = seal_faults + num_seal_faults;
    memset (fptr, 0, sizeof(_SUrfStruct_));

    num_seal_faults++;

    return fptr;

}


/*--------------------------------------------------------------------------*/

/*
 * Get the detachment surface after the fault connections have
 * been embedded into it.  The nodes, edges and triangles arrays
 * returned from this are copies that should be csw_Freed by the
 * calling function when needed.
 */
int FaultConnect::getConnectedDetachment (
    NOdeStruct        **nodes,
    int               *num_nodes,
    EDgeStruct        **edges,
    int               *num_edges,
    TRiangleStruct    **tris,
    int               *num_tris,
    int               *sgpflag,
    double            *sgpdata)
{
    NOdeStruct        *np = NULL;
    EDgeStruct        *ep = NULL;
    TRiangleStruct    *tp = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (np);
            csw_Free (ep);
            csw_Free (tp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Initialize output in case of error.
 */
    *nodes = NULL;
    *edges = NULL;
    *tris = NULL;
    *num_nodes = 0;
    *num_edges = 0;
    *num_tris = 0;

    if (sealed_detach_surf == NULL) {
        return 0;
    }

/*
 * Allocate space for the trimesh copy.
 */
    np = (NOdeStruct *)csw_Malloc (sealed_detach_surf->num_nodes * sizeof(NOdeStruct));
    ep = (EDgeStruct *)csw_Malloc (sealed_detach_surf->num_edges * sizeof(EDgeStruct));
    tp = (TRiangleStruct *)csw_Malloc (sealed_detach_surf->num_tris * sizeof(TRiangleStruct));

    if (np == NULL  ||  ep == NULL  ||  tp == NULL) {
        return -1;
    }

/*
 * Make the copy and return.
 */
    memcpy (np, sealed_detach_surf->nodes,
            sealed_detach_surf->num_nodes * sizeof(NOdeStruct));
    memcpy (ep, sealed_detach_surf->edges,
            sealed_detach_surf->num_edges * sizeof(EDgeStruct));
    memcpy (tp, sealed_detach_surf->tris,
            sealed_detach_surf->num_tris * sizeof(TRiangleStruct));

    *nodes = np;
    *edges = ep;
    *tris = tp;
    *num_nodes = sealed_detach_surf->num_nodes;
    *num_edges = sealed_detach_surf->num_edges;
    *num_tris = sealed_detach_surf->num_tris;

    *sgpflag = 0;
    memset (sgpdata, 0, 6 * sizeof(double));
    if (sealed_detach_surf->sgp) {
        *sgpflag = 1;
        sealed_detach_surf->sgp->getCoefsAndOrigin (
            sgpdata,
            sgpdata + 1,
            sgpdata + 2,
            sgpdata + 3,
            sgpdata + 4,
            sgpdata + 5);
    }

    bsuccess = true;

    return 1;

}


/*--------------------------------------------------------------------------*/

/*
 * Return the number of faults that have been connected
 * to the detachment surface.
 */
int FaultConnect::getNumberOfConnectedFaults (void)
{
    return num_seal_faults;
}




/*--------------------------------------------------------------------------*/

/*
 * Return the trimesh and id for a fault that has been connected
 * to the detachment surface.
 */
int FaultConnect::getConnectedFault (
    int               index,
    int               *id,
    NOdeStruct        **nodes,
    int               *num_nodes,
    EDgeStruct        **edges,
    int               *num_edges,
    TRiangleStruct    **tris,
    int               *num_tris,
    int               *sgpflag,
    double            *sgpdata)
{
    NOdeStruct        *np = NULL;
    EDgeStruct        *ep = NULL;
    TRiangleStruct    *tp = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (np);
            csw_Free (ep);
            csw_Free (tp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Initialize output in case of error.
 */
    *nodes = NULL;
    *edges = NULL;
    *tris = NULL;
    *num_nodes = 0;
    *num_edges = 0;
    *num_tris = 0;

    if (index < 0  ||  index >= num_seal_faults) {
        return 0;
    }

    if (seal_faults == NULL  ||  num_seal_faults < 1) {
        return 0;
    }

    _SUrfStruct_    *fptr;

    fptr = seal_faults + index;

/*
 * Allocate space for the trimesh copy.
 */
    np = (NOdeStruct *)csw_Malloc (fptr->num_nodes * sizeof(NOdeStruct));
    ep = (EDgeStruct *)csw_Malloc (fptr->num_edges * sizeof(EDgeStruct));
    tp = (TRiangleStruct *)csw_Malloc (fptr->num_tris * sizeof(TRiangleStruct));

    if (np == NULL  ||  ep == NULL  ||  tp == NULL) {
        return -1;
    }

/*
 * Make the copy and return.
 */
    memcpy (np, fptr->nodes,
            fptr->num_nodes * sizeof(NOdeStruct));
    memcpy (ep, fptr->edges,
            fptr->num_edges * sizeof(EDgeStruct));
    memcpy (tp, fptr->tris,
            fptr->num_tris * sizeof(TRiangleStruct));

    *nodes = np;
    *edges = ep;
    *tris = tp;
    *num_nodes = fptr->num_nodes;
    *num_edges = fptr->num_edges;
    *num_tris = fptr->num_tris;
    *id = fptr->id;

    *sgpflag = 0;
    memset (sgpdata, 0, 6 * sizeof(double));
    if (fptr->sgp) {
        *sgpflag = 1;
        fptr->sgp->getCoefsAndOrigin (
            sgpdata,
            sgpdata + 1,
            sgpdata + 2,
            sgpdata + 3,
            sgpdata + 4,
            sgpdata + 5);
    }

    bsuccess = true;

    return 1;

}


/*--------------------------------------------------------------------------*/

/*
 * Return a copy of the xyz points for the contact line between the specified
 * index of fault and the detachment.  The id of the fault that uses
 * the contact line is also returned.  The x, y and z arrays are allocated
 * separately here.  The calling function should csw_Free them separately
 * when needed.
 */
int FaultConnect::getFaultContactLine (
    int               index,
    int               *id,
    double            **x,
    double            **y,
    double            **z,
    int               *npts)
{

    _ILineStruct_    *lptr = NULL;
    double           *xa = NULL, *ya = NULL, *za = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xa);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Initialize output in case of error.
 */
    *id = -1;
    *x = NULL;
    *y = NULL;
    *z = NULL;
    *npts = 0;

    if (seal_fault_contacts == NULL  ||  num_seal_fault_contacts < 1) {
        return 0;
    }

    if (seal_faults == NULL) {
        return 0;
    }

    if (index < 0  ||  index >= num_seal_fault_contacts) {
        return 0;
    }

/*
 * Allocate space for the copy.
 */
    lptr = seal_fault_contacts + index;
    xa = (double *)csw_Malloc (lptr->npts * sizeof(double));
    ya = (double *)csw_Malloc (lptr->npts * sizeof(double));
    za = (double *)csw_Malloc (lptr->npts * sizeof(double));

    if (xa == NULL  ||  ya == NULL  ||  za == NULL) {
        csw_Free (xa);
        csw_Free (ya);
        csw_Free (za);
        return -1;
    }

/*
 * Make the copy.
 */
    memcpy (xa, lptr->x, lptr->npts * sizeof(double));
    memcpy (ya, lptr->y, lptr->npts * sizeof(double));
    memcpy (za, lptr->z, lptr->npts * sizeof(double));

    *x = xa;
    *y = ya;
    *z = za;
    *npts = lptr->npts;
    *id = lptr->fid;

    bsuccess = true;

    return 1;

}


/*-------------------------------------------------------------------------------*/

/*
 * If a trimesh exists for the input detachment, find the average
 * edge length for later use as the padded grid spacing.
 */
void FaultConnect::CalcAverageEdgeLength (void)
{
    int          i, n;
    EDgeStruct   *eptr;

    if (detach_surf == NULL) {
        average_edge_length = -1.0;
        return;
    }

    average_edge_length = -1.0;
    if (detach_surf->edges != NULL) {
        average_edge_length = 0.0;
        n = 0;
        for (i=0; i<detach_surf->num_edges; i++) {
            eptr = detach_surf->edges + i;
            if (eptr->deleted == 0) {
                average_edge_length += EdgeLength (eptr, detach_surf->nodes);
                n++;
            }
        }

        if (n > 0) {
             average_edge_length /= n;
        }
        else {
             average_edge_length = -1.0;
        }
    }

/*
 * The original triangles were right isoceles, which means about
 * 1/3 of the edges have cell diagonal lengths.  To correct for this
 * divide by 1.13 (2 * 1 + 1.4) / 3.0
 */
    if (average_edge_length > 0.0) {
        average_edge_length /= 1.13;
    }

    return;

}

/*-------------------------------------------------------------------------------*/

/*
 * Find the cumulative x, y, z limits of the detachment and faults.
 * Put them in dxmin, dymin, etc.
 */

void FaultConnect::CalcXYZLimits (void)
{
    NOdeStruct    *nodes, *nptr;
    int           i, j, nn;

    dxmin = 1.e30;
    dymin = 1.e30;
    dzmin = 1.e30;
    dxmax = -1.e30;
    dymax = -1.e30;
    dzmax = -1.e30;

    if (detach_surf != NULL) {
        nodes = detach_surf->nodes;
        nn = detach_surf->num_nodes;
        for (j=0; j<nn; j++) {
            nptr = nodes + j;
            if (nptr->deleted == 1) continue;
            if (nptr->x < dxmin) dxmin = nptr->x;
            if (nptr->y < dymin) dymin = nptr->y;
            if (nptr->z < dzmin) dzmin = nptr->z;
            if (nptr->x > dxmax) dxmax = nptr->x;
            if (nptr->y > dymax) dymax = nptr->y;
            if (nptr->z > dzmax) dzmax = nptr->z;
        }
    }

    if (input_faults != NULL) {
        for (i=0; i<num_input_faults; i++) {
            nodes = input_faults[i].nodes;
            nn = input_faults[i].num_nodes;
            for (j=0; j<nn; j++) {
                nptr = nodes + j;
                if (nptr->deleted == 1) continue;
                if (nptr->x < dxmin) dxmin = nptr->x;
                if (nptr->y < dymin) dymin = nptr->y;
                if (nptr->z < dzmin) dzmin = nptr->z;
                if (nptr->x > dxmax) dxmax = nptr->x;
                if (nptr->y > dymax) dymax = nptr->y;
                if (nptr->z > dzmax) dzmax = nptr->z;
            }
        }
    }

    return;

}


/*-------------------------------------------------------------------------------*/

/*
 * This is the public method called to do the work of connecting things
 * together after all the fault and detachment data have been defined.  On
 * success, 1 is returned.  On a system failure, -1 is returned.  If the
 * object has not been set up properly for calculating the connections,
 * zero is returned.
 */
int FaultConnect::connectFaults (void)
{
    NOdeStruct      *nodes = NULL;
    EDgeStruct      *edges = NULL;
    TRiangleStruct  *tris = NULL;
    int             nn, ne, nt;

    int    i, istat;
    _SUrfStruct_  *fptr = NULL;

    SealedModel       *smodel = NULL;
    double            v6[6];
    SurfaceGroupPlane *sgp = NULL;
    int               vflag;

    CSWTriMeshStruct       *dsurf = NULL;
    CSWTriMeshStruct       *flist = NULL;
    int                    nflist;
    _INtersectionLineList_ *linelist = NULL;

    CSWTriMeshStruct *ftmesh = NULL;
    _SUrfStruct_     *fseal = NULL, *fpad = NULL;

    char fname[100];
    int do_write;

    int                       nlines;
    const _INtersectionLine_  *iptr = NULL;
    _ILineStruct_             *lptr = NULL;
    double                    *x = NULL, *y = NULL, *z = NULL;
    int                       npts;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        delete (smodel);

        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (tris);
            FreeSurf (sealed_detach_surf, 1);
            FreeSurf (seal_faults, num_seal_faults);
            FreeLine (seal_fault_contacts, num_seal_fault_contacts);
            sealed_detach_surf = NULL;
            seal_faults = NULL;
            seal_fault_contacts = NULL;
            num_seal_faults = 0;
            num_seal_fault_contacts = 0;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    do_write = csw_GetDoWrite ();

    if (detach_surf == NULL) {
        return 0;
    }

    if (input_faults == NULL  ||  num_input_faults < 1) {
        return 0;
    }

    CalcXYZLimits ();

    if (average_edge_length <= 0.0) {
        CalcAverageEdgeLength ();
    }

    istat = PadDetachment ();
    if (istat != 1) {
        return istat;
    }

/*
 * Pad the faults to insure intersection with the detachment.
 */
    for (i=0; i<num_input_faults; i++) {
        istat = PadFault (i);
        if (istat != 1) {
            return istat;
        }
    }

/*
 * Create a sealed model object and use it to intersect, seal and
 * clip the faults and detachment.  It was easier to add a slightly
 * different workflow to SealedModel than to replicate a lot of its
 * functionality here in this class.
 */
    if (pad_detach_surf == NULL  ||  pad_faults == NULL) {
        return -1;
    }

    try {
        smodel = new SealedModel ();
    }
    catch (...) {
        smodel = NULL;
        return -1;
    }

    istat =
      smodel->setPaddedDetachment (
        pad_detach_surf->nodes, pad_detach_surf->num_nodes,
        pad_detach_surf->edges, pad_detach_surf->num_edges,
        pad_detach_surf->tris, pad_detach_surf->num_tris);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<num_pad_faults; i++) {
        fptr = pad_faults + i;
        sgp = fptr->sgp;
        v6[0] = 1.e30;
        v6[1] = 1.e30;
        v6[2] = 1.e30;
        v6[3] = 1.e30;
        v6[4] = 1.e30;
        v6[5] = 1.e30;
        vflag = 0;
        if (sgp != NULL) {
            istat =
              sgp->getCoefsAndOrigin (
                v6,
                v6+1,
                v6+2,
                v6+3,
                v6+4,
                v6+5);
            if (istat != 1) {
                return -1;
            }
            vflag = 1;
        }

        istat =
          smodel->addPaddedFault (
              fptr->id, vflag, v6,
              fptr->nodes, fptr->num_nodes,
              fptr->edges, fptr->num_edges,
              fptr->tris, fptr->num_tris);
        if (istat == -1) {
            return -1;
        }
    }

    istat =
      smodel->sealFaultsToDetachment ();
    if (istat == -1) {
        return -1;
    }
    if (istat == -2) {
        return -2;
    }

/*
 * Retrieve the sealed detachment and sealed faults from the
 * sealed model object.
 */
    dsurf =
      smodel->getSealedDetachment ();
    smodel->getSealedFaults (&flist, &nflist);
    linelist = smodel->getRawIntersectionLines ();

    if (dsurf == NULL  ||  flist == NULL  ||  nflist < 1) {
        return -1;
    }
    if (linelist == NULL) {
        return -1;
    }
    if (linelist->nlist < 1) {
        return -1;
    }

/*
 * Free the old sealed detachment and sealed faults.
 */
    FreeSurf (sealed_detach_surf, 1);
    FreeSurf (seal_faults, num_seal_faults);
    FreeLine (seal_fault_contacts, num_seal_fault_contacts);

    sealed_detach_surf = NULL;
    seal_faults = NULL;
    seal_fault_contacts = NULL;

    num_seal_faults = 0;
    num_seal_fault_contacts = 0;

/*
 * Copy into the sealed_detach_surf for
 * later retrieval via the accessor methods.
 */
    sealed_detach_surf = (_SUrfStruct_ *)csw_Calloc (sizeof(_SUrfStruct_));
    seal_faults = (_SUrfStruct_*)csw_Calloc (nflist * sizeof(_SUrfStruct_));
    if (sealed_detach_surf == NULL  ||  seal_faults == NULL) {
        return -1;
    }

    nn = dsurf->num_nodes;
    ne = dsurf->num_edges;
    nt = dsurf->num_tris;

    nodes = (NOdeStruct *)csw_Malloc (nn * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (ne * sizeof(EDgeStruct));
    tris = (TRiangleStruct *)csw_Malloc (nt * sizeof(TRiangleStruct));
    if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
        return -1;
    }

    memcpy (nodes, dsurf->nodes, nn * sizeof(NOdeStruct));
    memcpy (edges, dsurf->edges, ne * sizeof(EDgeStruct));
    memcpy (tris, dsurf->tris, nt * sizeof(TRiangleStruct));

    sealed_detach_surf->nodes = nodes;
    sealed_detach_surf->num_nodes = nn;
    sealed_detach_surf->edges = edges;
    sealed_detach_surf->num_edges = ne;
    sealed_detach_surf->tris = tris;
    sealed_detach_surf->num_tris = nt;

/*
 * Copy the sealed faults into the structures used
 * by the accessor methods.
 */
    for (i=0; i<nflist; i++) {

        ftmesh = flist + i;
        fseal = seal_faults + i;
        fpad = FindPadFaultForID (ftmesh->external_id);
        if (fpad == NULL) {
            assert (0);
        }

        nn = ftmesh->num_nodes;
        ne = ftmesh->num_edges;
        nt = ftmesh->num_tris;

        nodes = (NOdeStruct *)csw_Malloc (nn * sizeof(NOdeStruct));
        edges = (EDgeStruct *)csw_Malloc (ne * sizeof(EDgeStruct));
        tris = (TRiangleStruct *)csw_Malloc (nt * sizeof(TRiangleStruct));
        if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
            return -1;
        }

        memcpy (nodes, ftmesh->nodes, nn * sizeof(NOdeStruct));
        memcpy (edges, ftmesh->edges, ne * sizeof(EDgeStruct));
        memcpy (tris, ftmesh->tris, nt * sizeof(TRiangleStruct));

        fseal->nodes = nodes;
        fseal->num_nodes = nn;
        fseal->edges = edges;
        fseal->num_edges = ne;
        fseal->tris = tris;
        fseal->num_tris = nt;

        if (fpad->sgp != NULL) {
            try {
                if (fseal->sgp != NULL) {
                    delete fseal->sgp;
                    fseal->sgp = NULL;
                }
                fseal->sgp = new SurfaceGroupPlane (fpad->sgp);
            }
            catch (...) {
                fseal->sgp = NULL;
                return -1;
            }
        }
        fseal->id = fpad->id;

        if (do_write) {
            sprintf (fname, "confault_%d.tri", i);
            grd_fileio_ptr->grd_write_text_tri_mesh_file (
                0, NULL,
                tris, nt,
                edges, ne,
                nodes, nn,
                fname);
        }

    }

    num_seal_faults = nflist;

/*
 * Copy into the seal_fault_contacts for later access.
 */
    FreeLine (seal_fault_contacts, num_seal_fault_contacts);
    seal_fault_contacts = NULL;
    num_seal_fault_contacts = 0;

    nlines = linelist->nlist;
    seal_fault_contacts = (_ILineStruct_*)
        csw_Calloc (nlines * sizeof(_ILineStruct_));
    if (seal_fault_contacts == NULL) {
        return -1;
    }

    for (i=0; i<nlines; i++) {
        iptr = linelist->list + i;
        lptr = seal_fault_contacts + i;
        npts = iptr->npts;
        x = (double *)csw_Malloc (3 * npts * sizeof(double));
        if (x == NULL) {
            return -1;
        }
        y = x + npts;
        z = y + npts;
        memcpy (x, iptr->x, npts * sizeof(double));
        memcpy (y, iptr->y, npts * sizeof(double));
        memcpy (z, iptr->z, npts * sizeof(double));
        lptr->x = x;
        lptr->y = y;
        lptr->z = z;
        lptr->npts = npts;
        if (iptr->external_id1 >= 0) {
            lptr->fid = iptr->external_id1;
        }
        else if (iptr->external_id2 >= 0) {
            lptr->fid = iptr->external_id2;
        }
    }
    num_seal_fault_contacts = nlines;

    bsuccess = true;

    return 1;

}




/*-------------------------------------------------------------------------------*/

/*
 * This private method extends the detachment surface to a little further
 * than the extents defined by dxmin, dymin, dxmax and dymax.  If these
 * limits have not been defined, they are calculated from the data available
 * in the input detachment surface.
 */
int FaultConnect::PadDetachment (void)
{
    double      *xa, *ya, *za, dx, dy;
    int         i, n, ntot, npts, istat;

    if (detach_surf == NULL) {
        assert (0);
    }

    if (detach_surf->nodes == NULL  &&  detach_surf->x == NULL) {
        return 0;
    }

/*
 * Allocate space for detachment x y and z arrays.
 */
    ntot = detach_surf->npts + detach_surf->num_nodes;
    if (ntot < 3) {
        return 0;
    }

    xa = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + ntot;
    za = ya + ntot;

/*
 * Populate the xyz arrays from nodes and points in the input
 * detachment, whichever are available.
 */
    n = 0;
    if (detach_surf->nodes != NULL) {
        NOdeStruct *nptr;
        for (i=0; i<detach_surf->num_nodes; i++) {
            nptr = detach_surf->nodes + i;
            if (nptr->deleted == 0) {
                xa[n] = nptr->x;
                ya[n] = nptr->y;
                za[n] = nptr->z;
                n++;
            }
        }
    }

    if (detach_surf->x != NULL) {
        for (i=0; i<detach_surf->npts; i++) {
            xa[n] = detach_surf->x[i];
            ya[n] = detach_surf->y[i];
            za[n] = detach_surf->z[i];
            n++;
        }
    }

    if (n < 3) {
        csw_Free (xa);
        return 0;
    }

    npts = n;

/*
 * If the detachment padding limits have not been set,
 * calculate them from the point bounding box.
 */
    if (dxmin >= dxmax  ||  dymin >= dymax) {
        dxmin = 1.e30;
        dymin = 1.e30;
        dxmax = -1.e30;
        dymax = -1.e30;
        dzmin = 1.e30;
        dzmax = -1.e30;
        for (i=0; i<n; i++) {
            if (xa[i] < dxmin) dxmin = xa[i];
            if (ya[i] < dymin) dymin = ya[i];
            if (xa[i] > dxmax) dxmax = xa[i];
            if (ya[i] > dymax) dymax = ya[i];
            if (za[i] < dzmin) dzmin = za[i];
            if (za[i] > dzmax) dzmax = za[i];
        }
        if (dxmin >= dxmax  ||  dymin >= dymax) {
            csw_Free (xa);
            return 0;
        }

    }

    if (dxmin >= dxmax  ||  dymin >= dymax) {
        csw_Free (xa);
        return 0;
    }

    dx = dxmax - dxmin;
    dy = dymax - dymin;
    dx /= 100.0;
    dy /= 100.0;
    dxmin -= dx;
    dymin -= dy;
    dxmax += dx;
    dymax += dy;

/*
 * Calculate a best fit plane to the detachment and
 * save its normal and a point on it.
 */
    istat =
      CalcDetachmentPlane (xa, ya, za, npts);
    if (istat == -1) {
        csw_Free (xa);
        return -1;
    }

/*
 * Allocate space for a grid of the detachment.
 */
    CSW_F    *grid;
    int      ncol, nrow;

    if (average_edge_length > 0.0) {
        ncol = (int)((dxmax - dxmin) / average_edge_length + 1.5);
        nrow = (int)((dymax - dymin) / average_edge_length + 1.5);
    }
    else {
        istat =
          grd_utils_ptr->grd_recommended_size (
            xa, ya, npts,
            &dxmin, &dymin, &dxmax, &dymax,
            &ncol, &nrow);
        if (istat != 1) {
            csw_Free (xa);
            return istat;
        }
    }

    if (ncol < 2) ncol = 2;
    if (nrow < 2) nrow = 2;

    grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (grid == NULL) {
        csw_Free (xa);
        return -1;
    }

/*
 * Calculate a grid of the detachment using default options.
 */
    istat =
      grd_calc_ptr->grd_calc_grid_from_double (
        xa, ya, za, NULL, npts,
        grid, NULL, NULL, ncol, nrow,
        dxmin, dymin, dxmax, dymax,
        NULL, 0, NULL);

    csw_Free (xa);
    xa = ya = za = NULL;
    if (istat != 1) {
        csw_Free (grid);
        return istat;
    }

/*
 * Convert the grid into a trimesh without constraints.
 */
    NOdeStruct    *nodes;
    EDgeStruct    *edges;
    TRiangleStruct  *tris;
    int           n_nodes, n_edges, n_tris;

    istat =
      grd_triangle_ptr->grd_calc_tri_mesh_from_grid (
        grid, ncol, nrow,
        dxmin, dymin, dxmax, dymax,
        NULL, NULL, NULL,
        NULL, NULL, 0,
        GRD_EQUILATERAL,
        &nodes,
        &edges,
        &tris,
        &n_nodes,
        &n_edges,
        &n_tris);
    csw_Free (grid);
    grid = NULL;

    if (istat != 1) {
        csw_Free (grid);
        return istat;
    }

/*
 * Create a new padded detachment surface and populate it.
 */
    if (pad_detach_surf) {
        FreeSurf (pad_detach_surf, 1);
        pad_detach_surf = NULL;
    }

    pad_detach_surf = (_SUrfStruct_ *)csw_Calloc (sizeof(_SUrfStruct_));
    if (pad_detach_surf == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (tris);
        return -1;
    }

    pad_detach_surf->nodes = nodes;
    pad_detach_surf->edges = edges;
    pad_detach_surf->tris = tris;
    pad_detach_surf->num_nodes = n_nodes;
    pad_detach_surf->num_edges = n_edges;
    pad_detach_surf->num_tris = n_tris;

    int do_write;
    do_write = csw_GetDoWrite ();
    if (do_write) {
        char fname[100];
        sprintf (fname, "pad_detach.tri");
        grd_fileio_ptr->grd_write_text_tri_mesh_file (
            0, NULL,
            tris, n_tris,
            edges, n_edges,
            nodes, n_nodes,
            fname);
    }

    return 1;

}

/*----------------------------------------------------------------------------*/

int FaultConnect::PadFault (int index)
{
    if (input_faults == NULL  ||  num_input_faults < 1) {
        return 0;
    }
    if (index < 0  ||  index >= num_input_faults) {
        return 0;
    }

    _SUrfStruct_    *sptr, *pptr;
    SurfaceGroupPlane    *sgp;

    sptr = input_faults + index;

    double          *xa, *ya, *za;
    double          tx[3], ty[3], tz[3];
    int             n1, n2, n3, ntot, i, n;
    int             istat, npts;
    TRiangleStruct  *tptr;

/*
 * Collect the centerpoints of non horizontal triangles that
 * are not close to the detachment.
 */
    ntot = sptr->num_tris;
    xa = (double *)csw_Malloc (9 * ntot * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + 3 * ntot;
    za = ya + 3 * ntot;

    int do_write;
    do_write = csw_GetDoWrite ();
    if (do_write) {
        char fname[100];
        sprintf (fname, "unpad_flt_%d.tri", index);
        grd_fileio_ptr->grd_write_text_tri_mesh_file (
            0, NULL,
            sptr->tris, sptr->num_tris,
            sptr->edges, sptr->num_edges,
            sptr->nodes, sptr->num_nodes,
            fname);
    }

    int    ndo;
    double max_z_normal = _MAX_Z_NORMAL_;

    for (ndo=0; ndo<10; ndo++) {

        for (i=0; i<sptr->num_nodes; i++) {
            sptr->nodes[i].shifted = 0;
        }

        n = 0;
        for (i=0; i<ntot; i++) {
            tptr = sptr->tris + i;
            istat =
              grd_triangle_ptr->grd_get_nodes_for_triangle (
                tptr, sptr->edges,
                &n1, &n2, &n3);
            if (istat != 1) {
                continue;
            }
            tx[0] = sptr->nodes[n1].x;
            ty[0] = sptr->nodes[n1].y;
            tz[0] = sptr->nodes[n1].z;
            tx[1] = sptr->nodes[n2].x;
            ty[1] = sptr->nodes[n2].y;
            tz[1] = sptr->nodes[n2].z;
            tx[2] = sptr->nodes[n3].x;
            ty[2] = sptr->nodes[n3].y;
            tz[2] = sptr->nodes[n3].z;
            TriangleNormal (tx, ty, tz);
            if (tnzNorm > max_z_normal) {
                continue;
            }
/*
            xt = (tx[0] + tx[1] + tx[2]) / 3.0;
            yt = (ty[0] + ty[1] + ty[2]) / 3.0;
            zt = (tz[0] + tz[1] + tz[2]) / 3.0;
*/

            if (sptr->nodes[n1].shifted == 0) {
                xa[n] = tx[0];
                ya[n] = ty[0];
                za[n] = tz[0];
                n++;
                sptr->nodes[n1].shifted = 0;
            }
            if (sptr->nodes[n2].shifted == 0) {
                xa[n] = tx[1];
                ya[n] = ty[1];
                za[n] = tz[1];
                n++;
                sptr->nodes[n2].shifted = 0;
            }
            if (sptr->nodes[n3].shifted == 0) {
                xa[n] = tx[2];
                ya[n] = ty[2];
                za[n] = tz[2];
                n++;
                sptr->nodes[n3].shifted = 0;
            }

        }

        if (n < 3) {
            max_z_normal *= 1.05;
            if (max_z_normal > .99) max_z_normal = .99;
            continue;
        }

        break;

    }

    for (i=0; i<sptr->num_nodes; i++) {
        sptr->nodes[i].shifted = 0;
    }

    if (n < 3) {
        csw_Free (xa);
        return 0;
    }

    npts = n;

    test_data_flag = 0;
    do_write = csw_GetDoWrite ();
    if (do_write) {
        CreateTestData (xa,ya, za, &npts);
        test_data_flag = 1;
    }

/*
 * Find some points that are near the minimum z of the pre rotated points.
 */
    int     lowlist[100], nlow, maxlow, imin;
    double  zmin = 1.e30;
    double  zmax = -1.e30;
    double  zcut, zcpct;

    imin = -1;
    for (i=0; i<npts; i++) {
        if (za[i] < zmin) {
            zmin = za[i];
            imin = i;
        }
        if (za[i] > zmax) zmax = za[i];
    }
    maxlow = npts / 20;
    if (maxlow < 1) {
        nlow = 1;
        lowlist[0] = imin;
    }
    else {
        zcpct = 10.0;
        if (maxlow > 100) {
            zcpct = (double)maxlow / 10.0;
            maxlow = 100;
        }
        if (zcpct > 100.0) zcpct = 100.0;
        zcut = zmin + (zmax - zmin) / zcpct;
        nlow = 0;
        for (i=0; i<npts; i++) {
            if (za[i] <= zcut) {
                lowlist[nlow] = i;
                nlow++;
            }
            if (nlow >= maxlow) break;
        }
    }

    if (do_write) {
      char    fname[200];
      sprintf (fname, "flt_pts_%d.xyz", index);
      grd_triangle_ptr->grd_WritePoints (
        xa, ya, za, npts,
        fname);
    }

/*
 * Set up the SurfaceGroupPlane object to rotate the
 * points to "horizontal".
 */
    sgp = sptr->sgp;
    if (sgp == NULL) {
        try {
            sgp = new SurfaceGroupPlane ();
        }
        catch (...) {
            sgp = NULL;
        }
        if (sgp == NULL) {
            return -1;
        }
        istat = sgp->addPointsForFit (xa, ya, za, npts);
        if (istat == -1) {
            delete sgp;
            return -1;
        }

        sgp->calcPlaneCoefs ();
        sgp->freeFitPoints ();

        sptr->sgp = sgp;
    }

    NOdeStruct        *nodes;
    EDgeStruct        *edges;
    TRiangleStruct    *tris;
    int               n_nodes,
                      n_edges,
                      n_tris;
    CSW_F             *grid;
    int               ncol, nrow;
    double            gxmin, gymin, gxmax, gymax, avspace;

/*
 * Set this rotaion object in the Padding functions.
 */
    pad_surface_obj.PadSetPadShapeGrid (NULL, 0, 0,
                        1.e30, 1.e30, -1.e30, -1.e30);
    pad_surface_obj.PadSetSurfaceGroupPlane (sgp);
    pad_surface_obj.PadSetSimPaddingFlag (0);

    grid = NULL;
    nodes = NULL;
    edges = NULL;
    tris = NULL;
    avspace = -1.0;
    n_nodes = 0;
    n_edges = 0;
    n_tris = 0;

/*
 * Calculate a local average spacing if a global onr is not available.
 */
    avspace = average_edge_length;
    if (avspace <= 0.0) {
        if (sptr->edges != NULL  &&  sptr->nodes != NULL) {
            EDgeStruct    *eptr;
            avspace = 0.0;
            n = 0;
            for (i=0; i<sptr->num_edges; i++) {
                eptr = sptr->edges + i;
                if (eptr->deleted == 0) {
                    avspace += EdgeLength (eptr, sptr->nodes);
                    n++;
                }
            }

            if (n > 0) {
                 avspace /= n;
            }
            else {
                 avspace = -1.0;
            }
        }
    }

    if (test_data_flag == 1) {
        avspace = -1.0;
    }

/*
 * Pad the fault surface downward in the dip direction only.
 */
    istat =
      pad_surface_obj.PadFaultSurfaceForSim (
        xa, ya, za, npts,
        lowlist, nlow,
        dxmin, dxmax,
        dymin, dymax,
        dzmin, dzmax,
        &grid, &ncol, &nrow,
        &gxmin, &gymin, &gxmax, &gymax,
        &nodes,
        &edges,
        &tris,
        &n_nodes,
        &n_edges,
        &n_tris,
        avspace);
    if (istat == -1) {
        return -1;
    }

    if (do_write) {
        char fname[100];
        sprintf (fname, "pad_flt_%d.tri", index);
        grd_fileio_ptr->grd_write_text_tri_mesh_file (
            0, NULL,
            tris, n_tris,
            edges, n_edges,
            nodes, n_nodes,
            fname);
    }


    csw_Free (grid);
    grid = NULL;

    if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (tris);
        return -1;
    }

    pptr = NextPadFaultSurf ();
    if (pptr == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (tris);
        return -1;
    }

    pptr->nodes = nodes;
    pptr->edges = edges;
    pptr->tris = tris;
    pptr->num_nodes = n_nodes;
    pptr->num_edges = n_edges;
    pptr->num_tris = n_tris;
    pptr->id = sptr->id;
    try {
        if (pptr->sgp != NULL) {
            delete pptr->sgp;
            pptr->sgp = NULL;
        }
        pptr->sgp = new SurfaceGroupPlane (sgp);
    }
    catch (...) {
        pptr->sgp = NULL;
    }

    return 1;

}


/*-------------------------------------------------------------------------*/

/*
 * Set a new fault to the specified trimesh.  If a fault with the
 * specified id already exists, it's trimesh and points are replaced
 * with the specified trimesh.
 */
int FaultConnect::setFault (
    int               id,
    int               sgpflag,
    double            *sgpdata,
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
)
{
    _SUrfStruct_      *fptr;

    fptr = FindInputFaultForID (id);
    if (fptr) {
        CleanSurf (fptr);
    }

    if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
        return 1;
    }
    if (num_nodes < 3  ||  num_edges < 3  ||  num_tris < 1) {
        return 1;
    }

    NOdeStruct     *nt;
    EDgeStruct     *et;
    TRiangleStruct *tt;

    nt = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    et = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    tt = (TRiangleStruct *)csw_Malloc (num_tris * sizeof(TRiangleStruct));

    if (nt == NULL  ||  et == NULL  ||  tt == NULL) {
        csw_Free (nt);
        csw_Free (et);
        csw_Free (tt);
        return -1;
    }

    if (fptr == NULL) {
        fptr = NextInputFaultSurf ();
    }
    if (fptr == NULL) {
        csw_Free (nt);
        csw_Free (et);
        csw_Free (tt);
        return -1;
    }

    memcpy (nt, nodes, num_nodes * sizeof(NOdeStruct));
    memcpy (et, edges, num_edges * sizeof(EDgeStruct));
    memcpy (tt, tris, num_tris * sizeof(TRiangleStruct));

    fptr->nodes = nt;
    fptr->edges = et;
    fptr->tris = tt;
    fptr->num_nodes = num_nodes;
    fptr->num_edges = num_edges;
    fptr->num_tris = num_tris;
    fptr->id = id;

    if (sgpflag) {
        SurfaceGroupPlane    *sgp = NULL;
        try {
            sgp = new SurfaceGroupPlane ();
        }
        catch (...) {
            sgp = NULL;
        }
        if (sgp != NULL) {
            sgp->setPlaneCoefs (sgpdata);
            sgp->setOrigin (sgpdata[3],
                            sgpdata[4],
                            sgpdata[5]);
        }
        fptr->sgp = sgp;
    }

    return 1;

}


/*------------------------------------------------------------------------*/

double FaultConnect::EdgeLength (EDgeStruct *eptr,
                                 NOdeStruct *nodes)
{
    if (eptr->node1 < 0  ||  eptr->node2 < 0) {
        return 0.0;
    }

    double      x1, y1, x2, y2;
    double      dx, dy, dist;

    x1 = nodes[eptr->node1].x;
    y1 = nodes[eptr->node1].y;
    x2 = nodes[eptr->node2].x;
    y2 = nodes[eptr->node2].y;

    dx = x1 - x2;
    dy = y1 - y2;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);

    return dist;

}


/*--------------------------------------------------------------------------------*/

/*
 * Calculate the normal vector to the specified triangle.
 * The normal will always have zero or positive z.  The
 * normal is scaled so that the length is 1.0.
 *
 * The components of the normal are put into the private
 * instance variables tnxNorm, tnyNorm and tnzNorm.
 */
void FaultConnect::TriangleNormal (double *x, double *y, double *z)
{
    double    x1, y1t, z1, x2, y2, z2,
              px, py, pz;
    double    dist;

    x1 = x[1] - x[0];
    y1t = y[1] - y[0];
    z1 = z[1] - z[0];
    x2 = x[2] - x[0];
    y2 = y[2] - y[0];
    z2 = z[2] - z[0];

    px = y1t * z2 - z1 * y2;
    py = z1 * x2 - x1 * z2;
    pz = x1 * y2 - y1t * x2;

  /*
   * If the z component of the normal points down, the triangle
   * is clockwise when viewed from above, and it needs to be
   * changed to ccw.
   */
    if (pz < 0.0) {
        px = -px;
        py = -py;
        pz = -pz;
    }

    dist = px * px + py * py + pz * pz;
    dist = sqrt (dist);

    if (dist <= 1.e-30) {
        tnxNorm = 0.0;
        tnyNorm = 0.0;
        tnzNorm = 1.0;
    }
    else {
        tnxNorm = px / dist;
        tnyNorm = py / dist;
        tnzNorm = pz / dist;
    }

    return;

}


/*-----------------------------------------------------------------------*/

int FaultConnect::CalcDetachmentPlane (
    double    *xa,
    double    *ya,
    double    *za,
    int       npts)
{
    int       istat;

    istat =
      grd_utils_ptr->grd_calc_double_plane (xa, ya, za, npts, dcoefs);
    if (istat == -1) {
        return -1;
    }

    return 1;

}

/*-----------------------------------------------------------------------*/

double FaultConnect::DetachmentPlaneDist (
    double x,
    double y,
    double z)
{
    double    zp, dist;

    zp = dcoefs[0] + dcoefs[1] * x + dcoefs[2] * y;
    dist = z - zp;

    return dist;
}



/*-------------------------------------------------------------------------*/

void FaultConnect::CreateTestData (double *xa,
                            double *ya,
                            double *za,
                            int *npts)
{
    FILE      *fptr;
    int       i;
    char      line[200], *ctmp;
    double    x[200], y[200], z[200];

    fptr = fopen ("sgplane.tst", "rb");

    if (fptr == NULL) {
        xa[0] = 10;
        xa[1] = 11;
        xa[2] = 11;
        xa[3] = 0;
        xa[4] = 0;
        xa[5] = -1;
        ya[0] = 0;
        ya[1] = 0;
        ya[2] = 1;
        ya[3] = -10;
        ya[4] = -11;
        ya[5] = -11;
        za[0] = 0;
        za[1] = 0;
        za[2] = -2;
        za[3] = 0;
        za[4] = 0;
        za[5] = -2;
        *npts = 6;
        return;
    }

    memset (x, 0, 200 * sizeof(double));
    memset (y, 0, 200 * sizeof(double));
    memset (z, 0, 200 * sizeof(double));

    i = 0;
    for (;;) {
        ctmp = fgets (line, 200, fptr);
        if (ctmp == NULL) {
            break;
        }
        sscanf (line, "%lf %lf %lf", x+i, y+i, z+i);
        i++;
        if (i == 200) break;
    }

    fclose (fptr);
    fptr = NULL;

    if (i < 6) {
        return;
    }

    *npts = i;
    memcpy (xa, x, i * sizeof(double));
    memcpy (ya, y, i * sizeof(double));
    memcpy (za, z, i * sizeof(double));

    return;

}

/*-------------------------------------------------------------------------*/

/*
 * Set the detach_lower_surf member of the object.  If a non NULL
 * detach_lower_surf already exists it is deleted and then this one
 * is used.
 */
int FaultConnect::setDetachmentLowerSurface (
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
)
{
    FreeSurf (detach_lower_surf, 1);
    detach_lower_surf = NULL;

    if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
        return 1;
    }
    if (num_nodes < 3  ||  num_edges < 3  ||  num_tris < 1) {
        return 1;
    }

    NOdeStruct     *nt;
    EDgeStruct     *et;
    TRiangleStruct *tt;

    nt = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    et = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    tt = (TRiangleStruct *)csw_Malloc (num_tris * sizeof(TRiangleStruct));
    detach_lower_surf = (_SUrfStruct_ *)csw_Calloc (sizeof(_SUrfStruct_));

    if (nt == NULL  ||  et == NULL  ||  tt == NULL  ||
        detach_lower_surf == NULL) {
        csw_Free (nt);
        csw_Free (et);
        csw_Free (tt);
        csw_Free (detach_lower_surf);
        return -1;
    }

    memcpy (nt, nodes, num_nodes * sizeof(NOdeStruct));
    memcpy (et, edges, num_edges * sizeof(EDgeStruct));
    memcpy (tt, tris, num_tris * sizeof(TRiangleStruct));

    detach_lower_surf->nodes = nt;
    detach_lower_surf->edges = et;
    detach_lower_surf->tris = tt;
    detach_lower_surf->num_nodes = num_nodes;
    detach_lower_surf->num_edges = num_edges;
    detach_lower_surf->num_tris = num_tris;

    return 1;

}

/*-------------------------------------------------------------------------*/

/*
 * Set the detach_upper_surf member of the object.  If a non NULL
 * detach_lower_surf already exists it is deleted and then this one
 * is used.
 */
int FaultConnect::setDetachmentUpperSurface (
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
)
{
    FreeSurf (detach_upper_surf, 1);
    detach_upper_surf = NULL;

    if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
        return 1;
    }
    if (num_nodes < 3  ||  num_edges < 3  ||  num_tris < 1) {
        return 1;
    }

    NOdeStruct     *nt;
    EDgeStruct     *et;
    TRiangleStruct *tt;

    nt = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    et = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    tt = (TRiangleStruct *)csw_Malloc (num_tris * sizeof(TRiangleStruct));
    detach_upper_surf = (_SUrfStruct_ *)csw_Calloc (sizeof(_SUrfStruct_));

    if (nt == NULL  ||  et == NULL  ||  tt == NULL  ||
        detach_upper_surf == NULL) {
        csw_Free (nt);
        csw_Free (et);
        csw_Free (tt);
        csw_Free (detach_upper_surf);
        return -1;
    }

    memcpy (nt, nodes, num_nodes * sizeof(NOdeStruct));
    memcpy (et, edges, num_edges * sizeof(EDgeStruct));
    memcpy (tt, tris, num_tris * sizeof(TRiangleStruct));

    detach_upper_surf->nodes = nt;
    detach_upper_surf->edges = et;
    detach_upper_surf->tris = tt;
    detach_upper_surf->num_nodes = num_nodes;
    detach_upper_surf->num_edges = num_edges;
    detach_upper_surf->num_tris = num_tris;

    return 1;

}

/*-------------------------------------------------------------------------*/

/*
 * If there are valid detach_lower_surf and detach_upper_surf structures,
 * calculate a third surface roughly in the middle between the lower and
 * upper constraints with the new surface being as smooth as possible
 * without crossing either of the constraint surfaces.
 */
int FaultConnect::calcDetachment (double lower_age, double upper_age, double age)
{
    CSW_F        *grid1, *grid2, *grid3;
    double       x1, y1, x2, y2;
    double       sum1, sum2, avspace;
    int          nc, nr, istat, i;
    NOdeStruct   *nodes, *nptr, *nodesout;
    EDgeStruct   *edges, *eptr, *edgesout;
    TRiangleStruct  *trisout;
    int          nn, ne, nt;
    NOdeStruct  *np1, *np2;
    double      dx, dy, dist, tiny;

    if (detach_lower_surf == NULL  ||
        detach_upper_surf == NULL) {
        return -1;
    }

/*
 * Find the average edge length of all constraint surface triangles and
 * also find the cumulative limits of all constraint surface nodes.
 */
    sum1 = 0.0;
    sum2 = 0.0;
    x1 = 1.e30;
    y1 = 1.e30;
    x2 = -1.e30;
    y2 = -1.e30;

    nodes = detach_lower_surf->nodes;
    nn = detach_lower_surf->num_nodes;
    edges = detach_lower_surf->edges;
    ne = detach_lower_surf->num_edges;

    for (i=0; i<ne; i++) {
        eptr = edges + i;
        if (eptr->deleted) continue;
        if (eptr->length > 0) {
            sum1 += eptr->length;
            sum2++;
        }
        else {
            np1 = nodes + eptr->node1;
            np2 = nodes + eptr->node2;
            dx = np2->x - np1->x;
            dy = np2->y - np1->y;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            eptr->length = dist;
            sum1 += dist;
            sum2++;
        }
    }

    for (i=0; i<nn; i++) {
        nptr = nodes + i;
        if (nptr->x < x1) x1 = nptr->x;
        if (nptr->y < y1) y1 = nptr->y;
        if (nptr->x > x2) x2 = nptr->x;
        if (nptr->y > y2) y2 = nptr->y;
    }

    nodes = detach_upper_surf->nodes;
    nn = detach_upper_surf->num_nodes;
    edges = detach_upper_surf->edges;
    ne = detach_upper_surf->num_edges;

    for (i=0; i<ne; i++) {
        eptr = edges + i;
        if (eptr->deleted) continue;
        if (eptr->length > 0) {
            sum1 += eptr->length;
            sum2++;
        }
        else {
            np1 = nodes + eptr->node1;
            np2 = nodes + eptr->node2;
            dx = np2->x - np1->x;
            dy = np2->y - np1->y;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            eptr->length = dist;
            sum1 += dist;
            sum2++;
        }
    }

    for (i=0; i<nn; i++) {
        nptr = nodes + i;
        if (nptr->x < x1) x1 = nptr->x;
        if (nptr->y < y1) y1 = nptr->y;
        if (nptr->x > x2) x2 = nptr->x;
        if (nptr->y > y2) y2 = nptr->y;
    }

    if (sum1 <= 0.0  ||  sum2 <= 0.0  ||
        x1 >= x2  ||  y1 >= y2) {
        return -1;
    }

    avspace = sum1 / sum2;

/*
 * The original triangles were right isoceles, which means about
 * 1/3 of the edges have diagonal lengths.  To correct for this
 * divide by 1.13 (2 * 1 + 1.4) / 3.0
 */
    avspace /= 1.13;

/*
 * Allocate space for 3 grids all at the same grid geometry.
 */
    nc = (int)((x2 - x1) / avspace + 1.5);
    nr = (int)((y2 - y1) / avspace + 1.5);
    if (nc < 2) nc = 2;
    if (nr < 2) nr = 2;

    grid1 = (CSW_F *)csw_Malloc (nc * nr * 3 * sizeof(CSW_F));
    if (grid1 == NULL) {
        return -1;
    }
    grid2 = grid1 + nc * nr;
    grid3 = grid2 + nc * nr;

/*
 * Calculate a grid for the lower constraint.  The grid will
 * have no NULL values.
 */
    istat =
      grd_triangle_ptr->grd_calc_grid_from_trimesh (
        detach_lower_surf->nodes,
        detach_lower_surf->num_nodes,
        detach_lower_surf->edges,
        detach_lower_surf->num_edges,
        detach_lower_surf->tris,
        detach_lower_surf->num_tris,
        1.e30,
        grid1, nc, nr,
        x1, y1, x2, y2);
    if (istat == -1) {
        csw_Free (grid1);
        return -1;
    }
    istat =
      grd_triangle_ptr->grd_FillNullValues (
        grid1, nc, nr,
        (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2,
        NULL, 0, 1.e30f,
        NULL, NULL);
    if (istat == -1) {
        csw_Free (grid1);
        return -1;
    }

/*
 * Calculate a grid for the upper constraint.  The grid will
 * have no NULL values.
 */
    istat =
      grd_triangle_ptr->grd_calc_grid_from_trimesh (
        detach_upper_surf->nodes,
        detach_upper_surf->num_nodes,
        detach_upper_surf->edges,
        detach_upper_surf->num_edges,
        detach_upper_surf->tris,
        detach_upper_surf->num_tris,
        1.e30,
        grid2, nc, nr,
        x1, y1, x2, y2);
    if (istat == -1) {
        csw_Free (grid1);
        return -1;
    }
    istat =
      grd_triangle_ptr->grd_FillNullValues (
        grid2, nc, nr,
        (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2,
        NULL, 0, 1.e30f,
        NULL, NULL);
    if (istat == -1) {
        csw_Free (grid1);
        return -1;
    }

/*
 * Initially place the detachment in the middle between
 * each upper and lower constraint node.
 */
    for (i=0; i<nc*nr; i++) {
        grid3[i] = (grid1[i] + grid2[i]) / 2.0f;
    }

/*
 * Smooth the detachment grid, and if any points in the
 * smoothed grid fall outside the constraints, move them
 * inside the constraints and attempt smoothing again.
 * In subsequent smoothing, a smaller smoothing factor is used.
 */
    int    ndo, noutside;
    ndo = 0;

    for (;;) {
        if (ndo > 8) {
            break;
        }
        istat =
          grd_calc_ptr->grd_smooth_grid (
            grid3, nc, nr, 9-ndo,
            NULL, 0,
            (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2,
            -1.e30f, 1.e30f, NULL);
        if (istat == -1) {
            csw_Free (grid1);
            return -1;
        }
        noutside = 0;
        for (i=0; i<nc*nr; i++) {
            tiny = (grid2[i] - grid1[i]) / 100.0;
            if (grid3[i] < grid1[i] + tiny) {
                grid3[i] = (CSW_F)(grid1[i] + tiny * 10.0);
                noutside++;
            }
            else if (grid3[i] > grid2[i] - tiny) {
                grid3[i] = (CSW_F)(grid2[i] - tiny * 10.0);
                noutside++;
            }
        }
        if (noutside == 0) {
            break;
        }

        ndo++;

    }

/*
 * If no smooth grid could fit between the constraints, return zero.
 */
    if (ndo > 8) {
        csw_Free (grid1);
        return 0;
    }

/*
 * Move the detachment grid vertically depending on the lower age, upper age and age.
 */
    double    fraction;

/*
 * Calculate the fractional age between the top and the detachment.
 */
    if (lower_age > upper_age) {
        fraction = (age - upper_age) / (lower_age - upper_age);
        if (fraction > 1.0) fraction = 1.0;
        if (fraction < 0.0) fraction = 0.0;
    }
    else {
        fraction = .5;
    }

    double gap1, dz1;

/*
 * If the age is closer to the top, move it up a percentage of the minimum
 * gap between the detachment and the top.  Note that if the fraction is
 * quite near .5. no vertical movement is done.
 */
    if (fraction < .49) {
        gap1 = 1.e30;
        for (i=0; i<nc*nr; i++) {
            dz1 = grid2[i] - grid3[i];
            if (dz1 < gap1) gap1 = dz1;
        }
        gap1 *= 1.8;
        gap1 *= (.5 - fraction);
        for (i = 0; i<nc*nr; i++) {
            grid3[i] += (CSW_F)gap1;
        }
    }

/*
 * If the age is closer to the bottom, move it down a percentage of the minimum
 * gap between the detachment and the bottom.
 */
    else if (fraction > .51) {
        gap1 = 1.e30;
        for (i=0; i<nc*nr; i++) {
            dz1 = grid3[i] - grid1[i];
            if (dz1 < gap1) gap1 = dz1;
        }
        gap1 *= 1.8;
        gap1 *= (fraction - .5);
        for (i = 0; i<nc*nr; i++) {
            grid3[i] -= (CSW_F)gap1;
        }
    }

/*
 * Calculate the detach_calc_surf trimesh from the smooth grid
 * fit between the constraints.
 */
    istat =
      grd_triangle_ptr->grd_calc_tri_mesh_from_grid (
        grid3, nc, nr,
        x1, y1, x2, y2,
        NULL, NULL, NULL,
        NULL, NULL, 0,
        GRD_EQUILATERAL,
        &nodesout, &edgesout, &trisout,
        &nn, &ne, &nt);
    csw_Free (grid1);
    grid1 = grid2 = grid3 = NULL;
    if (istat == -1) {
        return -1;
    }

    FreeSurf (detach_calc_surf, 1);
    detach_calc_surf = (_SUrfStruct_ *)csw_Calloc (sizeof(_SUrfStruct_));
    if (detach_calc_surf == NULL) {
        csw_Free (nodesout);
        csw_Free (edgesout);
        csw_Free (trisout);
        return -1;
    }

    detach_calc_surf->nodes = nodesout;
    detach_calc_surf->num_nodes = nn;
    detach_calc_surf->edges = edgesout;
    detach_calc_surf->num_edges = ne;
    detach_calc_surf->tris = trisout;
    detach_calc_surf->num_tris = nt;

    SurfaceGroupPlane  *sgp = NULL;
    try {
        sgp = new SurfaceGroupPlane ();
        sgp->addTriMeshForFit (nodesout, nn,
                               edgesout, ne,
                               trisout, nt);
        sgp->calcPlaneCoefs ();
        sgp->freeFitPoints ();
    }
    catch (...) {
        sgp = NULL;
    }
    detach_calc_surf->sgp = sgp;

    return 1;

}


/*-------------------------------------------------------------------------*/

void FaultConnect::clearCalculatedDetachmentData (void)
{
    FreeSurf (detach_upper_surf, 1);
    FreeSurf (detach_lower_surf, 1);
    FreeSurf (detach_calc_surf, 1);
    detach_upper_surf = NULL;
    detach_lower_surf = NULL;
    detach_calc_surf = NULL;
    return;
}

/*--------------------------------------------------------------------------*/

/*
 * Get the detachment surface after the fault connections have
 * been embedded into it.  The nodes, edges and triangles arrays
 * returned from this are copies that should be csw_Freed by the
 * calling function when needed.
 */
int FaultConnect::getCalculatedDetachment (
    NOdeStruct        **nodes,
    int               *num_nodes,
    EDgeStruct        **edges,
    int               *num_edges,
    TRiangleStruct    **tris,
    int               *num_tris,
    int               *sgpflag,
    double            *sgpdata)
{
    NOdeStruct        *np;
    EDgeStruct        *ep;
    TRiangleStruct    *tp;

/*
 * Initialize output in case of error.
 */
    *nodes = NULL;
    *edges = NULL;
    *tris = NULL;
    *num_nodes = 0;
    *num_edges = 0;
    *num_tris = 0;

    if (detach_calc_surf == NULL) {
        return 0;
    }

/*
 * Allocate space for the trimesh copy.
 */
    np = (NOdeStruct *)csw_Malloc (detach_calc_surf->num_nodes * sizeof(NOdeStruct));
    ep = (EDgeStruct *)csw_Malloc (detach_calc_surf->num_edges * sizeof(EDgeStruct));
    tp = (TRiangleStruct *)csw_Malloc (detach_calc_surf->num_tris * sizeof(TRiangleStruct));

    if (np == NULL  ||  ep == NULL  ||  tp == NULL) {
        csw_Free (np);
        csw_Free (ep);
        csw_Free (tp);
        return -1;
    }

/*
 * Make the copy and return.
 */
    memcpy (np, detach_calc_surf->nodes,
            detach_calc_surf->num_nodes * sizeof(NOdeStruct));
    memcpy (ep, detach_calc_surf->edges,
            detach_calc_surf->num_edges * sizeof(EDgeStruct));
    memcpy (tp, detach_calc_surf->tris,
            detach_calc_surf->num_tris * sizeof(TRiangleStruct));

    *nodes = np;
    *edges = ep;
    *tris = tp;
    *num_nodes = detach_calc_surf->num_nodes;
    *num_edges = detach_calc_surf->num_edges;
    *num_tris = detach_calc_surf->num_tris;

    *sgpflag = 0;
    memset (sgpdata, 0, 6 * sizeof(double));
    if (detach_calc_surf->sgp) {
        *sgpflag = 1;
        detach_calc_surf->sgp->getCoefsAndOrigin (
            sgpdata,
            sgpdata + 1,
            sgpdata + 2,
            sgpdata + 3,
            sgpdata + 4,
            sgpdata + 5);
    }

    return 1;

}
