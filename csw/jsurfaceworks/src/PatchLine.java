
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
 Store the points for a line on a patch.
*/
public class PatchLine extends XYZPolyline
{
    int    npts;
    int    flag;

    PatchLine (int npts)
    {
        super(npts);
        this.npts = npts;
    }

    public int getNpts ()
    {
        return npts;
    }

    public int getFlag ()
    {
        return flag;
    }
}    

