
/*
         ************************************************
         *                                              *
         *    Copyright (2017) Glenn Pinkerton.         *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

import java.util.ArrayList;
import java.lang.Exception;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.GridLayout;

import javax.swing.JFrame;
import javax.swing.WindowConstants;




/**
  This class provides a base framework for creating graphics
  on a frame window.  Each JDLFrame (or derived from it) object
  is managed on a static collection in this class.  If you want
  to clean up resources for a frame or for all frames, you should
  use the static methods here.  

  @author Glenn Pinkerton

*/
public class JDLFrame extends JFrame {

    private static final long serialVersionUID = 1357L;

    public static final int    FRAME_WITH_TOOLBAR = 0;
    public static final int    FRAME_WITHOUT_TOOLBAR = 1;

 /**
  *  The dl object is created by the construcor and can be accessed
  *  by a derived class via the getDL method.
  */
    private JDisplayList  dl = null;
    private int           dlist_id = -1;

/**
 *  This constructor will create a frame with a "tool bar" at the
 *  top.
 */
    public JDLFrame () 
    {
        init (FRAME_WITH_TOOLBAR);
    }


/**
 *  This constructor triggers different types of frames.  For now,
 *  a type value is either DLFrame.FRAME_WITH_TOOLBAR or 
 *  DLFrame.FRAME_WITHOUT_TOOLBAR.
 */
    public JDLFrame (int   type) {
        if (type != FRAME_WITHOUT_TOOLBAR) {
            type = FRAME_WITH_TOOLBAR;
        }
        init (type);
    }


/**
 *  Called by constructors to do the work of initializing the specified
 *  type of frame.
 */
    private void init (int type) {

        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);
        setSize (700, 700);
        setResizable (true);

        if (type == FRAME_WITH_TOOLBAR) {
          JEasyXGraphicsPanel gpanel = new JEasyXGraphicsPanel (BorderLayout.NORTH);
          Container contentPane = getContentPane ();
          contentPane.add (gpanel);
          dl = gpanel.getDisplayList ();
        }
        else {
          JDisplayListPanel dpanel = new JDisplayListPanel ();
          Container contentPane = getContentPane ();
          contentPane.add (dpanel);
          dl = dpanel.getDisplayList ();
        }

        dlist_id = dl.getNativeID ();

        long  nid = 0;

        JDLFrameList.addFrame (nid, this);

    }

    public JDisplayList getDL () {return dl;}

    public int getNativeID () {return dlist_id;};

/*
 * derived class needs to override this method for the
 * object to be useful.
 */
    public void populateDlist () 
    {
    }

}  // end of JDLFrame class