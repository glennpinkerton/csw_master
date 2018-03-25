
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
#include <csw/jeasyx/private_include/DisplayList.h>



/*
 * Destructor clears all the CanvasList stuff
 */
CanvasManager::~CanvasManager ()
{

    RemoveAllFromManager ();

}



/*
 *  Create a new Graphics Canvas Struct and it's contents.
 *  retgurn the index into the canvas manager vector for the
 *  structure created.
 */
int CanvasManager::CreateGraphicsCanvas ()
{
    GRaphicsCanvasStruct    *cptr = NULL;
    GRaphicsCanvasStruct    *cl_data = NULL;
    int                     i, cl_size = 0;

    cl_data = CanvasList.data ();
    cl_size = CanvasList.size ();

/*
 * If a slot is not being used, use it.
 */
    for (i=0; i<cl_size; i++) {
        cptr = cl_data + i;
        if (cptr->dlist == NULL) {
            try {
                cptr->dlist = new CDisplayList (i, ifile);
                ifile++;
            }
            catch (...) {
                printf ("\n*****  caught new exception creating display list  *****\n\n");
                fflush (stdout);
                cptr->dlist = NULL;
                break;
            }
            cptr->java_num = i;
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
        cptr->dlist = new CDisplayList (cl_size - 1, ifile);
        ifile++;
    }
    catch (...) {
        printf ("\n*****  caught new exception on display list  *****\n\n");
        fflush (stdout);
        cptr->dlist = NULL;
    }
    cptr->java_num = cl_size - 1;

    return (cl_size - 1);

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


int CanvasManager::RemoveAllFromManager ()
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

    return 1;

}

