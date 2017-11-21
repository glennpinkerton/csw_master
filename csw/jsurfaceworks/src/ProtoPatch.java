
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;

import java.util.ArrayList;

import csw.jutils.src.XYZPolyline;

/**
 This class stores the data from the patch splitting algorithm.
*/
public class ProtoPatch
{
    double[]      xpts;
    double[]      ypts;
    double[]      zpts;
    int           npts;
    ArrayList<JSBorderSegment>     borderList =
	    new ArrayList<JSBorderSegment> ();
    TriMesh       tmesh;
    int           patchid;
    long          origPatchID,
                  origSurfID;
    int           majorFlag;
    ArrayList<AttributeTriMesh>     atTriMeshList =
	    new ArrayList<AttributeTriMesh> ();
	ArrayList<XYZPolyline>          patchLineList =
		new ArrayList<XYZPolyline> ();


/*---------------------------------------------------------------------------*/

    ProtoPatch ()
    {
        patchid = -1;
        origSurfID = -1;
        origPatchID = -1;
    }

/*---------------------------------------------------------------------------*/

    public double[] getXpts ()
    {
        return xpts;
    }

    public double[] getYpts ()
    {
        return ypts;
    }

    public double[] getZpts ()
    {
        return zpts;
    }

    public int getNpts ()
    {
        return npts;
    }

    public TriMesh getTriMesh ()
    {
        return tmesh;
    }

    public ArrayList<AttributeTriMesh> getAtTriMeshList ()
    {
        return atTriMeshList;
    }

    public void addAtTriMesh (AttributeTriMesh atmesh)
    {
        atTriMeshList.add (atmesh);
    }

    public int getPatchID ()
    {
        return patchid;
    }

    public void setOrigPatchID (long lval)
    {
        origPatchID = lval;
    }

    public void setOrigSurfID (long lval)
    {
        origSurfID = lval;
    }

    public long getOrigPatchID ()
    {
        return origPatchID;
    }

    public long getOrigSurfID ()
    {
        return origSurfID;
    }

    public ArrayList<JSBorderSegment> getBorderList ()
    {
        return borderList;
    }

    public int getMajorFlag ()
    {
        return majorFlag;
    }

    public void setMajorFlag (int ival)
    {
        majorFlag = ival;
    }

	public ArrayList<XYZPolyline> getPatchLineList ()
	{
		return patchLineList;
	}

/*---------------------------------------------------------------------------*/

    void clearAll ()
    {
        xpts = null;
        ypts = null;
        zpts = null;
        npts = 0;
        borderList.clear ();
        tmesh = null;
        patchid = -1;
        origPatchID = -1;
        origSurfID = -1;
    }

/*---------------------------------------------------------------------------*/

    void addBorderSegment (
        double[] x,
        double[] y,
        double[] z,
        int      npts,
        int      type,
        int      direction)
    {

        JSBorderSegment    bseg = new JSBorderSegment (npts);
        double[] xa = bseg.getXArray ();
        double[] ya = bseg.getYArray ();
        double[] za = bseg.getZArray ();

        System.arraycopy (xa, 0, x, 0, npts);
        System.arraycopy (ya, 0, y, 0, npts);
        System.arraycopy (za, 0, z, 0, npts);

        bseg.npts = npts;
        bseg.type = type;
        bseg.direction = direction;

        borderList.add (bseg);

        return;
    }

/*---------------------------------------------------------------------------*/

    void addPatchLine (
        double[] x,
        double[] y,
        double[] z,
        int      npts,
        int      flag)
    {

		XYZPolyline pline = new XYZPolyline (x, y, z, flag);

        patchLineList.add (pline);

        return;
    }

/*---------------------------------------------------------------------------*/

    void copyPoints (
        double[] x,
        double[] y,
        double[] z,
        int      npts)
    {

        xpts = new double[npts];
        ypts = new double[npts];
        zpts = new double[npts];

        System.arraycopy (xpts, 0, x, 0, npts);
        System.arraycopy (ypts, 0, y, 0, npts);
        System.arraycopy (zpts, 0, z, 0, npts);

        this.npts = npts;

        return;

    }

}  // end of main class

