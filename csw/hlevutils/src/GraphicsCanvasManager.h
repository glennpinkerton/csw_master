
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#include <csw/jeasyx/private_include/DisplayList.h>

#ifndef _GRAPHICS_CANVAS_MANAGER_H_
#define _GRAPHICS_CANVAS_MANAGER_H_



#include "csw/jeasyx/private_include/DisplayList.h"


typedef struct {
    CDisplayList      *dlist = NULL;
    long              java_num = 0;
    char              name[100] = {'\0'};
} GRaphicsCanvasStruct;


class CanvasManager
{

  public:

    CanvasManager () {};
    ~CanvasManager ();


  private:

    GRaphicsCanvasStruct    *CanvasList = NULL;
    int                     NumCanvasList = 0;
    int                     MaxCanvasList = 0;
    int                     First = 1;

    GRaphicsCanvasStruct    *ActiveCanvas = NULL;
    int                     ActiveIndex = -1;


  public:

    int ezx_AddGraphicsCanvasToManager (char *name, long java_num);
    int ezx_RemoveGraphicsCanvasFromManager (char *name);
    int ezx_RemoveGraphicsCanvasFromManager (int index);
    int ezx_SetActiveGraphicsCanvas (char *name);
    int ezx_SetActiveGraphicsCanvas (int index);
    CDisplayList *ezx_GetActiveDisplayList (void);
    int ezx_GetActiveIndex (void);

}; //end of main class

#endif
