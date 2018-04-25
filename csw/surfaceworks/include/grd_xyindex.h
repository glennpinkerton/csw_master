
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * Define the interface for the XYIndex2D class.  The
 * body of the class is in grd_xyindex.cc, in the ..\src
 * directory.
 */

#ifndef GRD_XYINDEX2D_H
#define GRD_XYINDEX2D_H

class XYIndex2D
{

  public:

    XYIndex2D (double xmin, double ymin, double xmax, double ymax);

    virtual ~XYIndex2D ();


// Objects of class not meant to be copied or moved.

  private:

    XYIndex2D (const XYIndex2D &other) {};
    const XYIndex2D &operator= (const XYIndex2D &other) {return *this;};
    XYIndex2D (const XYIndex2D &&other) {};
    const XYIndex2D &operator= (const XYIndex2D &&other) {return *this;};


  public:

    int SetGeometry (double xmin,
                     double ymin,
                     double xmax,
                     double ymax,
                     double xspace,
                     double yspace);

    int AddPoints (double *x, double *y, int npts);

    void Clear (void);

    int GetClosePoints (
        double x,
        double y,
        int    *list,
        int    *nlist,
        int    maxlist);

    void GetIndexGrid (
        int    const ***igrid,
        int    *ncol,
        int    *nrow,
        double *xmin,
        double *ymin,
        double *xmax,
        double *ymax);


  private:

  /*
   * Private methods.
   */
    int AddPoint (double x,
                  double y,
                  int pointNumber);

    int AddToIndexGrid (int index,
                        int pointNumber);

    int CreateIndexGrid (void);

  /*
   * Private data members.
   */
    int           **IndexGrid;
    int           IndexNcol,
                  IndexNrow;
    double        IndexXmin,
                  IndexYmin,
                  IndexXspace,
                  IndexYspace,
                  IndexXmax,
                  IndexYmax;

    int           geometryAllowed;

};


#endif
