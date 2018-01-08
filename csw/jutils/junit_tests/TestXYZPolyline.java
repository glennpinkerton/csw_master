/*
 * This class tests the jutils XYZPolyline class using junit.
 * The results of the tests are logged using log4j2.  The
 * tests are admittedly very trivial.  The primary purpose
 * for writing these simple tests is to learn a bit of junit
 * and a bit of log4j2.
 */

package csw.jutils.junit_tests;

import org.junit.Test;
//import org.junit.Assert;
//import static org.junit.Assert.assertEquals;

import org.junit.Rule;
import org.junit.rules.TestWatcher;

import jutest.Log4jTestWatcher;


//import csw.jutils.src.XYZPolyline;


public class TestXYZPolyline {



// The test watcher will use log4j2 to write information
// about each test.  A config file named log4j2.xml needs
// to be created.  For my (Glenn's) test environment I have
// put a simple log4j2.xml file in the CSW_PARENT directory.
// You can use other formats than xml for the config file.
// I leave the further exploration of log4j2 as an exercise
// for anyone who cares to pursue it.


    @Rule
    public TestWatcher watchman = new Log4jTestWatcher("csw_utest");


// The "best practices" for junit encourage a large number of
// simple tests.  None of the simple tests should be dependent 
// on each other, at least in terms of success and failure.
// Also, the TestWatcher rule (as used here) is a test by test watching
// mechanism.  Each @Test annotated method seems to invoke the 
// TestWatcher when it finishes.  This also encourages small, simple
// test methods for which the results of each can be logged.



// Construct with known points and then compare the limits 
// of the constructed object with tghe known limits.
    @Test
    public void testXYZPoly1() {
    }

// Construct with known points and then compare the  
// size and values of the constructed object with the
// known points.
    @Test
    public void testXYZPoly2() {
    }

}
