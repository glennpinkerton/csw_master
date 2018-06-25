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
#include  "csw/hlevutils/src/simulate_new.h"

#include  "csw/utils/private_include/csw_scope.h"


namespace ThreadGuard
{

// implement collections of resources via stl map objects.
// The index of each map is the hashcode from the java resource
// object and the value is the pointer to the c++ object.
// The java code is responsible for synchronizing a resource
// when it is being modified.  The c++ JNI level code uses
// EnterMonitor and ExitMonitor to "lock" the update of any
// of the maps defined here.

// The original code for this used a map of structures, with each
// structure having pointers to various objects.  Experience says
// that different object types can and will have different key
// values.  Thus, separate maps are needed for each object type.

// Many web sites say unique pointers should be used in collections
// like those defined below.  The syntax complexity of the unique
// pointers obfuscates the code  more than it helps in this context
// (at least in my opinion).  I have chosen to forego the use of
// unique pointers here.

    CanvasManager   CanvasMgr;
    std::map <long, CSWGrdAPI*> grd_api_map;
    std::map <long, PATCHSplit*> patch_split_map;
    std::map <long, GRDVert*> grd_vert_map;
    std::map <long, SWCalc*> sw_calc_map;

    std::map <long, FILE*> sw_save_log_file_map;
    std::map <long, FILE*> sw_log_file_map;
    std::map <long, FILE*> ez_log_file_map;

    int  ifile = 0;

}  // end of ThreadGuard namespace


void ThreadGuard::RemoveAllThreadData (void)
{
    std::map<long, CSWGrdAPI*>::iterator gapi_it;
    std::map<long, PATCHSplit*>::iterator ps_it;
    std::map<long, GRDVert*>::iterator gv_it;
    std::map<long, SWCalc*>::iterator sw_it;
    std::map<long, FILE*>::iterator ezf_it;
    std::map<long, FILE*>::iterator swf_it;
    std::map<long, FILE*>::iterator swsf_it;

    gapi_it = grd_api_map.begin();
    while (gapi_it != grd_api_map.end()) {
        delete (gapi_it->second);
        gapi_it->second = NULL;
        gapi_it++;
    }
    grd_api_map.clear();

    ps_it = patch_split_map.begin();
    while (ps_it != patch_split_map.end()) {
        delete (ps_it->second);
        ps_it->second = NULL;
        ps_it++;
    }
    patch_split_map.clear();

    gv_it = grd_vert_map.begin();
    while (gv_it != grd_vert_map.end()) {
        delete (gv_it->second);
        gv_it->second = NULL;
        gv_it++;
    }
    grd_vert_map.clear();

    sw_it = sw_calc_map.begin();
    while (sw_it != sw_calc_map.end()) {
        delete (sw_it->second);
        sw_it->second = NULL;
        sw_it++;
    }
    sw_calc_map.clear();

    swf_it = sw_log_file_map.begin();
    while (swf_it != sw_log_file_map.end()) {
        if (swf_it->second != NULL) {
            fclose (swf_it->second);
        }
        swf_it->second = NULL;
        swf_it++;
    }
    sw_save_log_file_map.clear();

    swsf_it = sw_save_log_file_map.begin();
    while (swsf_it != sw_save_log_file_map.end()) {
        if (swsf_it->second != NULL) {
            fclose (swsf_it->second);
        }
        swsf_it->second = NULL;
        swsf_it++;
    }
    sw_save_log_file_map.clear();

    ezf_it = ez_log_file_map.begin();
    while (ezf_it != ez_log_file_map.end()) {
        if (ezf_it->second != NULL) {
            fclose (ezf_it->second);
        }
        ezf_it->second = NULL;
        ezf_it++;
    }
    ez_log_file_map.clear();

    CanvasMgr.RemoveAllFromManager ();
    CanvasMgr.CleanAll ();
}



