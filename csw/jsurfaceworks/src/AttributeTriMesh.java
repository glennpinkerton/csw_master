
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
  int                   numRDP;

/*------------------------------------------------------------------------*/

/**
 There is no public constructor, only a package scope constructor.
 This object is only constructed via various methods on the {@link JSurfaceWorks}
 class.
*/
  AttributeTriMesh ()
  {
    rdpList = new RDPAtValues [maxList];
    numRDP = 0;
  }

  AttributeTriMesh (TriMesh tmesh)
  {
    rdpList = new RDPAtValues [maxList];
    numRDP = 0;
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

  public TriMesh getTriMesh ()
  {
    return tmesh;
  }

/*------------------------------------------------------------------------*/

  public int getNumRDPAttributes ()
  {
    return numRDP;
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
 Get the first rdp attribute from the list.  This is meant as a
 convenience method for the case where there is only one attribute
 defined in the list.
*/
  public RDPAtValues getRDPValues ()
  {
    return rdpList[0];
  }


/*------------------------------------------------------------------------*/

/**
 Public method to transfer the contents of this object to another
 AttributeTriMesh object.  Since the public cannot change any
 parts of the object, I do a shallow copy only.
*/
  public void transfer (AttributeTriMesh at)
  {
    System.arraycopy (rdpList, 0, at.rdpList, 0, maxList);
    at.numRDP = numRDP;
    at.tmesh = tmesh;
  }

}
