package csw.mysql.jtest.src;

import java.lang.Math;

/**
 * The SpatialObject class is a very simple java data store 
 * for the results of queries into the glp database "objects"
 * table.  This is convenient for making java collections of
 * objects selected from the database.
 *
 * This class is package scope.  It is assumed that other classes
 * in the package will avoid obvious errors and thus exception
 * throwing is minimal here.
 */

class SpatialObj {

    long     object_id = 0;
    int      object_type = -1;
    int      layer_id = -1;
    double   xmin = 0.0;
    double   ymin = 0.0;
    double   xmax = 0.0;
    double   ymax = 0.0;
    long     spatial_index = 0;

    SpatialObj (long obid,
                double x1, double y1,
                double x2, double y2)  {

        object_id = obid;
        xmin = Math.min (x1, x2);
        ymin = Math.min (y1, y2);
        xmax = Math.max (x1, x2);
        ymax = Math.max (y1, y2);

    }


    SpatialObj (long obid,
                int obtype,
                int layerid,
                double x1, double y1,
                double x2, double y2)  {
                
        object_id = obid;
        object_type = obtype;
        layer_id = layerid;
        xmin = Math.min (x1, x2);
        ymin = Math.min (y1, y2);
        xmax = Math.max (x1, x2);
        ymax = Math.max (y1, y2);

    }

}
