
package csw.jutils.junit_tests;

import org.junit.runner.JUnitCore;


public class TestRunner {

    public static void main (String[] args) {
        JUnitCore.runClasses (csw.jutils.junit_tests.TestBounds2D.class);
        JUnitCore.runClasses (csw.jutils.junit_tests.TestBounds3D.class);
    }


}
