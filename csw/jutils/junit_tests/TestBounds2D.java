/*
 * This class tests some of the jutils package using junit.
 * The results of the tests are logged using log4j2.  The
 * tests are admittedly very trivial.  The primary purpose
 * for writing these simple tests is to learn a bit of junit
 * and a bit of log4j2.
 */

package csw.jutils.junit_tests;

import org.junit.Test;
import org.junit.Assert;
import static org.junit.Assert.assertEquals;

import org.junit.Rule;
import org.junit.rules.TestWatcher;

import jutest.Log4jTestWatcher;


import csw.jutils.src.Bounds2D;


public class TestBounds2D {



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



// Test expand of a "normal and sane" bounds 2d.
    @Test
    public void testB2DExpand1() {
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

// Test expand where both width and height of Bounds2D are zero.
    @Test
    public void testB2DExpand2() {
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

// Test Bounds2D expand where width is zero and height is "sane"
    @Test
    public void testB2DExpand3() {
        Bounds2D  b2d = new Bounds2D (200.0, 0.0, 200.0, 100.0);
        double  epx = 5.0;
        double  epy = -10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 0.0;
        h2 = 100.0 * (1. + epy);
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
    }

// Test Bounds2D expand where width is "sane" and height is zero
    @Test
    public void testB2DExpand4() {
        Bounds2D  b2d = new Bounds2D (0.0, 100.0, 200.0, 100.0);
        double  epx = 5.0;
        double  epy = -10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 200.0 * (1. + epx);
        h2 = 0.0;
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
    }


// Test expand when constructed from sane arrays
    @Test
    public void testB2DArrayExpand5() {
        double[] xa, ya;
        xa = new double[5];
        ya = new double[5];
        for (int i=0; i<5; i++) {
            xa[i] = (double) i;
            ya[i] = (double) i;
        } 
        Bounds2D  b2d;
        try {
            b2d = new Bounds2D (xa, ya);
        }
        catch (IllegalArgumentException ex) {
            Assert.fail(ex.toString());
            return;
        }
        double  epx = 5.0;
        double  epy = -10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 4.0 * (1. + epx);
        h2 = 4.0 * (1. + epy);
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.001);
        assertEquals(h2, h, 0.001);
    }

// Test expand when constructed from arrays only varying in x
    @Test
    public void testB2DArrayExpand6() {
        double[] xa, ya;
        xa = new double[5];
        ya = new double[5];
        for (int i=0; i<5; i++) {
            xa[i] = (double) i;
            ya[i] = 2.0;
        } 
        Bounds2D  b2d;
        try {
            b2d = new Bounds2D (xa, ya);
        }
        catch (IllegalArgumentException ex) {
            Assert.fail(ex.toString());
            return;
        }
        double  epx = 5.0;
        double  epy = -10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 4.0 * (1. + epx);
        h2 = 0.0;
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
    }

// Test expand when constructed from arrays only varying in y
    @Test
    public void testB2DArrayExpand7() {
        double[] xa, ya;
        xa = new double[5];
        ya = new double[5];
        for (int i=0; i<5; i++) {
            xa[i] = 2.0;
            ya[i] = (double) i;
        } 
        Bounds2D  b2d;
        try {
            b2d = new Bounds2D (xa, ya);
        }
        catch (IllegalArgumentException ex) {
            Assert.fail(ex.toString());
            return;
        }
        double  epx = 5.0;
        double  epy = -10.0;
        b2d.expandByPercentage (epx, epy);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, w2, h2;
        w2 = 0.0;
        h2 = 4.0 * (1. + epy);
        w = b2d.getWidth();
        h = b2d.getHeight();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
    }

// Test different array lengths
    @Test
    public void testB2DArrayLengths() {
        double[] xa, ya;
        xa = new double[4];
        ya = new double[5];
        for (int i=0; i<4; i++) {
            xa[i] = 2.0;
        } 
        for (int i=0; i<5; i++) {
            ya[i] = 2.0;
        } 
        Bounds2D  b2d;
        try {
            b2d = new Bounds2D (xa, ya);
        }
        catch (IllegalArgumentException ex) {
            return;
        }
        Assert.fail();
    }

// Test expand when constructed from null arrays
    @Test
    public void testB2DNullArrays() {
        Bounds2D  b2d;
        try {
            b2d = new Bounds2D (null, null);
        }
        catch (IllegalArgumentException ex) {
            return;
        }
        Assert.fail();
    }

}
