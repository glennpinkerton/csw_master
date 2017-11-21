
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;

import csw.jutils.src.XYZPolyline;

/**
  Store the points, center line index used, fault index used and
  work polygon used for a contact line.
*/
public class DBDrapeLine extends XYZPolyline
{
    int    npts;

    DBDrapeLine (int npts)
    {
        super(npts);
        this.npts = npts;
    }
}    

