
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
This abstract class implements the DLRightClickListener interface.  It
can be extended by the application to listen for right clicks on a 
{@link JDisplayListPanel} object.
*/
public abstract class DLRightClickAdapter implements DLRightClickListener
{

/**
This method is called whenever a right mouse button click is made in the
{@link JDisplayListPanel}.  The {@link DLRightClickInfo} object has information
about which object was clicked on and the location of the click.  If the listener
returns true from this method, then the listener has not handled this method
and it is asking the system to handle it by popping up the default menu.
If the listener returns false, the listener has completely handled the right
click and the system does nothing more.
*/
    public boolean processRightClick (DLRightClickInfo info){return true;}

}  
