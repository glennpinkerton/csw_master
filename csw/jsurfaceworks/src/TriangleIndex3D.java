
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

/**
 This class acts as a "peer" to a C++ Spatial3DTriangleIndex object.  The actual
 data and the manipulation of the data for a 3d triangle index are done in native
 code.  The 3d triangle index is used to rapidly retrieve triangles that are
 inside a 3d bounding box.  It is used primarily in sealing models.  There is no
 public constructor for this class.  You should create an object of this type
 using the static {@link JSurfaceWorks#create3DTriangleIndex} method.
*/

public class TriangleIndex3D

{
	private static long StaticID = 0;

/*--------------------------------------------------------------------------*/

    static public TriangleIndex3D instance ()
    {
        TriangleIndex3D t3d;

        t3d = new TriangleIndex3D ();

        return t3d;
    }

/*--------------------------------------------------------------------------*/

    private TriangleIndex3D () {}


/*--------------------------------------------------------------------------*/

    private int                id1 = 0, id2 = 0;
    private JSurfaceWorks      jsw;

    private long getUniqueID ()
	{
		StaticID++;
		return StaticID;
	}

    private void createIDS ()
    {
        if (id1 == 0  &&  id2 == 0) {
            long id = getUniqueID();
            long left, right;
            left = id >>> 32;
            right = id << 32;
            right = right >>> 32;
            id1 = (int)left;
            id2 = (int)right;
        }
    }

    int getID1 ()
    {
        createIDS ();
        return id1;
    }

    int getID2 ()
    {
        createIDS ();
        return id2;
    }



/*--------------------------------------------------------------------------*/

  /**
   Reset the bounds and spacings of the index.  This must be done prior to adding any
   trimesh data to the index.
   @param xmin Minimum X coordinate of the 3d boundary box.
   @param ymin Minimum Y coordinate of the 3d boundary box.
   @param zmin Minimum Z coordinate of the 3d boundary box.
   @param xmax Maximum X coordinate of the 3d boundary box.
   @param ymax Maximum Y coordinate of the 3d boundary box.
   @param zmax Maximum Z coordinate of the 3d boundary box.
   @param xspace Grid cell size in the X dimension.
   @param yspace Grid cell size in the Y dimension.
   @param zspace Grid cell size in the Z dimension.
   @return Returns 1 on success, returns zero if the parameters are bad, returns
  -1 if a memory allocation failed in the native code.
  */
    public int setGeometry (
        double    xmin,
        double    ymin,
        double    zmin,
        double    xmax,
        double    ymax,
        double    zmax,
        double    xspace,
        double    yspace,
        double    zspace)
    {

        createIDS ();

        jsw = new JSurfaceWorks ();

        int istat =
          jsw.swSet3DTriangleIndexGeometry (
            id1, id2,
            xmin, ymin, zmin,
            xmax, ymax, zmax,
            xspace, yspace, zspace);

        return istat;

    }

/*------------------------------------------------------------------------------*/

  /**
   Add a {@link TriMesh} object to this index.  The trimeshid should be unique
   for this index.  It is used to identify which trimesh a triangle belongs to
   when you get triangles from the index.  It is up to the application to assign
   id values that will identify the trimesh properly.  I do not use the unique id
   of the trimesh for this because this identifier is placed with every triangle in
   the index.  I prefer to not use 8 bytes for this due to memory considerations.
   @param tmesh A {@link TriMesh} object to put into the index.
   @return Returns 1 on success or returns -1 if there is a memory allocation
    error in the native code.
  */
    public int addTriMesh (TriMesh tmesh)
    {
        int        istat;

        addTmeshID (tmesh);

        createIDS ();

        jsw = new JSurfaceWorks ();

        istat =
          jsw.swAdd3DTriangleIndexTriMesh (id1, id2, tmeshID, tmesh);

        return istat;

    }


/*-------------------------------------------------------------------------------*/

  /**
   Clear all the triangle data that has been added to this index via {@link #addTriMesh}.
   The geometry of the index is not affected by calling this method.
  */
    public void Clear ()
    {
        createIDS ();

        jsw = new JSurfaceWorks ();

        jsw.swClear3DTriangleIndex (id1, id2);

        return;

    }


/*-------------------------------------------------------------------------------*/

  /**
   Retrieve a list of triangle index objects that intersect or come quite close
   to intersecting the specified 3d bounding box.
   @param xmin Minimum X coordinate of the 3d boundary box.
   @param ymin Minimum Y coordinate of the 3d boundary box.
   @param zmin Minimum Z coordinate of the 3d boundary box.
   @param xmax Maximum X coordinate of the 3d boundary box.
   @param ymax Maximum Y coordinate of the 3d boundary box.
   @param zmax Maximum Z coordinate of the 3d boundary box.
  */
    public TriangleIndex3DResultList getTriangles (
        double      xmin,
        double      ymin,
        double      zmin,
        double      xmax,
        double      ymax,
        double      zmax)
    {
        createIDS ();

        jsw = new JSurfaceWorks ();

        int istat =
          jsw.swGet3DTriangles (
            id1, id2,
            xmin, ymin, zmin,
            xmax, ymax, zmax);

        if (istat == -1) {
            return null;
        }

        TriangleIndex3DResultList result =
          jsw.getTriangleIndex3DResultList ();

        return result;

    }

/*-------------------------------------------------------------------------------*/

  /**
   When this object is garbage collected, make sure the C++ object
   associated with it is deleted also.
  */
    protected void finalize() throws Throwable {

        createIDS ();
        JSurfaceWorks.addFinalized3DTriangleIndex (id1, id2);

        super.finalize();
        return;
    }

/*--------------------------------------------------------------------------------*/

    ArrayList<TriMesh>     idList = 
		new ArrayList<TriMesh> (10);
    int                    tmeshID = -1;

    private void addTmeshID (TriMesh tmesh)
    {
        int          i;
        TriMesh      tm;

        int size = idList.size ();

        for (i=0; i<size; i++) {
            tm = idList.get (i);
            if (tm == tmesh) {
                tmeshID = i;
                return;
            }
        }

        tmeshID = size;
        idList.add (tmesh);

        return;

    }


}  // end of main class
