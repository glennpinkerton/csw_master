
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    sim_stubs.cc

    This is a stubbed out version of the externally accessible functions
    from sim_wrapper.c.  This version is linked with production versions
    of csw software.
*/


int wrap_set_file_and_line_for_malloc (const char *file, int line)
{
    file = file;
    line = line;

    return 1;
}
