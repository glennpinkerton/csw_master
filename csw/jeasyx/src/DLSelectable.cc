
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 *  DLSelectable.cc
 *
 *  This is the implementation of the DLSelectable class.
 *  It is used to list pimitive numbers that belong to 
 *  a selectable object.
 */

#include <assert.h>
#include <stdlib.h>

#include "csw/jeasyx/private_include/DLSelectable.h"

#include "csw/utils/include/csw_.h"


/*  constants for the file  */


/*------------------------------------------------------------------*/

/*
 * Empty constructor
 */

DLSelectable::DLSelectable ()
{

    index_num = -1;
    deleted_flag = 0;

    lines = NULL;
    nlines = 0;
    maxlines = 0;

    fills = NULL;
    nfills = 0;
    maxfills = 0;

    texts = NULL;
    ntexts = 0;
    maxtexts = 0;

    symbs = NULL;
    nsymbs = 0;
    maxsymbs = 0;

    shapes = NULL;
    nshapes = 0;
    maxshapes = 0;

    contours = NULL;
    ncontours = 0;
    maxcontours = 0;

    axes = NULL;
    naxes = 0;
    maxaxes = 0;

    select_state = 0;

}

DLSelectable::~DLSelectable ()
{
    csw_Free (lines);
    csw_Free (fills);
    csw_Free (texts);
    csw_Free (symbs);
    csw_Free (shapes);
    csw_Free (contours);
    csw_Free (axes);
}


void DLSelectable::ToggleSelectState (void)
{
    if (select_state == 0) {
        select_state = 1;
    }
    else {
        select_state = 0;
    }

    return;
}


void DLSelectable::SetSelectState (int ival)
{
    if (ival == 0) {
        select_state = 0;
    }
    else {
        select_state = 1;
    }
}


int DLSelectable::GetSelectState (void)
{
    return select_state;
}



/*--------------------------------------------------------------------------*/

int DLSelectable::GetNumLines (void)
{
    return nlines;
}

int DLSelectable::GetNumFills (void)
{
    return nfills;
}

int DLSelectable::GetNumTexts (void)
{
    return ntexts;
}

int DLSelectable::GetNumSymbs (void)
{
    return nsymbs;
}

int DLSelectable::GetNumShapes (void)
{
    return nshapes;
}

int DLSelectable::GetNumContours (void)
{
    return ncontours;
}

int DLSelectable::GetNumAxes (void)
{
    return naxes;
}

/*---------------------------------------------------------------------------*/

int *DLSelectable::GetLines (void)
{
    return lines;
}

int *DLSelectable::GetFills (void)
{
    return fills;
}

int *DLSelectable::GetTexts (void)
{
    return texts;
}

int *DLSelectable::GetSymbs (void)
{
    return symbs;
}

int *DLSelectable::GetShapes (void)
{
    return shapes;
}

int *DLSelectable::GetContours (void)
{
    return contours;
}

int *DLSelectable::GetAxes (void)
{
    return axes;
}

/*-----------------------------------------------------------------------------*/

int DLSelectable::AddLine (int ival)
{
    int                jlast, j;

    if (nlines >= maxlines) {
        jlast = maxlines;
        maxlines += 100;
        lines = (int *)csw_Realloc (lines, maxlines * sizeof(int));
        if (lines != NULL) {
            for (j=jlast; j<maxlines; j++) {
                lines[j] = -1;
            }
        }
    }

    if (lines == NULL) {
        return -1;
    }
        
    lines[nlines] = ival;
    nlines++;

    return 1;
}

/*-----------------------------------------------------------------------------*/

int DLSelectable::AddFill (int ival)
{
    int                jlast, j;

    if (nfills >= maxfills) {
        jlast = maxfills;
        maxfills += 100;
        fills = (int *)csw_Realloc (fills, maxfills * sizeof(int));
        if (fills != NULL) {
            for (j=jlast; j<maxfills; j++) {
                fills[j] = -1;
            }
        }
    }

    if (fills == NULL) {
        return -1;
    }
        
    fills[nfills] = ival;
    nfills++;

    return 1;
}

/*-----------------------------------------------------------------------------*/

int DLSelectable::AddText (int ival)
{
    int                jlast, j;

    if (ntexts >= maxtexts) {
        jlast = maxtexts;
        maxtexts += 100;
        texts = (int *)csw_Realloc (texts, maxtexts * sizeof(int));
        if (texts != NULL) {
            for (j=jlast; j<maxtexts; j++) {
                texts[j] = -1;
            }
        }
    }

    if (texts == NULL) {
        return -1;
    }
        
    texts[ntexts] = ival;
    ntexts++;

    return 1;
}

/*-----------------------------------------------------------------------------*/

int DLSelectable::AddSymb (int ival)
{
    int                jlast, j;

    if (nsymbs >= maxsymbs) {
        jlast = maxsymbs;
        maxsymbs += 100;
        symbs = (int *)csw_Realloc (symbs, maxsymbs * sizeof(int));
        if (symbs != NULL) {
            for (j=jlast; j<maxsymbs; j++) {
                symbs[j] = -1;
            }
        }
    }

    if (symbs == NULL) {
        return -1;
    }
        
    symbs[nsymbs] = ival;
    nsymbs++;

    return 1;
}

/*-----------------------------------------------------------------------------*/

int DLSelectable::AddShape (int ival)
{
    int                jlast, j;

    if (nshapes >= maxshapes) {
        jlast = maxshapes;
        maxshapes += 100;
        shapes = (int *)csw_Realloc (shapes, maxshapes * sizeof(int));
        if (shapes != NULL) {
            for (j=jlast; j<maxshapes; j++) {
                shapes[j] = -1;
            }
        }
    }

    if (shapes == NULL) {
        return -1;
    }
        
    shapes[nshapes] = ival;
    nshapes++;

    return 1;
}

/*-----------------------------------------------------------------------------*/

int DLSelectable::AddContour (int ival)
{
    int                jlast, j;

    if (ncontours >= maxcontours) {
        jlast = maxcontours;
        maxcontours += 100;
        contours = (int *)csw_Realloc (contours, maxcontours * sizeof(int));
        if (contours != NULL) {
            for (j=jlast; j<maxcontours; j++) {
                contours[j] = -1;
            }
        }
    }

    if (contours == NULL) {
        return -1;
    }
        
    contours[ncontours] = ival;
    ncontours++;

    return 1;
}

/*-----------------------------------------------------------------------------*/

int DLSelectable::AddAxis (int ival)
{
    int                jlast, j;

    if (naxes >= maxaxes) {
        jlast = maxaxes;
        maxaxes += 10;
        axes = (int *)csw_Realloc (axes, maxaxes * sizeof(int));
        if (axes != NULL) {
            for (j=jlast; j<maxaxes; j++) {
                axes[j] = -1;
            }
        }
    }

    if (axes == NULL) {
        return -1;
    }
        
    axes[naxes] = ival;
    naxes++;

    return 1;
}
