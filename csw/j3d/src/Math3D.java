
package csw.j3d.src;


/**
 * This class is a collection of static utility functions that do not belong
 * to any class, but may be useful to the inner workings of several
 * of the 3d viewer classes.
 */
public class Math3D
{

    /**
     * Function to intersect two 3d line segments.  The results are the closest
     * points on the two specified lines.  If the closest points on the lines
     * are outside of both segments specified, then 0 is returned.  If the
     * points are on one of the segments, 1 is returned.  If the points are on
     * both of the segments, 2 is returned. In any case where the return value
     * is zero or greater, the close1 and close2 objects still have the line
     * intersection points, regardless of whether they are on the segments.
    <p>
     * This function calls {@link #intersect3DLines} and then checks if the
     * results are on the segment.  If the intersect3DLines returns an error,
     * then this method returns -1.  If -1 is returned, the line intersection
     * points in close1 and close2 will have x, y and z set to 1.e30 values.
     */
    static public int intersect3DLineSegments (
        double x1, double y1, double z1,
        double x2, double y2, double z2,
        JPoint3D near,
        JPoint3D far,
        JPoint3D close1,
        JPoint3D close2)
    {

        int istat =
        intersect3DLines (
            x1, y1, z1,
            x2, y2, z2,
            near, far,
            close1, close2
        );

        if (istat == -1) {
            return -1;
        }

        int n = 0;

    /*
     * check the 1-2 segment
     */
        if ((close1.x - x1) * (x2 - close1.x) >= 0.0  &&
            (close1.y - y1) * (y2 - close1.y) >= 0.0  &&
            (close1.z - z1) * (z2 - close1.z) >= 0.0) {
            n++;
        }

    /*
     * check the near - far segment
     */
        if ((close2.x - near.x) * (far.x - close2.x) >= 0.0  &&
            (close2.y - near.y) * (far.y - close2.y) >= 0.0  &&
            (close2.z - near.z) * (far.z - close2.z) >= 0.0) {
            n++;
        }

        return n;

    }



/*-----------------------------------------------------------------------------------*/



    /**
     * Function to intersect two 3d lines.  The results are the closest
     * points on each line to each other.  The close1 result is on the
     * line from 1 to 2.  The close2 result is on the line from near to far.
     * On success, 1 is returned.  If there is a problem, -1 is returned
     * and the results are filled with 1.e30 values.
     *
     * The algorithm is from Graphics Gems, page 304, "Intersection of
     * Two Lines in 3D Space", by Ronald Goldman.
     */
    static public int intersect3DLines (
        double x1, double y1, double z1,
        double x2, double y2, double z2,
        JPoint3D near,
        JPoint3D far,
        JPoint3D close1,
        JPoint3D close2)
    {

        double             x3, y3, z3;
        double             x4, y4, z4;
        double             dx, dy, dz, dist;
        double             vx1, vy1, vz1,
                           vx2, vy2, vz2;
        double             px1, py1, pz1,
                           px2, py2, pz2;
        double             cpx, cpy, cpz, cpsq;
        double             t, s;
        double             vx, vy, vz, m11, m12, m13;
        double             xt1, yt1, zt1, xt2, yt2, zt2;

    /*
     * Initialize results to "null" values in case of error.
     */
        close1.x = 1.e30;
        close1.y = 1.e30;
        close1.z = 1.e30;

        close2.x = 1.e30;
        close2.y = 1.e30;
        close2.z = 1.e30;

    /*
     * local variables for near and far.
     */
        x3 = near.x;
        y3 = near.y;
        z3 = near.z;

        x4 = far.x;
        y4 = far.y;
        z4 = far.z;

    /*
     * Get the origin point and unit vector for the first ray.
     */
        px1 = x1;
        py1 = y1;
        pz1 = z1;
        dx = x2 - x1;
        dy = y2 - y1;
        dz = z2 - z1;
        dist = dx * dx + dy * dy + dz * dz;
        dist = Math.sqrt (dist);
        if (dist <= 0.0  ||  dist > 1.e15) {
            return -1;
        }
        else {
            vx1 = dx / dist;
            vy1 = dy / dist;
            vz1 = dz / dist;
        }

    /*
     * Get the origin point and unit vector for the second ray.
     */
        px2 = x3;
        py2 = y3;
        pz2 = z3;
        dx = x4 - x3;
        dy = y4 - y3;
        dz = z4 - z3;
        dist = dx * dx + dy * dy + dz * dz;
        dist = Math.sqrt (dist);
        if (dist <= 0.0  ||  dist > 1.e15) {
            return -1;
        }
        else {
            vx2 = dx / dist;
            vy2 = dy / dist;
            vz2 = dz / dist;
        }

    /*
     * Get the cross product of the two unit vectors and
     * the square of the cross product length.
     */
        cpx = vy1 * vz2 - vz1 * vy2;
        cpy = vz1 * vx2 - vx1 * vz2;
        cpz = vx1 * vy2 - vy1 * vx2;
        cpsq = cpx * cpx + cpy * cpy + cpz * cpz;

        if (cpsq <= 0.0  ||  cpsq > 1.e15) {
            return -1;
        }

        dx = px2 - px1;
        dy = py2 - py1;
        dz = pz2 - pz1;

    /*
     * The t value represents the number of unit vectors
     * along the ray from point 1 to point 2 for the
     * intersection.  It is the determinant of the matrix
     * as described in the referenced article.
     */
        vx = vx2;
        vy = vy2;
        vz = vz2;

        m11 = vy * cpz - vz * cpy;
        m12 = dy * cpz - cpy * dz;
        m13 = dy * vz - dz * vy;

        t = dx * m11 - vx * m12 + cpx * m13;

        t /= cpsq;

    /*
     * s is the number of unit vectors along ray from
     * point 3 to point 4.
     */
        vx = vx1;
        vy = vy1;
        vz = vz1;

        m11 = vy * cpz - vz * cpy;
        m12 = dy * cpz - cpy * dz;
        m13 = dy * vz - dz * vy;

        s = dx * m11 - vx * m12 + cpx * m13;

        s /= cpsq;

    /*
     * Use t to get the point on the ray from 1 to 2
     */
        xt1 = px1 + vx1 * t;
        yt1 = py1 + vy1 * t;
        zt1 = pz1 + vz1 * t;

    /*
     * Use s to get the point on the ray from 3 to 4
     */
        xt2 = px2 + vx2 * s;
        yt2 = py2 + vy2 * s;
        zt2 = pz2 + vz2 * s;

        close1.x = xt1;
        close1.y = yt1;
        close1.z = zt1;

        close2.x = xt2;
        close2.y = yt2;
        close2.z = zt2;

        return 1;
    }



/*-----------------------------------------------------------------------------------*/


