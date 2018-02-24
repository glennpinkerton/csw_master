
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
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

#ifdef __cplusplus
}
#endif

#endif
