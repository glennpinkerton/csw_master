
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

    GRaphicsCanvasStruct    *cl_data = NULL;
    int                     cl_size = 0;

    cl_data = CanvasList.data();
    cl_size = CanvasList.size();
    
    if (cl_data != NULL  &&  cl_size > 0) {
        for (int i=0; i<cl_size; i++) {
            CDisplayList  *dl = CanvasList[i].dlist;
            if (dl != NULL) {
                delete dl;
            }
            CanvasList[i].dlist = NULL;
        }
    }

}



/*
 * Check for a display list for the name and java_num.
 * If none is found, create a new display list and add
 * it, it's name and it's index to the CanvasList. If
 * a valid display list if found or created, the index
 * of the CanvasList element is returned.  On error,
 * -1 is returned.
 */
int CanvasManager::CreateGraphicsCanvas (char *name,
                                         long java_num)
{
    GRaphicsCanvasStruct    *cptr = NULL;
    GRaphicsCanvasStruct    *cl_data = NULL;
    int                     i, cval, cl_size = 0;
    char                    lname1[200], lname2[200];

    if (name == NULL) {
        return -1;
    }

    cl_data = CanvasList.data ();
    cl_size = CanvasList.size();

/*
 * If there is already a display list of this name and num, 
 * use it.  Do not create a new dlist.
 */
    csw_StrClean2 (lname1, name, 200);
    for (i=0; i<cl_size; i++) {
        cptr = cl_data + i;
        if (cptr->dlist == NULL) {
            continue;
        }
        csw_StrClean2 (lname2, cptr->name, 200);
        cval = strcmp (lname1, lname2);
        if (cval == 0  &&  java_num == cptr->java_num) {
            return i;
        }
    }

/*
 * If a slot is not being used, use it.
 */
    for (i=0; i<cl_size; i++) {
        cptr = cl_data + i;
        if (cptr->dlist == NULL) {
            try {
                SNF
                cptr->dlist = new CDisplayList ();
            }
            catch (...) {
                printf ("\n*****  caught new exception creating display list  *****\n\n");
                fflush (stdout);
                cptr->dlist = NULL;
                break;
            }
            cptr->java_num = java_num;
            strncpy (cptr->name, name, 99);
            cptr->name[99] = '\0';
            return i;
        }
    }

/*
 * Grow the canvas list if needed.
 */
    
    try {
        GRaphicsCanvasStruct   gcs;
        CanvasList.push_back (gcs);
    }
    catch (...) {
        printf ("\n***** exception adding GraphicsCanvasStruct *****\n\n");
        fflush (stdout);
        return -1;
    }

    cl_data = CanvasList.data();
    cl_size = CanvasList.size();

/*
 * Associate the new dlist with the canvas struct just pushed back.
 */
    cptr = cl_data + cl_size - 1;

    try {
        SNF
        cptr->dlist = new CDisplayList ();
    }
    catch (...) {
        printf ("\n*****  caught new exception on display list  *****\n\n");
        fflush (stdout);
        cptr->dlist = NULL;
    }
    cptr->java_num = cl_size - 1;
    strncpy (cptr->name, name, 99);
    cptr->name[99] = '\0';

    return (cl_size - 1);

}



int CanvasManager::RemoveGraphicsCanvasFromManager (char *name)
{
    GRaphicsCanvasStruct    *cptr = NULL, *cl_data = NULL;
    int                     i, cval, istat, cl_size = 0;
    char                    local1[100], local2[100];
    
    if (name == NULL) {
        return -1;
    }

    cl_data = CanvasList.data ();
    cl_size = CanvasList.size ();

    if (cl_data == NULL  ||  cl_size < 1) {
        return -1;
    }

    csw_StrClean2 (local1, name, 100);
    for (i=0; i<cl_size; i++) {
        cptr = cl_data + i;
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


int CanvasManager::RemoveGraphicsCanvasFromManager (long index)
{
    GRaphicsCanvasStruct    *cptr = NULL, *cl_data = NULL;
    int                     cl_size = 0;

    cl_data = CanvasList.data();
    if (cl_data == NULL) {
        return -1;
    }
    cl_size = CanvasList.size();
    
    if (index < 0  ||  index >= cl_size) {
        return -1;
    }

    cptr = cl_data + index;

    if (cptr->dlist != NULL) {
        delete (cptr->dlist);
    }
    cptr->dlist = NULL;
    cptr->java_num = -1;
    cptr->name[0] = '\0';

    return 1;

}



CDisplayList *CanvasManager::GetDisplayList (int index)
{
    int cl_size = CanvasList.size();
    GRaphicsCanvasStruct *cl_data = CanvasList.data();

    if (cl_data == NULL  ||  cl_size < 1) {
        return NULL;
    }

    if (index < 0  ||  index >= cl_size) {
        return NULL;
    }

    GRaphicsCanvasStruct *cs = cl_data + index;

    return cs->dlist;

}