void ThreadGuard::RemoveThreadData (long jside_id)
{
    std::map<long, PATCHSplit*>::iterator it_ps;
    std::map<long, GRDVert*>::iterator it_gv;
    std::map<long, CSWGrdAPI*>::iterator it_ap;
    std::map<long, SWCalc*>::iterator it_sw;
    std::map<long, FILE*>::iterator ezf_it;
    std::map<long, FILE*>::iterator swf_it;
    std::map<long, FILE*>::iterator swsf_it;

    it_ps = patch_split_map.find (jside_id);
    if (it_ps != patch_split_map.end()) {
        delete (it_ps->second);
        it_ps->second = NULL;
    }

    it_gv = grd_vert_map.find (jside_id);
    if (it_gv != grd_vert_map.end()) {
        delete (it_gv->second);
        it_gv->second = NULL;
    }

    it_ap = grd_api_map.find(jside_id);
    if (it_ap != grd_api_map.end()) {
        delete (it_ap->second);
        it_ap->second = NULL;
    }

    it_sw = sw_calc_map.find(jside_id);
    if (it_sw != sw_calc_map.end()) {
        delete (it_sw->second);
        it_sw->second = NULL;
    }

    ezf_it = ez_log_file_map.find (jside_id);
    if (ezf_it != ez_log_file_map.end()) {
        if (ezf_it->second != NULL) {
            fclose (ezf_it->second);
        }
        ezf_it->second = NULL;
    }

    swf_it = sw_log_file_map.find (jside_id);
    if (swf_it != sw_log_file_map.end()) {
        if (swf_it->second != NULL) {
            fclose (swf_it->second);
        }
        swf_it->second = NULL;
    }

    swsf_it = sw_save_log_file_map.find (jside_id);
    if (swsf_it != sw_save_log_file_map.end()) {
        if (swsf_it->second != NULL) {
            fclose (swsf_it->second);
        }
        swf_it->second = NULL;
    }

}




PATCHSplit *ThreadGuard::GetPatchSplit (long jside_id)
{

    std::map<long, PATCHSplit*>::iterator it;
    PATCHSplit  *ps = NULL;

    auto fscope = [&]()
    {
    };
    CSWScopeGuard func_scope_guard (fscope);

    it = patch_split_map.find(jside_id);

    if (it != patch_split_map.end()) {
        ps = it->second;
        if (ps == NULL) {
            try {
                SNF;
                ps = new PATCHSplit ();
            }
            catch (...) {
                printf ("\n***** Exception from new in GetPatchSplit *****\n\n");
                return NULL;
            }
            it->second = ps;
        }
        return ps;
    }

    try {
        SNF;
        ps = new PATCHSplit ();
    }
    catch (...) {
        printf ("\n***** Exception from new in GetPatchSplit *****\n\n");
        delete (ps);
        ps = NULL;
        return ps;
    }

    try {
        patch_split_map[jside_id] = ps;
    }
    catch (...) {
        delete (ps);
        ps = NULL;
    }

    return ps;
}




GRDVert *ThreadGuard::GetGrdVert (long jside_id)
{

    std::map<long, GRDVert*>::iterator it;
    GRDVert  *gv = NULL;

    auto fscope = [&]()
    {
    };
    CSWScopeGuard func_scope_guard (fscope);

    it = grd_vert_map.find(jside_id);

    if (it != grd_vert_map.end()) {
        gv = it->second;
        if (gv == NULL) {
            try {
                SNF;
                gv = new GRDVert ();
            }
            catch (...) {
                printf ("\n***** Exception from new in GetGrdVert *****\n\n");
                return NULL;
            }
            it->second = gv;
        }
        return gv;
    }

    try {
        SNF;
        gv = new GRDVert ();
    }
    catch (...) {
        printf ("\n***** Exception from new in GetGrdVert *****\n\n");
        delete (gv);
        gv = NULL;
        return gv;
    }

    try {
        grd_vert_map[jside_id] = gv;
    }
    catch (...) {
        printf ("\n***** Exception setting grd vert in map *****\n\n");
        delete (gv);
        gv = NULL;
    }

    return gv;
}




CSWGrdAPI *ThreadGuard::GetGrdAPI (long jside_id)
{

    std::map<long, CSWGrdAPI*>::iterator it;
    CSWGrdAPI  *gapi = NULL;

    auto fscope = [&]()
    {
    };
    CSWScopeGuard func_scope_guard (fscope);

    it = grd_api_map.find(jside_id);

    if (it != grd_api_map.end()) {
        gapi = it->second;
        if (gapi == NULL) {
            try {
                SNF;
                gapi = new CSWGrdAPI ();
            }
            catch (...) {
                printf ("\n***** Exception from new in GetCSWGrdAPI *****\n\n");
                return NULL;
            }
            it->second = gapi;
        }
        return gapi;
    }

    try {
        SNF;
        gapi = new CSWGrdAPI ();
    }
    catch (...) {
        printf ("\n***** Exception from new in GetCSWGrdAPI *****\n\n");
        delete (gapi);
        gapi = NULL;
        return gapi;
    }

    try {
        grd_api_map[jside_id] = gapi;
    }
    catch (...) {
        delete (gapi);
        gapi = NULL;
    }

    return gapi;
}




