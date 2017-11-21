
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
This interface returns a list of currently selected objects in the
3d panel whenever a selection change is made.
*/
public interface J3DSelectListener
{

/**
This method will be called whenever a selection change is made by
the user (via mouse interaction).  If any of the parameters
is null, that means that no objects of the type have been selected.
A non null list can be empty, which also means that no object
of the type stored in the list is selected.
<p>
This is only called when the user changes selection.  It is not called
when an application explicitly changes the selection state of an object.
However, the {@link J3DSelectionChangedInfo} object includes lists of
all selected objects, regardless of how they were selected.
@param info A {@link J3DSelectionChangedInfo} object with information about
the current selected objects and about the individual selection change that
triggered this listener.
*/
    public void selectionChanged (J3DSelectionChangedInfo info);

/**
This method is called when the user changes the picking mode.
*/
    public void pickModeChanged ();

}  // end of interface definition
