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

import csw.jutils.src.Bounds3D;
import csw.jutils.src.XYZPolyline;




public class TestBounds3D {



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



// Test expand of a "normal and sane" bounds 3d
// constructed from min and max values.
    @Test
    public void testB3DExpand1() {
        Bounds3D  b3d = new Bounds3D (0.0, 0.0, 0.0, 200.0, 100.0, 100.0);
        double  epx = 5.0;
        double  epy = 10.0;
        double  epz = 10.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        epz /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 200.0 * (1. + epx);
        h2 = 100.0 * (1. + epy);
        d2 = 100.0 * (1. + epz);
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getDepth();
        assertEquals(w2, w, 0.001);
        assertEquals(h2, h, 0.001);
        assertEquals(d2, d, 0.001);
    }

    @Test
    public void testB3DExpand111() {
        Bounds3D  b3d = new Bounds3D (0.0, 0.0, 0.0, 200.0, 100.0, 100.0);
        double  epx = 5.0;
        double  epy = -2000.0;
        double  epz = 10.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        epz /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 200.0 * (1. + epx);
        h2 = 100.0 * (1. + epy);
        d2 = 100.0 * (1. + epz);
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getDepth();
        assertEquals(w2, w, 0.001);
        assertEquals(h2, h, 0.01);
        assertEquals(d2, d, 0.001);
    }

// Test expand where width, height and depth of Bounds3D are zero.
    @Test
    public void testB3DExpand2() {
        Bounds3D  b3d = new Bounds3D (200.0, 100.0, 0.0, 200.0, 100.0, 0.0);
        double  epx = 5.0;
        double  epy = 10.0;
        double  epz = 10.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        epz /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 0.0;
        h2 = 0.0;
        d2 = 0.0;
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getDepth();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
        assertEquals(d2, d, 0.01);
    }

// Test Bounds3D expand where width is zero and height, depth are "sane"
    @Test
    public void testB3DExpand3() {
        Bounds3D  b3d = new Bounds3D (200.0, 0.0, 0.0, 200.0, 100.0, 100.0);
        double  epx = 5.0;
        double  epy = -10.0;
        double  epz = -10.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        epz /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 0.0;
        h2 = 100.0 * (1. + epy);
        d2 = 100.0 * (1. + epz);
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getDepth();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
        assertEquals(d2, d, 0.01);
    }

// Test Bounds3D expand where width, depth are "sane" and height is zero
    @Test
    public void testB3DExpand4() {
        Bounds3D  b3d = new Bounds3D (0.0, 100.0, 0.0, 200.0, 100.0, 100.0);
        double  epx = 5.0;
        double  epy = -10.0;
        double  epz = 10.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        epz /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 200.0 * (1. + epx);
        h2 = 0.0;
        d2 = 100.0 * (1. + epz);
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getDepth();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
        assertEquals(d2, d, 0.01);
    }




// Test expand when constructed from sane arrays
    @Test
    public void testB3DArrayExpand5() {
        double[] xa, ya, za;
        xa = new double[5];
        ya = new double[5];
        za = new double[5];
        for (int i=0; i<5; i++) {
            xa[i] = (double) i;
            ya[i] = (double) i;
            za[i] = (double) i;
        } 
        Bounds3D  b3d;
        try {
            b3d = new Bounds3D (xa, ya, za);
        }
        catch (IllegalArgumentException ex) {
            Assert.fail(ex.toString());
            return;
        }
        double  epx = 5.0;
        double  epy = -10.0;
        double  epz = 10.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        epz /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 4.0 * (1. + epx);
        h2 = 4.0 * (1. + epy);
        d2 = 4.0 * (1. + epz);
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getDepth();
        assertEquals(w2, w, 0.001);
        assertEquals(h2, h, 0.001);
        assertEquals(d2, d, 0.001);
    }

// Test expand when constructed from arrays only varying in x
    @Test
    public void testB3DArrayExpand6() {
        double[] xa, ya, za;
        xa = new double[5];
        ya = new double[5];
        za = new double[5];
        for (int i=0; i<5; i++) {
            xa[i] = (double) i;
            ya[i] = 2.0;
            za[i] = 2.0;
        } 
        Bounds3D  b3d;
        try {
            b3d = new Bounds3D (xa, ya, za);
        }
        catch (IllegalArgumentException ex) {
            Assert.fail(ex.toString());
            return;
        }
        double  epx = 5.0;
        double  epy = -10.0;
        double  epz = -10.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        epz /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 4.0 * (1. + epx);
        h2 = 0.0;
        d2 = 0.0;
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getDepth();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
        assertEquals(d2, d, 0.01);
    }

// Test expand when constructed from arrays only varying in y
    @Test
    public void testB3DArrayExpand7() {
        double[] xa, ya, za;
        xa = new double[5];
        ya = new double[5];
        za = new double[5];
        for (int i=0; i<5; i++) {
            xa[i] = 2.0;
            ya[i] = (double) i;
            za[i] = 2.0;
        } 
        Bounds3D  b3d;
        try {
            b3d = new Bounds3D (xa, ya, za);
        }
        catch (IllegalArgumentException ex) {
            Assert.fail(ex.toString());
            return;
        }
        double  epx = 5.0;
        double  epy = -10.0;
        double  epz = -10.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        epz /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 0.0;
        h2 = 4.0 * (1. + epy);
        d2 = 0.0;
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getDepth();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
        assertEquals(d2, d, 0.01);
    }


// Test different array lengths
    @Test
    public void testB3DArrayLengths() {
        double[] xa, ya, za;
        xa = new double[4];
        ya = new double[5];
        za = new double[5];
        for (int i=0; i<4; i++) {
            xa[i] = 2.0;
        } 
        for (int i=0; i<5; i++) {
            ya[i] = 2.0;
            za[i] = 2.0;
        } 
        Bounds3D  b3d = null;
        try {
            b3d = new Bounds3D (xa, ya, za);
        }
        catch (IllegalArgumentException ex) {
            if (b3d == null) return;
        }
        Assert.fail();
    }

// Test expand when constructed from null arrays
    @Test
    public void testB3DNullArrays() {
        Bounds3D  b3d = null;
        try {
            b3d = new Bounds3D (null, null, null);
        }
        catch (IllegalArgumentException ex) {
            if (b3d == null) return;
        }
        Assert.fail();
    }


// Test expand when constructed from "sane" XYZPolyline
    @Test
    public void testB3DPolyline1() {
        double[] xa, ya, za;
        xa = new double[5];
        ya = new double[5];
        za = new double[5];
        for (int i=0; i<5; i++) {
            xa[i] = (double) i;
            ya[i] = (double) i;
            za[i] = (double) i;
        } 
        XYZPolyline  xyzp = new XYZPolyline (xa, ya, za);

        Bounds3D  b3d;
        try {
            b3d = new Bounds3D (xyzp);
        }
        catch (IllegalArgumentException ex) {
            Assert.fail(ex.toString());
            return;
        }
        double  epx = 5.0;
        double  epy = 10.0;
        double  epz = -8.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        epz /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 4.0 * (1. + epx);
        h2 = 4.0 * (1. + epy);
        d2 = 4.0 * (1. + epz);
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getDepth();
        assertEquals(w2, w, 0.001);
        assertEquals(h2, h, 0.001);
        assertEquals(d2, d, 0.001);
    }

// Test expand when constructed from polyline only varying in x
    @Test
    public void testB3DPolyline2() {
        double[] xa, ya, za;
        xa = new double[5];
        ya = new double[5];
        za = new double[5];
        for (int i=0; i<5; i++) {
            xa[i] = (double) i;
            ya[i] = 2.0;
            za[i] = 2.0;
        } 
        XYZPolyline  xyzp = new XYZPolyline (xa, ya, za);

        Bounds3D  b3d;
        try {
            b3d = new Bounds3D (xyzp);
        }
        catch (IllegalArgumentException ex) {
            Assert.fail(ex.toString());
            return;
        }
        double  epx = 5.0;
        double  epy = -10.0;
        double  epz = 12.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        epz /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 4.0 * (1. + epx);
        h2 = 0.0;
        d2 = 0.0;
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getDepth();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
        assertEquals(d2, d, 0.01);
    }

// Test expand when constructed from polyline only varying in y
    @Test
    public void testB3DPolyline3() {
        double[] xa, ya, za;
        xa = new double[5];
        ya = new double[5];
        za = new double[5];
        for (int i=0; i<5; i++) {
            xa[i] = 2.0;
            ya[i] = (double) i;
            za[i] = 2.0;
        } 
        XYZPolyline  xyzp = new XYZPolyline (xa, ya, za);

        Bounds3D  b3d;
        try {
            b3d = new Bounds3D (xyzp);
        }
        catch (IllegalArgumentException ex) {
            Assert.fail(ex.toString());
            return;
        }
        double  epx = 5.0;
        double  epy = -10.0;
        double  epz = -9.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        epz /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 0.0;
        h2 = 4.0 * (1. + epy);
        d2 = 0.0;
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getDepth();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
        assertEquals(d2, d, 0.01);
    }

// Test when constructed from null polyline
    @Test
    public void testB3DNullPolyline() {
        Bounds3D  b3d = null;
        try {
            b3d = new Bounds3D ((XYZPolyline) null);
        }
        catch (IllegalArgumentException ex) {
            if (b3d == null) return;
        }
        Assert.fail();
    }


// Test expand when constructed from polyline all at same point
    @Test
    public void testB3DPolyline4() {
        double[] xa, ya, za;
        xa = new double[5];
        ya = new double[5];
        za = new double[5];
        for (int i=0; i<5; i++) {
            xa[i] = 2.0;
            ya[i] = 2.0;
            za[i] = 2.0;
        } 
        XYZPolyline  xyzp = new XYZPolyline (xa, ya, za);

        Bounds3D  b3d;
        try {
            b3d = new Bounds3D (xyzp);
        }
        catch (IllegalArgumentException ex) {
            Assert.fail(ex.toString());
            return;
        }
        double  epx = 5.0;
        double  epy = -10.0;
        double  epz = -10.0;
        b3d.expandByPercentage (epx, epy, epz);
        epx /= 100.0;
        epy /= 100.0;
        double  w, h, d, w2, h2, d2;
        w2 = 0.0;
        h2 = 0.0;
        d2 = 0.0;
        w = b3d.getWidth();
        h = b3d.getHeight();
        d = b3d.getHeight();
        assertEquals(w2, w, 0.01);
        assertEquals(h2, h, 0.01);
        assertEquals(d2, d, 0.01);
    }

}

