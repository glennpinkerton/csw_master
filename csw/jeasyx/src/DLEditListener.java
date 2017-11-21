
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
 * This interface should be implemented by any application object that wants
 * to be notified about edit operations on the display list.
 It is registered with an editor such
 as the {@link JLineEditor} object to receive results of a particular edit
 operation.
 */

public interface DLEditListener {
    
  /**
  This is called when a piece of the objects being edited is changed.  For example, when
  a new point is added to a line or a point is deleted from a line or a point is moved
  in a line, this will be called.  The motion itself does not call this method.  If a 
  point is moved, the method is called when the point has been placed in its new
  location.  The editMoved method is called while the point is being moved.
  <p> A return value of true will update the editable items with whatever is
      currently selected.  This is not well defined or working yet, so you
      should always return false for now.
  @param dls A {@link DLSelectable} object with the current changed versions
  of the primitives.
  */
    public boolean editChanged (DLSelectable dls);

  /**
  This is called when a piece of the editing is being moved.  For example, as a point is
  being moved to a new location, this method is constantly called with each incremental
  relocation of the point.
  <p> A return value of true will update the editable items with whatever is
      currently selected.  This is not well defined or working yet, so you
      should always return false for now.
  @param dls A {@link DLSelectable} object with the versions of the primitives as they
  are being moved.
  */
    public boolean editMoved (DLSelectable dls);

  /**
  This is called when the creation or edit of a primitive is finished.  The new versions
  of any primitives involved in the edit are returned in the {@link DLSelectable}
  object.
  <p> A return value of true will update the editable items with whatever is
      currently selected.  This is not well defined or working yet, so you
      should always return false for now.
  @param dls A {@link DLSelectable} object with the new versions of the primitives.
  */
    public boolean editFinished (DLSelectable dls);

}

