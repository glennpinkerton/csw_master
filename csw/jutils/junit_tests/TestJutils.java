/*
 * This class tests some of the jutils package using junit.
 * The results of the tests are logged using log4j2.  The
 * tests are admittedly very trivial.  The primary purpose
 * for writing these simple tests is to learn a bit of junit
 * and a bit of log4j2.
 */

package csw.jutils.junit_tests;

import org.junit.Test;
//import org.junit.Assert;
import static org.junit.Assert.assertEquals;

import org.junit.Rule;
import org.junit.rules.TestWatcher;


import csw.jutils.src.Bounds2D;


public class TestJutils {



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


// The first 4 tests (testExpand1 - testExpand4) modify
// a Bounds\2D object which was sanely created.  The 
// expandByPercentage method is called with positive and
// negative percentages.

    @Test
    public void testExpand1() {
        Bounds2D  b2d = new Bounds2D (0.0, 0.0, 200.0, 100.0);
        double  epx = 5.0;
        double  epy = 10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 200.0 * (1. + epx);
        h2 = 100.0 * (1. + epy);
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.001);
        assertEquals(h2, h, 0.001);
    }

    @Test
    public void testExpand2() {
        Bounds2D  b2d = new Bounds2D (0.0, 0.0, 200.0, 100.0);
        double  epx = -5.0;
        double  epy = 10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 200.0 * (1. + epx);
        h2 = 100.0 * (1. + epy);
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.001);
        assertEquals(h2, h, 0.001);
    }

    @Test
    public void testExpand3() {
        Bounds2D  b2d = new Bounds2D (0.0, 0.0, 200.0, 100.0);
        double  epx = 5.0;
        double  epy = -10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 200.0 * (1. + epx);
        h2 = 100.0 * (1. + epy);
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.001);
        assertEquals(h2, h, 0.001);
    }

    @Test
    public void testExpand4() {
        Bounds2D  b2d = new Bounds2D (0.0, 0.0, 200.0, 100.0);
        double  epx = -5.0;
        double  epy = -10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 200.0 * (1. + epx);
        h2 = 100.0 * (1. + epy);
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.001);
        assertEquals(h2, h, 0.001);
    }


// The next 4 tests (testExpand11 - testExpand14) modify
// a Bounds2D object which was not sanely created.  The 
// expandByPercentage method is called with positive and
// negative percentages.  The insanity in creation is 
// creating a zero area bounds.

    @Test
    public void testExpand11() {
        Bounds2D  b2d = new Bounds2D (200.0, 100.0, 200.0, 100.0);
        double  epx = 5.0;
        double  epy = 10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 0.0;
        h2 = 0.0;
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
    }

    @Test
    public void testExpand12() {
        Bounds2D  b2d = new Bounds2D (200.0, 100.0, 200.0, 100.0);
        double  epx = -5.0;
        double  epy = 10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 0.0;
        h2 = 0.0;
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
    }

    @Test
    public void testExpand13() {
        Bounds2D  b2d = new Bounds2D (200.0, 100.0, 200.0, 100.0);
        double  epx = 5.0;
        double  epy = -10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 0.0;
        h2 = 0.0;
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
    }

    @Test
    public void testExpand14() {
        Bounds2D  b2d = new Bounds2D (200.0, 100.0, 200.0, 100.0);
        double  epx = -5.0;
        double  epy = -10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 0.0;
        h2 = 0.0;
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
    }

}
