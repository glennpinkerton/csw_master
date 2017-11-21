
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
public class JSBorderSegment extends XYZPolyline
{
    int    npts;
    int    type;
    int    direction;

    JSBorderSegment (int npts)
    {
        super(npts);
        this.npts = npts;
    }

    public int getNpts ()
    {
        return npts;
    }

    public int getType ()
    {
        return type;
    }

    public int getDirection ()
    {
        return direction;
    }

}    

