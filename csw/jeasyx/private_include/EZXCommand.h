
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#ifndef _EZX_COMMAND_H_
#define _EZX_COMMAND_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

int ezx_create_new_dlist_data ();

int ezx_process_command (
    int               dlist_index,
    int               command_id,
    long              threadid,
    long              *llist,
    int               *ilist,
    char              *cdata,
    unsigned char     *bdata,
    short int         *sdata,
    int               *idata,
    float             *fdata,
    double            *ddata,
    FILE              *dfile
);

void ezx_set_void_ptrs (int dlist_id, void *v_jenv, void *v_jobj);

int  ezx_get_jenv (int dlist_index,
                   void **venv,
                   void **vobj);

#ifdef __cplusplus
}
#endif

#endif
