
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
  This class has static methods to manage the static collection of
  JDLFrame object that are active.  The static methods here provide
  a way to clean up all the native resources (in the C and C++ code)
  used by a frame.  The cleanup can either be done by calling the
  remove... methods or if cleanup is done elsewhere, the JDLFrame
  can be marked as already cleaned up via these methods.

  @author Glenn Pinkerton

*/

class FrStruct {
    long      frame_id;
    JDLFrame  frame_obj;
    FrStruct (long id, JDLFrame obj) {
      frame_id = id;
      frame_obj = obj;
    }
}

public class JDLFrameList {

    private static final long serialVersionUID = 1357L;

    private static ArrayList<FrStruct>  frameIdList = new ArrayList<FrStruct> (8);

    private JDLFrameList () {};

 /*
  * Static methods.
  */
    public static synchronized void addFrame (
        long      frame_id,
        JDLFrame  frame_obj)
    {
        FrStruct  fr = new FrStruct (frame_id, frame_obj);
        int  idx = frameIdList.indexOf(null);
        if (idx >= 0) {
          frameIdList.set (idx, fr);
        }
        else {
          try {
            frameIdList.add (fr);
          }
          catch (Throwable ex) {
            System.out.println ();
            System.out.println ("Exception adding to frameIdList");
            System.out.println ();
            System.out.flush ();
          }
        }
    }


    public static synchronized void markFrameAsDeleted (
        JDisplayList  dlin)
    {
        if (dlin == null) return;

        for (FrStruct fr : frameIdList) {
          if (fr == null) continue;
            if (fr.frame_obj != null) {
              if (fr.frame_obj.getDL() != null) {
                if (dlin == fr.frame_obj.getDL()) {
                  fr.frame_obj = null;
                  int idx = frameIdList.indexOf (fr);
                  if (idx >= 0) {
                      frameIdList.set (idx, null);
                  }
                  break;
                }
              }
            }
        }

    }

    public static synchronized void markFrameAsDeleted (
        JFrame  fin)
    {
        if (fin == null) return;

        if (!(fin instanceof JDLFrame)) {
            return;
        }

        JDLFrame jdfin = (JDLFrame)fin;

        for (FrStruct fr : frameIdList) {
          if (fr == null) continue;
            if (fr.frame_obj != null) {
              if (fr.frame_obj.equals(jdfin)) {
                  fr.frame_obj = null;
                  int idx = frameIdList.indexOf (fr);
                  if (idx >= 0) {
                      frameIdList.set (idx, null);
                  }
                  break;
              }
            }
        }

    }



    public static synchronized void removeFrame (
        long      frame_id,
        JDLFrame  frame_obj)
    {

        if (frame_obj == null) return;

        for (FrStruct fr : frameIdList) {
          if (fr == null) continue;
            if (fr.frame_obj == frame_obj) {
                if (frame_obj.getDL() != null) {
                    fr.frame_obj.getDL().cleanup ();
                }
                fr.frame_obj = null;
                int idx = frameIdList.indexOf (fr);
                if (idx >= 0) {
                    frameIdList.set (idx, null);
                }
                break;
            }
        }
    }

    public static synchronized void removeAllFrames ()
    {

        for (FrStruct fr : frameIdList) {
          if (fr != null) {
            if (fr.frame_obj != null) {
              if (fr.frame_obj.getDL() != null) {
                fr.frame_obj.getDL().cleanup ();
              }
            }
            fr.frame_obj = null;
            fr.frame_id = -1;
          }
        }
        frameIdList.clear ();
    }

}  // end of JDLFrameList class
