
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
 information on zooming and panning in the JDisplayList object.
 */

public abstract class DLZoomPanAdapter implements DLZoomPanListener {

  /**
  This is called when a zoom or pan by the user changes the visible limits of
  any frame in the display.  This call is made prior to repainting the screen
  to reflect the new limits.  Note that this is only called when the user
  zooms or pans via the zoom pan controls, not when the application changes
  axis limits programatically.
  @param frameName  The name of the frame being changed.
  @param oldX1 The frame x coordinate of the lower left prior to the change.
  @param oldY1 The frame y coordinate of the lower left prior to the change.
  @param oldX2 The frame x coordinate of the upper right prior to the change.
  @param oldY2 The frame y coordinate of the upper right prior to the change.
  @param newX1 The frame x coordinate of the lower left after the change.
  @param newY1 The frame y coordinate of the lower left after the change.
  @param newX2 The frame x coordinate of the upper right after the change.
  @param newY2 The frame y coordinate of the upper right after the change.
  */
    public void zoomPanChanged (String frameName,
                                double oldX1,
                                double oldY1,
                                double oldX2,
                                double oldY2,
                                double newX1,
                                double newY1,
                                double newX2,
                                double newY2)
    {};

}

