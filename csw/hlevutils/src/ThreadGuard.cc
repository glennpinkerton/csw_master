/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#include <stdio.h>

#include  "csw/hlevutils/src/ThreadGuard.h"

#include  "csw/utils/private_include/csw_scope.h"


namespace ThreadGuard
{

// implement the thread data collection via an stl map object

    std::map <int, ThreadGuardData> guard_map;

}  // end of ThreadGuard namespace



PATCHSplit *ThreadGuard::GetPatchSplit (int threadid)
{

    ThreadGuardData  tgd;

// Ownership of the psplit pointer is passed off to the guard_map.
// Set psplit to NULL in tgd to prevent it from being deleted when
// tgd goes out of scope.
    auto fscope = [&]()
    {
        tgd.psplit = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);

    std::map<int, ThreadGuardData>::iterator it;
    PATCHSplit  *ps = NULL;

    it = guard_map.find(threadid);

    if (it != guard_map.end()) {
        ps = it->second.psplit;
        if (ps == NULL) {
            try {
                ps = new PATCHSplit ();
            }
            catch (...) {
                return NULL;
            }
            it->second.psplit = ps;
        }
        return ps;
    }

    try {
        tgd.psplit = new PATCHSplit ();
    }
    catch (...) {
        return NULL;
    }

    try {
        guard_map[threadid] = tgd;
        ps = tgd.psplit;
    }
    catch (...) {
        delete (tgd.psplit);
        ps = NULL;
    }

    return ps;
}



void ThreadGuard::RemoveAllThreadData (void)
{
    guard_map.clear();
}




void ThreadGuard::RemoveThreadData (int threadid)
{

    std::map<int, ThreadGuardData>::iterator it;

    it = guard_map.find(threadid);

    if (it != guard_map.end()) {
        guard_map.erase (threadid);
    }

}




GRDVert *ThreadGuard::GetGRDVert (int threadid)
{

    ThreadGuardData  tgd;

// Ownership of the gvert pointer is passed off to the guard_map.
// Set gvert to NULL in tgd to prevent it from being deleted when
// tgd goes out of scope.

    auto fscope = [&]()
    {
        tgd.gvert = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);

    std::map<int, ThreadGuardData>::iterator it;
    GRDVert     *gv = NULL;

    it = guard_map.find(threadid);

    if (it != guard_map.end()) {
        gv = it->second.gvert;
        if (gv == NULL) {
            try {
                gv = new GRDVert ();
            }
            catch (...) {
                return NULL;
            }
            it->second.gvert = gv;
        }
        return gv;
    }

    try {
        tgd.gvert = new GRDVert ();
    }
    catch (...) {
        return NULL;
    }

    try {
        guard_map[threadid] = tgd;
        gv = tgd.gvert;
    }
    catch (...) {
        delete (tgd.gvert);
        gv = NULL;
    }

    return gv;
}





CSWGrdAPI *ThreadGuard::GetGrdAPI (int threadid)
{

    ThreadGuardData  tgd;

// Ownership of the grdapi pointer is passed off to the guard_map.
// Set grdapi to NULL in tgd to prevent it from being deleted when
// tgd goes out of scope.

    auto fscope = [&]()
    {
        tgd.grdapi = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);

    std::map<int, ThreadGuardData>::iterator it;
    CSWGrdAPI   *gapi = NULL;

    it = guard_map.find(threadid);

    if (it != guard_map.end()) {
        gapi = it->second.grdapi;
        if (gapi == NULL) {
            try {
                gapi = new CSWGrdAPI ();
                it->second.grdapi = gapi;
            }
            catch (...) {
                return gapi;
            }
        }
        return gapi;
    }

    try {
        tgd.grdapi = new CSWGrdAPI ();
    }
    catch (...) {
        return gapi;
    }

    try {
        guard_map[threadid] = tgd;
        gapi = tgd.grdapi;
    }
    catch (...) {
        delete (tgd.grdapi);
        gapi = NULL;
    }

    return gapi;
}






