/*
 */

package csw.j3d.src;


/**
This interface returns a list of currently selected objects in the 
3d panel whenever a selection change is made.
*/
public interface J3DVisibleListener 
{

/**
This method will be called whenever a visibility change is made on the
3d display.  This means that an object or set of objects was set to
visible or invisible via the toolbar buttons.  If an object's visibility is 
explicitly changed by the application, this listener is not called.
The {@link J3DVisibilityChangedInfo} object has lists of currently
visible and currently invisible objects.
*/
    public void visibilityChanged (J3DVisibilityChangedInfo info);

}  // end of interface definition
