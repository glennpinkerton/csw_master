
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_fileio.h

    This header defines the CSWGrdFileio class, which refactors the
    old grd_fileio.c code.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file grd_fileio.h.
#endif


/*
    Add nothing above this line
*/
#ifndef GRD_FILEIO_H
#  define GRD_FILEIO_H

#  include "csw/surfaceworks/include/grd_shared_structs.h"

/*
    Define constants for the class.
*/
#define READ_FORMAT_1001    "%d %d %lf %lf %lf %lf %lf %lf %lf %lf %lg"
#define READ_FORMAT_1002    "%d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lg"
#define READ_FORMAT_1003    "%d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lg"
#define WRITE_FORMAT_1003   "%d %d %d %14.12f %14.12f %14.12f %14.12f \
%14.12g %14.12g %14.12g %14.12g %14.12g"
#define FILE_VERSION        4002
#define HEADER_SIZE         500
#define COMMENT_SIZE        200

#define MASK_BASE           100000000 /* 100 million */

#define IDIV1               256
#define IDIV2               65536
#define IDIV3               16777216

#define MULTI_GRID_BASE     1000000
#define FAULT_VERSION       2000
#define EXPANDED_MASK_VERSION  4000

#define FAULT_SCALE_CONSTANT_XY    10000000.0
#define FAULT_SCALE_CONSTANT_Z     1000000.0
#define FAULT_DOUBLE_MIN           1.e-30
#define FAULT_NULL_INT             20000000

#define MARGIN_DIVISOR             2.0f

#define HARD_NULL           1.e19f
#define NULL_INT            20000000
#define NULL_INT_READ       19000000


class CSWGrdFileio;

#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_utils.h"

#include "csw/utils/private_include/csw_fileio.h"

class CSWGrdFileio
{

  private:

    CSWGrdFault *grd_fault_ptr = NULL;
    CSWGrdUtils *grd_utils_ptr = NULL;

    CSWFileioUtil  fileio_util_obj;

  public:

    CSWGrdFileio () {};

    void   SetGrdFaultPtr (CSWGrdFault *p) {grd_fault_ptr = p;};
    void   SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};

    int    grd_write_binary_trimesh_file (int vused, double *vbase,
                                          TRiangleStruct *triangles, int ntri,
                                          EDgeStruct *edges, int nedge,
                                          NOdeStruct *nodes, int nnode,
                                          char *filename);
    long   grd_append_binary_trimesh_file (int vused, double *vbase,
                                           TRiangleStruct *triangles, int ntri,
                                           EDgeStruct *edges, int nedge,
                                           NOdeStruct *nodes, int nnode,
                                           char *filename);
    int    grd_read_binary_trimesh_file (char *filename,
                                         int *vused,
                                         double **vbase,
                                         TRiangleStruct **triangles, int *ntri,
                                         EDgeStruct **edges, int *nedge,
                                         NOdeStruct **nodes, int *nnode);
    int    grd_read_binary_trimesh_from_multi_file (char *filename, long position,
                                         int *vused,
                                         double **vbase,
                                         TRiangleStruct **triangles, int *ntri,
                                         EDgeStruct **edges, int *nedge,
                                         NOdeStruct **nodes, int *nnode);

    int    grd_write_file (const char *, const char *,
                           CSW_F *, char *, char *mask2,
                           int, int,
                           double, double, double, double, int,
                           FAultLineStruct*, int);
    int    grd_read_file (const char *, char *,
                          CSW_F **, char **, char **mask2,
                          int*, int*,
                          double*, double*, double*, double*, int*,
                          FAultLineStruct**, int*);
    int    grd_write_multiple_file (char*, GRidFileRec*, int);
    int    grd_read_multiple_file (char*, GRidFileRec*, int);
    int    grd_clean_file_rec_list (GRidFileRec *list, int nlist);

    int    grd_write_text_tri_mesh_file
                 (int vused,
                  double *vbasein,
                  TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  char *filename);
    long   grd_append_text_tri_mesh_file
                 (int vused,
                  double *vbase,
                  TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  char *filename);

    int    grd_write_text_file
                   (CSW_F *grid, int ncol, int nrow,
                   double xmin, double ymin, double xmax, double ymax,
                   char *filename);

    int grd_write_tri_mesh_file_2
                 (TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  double xshift, double yshift,
                  double *xb, double *yb, int nb,
                  const char *filename);

    int grd_write_tri_mesh_file
                 (TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  double xshift, double yshift,
                  const char *filename);

    int grd_write_points
                 (double *x,
                  double *y,
                  double *z,
                  int npts,
                  char *fname);

    int grd_write_float_points (CSW_F  *x,
                          CSW_F  *y,
                          CSW_F  *z,
                          int npts,
                          const char *fname);

    int grd_write_float_lines (
                   CSW_F  *x,
                   CSW_F  *y,
                   CSW_F  *z,
                   int    np,
                   int    *nc,
                   int    *nv,
                   const char *fname);


  private:

/*
    Old static file variables become private class variables.
*/
    double      Xmin = 0.0, Ymin = 0.0,
                Xmax = 0.0, Ymax = 0.0,
                Zmin = 0.0, Zmax = 0.0;
    double      MultiZmin = 1.e30,
                MultiZmax = -1.e30;
    char        Header[HEADER_SIZE],
                Head1[10],
                Head2[10],
                Head3[10],
                Head4[10];
    char        Comment[COMMENT_SIZE];

    int         WildInt = 100000000;
    CSW_F       WildFloat = 1.e20f,
                TinyFloat = 1.e-6;

    int         FilePos[GRD_MAX_MULTI_FILES];

/*
    Old file static functions become private class methods.
*/
    int         BinFileWrite (const void *buffer,
                              int size, int count, int file);
    int         BinFileRead (void *buffer,
                             int size, int count, int file);
    int         OpenFile (const char *name, const char *mode);
    int         AppendFile (const char *name, const char *mode, long *position);
    int         CreateFile (const char *name, const char *mode);
    int         CloseFile (int file);
    int         WriteGrid (int filenum, int fileptr, const char *comment,
                           CSW_F *grid, char *mask,
                           int ncol, int nrow, 
                           double xmin, double ymin,
                           double xmax, double ymax,
                           int gridtype);
    int         ReadGrid (int filenum, int fileptr, char *comment,
                          CSW_F **grid, char **mask,
                          int *ncol, int *nrow, 
                          double *xmin, double *ymin,
                          double *xmax, double *ymax,
                          int *gridtype,
                          int version, int csize, int dlen, int hsize);

    void        PintToChar4 (int inum, unsigned char *cptr);
    void        Char4ToPint (const unsigned char *cptr, int *inum);
    void        IntToChar4 (int inum, char *ucptr);
    void        Char4ToInt (char *ucptr, int *inum);

    int         FreeFileRecList (GRidFileRec *list, int nlist);

    int         WriteFaults (int filenum, FAultLineStruct *faults, int nfaults);
    int         ReadFaults (int version, int filenum,
                            FAultLineStruct **faults, int *nfaults);
    void        FreeFaultLines (FAultLineStruct *flist, int nf);

    int         ReadExpandedMask (char **mask2,
                                  int filenum);

    int         NearZero (CSW_F value);

    int         read_binary_trimesh_file (char *filename, long position,
                                          int *vused,
                                          double **vbase,
                                          TRiangleStruct **triangles, int *ntri,
                                          EDgeStruct **edges, int *nedge,
                                          NOdeStruct **nodes, int *nnode);
};  // end of main class definition

/*
    Add nothing to this file below the following endif
*/
#endif