SWCalc  *ThreadGuard::GetSWCalc (int threadid)
{

    ThreadGuardData  tgd;

// Ownership of the swcalc pointer is passed off to the guard_map.
// Set swcalc to NULL in tgd to prevent it from being deleted when
// tgd goes out of scope.

    auto fscope = [&]()
    {
        tgd.swcalc = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);

    std::map<int, ThreadGuardData>::iterator it;
    SWCalc      *swc = NULL;

    it = guard_map.find(threadid);

    if (it != guard_map.end()) {
        swc = it->second.swcalc;
        if (swc == NULL) {
            try {
                swc = new SWCalc ();
                it->second.swcalc = swc;
            }
            catch (...) {
                return NULL;
            }
        }
        return swc;
    }

    try {
        tgd.swcalc = new SWCalc ();
    }
    catch (...) {
        return NULL;
    }

    try {
        guard_map[threadid] = tgd;
        swc = tgd.swcalc;
    }
    catch (...) {
        delete (tgd.swcalc);
        swc = NULL;
    }

    return swc;
}




FILE  *ThreadGuard::GetSWLogFile (int threadid)
{
    std::map<int, ThreadGuardData>::iterator it;
    FILE  *swf = NULL;

    it = guard_map.find(threadid);

    if (it != guard_map.end()) {
        swf = it->second.swlogfile;
        return swf;
    }

    return swf;
}




FILE  *ThreadGuard::GetSWSaveLogFile (int threadid)
{
    std::map<int, ThreadGuardData>::iterator it;
    FILE  *swf = NULL;

    it = guard_map.find(threadid);

    if (it != guard_map.end()) {
        swf = it->second.saveswlogfile;
        return swf;
    }

    return swf;
}



void ThreadGuard::SetSWLogFiles (int threadid, FILE *swf, FILE *swsf)
{

    ThreadGuardData  tgd;

// Ownership of the logfile pointers is passed off to the guard_map.
// Set the logfile pointers to NULL in tgd to prevent close when
// tgd goes out of scope.

    auto fscope = [&]()
    {
        tgd.swlogfile = NULL;
        tgd.saveswlogfile = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);

    std::map<int, ThreadGuardData>::iterator it;

    it = guard_map.find(threadid);

    if (it != guard_map.end()) {
        it->second.swlogfile = swf;
        it->second.saveswlogfile = swsf;
        return;
    }

    tgd.swlogfile = swf;
    tgd.saveswlogfile = swsf;

    try {
        guard_map[threadid] = tgd;
    }
    catch (...) {
        printf ("\n**** Error adding sw log files to Thread Guard ****\n\n");
    }

    return;
}




FILE  *ThreadGuard::GetEZLogFile (int threadid)
{
    std::map<int, ThreadGuardData>::iterator it;
    FILE  *ezf = NULL;

    it = guard_map.find(threadid);

    if (it != guard_map.end()) {
        ezf = it->second.ezlogfile;
        return ezf;
    }

    return ezf;
}



void ThreadGuard::SetEZLogFile (int threadid, FILE *ezf)
{

    ThreadGuardData  tgd;

// Ownership of the logfile pointer is passed off to the guard_map.
// Set the logfile pointer to NULL in tgd to prevent close when
// tgd goes out of scope.

    auto fscope = [&]()
    {
        tgd.ezlogfile = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);

    std::map<int, ThreadGuardData>::iterator it;

    it = guard_map.find(threadid);

    if (it != guard_map.end()) {
        it->second.ezlogfile = ezf;
        return;
    }

    tgd.ezlogfile = ezf;

    try {
        guard_map[threadid] = tgd;
    }
    catch (...) {
        printf ("\n**** Error adding ez log file to Thread Guard ****\n\n");
    }

    return;
}




CanvasManager *ThreadGuard::GetCanvasManager (int threadid)
{

    ThreadGuardData  tgd;

// Ownership of the canvas_manager pointer is passed off to the guard_map.
// Set canvas_manager to NULL in tgd to prevent it from being deleted when
// tgd goes out of scope.

    auto fscope = [&]()
    {
        tgd.canvas_manager = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);

    std::map<int, ThreadGuardData>::iterator it;
    CanvasManager  *cm = NULL;

    it = guard_map.find(threadid);

    if (it != guard_map.end()) {
        cm = it->second.canvas_manager;
        if (cm == NULL) {
            try {
                cm = new CanvasManager ();
                it->second.canvas_manager = cm;
            }
            catch (...) {
                return NULL;
            }
        }
        return cm;
    }

    try {
        tgd.canvas_manager = new CanvasManager ();
    }
    catch (...) {
        return NULL;
    }

    try {
        guard_map[threadid] = tgd;
        cm = tgd.canvas_manager;
    }
    catch (...) {
        delete (tgd.canvas_manager);
        cm = NULL;
    }

    return cm;
}

