
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
  work polygon used for a contact line.  This is only used by the
  mbuilder.controller.SplitAndSeal class and for debugging.  It
  is not useful by application objects in general.
*/
public class DBContactLine extends XYZPolyline
{
    int    npts;
    int    patchid1;
    int    patchid2;

    DBContactLine (int npts)
    {
        super(npts);
        this.npts = npts;
        patchid1 = -1;
        patchid2 = -1;
    }

    public int getPatchID1 ()
    {
        return patchid1;
    }

    public int getPatchID2 ()
    {
        return patchid2;
    }

    public int getNpts ()
    {
        return npts;
    }

}

