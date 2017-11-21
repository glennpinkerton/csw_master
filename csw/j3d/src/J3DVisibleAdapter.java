/*
 */

package csw.j3d.src;


/**
 Application classes can extend this class
 to get notification of when 3d objects become
 visible or invisible.  
*/
public abstract class J3DVisibleAdapter   implements J3DVisibleListener
{

/**
This method will be called whenever a visibility change is made on the
3d display.  This means that an object or set of objects was set to
visible or invisible via the toolbar buttons.  If an object's visibility is
explicitly changed by the application, this listener is not called.
The {@link J3DVisibilityChangedInfo} object has lists of currently
visible and currently invisible objects.
*/
    public void visibilityChanged (J3DVisibilityChangedInfo info)
    {
    };

}  // end of abstract class definition
