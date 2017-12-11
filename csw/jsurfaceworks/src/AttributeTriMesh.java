
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

//  private static final int    maxList = 100;

  TriMesh               tmesh;

/*------------------------------------------------------------------------*/

/**
 There is no public constructor, only a package scope constructor.
 This object is only constructed via various methods on the {@link JSurfaceWorks}
 class.
*/
  AttributeTriMesh ()
  {
  }

  AttributeTriMesh (TriMesh tmesh)
  {
    this.tmesh = tmesh;
  }

/*------------------------------------------------------------------------*/

  public TriMesh getTriMesh ()
  {
    return tmesh;
  }

/*------------------------------------------------------------------------*/


/**
 Public method to transfer the contents of this object to another
 AttributeTriMesh object.  Since the public cannot change any
 parts of the object, I do a shallow copy only.
*/
  public void transfer (AttributeTriMesh at)
  {
    at.tmesh = tmesh;
  }

}
