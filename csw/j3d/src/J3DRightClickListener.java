
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
 This interface is used to get information about right mouse button 
 clicks that are performed on objects in a {@link J3DPanel}.
*/
public interface J3DRightClickListener 
{

/**
This method is called whenever a right mouse button click is made in the
{@link J3DPanel}.  The {@link J3DRightClickInfo} object has information
about which object was clicked on and the location of the click.
*/
    public void processRightClick (J3DRightClickInfo info);

}  // end of interface definition
