
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;


/**
 This abstract class should be extended to allow an application object to get
 informed when edit operations occur.  It is registered with an editor such
 as the {@link JLineEditor} object to receive results of a particular edit
 operation.
 */

public abstract class DLEditAdapter implements DLEditListener {

  /**
  This is called when a piece of the objects being edited is changed.  For example, when
  a new point is added to a line or a point is deleted from a line or a point is moved
  in a line, this will be called.  The motion itself does not call this method.  If a
  point is moved, the method is called when the point has been placed in its new
  location.  The editMoved method is called while the point is being moved.
  @param dlsArray An array of {@link DLSelectable} objects with the new current versions
  of the primitives affected by the creation or edit.
  */
    public boolean editChanged (DLSelectable[] dlsArray)
    {return false;};

  /**
  This is called when a piece of the editing is being moved.  For example, as a point is
  being moved to a new location, this method is constantly called with each incremental
  relocation of the point.
  */
    public boolean editMoved (DLSelectable[] dlsArray)
    {return false;};
    
  /**
  This is called when the creation or edit of a primitive is finished.  The new versions
  of any primitives involved in the edit are returned in the {@link DLSelectable}
  object.
  @param dls A {@link DLSelectable} object with the new versions of the primitives affected
  by the creation or edit.
  */
    public boolean editFinished (DLSelectable dls)
    {return false;};

}

