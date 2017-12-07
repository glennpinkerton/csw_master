
package jutest;

import org.junit.runner.JUnitCore;


public class RunJutilTests {

    public static void test_run () {
        JUnitCore.runClasses (csw.jutils.junit_tests.TestBounds2D.class);
        JUnitCore.runClasses (csw.jutils.junit_tests.TestBounds3D.class);
    }

}
