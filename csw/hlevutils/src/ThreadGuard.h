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
 *  running in a multi threaded manner.  The basic idea is that a 
 *  collection of threadid, ThreadGuardData pairs is kept in static 
 *  variables.  Objects that may collide are put into the ThreadGuardData
 *  object and they are retrieved via the threadid.  The threadid is
 *  passed by the csw java code with every jni function call.
 *
 *  No attempt is made to make this work with any c++ threading.  The
 *  member functions which can edit the guard_map collection must be
 *  protected using the MonitorEnter and MonitorExit functions.
 *
 *  The objects that may interfere are those where the state must survive
 *  multiple jni calls.  Pointers to these objects are put into the
 *  ThreadGuardData object.
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

  class ThreadGuardData
  {

   public:

    PATCHSplit  *psplit = NULL;
    GRDVert     *gvert = NULL;
    CSWGrdAPI   *grdapi = NULL;
    SWCalc      *swcalc = NULL;
    FILE        *swlogfile = NULL;
    FILE        *saveswlogfile = NULL;
    FILE        *ezlogfile = NULL;

    CanvasManager  *canvas_manager = NULL;

  /*
   *  I have not been able to compile any file with the
   *  java 8 jni headers using the fedora 26 g++ compiler.
   *  Since this ThreadGuard definition needs to be
   *  included in c++ files, I cannot use structures 
   *  from the jni headers.  The use of void pointers
   *  here is a work around for the header file issue.
   */
    void        *v_jenv = NULL;
    void        *v_jobj = NULL;

    ThreadGuardData () {};
    ~ThreadGuardData ()
    {  
        if (swlogfile) fclose (swlogfile);
        swlogfile = NULL;
        if (saveswlogfile) fclose (saveswlogfile);
        saveswlogfile = NULL;
        if (ezlogfile) fclose (ezlogfile);
        ezlogfile = NULL;

        delete (psplit);
        psplit = NULL;
        delete (gvert);
        gvert = NULL;
        delete (grdapi);
        grdapi = NULL;
        delete (swcalc);
        swcalc = NULL;
        delete canvas_manager;
        canvas_manager = NULL;
    };
    
  };  // end of ThreadGuardData class

  PATCHSplit *GetPatchSplit (int threadid);
  GRDVert *GetGRDVert (int threadid);
  CSWGrdAPI *GetGrdAPI (int threadid);
  SWCalc *GetSWCalc (int threadid);

  CanvasManager  *GetCanvasManager (int threadid);

  FILE *GetSWLogFile (int threadid);
  FILE *GetSWSaveLogFile (int threadid);

  void SetSWLogFiles (int threadid, FILE *log, FILE *save);

  FILE *GetEZLogFile (int threadid);
  void SetEZLogFile (int threadid, FILE *log);

  void RemoveThreadData (int threadid);
  void RemoveAllThreadData (void);

  void *GetVoidJenv (int threadid, void *v_jenv);
  void *GetVoidJobj (int threadid, void *v_jobj);

}; // end of namespace


// Add nothing below this endif

#endif
