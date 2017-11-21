
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
 * This abstract class implements the J3DRightClickListener interface.  It
 * can be extended by the application to listen for right clicks on a 
 * {@link J3DPanel} object.
 */
public abstract class J3DRightClickAdapter implements J3DRightClickListener
{

/**
 * This method is called whenever a right mouse button click is made in the
 * {@link J3DPanel}.  The {@link J3DRightClickInfo} object has information
 * about which grtaphical object was clicked on and the location of the click.
 */
    public void processRightClick (J3DRightClickInfo info){}

}  // end of abstract class
