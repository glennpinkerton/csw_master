
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

import java.awt.event.MouseEvent;

/**
 This interface should be implemented by any graphic editors to get mouse and draw events from the 
 {@link JDisplayListPanel} object.
 */

public interface JDLEditListener {

    public void mouseClicked (MouseEvent e);
    public void mouseMoved (MouseEvent e);
    public void mouseDragged (MouseEvent e);
    public void mousePressed (MouseEvent e);
    public void mouseReleased (MouseEvent e);
    public void draw ();
    
}

