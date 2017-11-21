/*
 */

package csw.j3d.src;

/**
 This class is used to scale world points to java3d coordinates.
 All of the methods are package scope.  This is only used by other
 classes in the csw.j3d.src package.
 */
class Scaler
{

    double xExag;
    double yExag;
    double zExag;
    double cScale;
    double xCenter;
    double yCenter;
    double zCenter;

    Scaler ()
    {
        xExag = 1.0;
        yExag = 1.0;
        zExag = 1.0;
        cScale = 1.0;
        xCenter = 0.0;
        yCenter = 0.0;
        zCenter = 0.0;
    }

/*--------------------------------------------------------------------------*/

    void setLimits (double xmin, double ymin, double zmin,
                    double xmax, double ymax, double zmax,
                    double raw_z_exag)
    {
        double               x_exag,
                             y_exag,
                             z_exag,
                             cscale,
                             xcenter,
                             ycenter,
                             zcenter;
        double               ct, xt, yt, zt, dx, dy, dz;
        int                  i;

        if (raw_z_exag < 0.01) {
            raw_z_exag = 0.01;
        }

        xt = xmax - xmin;
        yt = ymax - ymin;
        zt = zmax - zmin;

        ct = xt;
        if (yt > ct) ct = yt;
        if (zt > ct) ct = zt;

        if (ct <= 0.0) {
            return;
        }

        cscale = ct / 2.0;
        xcenter = (xmin + xmax) / 2.0;
        ycenter = (ymin + ymax) / 2.0;
        zcenter = (zmin + zmax) / 2.0;

    /*
     * Apply the vertical exageration to either z or attrib,
     * and adjust the x and y scales so the z doesn't go out
     * of the cube.
     */
        x_exag = 1.0;
        y_exag = 1.0;
        z_exag = raw_z_exag;
        if (cscale > 1.e20) {
            return;
        }
        zt = (zmax - zcenter) / cscale * z_exag;
        if (zt > 1.0) {
            z_exag /= zt;
            x_exag /= zt;
            y_exag /= zt;
        }

        if (raw_z_exag < 1.0) {
            cscale *= raw_z_exag;
            cscale *= 1.1;
        }

    /*
     * Put the scaling parameters into instance variables that
     * can be used by the scalePoint method.
     */
        xExag = x_exag;
        yExag = y_exag;
        zExag = z_exag;
        cScale = cscale;
        xCenter = xcenter;
        yCenter = ycenter;
        zCenter = zcenter;

    }

/*--------------------------------------------------------------------------------*/

    JPoint3D scalePoint (double x, double y, double z)
    {
        double             xt, yt, zt;
        JPoint3D            pt;

        xt = (x - xCenter) / cScale;
        yt = (y - yCenter) / cScale;
        zt = (z - zCenter) / cScale;

        xt *= xExag;
        yt *= yExag;
        zt *= zExag;

    /*
     * The sign of y needs to be inverted, since we use
     * y as the opengl z value, and we want positive y
     * into the screen.  This produces a right handed
     * coordinate system when looking down at the 3d
     * view from the top.
     */
        yt = -yt;

        if (z > 1.e20  ||  z < -1.e20) {
            zt = 1.e30;
        }

        pt = new JPoint3D (xt, zt, yt);

        return pt;

    }

/*--------------------------------------------------------------------------------*/

    void scalePoint (double x, double y, double z, JPoint3D pt)
    {
        double             xt, yt, zt;

        xt = (x - xCenter) / cScale;
        yt = (y - yCenter) / cScale;
        zt = (z - zCenter) / cScale;

        xt *= xExag;
        yt *= yExag;
        zt *= zExag;

    /*
     * The sign of y needs to be inverted, since we use
     * y as the opengl z value, and we want positive y
     * into the screen.  This produces a right handed
     * coordinate system when looking down at the 3d
     * view from the top.
     */
        yt = -yt;

        pt.x = xt;
        pt.y = zt;
        pt.z = yt;

        return;

    }

/*--------------------------------------------------------------------------------*/

    JPoint3D scalePoint (float x, float y, float z)
    {
        double             xt, yt, zt;
        JPoint3D            pt;

        xt = (x - xCenter) / cScale;
        yt = (y - yCenter) / cScale;
        zt = (z - zCenter) / cScale;

        xt *= xExag;
        yt *= yExag;
        zt *= zExag;

    /*
     * The sign of y needs to be inverted, since we use
     * y as the opengl z value, and we want positive y
     * into the screen.  This produces a right handed
     * coordinate system when looking down at the 3d
     * view from the top.
     */
        yt = -yt;

        pt = new JPoint3D ((float)xt, (float)zt, (float)yt);

        return pt;

    }

/*--------------------------------------------------------------------------------*/

    JPoint3D unscalePoint (double x, double y, double z)
    {
        double             xt, yt, zt;
        JPoint3D            pt;

        xt = x;
        yt = z;
        zt = y;

        yt = -yt;

        xt /= xExag;
        yt /= yExag;
        zt /= zExag;

        xt = xt * cScale + xCenter;
        yt = yt * cScale + yCenter;
        zt = zt * cScale + zCenter;

        pt = new JPoint3D (xt, yt, zt);

        return pt;

    }

}  // end of class definition