    /**
     * Find the intersection of a plane and a line.
     * This is from "Intersection of a plane and line" by Paul Bourke,
     * 1991.  astronomy.swin.edu.au/~pbourke/geometry/planeline
     *
     *   tnx, tny, tnz is the normal vector to the plane
     *   tpx, tpy, tpz is a point on the plane.
     *   near is one end point of the line
     *   far is the other end point of the line
     *   result is the intersection point
     *
     * On success, 1 is returned.  On failure, -1 is returned and
     * the result is filled with 1.e30 values.
     */
    static public int intersectPlaneAndLine (double tnx, double tny, double tnz,
                                             double tpx, double tpy, double tpz,
                                             JPoint3D near,
                                             JPoint3D far,
                                             JPoint3D result)
    {
        double          x1, y1, z1, x2, y2, z2;
        double          dot1, dot2, dist, dx, dy, dz,
                        vx31, vy31, vz31, vx21, vy21, vz21,
                        u, xt, yt, zt;

    /*
     * Initialize results in case of error.
     */
        result.x = 1.e30;
        result.y = 1.e30;
        result.z = 1.e30;

    /*
     * local variables for near and far.
     */
        x1 = near.x;
        y1 = near.y;
        z1 = near.z;

        x2 = far.x;
        y2 = far.y;
        z2 = far.z;

    /*
     * Normalize the normal vector to a length of 1
     */
        dist = tnx * tnx + tny * tny + tnz * tnz;
        dist = Math.sqrt (dist);
        if (dist < 1.e-20) {
            return -1;
        }
        tnx /= dist;
        tny /= dist;
        tnz /= dist;

    /*
     * vector between plane point and first line point.
     * (P3 - P1 in the referenced paper)
     */
        vx31 = tpx - x1;
        vy31 = tpy - y1;
        vz31 = tpz - z1;

    /*
     * Vector between the two line points.
     * (P2 - P1 in the referenced paper)
     */
        vx21 = x2 - x1;
        vy21 = y2 - y1;
        vz21 = z2 - z1;

    /*
     * Dot product of plane normal and (P3 - P1)
     */
        dot1 = tnx * vx31 + tny * vy31 + tnz * vz31;

    /*
     * Dot product of plane normal and (P2 - P1)
     */
        dot2 = tnx * vx21 + tny * vy21 + tnz * vz21;

    /*
     * If dot2 is zero, then the line and plane
     * are parallel and no intersection is valid.
     */
        if (dot2 > -1.e-20  &&  dot2 < 1.e-20) {
            return -1;
        }

        u = dot1 / dot2;

        dx = x2 - x1;
        dy = y2 - y1;
        dz = z2 - z1;

        xt = x1 + u * dx;
        yt = y1 + u * dy;
        zt = z1 + u * dz;

        result.x = xt;
        result.y = yt;
        result.z = zt;

        return 1;

    }

}  // end of main class
