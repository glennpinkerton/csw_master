
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;


public abstract class DLSelectAdapter implements DLSelectListener {

  /**
  This is called any time the select set changes.  The call is made prior to repainting
  the screen to highlight the selected objects.
  */
    public void selectionChanged (DLSelectionInfo info)
    {};

}

