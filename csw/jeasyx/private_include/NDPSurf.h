
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * This is the class definition for the NDPSurf class.  This class
 * is used to store a trimesh object in the display list.  The display
 * list has a list of pointers to objects of this class.
 */

#ifndef _NDPSURF_H
#define _NDPSURF_H

#  include "csw/jeasyx/private_include/DLContour.h"
#  include "csw/surfaceworks/include/contour_api.h"
#  include "csw/surfaceworks/include/grid_api.h"
#  include "csw/surfaceworks/private_include/grd_image.h"
#  include "csw/utils/include/csw_.h"

class NDPSurf {

  private:

    CSWGrdAPI        grdapi_obj;
    CSWContourApi    conapi_obj;
    CSWGrdImage      grdimage_obj;

  public:

    NDPSurf ();
    ~NDPSurf ();

    int              image_id;
    int              saved_image_id;
    int              index_num;

    int              frame_num;
    int              layer_num;
    int              deleted_flag;
    int              visible_flag;

    char             name[100];

    double           gxmin,
                     gymin,
                     gxmax,
                     gymax;

    int              needs_reclip;
    int              needs_recalc;

    DLContourProperties conprop;

    void SetSurfaceID (int id);
    void SetName (char *txt);

    int  SetGridData (
                  int *v1,
                  int *v2,
                  int *v3,
                  int *v4,
                  double *f1,
                  double *f2,
                  double *f3,
                  double *f4,
                  int ncol,
                  int nrow,
                  double xmin,
                  double ymin,
                  double width,
                  double height);

    int SetColorData (
        int     nvals,
        int     *vals,
        int     *red,
        int     *green,
        int     *blue,
        int     *alpha);

    int SetFaultData (
        double *xf,
        double *yf,
        int    *npts,
        int    nf);

    int SetBoundaryData (
        double *xpoly,
        double *ypoly,
        int    *nvpoly,
        int    npoly);

    int CalcContours (void *dlist, int val);
    int CalcContours (void *dlist, int val, int inum);
    int CalcImage (void *dlist);
    int CalcImage (void *dlist,
                   double xmin, double ymin,
                   double xmax, double ymax,
                   int ncol, int nrow);

    void *GetContourProperties (void);
    void SetContourProperties (void *dlprops);
    COntourCalcOptions const *GetCalcOptions (void);
    COntourDrawOptions const *GetDrawOptions (void);

  private:

    int              *v1data,
                     *v2data,
                     *v3data,
                     *v4data;
    double           *f1data,
                     *f2data,
                     *f3data,
                     *f4data;

    CSW_F            *rgrid,
                     *ggrid,
                     *bgrid,
                     *agrid;

    int              nvals,
                     *vals,
                     *red,
                     *green,
                     *blue,
                     *alpha;

    double           *xpoly,
                     *ypoly;
    int              *nvpoly;
    int              npoly;

    int              ncol,
                     nrow;
    double           xmin,
                     ymin,
                     width,
                     height;

    FAultLineStruct  *faults;
    int              nfaults;

    COntourCalcOptions  calc_options;
    COntourDrawOptions  draw_options;


    double           last_image_xmin;
    double           last_image_ymin;
    double           last_image_xmax;
    double           last_image_ymax;
    int              last_image_ncol;
    int              last_image_nrow;

};

/*  do not add anything after this endif  */
#endif
