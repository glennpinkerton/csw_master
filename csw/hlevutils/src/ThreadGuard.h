/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

// Add nothing above this ifndef

#ifndef THREAD_GUARD_H
#define THREAD_GUARD_H

#include <stdio.h>
#include <map>

/*
 *  The comments here are intended for me (Glenn) to remind me of what
 *  is being attempted here.  These comments may be less sensible to
 *  any one else, but I'm not going to change them for a while.
 *
 *  The ThreadGuard class is meant for keeping native language objects
 *  from interfering with each other when the java part of the code is
 *  running in a multi threaded manner.
 *
 *  No attempt is made to make this work with any c++ threading.  The
 *  member functions which can edit the guard_map collection must be
 *  protected using the EnterMonitor and ExitMonitor functions.
 *
 *  The objects that may interfere are those where the state must survive
 *  multiple jni calls.  Pointers to these objects are put into the
 *  various stl collections managedd here.
 *
 *  Multi threading, especially with java and jni, can be tricky stuff.
 *  Think long and hard about implementing thread safety and subsequent
 *  code changes to this thread guard stuff.
 */

#include <csw/jsurfaceworks/private_include/PatchSplit.h>
#include "csw/jsurfaceworks/private_include/SWCalc.h"
#include "csw/surfaceworks/private_include/Vert.h"
#include "csw/surfaceworks/include/grid_api.h"

#include "csw/hlevutils/src/GraphicsCanvasManager.h"

namespace ThreadGuard
{

  PATCHSplit *GetPatchSplit (long jside_id);
  GRDVert *GetGrdVert (long jside_id);
  long    CreateGrdVert (long jside_id);
  CSWGrdAPI *GetGrdAPI (long jside_id);
  long    CreateGrdAPI (long jside_id);
  SWCalc *GetSWCalc (long jside_id);

  CanvasManager  *GetCanvasManager ();

  FILE *GetSWLogFile (long jside_id);
  FILE *GetSWSaveLogFile (long jside_id);

  void SetSWLogFiles (long jside_id, FILE *log, FILE *save);

  FILE *GetEZLogFile (long jside_id);
  void SetEZLogFile (long jside_id, FILE *log);

  void RemovePatchSplit (long jside_id);
  void RemoveGRDVert (long jside_id);
  void RemoveGrdAPI (long jside_id);
  void RemoveSWCalc (long jside_id);
  void RemoveSWLogFiles (long jside_id);
  void RemoveEZLogFile (long jside_id);

  void RemoveThreadData (long jsiae_id);
  void RemoveAllThreadData (void);

}; // end of namespace


// Add nothing below this endif

#endif