SWCalc  *ThreadGuard::GetSWCalc (long jside_id)
{

    std::map<long, SWCalc*>::iterator it;
    SWCalc  *sw = NULL;

    auto fscope = [&]()
    {
    };
    CSWScopeGuard func_scope_guard (fscope);

    it = sw_calc_map.find(jside_id);

    if (it != sw_calc_map.end()) {
        sw = it->second;
        if (sw == NULL) {
            try {
                SNF;
                sw = new SWCalc (ThreadGuard::ifile);
                ThreadGuard::ifile++;
            }
            catch (...) {
                printf ("\n***** Exception from new in GetSWCalc *****\n\n");
                sw = NULL;
            }
            it->second = sw;
        }
        return sw;
    }

    try {
        SNF;
        sw = new SWCalc (ThreadGuard::ifile);
        ThreadGuard::ifile++;
    }
    catch (...) {
        printf ("\n***** Exception from new in GetSWCalc *****\n\n");
        sw = NULL;
    }

    try {
        sw_calc_map[jside_id] = sw;
    }
    catch (...) {
        delete (sw);
        sw = NULL;
    }

    return sw;
}





FILE  *ThreadGuard::GetSWLogFile (long jside_id)
{
    std::map<long, FILE*>::iterator it;
    FILE  *swf = NULL;

    it = sw_log_file_map.find(jside_id);

    if (it != sw_log_file_map.end()) {
        swf = it->second;
        return swf;
    }

    return swf;
}




FILE  *ThreadGuard::GetSWSaveLogFile (long jside_id)
{
    std::map<long, FILE*>::iterator it;
    FILE  *swf = NULL;

    it = sw_save_log_file_map.find(jside_id);

    if (it != sw_save_log_file_map.end()) {
        swf = it->second;
        return swf;
    }

    return swf;
}



void ThreadGuard::SetSWLogFiles (long jside_id, FILE *swf, FILE *swsf)
{

    std::map<long, FILE*>::iterator it;
    std::map<long, FILE*>::iterator it2;

    it = sw_log_file_map.find(jside_id);
    it2 = sw_save_log_file_map.find(jside_id);

    if (it != sw_log_file_map.end()) {
        it->second = swf;
    }
    else {
        try {
            sw_log_file_map[jside_id] = swf;
        }
        catch (...) {
            printf ("\n**** Error adding sw log file to map ****\n\n");
        }
    }

    if (it2 != sw_save_log_file_map.end()) {
        it2->second = swsf;
    }
    else {
        try {
            sw_save_log_file_map[jside_id] = swsf;
        }
        catch (...) {
            printf ("\n**** Error adding sw save log file to map ****\n\n");
        }
    }

    return;
}




FILE  *ThreadGuard::GetEZLogFile (long jside_id)
{
    std::map<long, FILE*>::iterator it;
    FILE  *ezf = NULL;

    it = ez_log_file_map.find(jside_id);

    if (it != ez_log_file_map.end()) {
        ezf = it->second;
        return ezf;
    }

    return ezf;
}



void ThreadGuard::SetEZLogFile (long jside_id, FILE *ezf)
{

    std::map<long, FILE*>::iterator it;

    it = ez_log_file_map.find(jside_id);

    if (it != ez_log_file_map.end()) {
        it->second = ezf;
        return;
    }

    try {
        ez_log_file_map[jside_id] = ezf;
    }
    catch (...) {
        printf ("\n**** Error adding ez log file to map ****\n\n");
    }

    return;
}




CanvasManager *ThreadGuard::GetCanvasManager ()
{
    return &(ThreadGuard::CanvasMgr);
}



long ThreadGuard::CreateGrdAPI (long jside_id)
{

    std::map<long, CSWGrdAPI*>::iterator it;
    CSWGrdAPI  *gapi = NULL;

    auto fscope = [&]()
    {
    };
    CSWScopeGuard func_scope_guard (fscope);

    it = grd_api_map.find(jside_id);

    if (it != grd_api_map.end()) {
        return it->first;
    }

    try {
        SNF;
        gapi = new CSWGrdAPI ();
    }
    catch (...) {
        printf ("\n***** Exception from new in GetCSWGrdAPI *****\n\n");
        delete (gapi);
        return -1;
    }

    try {
        grd_api_map[jside_id] = gapi;
    }
    catch (...) {
        delete (gapi);
        return -1;
    }

    return jside_id;
}



long ThreadGuard::CreateGrdVert (long jside_id)
{

    std::map<long, GRDVert*>::iterator it;
    GRDVert  *gv = NULL;

    auto fscope = [&]()
    {
    };
    CSWScopeGuard func_scope_guard (fscope);

    it = grd_vert_map.find(jside_id);

    if (it != grd_vert_map.end()) {
        return it->first;
    }

    try {
        SNF;
        gv = new GRDVert ();
    }
    catch (...) {
        printf ("\n***** Exception from new in GetGRDVert *****\n\n");
        delete (gv);
        return -1;
    }

    try {
        grd_vert_map[jside_id] = gv;
    }
    catch (...) {
        delete (gv);
        return -1;
    }

    return jside_id;
}

