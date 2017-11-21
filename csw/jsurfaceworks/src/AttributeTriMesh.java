
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;


/**
 This class has a trimesh and various attribute arrays associated
 with nodes on the trimesh.  The trimesh is specified in the
 constructor and is not changeable after that.  The various attribute
 lists have exactly the same number of elements as nodes in the
 input trimesh and are in the same order.
 <p>
 An AttributeTriMesh object is only calculated via the
 {@link JSurfaceWorks} object.  There is no public constructor.

  @author Glenn Pinkerton
*/
public class AttributeTriMesh {

  private static final int    maxList = 100;

  TriMesh               tmesh;
  RDPAtValues[]         rdpList;
  NDPAtValues[]         ndpList;
  int                   numRDP,
                        numNDP;

/*------------------------------------------------------------------------*/

/**
 There is no public constructor, only a package scope constructor.
 This object is only constructed via various methods on the {@link JSurfaceWorks}
 class.
*/
  AttributeTriMesh ()
  {
    rdpList = new RDPAtValues [maxList];
    ndpList = new NDPAtValues [maxList];
    numRDP = 0;
    numNDP = 0;
  }

  AttributeTriMesh (TriMesh tmesh)
  {
    rdpList = new RDPAtValues [maxList];
    ndpList = new NDPAtValues [maxList];
    numRDP = 0;
    numNDP = 0;
    this.tmesh = tmesh;
  }

/*------------------------------------------------------------------------*/

/**
 Package scope method to add an RDP attribute value.
*/
  void addRDPValue (
    String         name,
    double[]       values,
    int            npts)
  {
    if (numRDP >= maxList) {
      return;
    }

    RDPAtValues   rdpVal = new RDPAtValues ();

    if (name == null) {
      rdpVal.name = null;
    }
    else {
      rdpVal.name = new String (name);
    }
    rdpVal.values = new double[npts];
    System.arraycopy (values, 0, rdpVal.values, 0, npts);
    rdpVal.nValues = npts;

    rdpList[numRDP] = rdpVal;
    numRDP++;

    return;
  }

/*-----------------------------------------------------------------------*/

/**
 Package scope method to add an NDP attribute value.
*/
  void addNDPValue (
    String         name,
    byte[]         v1,
    byte[]         v2,
    byte[]         v3,
    byte[]         v4,
    byte[]         p1,
    byte[]         p2,
    byte[]         p3,
    byte[]         p4,
    int            npts)
  {
    if (numNDP >= maxList) {
      return;
    }

    NDPAtValues  ndpVal = new NDPAtValues ();

    if (name == null) {
      ndpVal.name = null;
    }
    else {
      ndpVal.name = new String (name);
    }

    ndpVal.v1 = new byte[npts];
    ndpVal.v2 = new byte[npts];
    ndpVal.v3 = new byte[npts];
    ndpVal.v4 = new byte[npts];

    ndpVal.p1 = new byte[npts];
    ndpVal.p2 = new byte[npts];
    ndpVal.p3 = new byte[npts];
    ndpVal.p4 = new byte[npts];

    System.arraycopy (v1, 0, ndpVal.v1, 0, npts);
    System.arraycopy (v2, 0, ndpVal.v2, 0, npts);
    System.arraycopy (v3, 0, ndpVal.v3, 0, npts);
    System.arraycopy (v4, 0, ndpVal.v4, 0, npts);
    System.arraycopy (p1, 0, ndpVal.p1, 0, npts);
    System.arraycopy (p2, 0, ndpVal.p2, 0, npts);
    System.arraycopy (p3, 0, ndpVal.p3, 0, npts);
    System.arraycopy (p4, 0, ndpVal.p4, 0, npts);

    ndpVal.nValues = npts;

    ndpList[numNDP] = ndpVal;
    numNDP++;

  }

/*------------------------------------------------------------------------*/

  public TriMesh getTriMesh ()
  {
    return tmesh;
  }

/*------------------------------------------------------------------------*/

  public int getNumRDPAttributes ()
  {
    return numRDP;
  }

  public int getNumNDPAttributes ()
  {
    return numNDP;
  }

/*------------------------------------------------------------------------*/

/**
 Get the specified rdp attribute from the list.
*/
  public RDPAtValues getRDPValues (int index)
  {
    return rdpList[index];
  }

/**
 Get the specified ndp attribute from the list.
*/
  public NDPAtValues getNDPValues (int index)
  {
    return ndpList[index];
  }

/**
 Get the first rdp attribute from the list.  This is meant as a
 convenience method for the case where there is only one attribute
 defined in the list.
*/
  public RDPAtValues getRDPValues ()
  {
    return rdpList[0];
  }

/**
 Get the first ndp attribute from the list.  This is meant as a
 convenience method for the case where there is only one attribute
 defined in the list.
*/
  public NDPAtValues getNDPValues ()
  {
    return ndpList[0];
  }

/*------------------------------------------------------------------------*/

/**
 Public method to transfer the contents of this object to another
 AttributeTriMesh object.  Since the public cannot change any
 parts of the object, I do a shallow copy only.
*/
  public void transfer (AttributeTriMesh at)
  {
    System.arraycopy (ndpList, 0, at.ndpList, 0, maxList);
    System.arraycopy (rdpList, 0, at.rdpList, 0, maxList);
    at.numNDP = numNDP;
    at.numRDP = numRDP;
    at.tmesh = tmesh;
  }

}
