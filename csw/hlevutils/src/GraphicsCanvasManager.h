
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#include <vector>

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

    std::vector<GRaphicsCanvasStruct>    CanvasList;


  public:

    void CleanAll () {
        CanvasList.clear();
    };

    int AddGraphicsCanvasToManager (char *name, long java_num);
    int RemoveGraphicsCanvasFromManager (char *name);
    int RemoveGraphicsCanvasFromManager (long index);
    int CreateGraphicsCanvas (char *name, long index);
    CDisplayList *GetDisplayList (int index);

}; //end of main class

#endif
