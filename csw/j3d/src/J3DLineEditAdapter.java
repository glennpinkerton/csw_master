
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;


/**
 This is an abstract class that implements the {@link J3DLineEditListener}
 interface.  You can extend this class if you wish to be informed of line
 editing in the {@link J3DPanel} object.
 */

public abstract class J3DLineEditAdapter implements J3DLineEditListener
{
    
  /**
  This is called when a point in the {@link J3DLine} has been modified by moving,
  deletion or adding.  This is called after the point has been "locked" into its
  new position, not incrementally during the motion.  The J3DLine object specified
  is a temporary snapshot of the line at the time the change was made.
  */
    public void lineChanged (J3DLine line3d){}

  /**
  This is called while a point in the line or the entire line is being moved.  The
  {@link J3DLine} object is a snapshot of the line at a particular motion increment.
  @param line3d A {@link J3DLine} object with the versions of the versions of the 
  line points as they are being moved.
  */
    public void lineMoved (J3DLine line3d){}

  /**
  This is called when the editing for the line is finished.  This can occur from the
  user double clicking in line creation mode, from starting a line on a new surface,
  from ending the edit of an existing line, or from the user switching back to
  selection mode.  The {@link J3DLine} is the snapshot of the line when it was finished.
  @param line3d A {@link J3DLine} object with the versions of the version of the
  line points when the edit is finished.
  */
    public void lineFinished (J3DLine line3d){}

  /**
  This is called when the user explicitly ends the editing of a line or group of lines.
  A click on the toolbar select icon will end editing as well as a click on an end edit
  popup menu item might end editing.
  */
    public void editCompletelyFinished (){}

}

