
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
    long              java_num = -1;
} GRaphicsCanvasStruct;


class CanvasManager
{

    int      ifile = 0;

  public:

    CanvasManager () {};
    ~CanvasManager ();


  private:

    std::vector<GRaphicsCanvasStruct>    CanvasList;

// Do not allow copy or move with this class
// Compile error should be seen if copy or move is attempted.

    CanvasManager (const CanvasManager &other) {};
    const CanvasManager &operator= (const CanvasManager &other);
    CanvasManager (const CanvasManager &&other) {};
    const CanvasManager &operator= (const CanvasManager &&other);


  public:

    void CleanAll () {
        CanvasList.clear();
    };

    int RemoveAllFromManager ();
    int RemoveGraphicsCanvasFromManager (long index);
    int CreateGraphicsCanvas ();
    CDisplayList *GetDisplayList (int index);

}; //end of main class

#endif
