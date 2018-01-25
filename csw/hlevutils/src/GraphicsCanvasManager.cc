
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/


/*
 ***************************************************************************

  This file has a collection of stand alone C++ functions to manage the
  list of canvases associated with display list objects.  The java side
  will send messages through the JNI functions to create or set a 
  particular "window" via this file.  The display list for the active
  window can be retrieved in the JNI functions, and its members can be
  called to do graphic drawing.

 ***************************************************************************
*/

#include <exception>

#include <csw/utils/include/csw_.h>
#include <csw/hlevutils/src/simulate_new.h>

#include <csw/hlevutils/src/GraphicsCanvasManager.h>
#include <csw/jeasyx/private_include/gtx_drawprim.h>



/*
 * Destructor clears all the CanvasList stuff
 */
CanvasManager::~CanvasManager ()
{

    if (CanvasList != NULL  &&  NumCanvasList > 0) {
        int   i;
        for (i=0; i<NumCanvasList; i++) {
            CDisplayList  *dl = CanvasList[i].dlist;
            if (dl) {
                delete dl;
            }
            CanvasList[i].dlist = NULL;
        }
        csw_Free (CanvasList);
    }

}




int CanvasManager::ezx_AddGraphicsCanvasToManager (char *name,
                                    long java_num)
{
    GRaphicsCanvasStruct    *cptr = NULL;
    int                     i, cval;
    char                    lname1[200], lname2[200];

    if (name == NULL) {
        return -1;
    }

/*
 * If there is already a display list of this name and num, 
 * use it.  Do not create a new dlist.
 */
    csw_StrClean2 (lname1, name, 200);
    for (i=0; i<NumCanvasList; i++) {
        cptr = CanvasList + i;
        if (cptr->dlist == NULL) {
            continue;
        }
        csw_StrClean2 (lname2, cptr->name, 200);
        cval = strcmp (lname1, lname2);
        if (cval == 0  &&  java_num == cptr->java_num) {
            ActiveCanvas = cptr;
            ActiveIndex = i;
            return i;
        }
    }

/*
 * If a slot is not being used, use it.
 */
    for (i=0; i<NumCanvasList; i++) {
        cptr = CanvasList + i;
        if (cptr->dlist == NULL) {
            try {
                SNF
                cptr->dlist = new CDisplayList;
            }
            catch (...) {
                printf ("\n*****  caught new exception on display list  *****\n\n");
                fflush (stdout);
                cptr->dlist = NULL;
                break;
            }
            cptr->java_num = java_num;
            strncpy (cptr->name, name, 99);
            cptr->name[99] = '\0';
            ActiveCanvas = cptr;
            ActiveIndex = i;
            return i;
        }
    }

/*
 * Grow the canvas list if needed.
 */
    const int gcsize = sizeof(GRaphicsCanvasStruct);
    const int gchunk = 10;

    if (NumCanvasList >= MaxCanvasList) {
        if (CanvasList == NULL) {
            MaxCanvasList = gchunk;
            CanvasList = static_cast <GRaphicsCanvasStruct *>
                          (csw_Calloc (MaxCanvasList * gcsize));
        }
        else {
            int oldmax = MaxCanvasList;
            MaxCanvasList += gchunk;
            CanvasList = static_cast <GRaphicsCanvasStruct *>
                          (csw_Realloc(CanvasList, MaxCanvasList * gcsize));
            if (CanvasList != NULL) {
                memset ((void *)(CanvasList + oldmax), 0, gchunk * gcsize);
            }
        }  
    }

    if (CanvasList == NULL) {
        return -1;
    }

/*
 * Append to the end of the list.
 */
    cptr = CanvasList + NumCanvasList;

    try {
        SNF
        cptr->dlist = new CDisplayList;
    }
    catch (...) {
        printf ("\n*****  caught new exception on display list  *****\n\n");
        fflush (stdout);
        cptr->dlist = NULL;
    }
    cptr->java_num = java_num;
    strncpy (cptr->name, name, 99);
    cptr->name[99] = '\0';

    ActiveCanvas = cptr;
    ActiveIndex = NumCanvasList;

    NumCanvasList++;

    return (NumCanvasList - 1);

}



int CanvasManager::ezx_RemoveGraphicsCanvasFromManager (char *name)
{
    GRaphicsCanvasStruct    *cptr;
    int                     i, cval, istat;
    char                    local1[100], local2[100];
    
    if (name == NULL) {
        return -1;
    }

    csw_StrClean2 (local1, name, 100);
    for (i=0; i<NumCanvasList; i++) {
        cptr = CanvasList + i;
        csw_StrClean2 (local2, cptr->name, 100);
        cval = strcmp (local1, local2);
        if (cval == 0) {
            istat = cptr->java_num;
            if (cptr->dlist != NULL) {
                delete (cptr->dlist);
            }
            cptr->dlist = NULL;
            cptr->java_num = -1;
            cptr->name[0] = '\0';
            return istat;
        }
    }

    return -1;

}


int CanvasManager::ezx_RemoveGraphicsCanvasFromManager (int index)
{
    GRaphicsCanvasStruct    *cptr;
    
    if (index < 0  ||  index >= NumCanvasList) {
        return -1;
    }

    cptr = CanvasList + index;

    if (cptr->dlist != NULL) {
        delete (cptr->dlist);
    }
    cptr->dlist = NULL;
    cptr->java_num = -1;
    cptr->name[0] = '\0';

    return 1;

}


int CanvasManager::ezx_SetActiveGraphicsCanvas (char *name)
{
    int                     i, cval;
    char                    local1[100], local2[100];
    GRaphicsCanvasStruct    *cptr;

    ActiveCanvas = NULL;
    ActiveIndex = -1;
    if (name == NULL) {
        return -1;
    }

    csw_StrClean2 (local1, name, 100);
    for (i=0; i<NumCanvasList; i++) {
        cptr = CanvasList + i;
        csw_StrClean2 (local2, cptr->name, 100);
        cval = strcmp (local1, local2);
        if (cval == 0) {
            ActiveCanvas = cptr;
            ActiveIndex = -1;
            return cptr->java_num;
        }
    }

    return -1;

}


int CanvasManager::ezx_SetActiveGraphicsCanvas (int index)
{
    GRaphicsCanvasStruct    *cptr;

    ActiveCanvas = NULL;
    ActiveIndex = -1;
    if (index < 0  ||  index >= NumCanvasList) {
        return -1;
    }

    cptr = CanvasList + index;
    ActiveCanvas = cptr;
    ActiveIndex = index;

    return 1;

}


CDisplayList *CanvasManager::ezx_GetActiveDisplayList (void)
{
    if (ActiveCanvas == NULL) {
        return NULL;
    }

    return ActiveCanvas->dlist;

}


int CanvasManager::ezx_GetActiveIndex (void)
{
    return ActiveIndex;
}
