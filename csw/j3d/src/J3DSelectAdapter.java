
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
 Abstract class that implements the {@link J3DSelectListener} interface.
 The application may extend this class to listen for selection changes
 on the 3d view.
*/
public abstract class J3DSelectAdapter   implements J3DSelectListener
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
    public void selectionChanged (J3DSelectionChangedInfo info)
    {
    };

/**
This method is called when the user changes the picking mode.
*/
    public void pickModeChanged ()
    {
    }

}  // end of abstract class definition
