
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
 This interface should be implemented by any object which wants to be notified
 of selection changes in the {@link JDisplayList} object.
 */

public interface DLSelectListener {
    
  /**
  This is called any time the select set changes.  The call is made prior to repainting
  the screen to highlight the selected objects.
  @param info A {@link DLSelectionInfo} onject with the information on the surrently selected objects.
  */
    public void selectionChanged (DLSelectionInfo info);

}

