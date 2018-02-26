
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

/**
 *  The CreateNative class is only used for creating native side resources
 *  associated with the display list or the surface calculation code.
 *
 *  This class cannot be instantiated.  
 *  It provides static methods at public scope which in turn call native
 *  methods to create C and C++ resources on the native side.
 *
 *  I (Glenn) isolate these static methods in an attempt to lock the resource
 *  creation in concurrent thread circumstances.  Object locking does not
 *  seem to be sufficient, since the native code that creates resources can
 *  be run by more than one object.  Hopefully, having the native resource
 *  creation triggered only from here, in stativ methods, will allow class
 *  locking to insure that native resource creation does not clobber itself.
 */

public class CreateNative 
{

// private constructor should insure no objects of this class 
// can be created

    private CreateNative () {};

// static method to create display list native resources

    public static synchronized int  createDlistResources ()
    {
        int    istat = -1;

        istat = nativeCreateDlist ();
   
        return istat;
    }

    static private native  int nativeCreateDlist ();



}  // end of CreateNative class
