/*
 */

package csw.j3d.src;

import java.awt.Color;

import csw.jsurfaceworks.src.TriMesh;

/**
 This class stores information needed to draw a trimesh in the 3d
 display.  The j3d package is the only code that needs access to this
 code so it is declared with package scope.
 */
class TriMeshDraw3D
{


  /*
   * Change this debug flag to true to enable triangle labeling for
   * debugging purposes.
   */
    private static boolean DEBUG_LABEL_TRIANGLES = false;


    double[] nodeWorldXArray,
             nodeWorldYArray,
             nodeWorldZArray;

    private double[] nodeXArray,
                     nodeYArray,
                     nodeZArray;

    private double[] nodeXNormalArray,
                     nodeYNormalArray,
                     nodeZNormalArray;

    private double[] viewXNormalArray,
                     viewYNormalArray,
                     viewZNormalArray;

    private double[] nodeAttributeArray;
    private float[]  nodeRedArray;
    private float[]  nodeGreenArray;
    private float[]  nodeBlueArray;
    private float[]  nodeAlphaArray;

    private boolean  colorByAttribute = false;

    private boolean  doAtColor = false;
    private boolean  directColor = false;

    private int[]    n1TriArray,
                     n2TriArray,
                     n3TriArray;

    private AtColorLookup    colorLookup;

    int              numNodes,
                     numTriangles;

    private Color    globalColor = Color.lightGray;

    private Scaler   scaler;

    private boolean  triMeshAllowed = true;

/*--------------------------------------------------------------------------------*/

  /**
   This constructor takes the current contents of a TriMesh object
   and transfers it to this TriMeshDraw3D object.  If the specified
   tmesh is null, an empty object is created.
   */
    TriMeshDraw3D (TriMesh tmesh)
    {
        setTriMesh (tmesh);
    }


/*--------------------------------------------------------------------------------*/

  /**
  Constructor which explicitly sets the node xyz and triangle node arrays
  in world coordinates.
  */
    TriMeshDraw3D (
      double[] nodex,
      double[] nodey,
      double[] nodez,
      int[]    tri1,
      int[]    tri2,
      int[]    tri3)
    {
        buildXYZ (nodex, nodey, nodez,
                  tri1, tri2, tri3);
    }

    double   xmin = 1.e30;
    double   ymin = 1.e30;
    double   zmin = 1.e30;
    double   xmax = -1.e30;
    double   ymax = -1.e30;
    double   zmax = -1.e30;

    private void buildXYZ (
      double[] nodex,
      double[] nodey,
      double[] nodez,
      int[]    tri1,
      int[]    tri2,
      int[]    tri3)

    {
        int      nnode, nedge, ntri, i, size;

        nodeWorldXArray = null;
        nodeWorldYArray = null;
        nodeWorldZArray = null;

        nodeXArray = null;
        nodeYArray = null;
        nodeZArray = null;

        nodeXNormalArray = null;
        nodeYNormalArray = null;
        nodeZNormalArray = null;

        viewXNormalArray = null;
        viewYNormalArray = null;
        viewZNormalArray = null;

        nodeAttributeArray = null;
        colorLookup = null;
        colorByAttribute = false;
        nodeRedArray = null;
        nodeGreenArray = null;
        nodeBlueArray = null;
        nodeAlphaArray = null;

        doAtColor = false;

        nnode = nodex.length;
        if (nnode < 3) {
            return;
        }

        ntri = tri1.length;
        if (ntri < 1) {
            return;
        }

        for (i=0; i<nnode; i++) {
            if (nodez[i] > 1.e20  ||
                nodez[i] < -1.e20) {
                continue;
            }
            if (nodex[i] < xmin) xmin = nodex[i];
            if (nodey[i] < ymin) ymin = nodey[i];
            if (nodez[i] < zmin) zmin = nodez[i];
            if (nodex[i] > xmax) xmax = nodex[i];
            if (nodey[i] > ymax) ymax = nodey[i];
            if (nodez[i] > zmax) zmax = nodez[i];
        }

    /*
     * Use the original arrays for x y and z of the nodes
     * and allocate space for other things that will be needed later.
     */
        size = nodex.length;
        nodeWorldXArray = nodex;
        nodeWorldYArray = nodey;
        nodeWorldZArray = nodez;

        nodeXArray = new double[size];
        nodeYArray = new double[size];
        nodeZArray = new double[size];
        nodeXNormalArray = new double[size];
        nodeYNormalArray = new double[size];
        nodeZNormalArray = new double[size];
        viewXNormalArray = new double[size];
        viewYNormalArray = new double[size];
        viewZNormalArray = new double[size];

        numNodes = nnode;

    /*
     * Use the original triangle node arrays as well.
     */
        size = tri1.length;
        n1TriArray = tri1;
        n2TriArray = tri2;
        n3TriArray = tri3;

        numTriangles = ntri;

        CalcNormals ();

        triMeshAllowed = false;

    }

/*--------------------------------------------------------------------------------*/

  /**
   Set the internal node and triangle arrays to the TriMesh values.
   If the specified tmesh is null, the object is emptied.
  */
    void setTriMesh (TriMesh tmesh)
    {
        double[] nodex, nodey, nodez;
        int[]    edge1, edge2, tri1, tri2, tri3;
        int      nnode, nedge, ntri, i, size;

        nodeWorldXArray = null;
        nodeWorldYArray = null;
        nodeWorldZArray = null;

        nodeXArray = null;
        nodeYArray = null;
        nodeZArray = null;

        nodeXNormalArray = null;
        nodeYNormalArray = null;
        nodeZNormalArray = null;

        viewXNormalArray = null;
        viewYNormalArray = null;
        viewZNormalArray = null;

        nodeAttributeArray = null;
        colorLookup = null;
        colorByAttribute = false;
        nodeRedArray = null;
        nodeGreenArray = null;
        nodeBlueArray = null;
        nodeAlphaArray = null;

        doAtColor = false;

        if (tmesh == null) {
            return;
        }

    /*
     * Get the nodes, edges, and triangles from the trimesh.
     */
        nodex = tmesh.getNodeXArray ();
        nodey = tmesh.getNodeYArray ();
        nodez = tmesh.getNodeZArray ();

        if (nodex == null  ||  nodey == null  ||  nodez == null) {
            return;
        }

        nnode = tmesh.getNumNodes ();
        if (nnode < 3) {
            return;
        }

        edge1 = tmesh.getEdgeNode0Array ();
        edge2 = tmesh.getEdgeNode1Array ();
        if (edge1 == null  ||  edge2 == null) {
            return;
        }

        nedge = tmesh.getNumEdges ();
        if (nedge < 3) {
            return;
        }

        tri1 = tmesh.getTriangleEdge0Array ();
        tri2 = tmesh.getTriangleEdge1Array ();
        tri3 = tmesh.getTriangleEdge2Array ();
        if (tri1 == null  ||  tri2 == null  || tri3 == null) {
            return;
        }

        ntri = tmesh.getNumTriangles ();
        if (ntri < 1) {
            return;
        }

    /*
     * Use the trimesh node arrays directly and allocate
     * arrays for scaled nodes and normals.
     */
        size = nodex.length;
        nodeWorldXArray = nodex;
        nodeWorldYArray = nodey;
        nodeWorldZArray = nodez;

        nodeXArray = new double[size];
        nodeYArray = new double[size];
        nodeZArray = new double[size];
        nodeXNormalArray = new double[size];
        nodeYNormalArray = new double[size];
        nodeZNormalArray = new double[size];
        viewXNormalArray = new double[size];
        viewYNormalArray = new double[size];
        viewZNormalArray = new double[size];

        numNodes = nnode;

    /*
     * Fill in the triangle node arrays.
     */
        size = tri1.length;
        n1TriArray = new int[size];
        n2TriArray = new int[size];
        n3TriArray = new int[size];

        int e1, e2, e3, n1, n2, node1, node2, node3;

        for (i=0; i<ntri; i++) {
            e1 = tri1[i];
            e2 = tri2[i];
            e3 = tri3[i];
            node1 = edge1[e1];
            node2 = edge2[e1];
            n1 = edge1[e2];
            n2 = edge2[e2];
            if (node1 == n1  ||  node2 == n1) {
                node3 = n2;
            }
            else {
                node3 = n1;
            }
            n1TriArray[i] = node1;
            n2TriArray[i] = node2;
            n3TriArray[i] = node3;
        }

        numTriangles = ntri;

        CalcNormals ();

    }

/*--------------------------------------------------------------------------------*/

    void setScaler (Scaler s)
    {
        scaler = s;
    }

/*--------------------------------------------------------------------------------*/

    void setColor (Color color)
    {
        globalColor = color;
    }

/*--------------------------------------------------------------------------------*/

    Color getColor ()
    {
        return globalColor;
    }

/*--------------------------------------------------------------------------------*/

    void setNodeAttributeArray (double[] atValues)
    {
        nodeAttributeArray = atValues;
    }

/*--------------------------------------------------------------------------------*/

    double[] getNodeAttributeArray ()
    {
        return nodeAttributeArray;
    }

/*--------------------------------------------------------------------------------*/

    void setColorByAttribute (boolean atColor)
    {
        colorByAttribute = atColor;
    }

/*--------------------------------------------------------------------------------*/

    boolean getColorByAttribute ()
    {
        return colorByAttribute;
    }

/*--------------------------------------------------------------------------------*/

    void setColorLookup (AtColorLookup atLookup)
    {
        colorLookup = atLookup;
    }

/*--------------------------------------------------------------------------------*/

    AtColorLookup getColorLookup ()
    {
        return colorLookup;
    }

/*--------------------------------------------------------------------------------*/

    static int count;
    int setAttributeColorsDirectly (
        float[] ra,
        float[] ga,
        float[] ba,
        float[] aa,
        int     n_nodes)
    {
        if (nodeWorldXArray == null) {
          throw new IllegalStateException(
            "Attribute colors are set before nodes have been set"+
            "in setting colors directly.");
        }

        int num_nodes = nodeWorldXArray.length;

        if (ra != null) {
            if (n_nodes != num_nodes) {
              throw new IllegalStateException(
                "The number of trimesh nodes differs from "+
                "the number of colors set directly.");
            }
        }

        nodeRedArray = ra;
        nodeGreenArray = ga;
        nodeBlueArray = ba;
        nodeAlphaArray = aa;

        directColor = false;
        if (ra != null  &&
            ga != null  &&
            ba != null  &&
            aa != null) {
            directColor = true;
        }

        return 1;

    }



/*--------------------------------------------------------------------------------*/

/**
 * This must be called from somewhere in the J3D package to get color by attribute
 * on the trimesh.  The idea is that a new color lookup, attribute value and color
 * by attribute flag can be set, or any combination of these can be set by an
 * outside class.  Once these are set, the actual colors at nodes can be updated.
 * It make no sense to update the colors at nodes whenever any one of the above
 * mentioned members is set, since all 3 are needed to define a new attribute
 * coloring scheme.  So, I leave it up to the class that is drawing the trimesh
 * to update the colors at an appropriate time.
 */
    void updateAttributeColors ()
    {

        if (directColor == true) {
            return;
        }

    /*
     * If any of the members needed to define attribute colors is missing,
     * set the flag to false and the arrays to null.
     */
        if (colorByAttribute == false  ||
            nodeAttributeArray == null  ||
            colorLookup == null) {
            doAtColor = false;
            nodeRedArray = null;
            nodeGreenArray = null;
            nodeBlueArray = null;
            nodeAlphaArray = null;
            return;
        }

    /*
     * The state is acceptable for calculating attribute colors.
     * Fill in the red, green, blue and alpha node color arrays
     * with values from 0 to 1.0.  If a value is less than zero,
     * the node is not to be colored.
     */
        if (nodeWorldXArray == null) {
            return;
        }

        int        num_nodes, i;
        double     zt;
        JColor4D   clrd;
        Color      color;

        num_nodes = nodeWorldXArray.length;

        nodeRedArray = new float[num_nodes];
        nodeGreenArray = new float[num_nodes];
        nodeBlueArray = new float[num_nodes];
        nodeAlphaArray = new float[num_nodes];

        for (i=0; i<num_nodes; i++) {
            zt = nodeAttributeArray[i];
            color = colorLookup.lookupColor (zt);
            if (color == null) {
                nodeRedArray[i] = -1.0f;
                nodeGreenArray[i] = -1.0f;
                nodeBlueArray[i] = -1.0f;
                nodeAlphaArray[i] = -1.0f;
            }
            else {
                clrd = new JColor4D (color);
                nodeRedArray[i] = (float)clrd.r;
                nodeGreenArray[i] = (float)clrd.g;
                nodeBlueArray[i] = (float)clrd.b;
                nodeAlphaArray[i] = (float)clrd.a;
            }
        }

        doAtColor = true;

        return;

    }

/*--------------------------------------------------------------------------------*/

    /*
     * Calculate the surface normal at each triangle and
     * the average normal at each node.
     */
    private double    tnxNorm, tnyNorm, tnzNorm;

    private void CalcNormals()
    {
        int          i, n1, n2, n3,
                     ntmp;
        int[]        count;
        double[]     fx, fy, fz;
        double[]     x, y, z;
        double[]     tnxlist, tnylist, tnzlist;
        double       fc;

        x = new double[3];
        y = new double[3];
        z = new double[3];

        tnxlist = new double[numTriangles];
        tnylist = new double[numTriangles];
        tnzlist = new double[numTriangles];

    /*
     * First, calculate normals in the + z direction
     * for all triangles.
     */
        for (i=0; i<numTriangles; i++) {
            n1 = n1TriArray[i];
            n2 = n2TriArray[i];
            n3 = n3TriArray[i];
            x[0] = nodeWorldXArray[n1];
            y[0] = nodeWorldYArray[n1];
            z[0] = nodeWorldZArray[n1];
            x[1] = nodeWorldXArray[n2];
            y[1] = nodeWorldYArray[n2];
            z[1] = nodeWorldZArray[n2];
            x[2] = nodeWorldXArray[n3];
            y[2] = nodeWorldYArray[n3];
            z[2] = nodeWorldZArray[n3];
            TriangleNormal (x, y, z);
            tnxlist[i] = tnxNorm;
            tnylist[i] = tnyNorm;
            tnzlist[i] = tnzNorm;
        }

    /*
     * Allocate temporary space for averaging
     * normals at nodes.
     */
        fx = new double[numNodes];
        fy = new double[numNodes];
        fz = new double[numNodes];
        count = new int[numNodes];

    /*
     * Sum the count and normals arrays whenever a node
     * is used in a triangle.
     */
        for (i=0; i<numTriangles; i++) {
            ntmp = n1TriArray[i];
            if (ntmp >= 0  &&  ntmp < numNodes) {
                fx[ntmp] += tnxlist[i];
                fy[ntmp] += tnylist[i];
                fz[ntmp] += tnzlist[i];
                count[ntmp]++;
            }
            ntmp = n2TriArray[i];
            if (ntmp >= 0  &&  ntmp < numNodes) {
                fx[ntmp] += tnxlist[i];
                fy[ntmp] += tnylist[i];
                fz[ntmp] += tnzlist[i];
                count[ntmp]++;
            }
            ntmp = n3TriArray[i];
            if (ntmp >= 0  &&  ntmp < numNodes) {
                fx[ntmp] += tnxlist[i];
                fy[ntmp] += tnylist[i];
                fz[ntmp] += tnzlist[i];
                count[ntmp]++;
            }
        }

    /*
     * Calculate averages at each node that is used in
     * at least one triangle.
     */
        double    px, py, pz, dist;

        for (i=0; i<numNodes; i++) {
            fc = (double)count[i];
            if (fc > 0.0) {
                px = fx[i] / fc;
                py = fy[i] / fc;
                pz = fz[i] / fc;

                dist = px * px + py * py + pz * pz;
                dist = Math.sqrt (dist);

                if (dist <= 1.e-30) {
                    nodeXNormalArray[i] = 0.0;
                    nodeYNormalArray[i] = 0.0;
                    nodeZNormalArray[i] = 1.0;
                }
                else {
                    nodeXNormalArray[i] = px / dist;
                    nodeYNormalArray[i] = py / dist;
                    nodeZNormalArray[i] = pz / dist;
                }

            }
            else {
                nodeXNormalArray[i] = 0.0;
                nodeYNormalArray[i] = 0.0;
                nodeZNormalArray[i] = 1.0;
            }
            viewXNormalArray[i] = nodeXNormalArray[i];
            viewYNormalArray[i] = nodeZNormalArray[i];
            viewZNormalArray[i] = nodeYNormalArray[i];
        }

        return;

    }

/*--------------------------------------------------------------------------------*/

    /*
     * Calculate the normal vector to the specified triangle.
     * The normal will always have zero or positive z.  The
     * normal is scaled so that the length is 1.0.
     *
     * The components of the normal are put into the private
     * instance variables tnxNorm, tnyNorm and tnzNorm.
     */
    private void TriangleNormal (double[] x, double[] y, double[] z)
    {
        double    x1, y1t, z1, x2, y2, z2,
                  px, py, pz;
        double    dist;

        x1 = x[1] - x[0];
        y1t = y[1] - y[0];
        z1 = z[1] - z[0];
        x2 = x[2] - x[0];
        y2 = y[2] - y[0];
        z2 = z[2] - z[0];

        px = y1t * z2 - z1 * y2;
        py = z1 * x2 - x1 * z2;
        pz = x1 * y2 - y1t * x2;

      /*
       * If the z component of the normal points down, the triangle
       * is clockwise when viewed from above, and it needs to be
       * changed to ccw.
       */
        if (pz < 0.0) {
            px = -px;
            py = -py;
            pz = -pz;
        }

        dist = px * px + py * py + pz * pz;
        dist = Math.sqrt (dist);

        if (dist <= 1.e-30) {
            tnxNorm = 0.0;
            tnyNorm = 0.0;
            tnzNorm = 1.0;
        }
        else {
            tnxNorm = px / dist;
            tnyNorm = py / dist;
            tnzNorm = pz / dist;
            if (tnzNorm < 0.1) {
                if (tnxNorm < 0.0) {
                    tnxNorm = -tnxNorm;
                    tnyNorm = -tnyNorm;
                    tnzNorm = -tnzNorm;
                    if (tnxNorm < 0.1) {
                        if (tnyNorm < 0.0) {
                            tnxNorm = -tnxNorm;
                            tnyNorm = -tnyNorm;
                            tnzNorm = -tnzNorm;
                        }
                    }
                }
            }
        }

        return;

    }

/*------------------------------------------------------------------------*/

    private int      wireList,
                     solidList;
    private int      lastList = 0;

/*-----------------------------------------------------------------------------*/

    void setLists (int wire, int solid)
    {
        wireList = wire;
        solidList = solid;
    }

    int getLastList ()
    {
        return lastList;
    }

/*-----------------------------------------------------------------------------*/

    private double           last_x_exag = 1.e30;
    private double           last_y_exag = 1.e30;
    private double           last_z_exag = 1.e30;

    private boolean          labelFlag = false;

    void setLabelFlag (boolean bval)
    {
        labelFlag = bval;
    }

    void scaleAndDrawTriangles (
                              double pretty_xmin,
                              double pretty_ymin,
                              double pretty_zmin,
                              double pretty_xmax,
                              double pretty_ymax,
                              double pretty_zmax,
                              double raw_z_exag,
                              Color  gColor,
                              boolean solidFlag,
                              J3DPanel panel,
                              JGL jgl
                             )
    {
        double               x_exag,
                             y_exag,
                             z_exag,
                             cscale,
                             xcenter,
                             ycenter,
                             zcenter;
        double               ct, xt, yt, zt, dx, dy, dz;
        double               xpct, ypct, zpct;
        int                  i;
        double               drawRed, drawGreen, drawBlue, drawAlpha;

        if (gColor == null) {
            return;
        }

        drawRed = gColor.getRed();
        drawGreen = gColor.getGreen();
        drawBlue = gColor.getBlue();
        drawAlpha = gColor.getAlpha();
        drawRed /= 255.0;
        drawGreen /= 255.0;
        drawBlue /= 255.0;
        drawAlpha /= 255.0;
        if (drawRed < 0.0) drawRed = 0.0;
        if (drawGreen < 0.0) drawGreen = 0.0;
        if (drawBlue < 0.0) drawBlue = 0.0;
        if (drawAlpha < 0.0) drawAlpha = 0.0;
        if (drawRed > 1.0) drawRed = 1.0;
        if (drawGreen > 1.0) drawGreen = 1.0;
        if (drawBlue > 1.0) drawBlue = 1.0;
        if (drawAlpha > 1.0) drawAlpha = 1.0;

        xt = pretty_xmax - pretty_xmin;
        yt = pretty_ymax - pretty_ymin;
        zt = pretty_zmax - pretty_zmin;

        ct = xt;
        if (yt > ct) ct = yt;
        if (zt > ct) ct = zt;

        if (ct <= 0.0) {
            return;
        }

        cscale = ct / 2.0;
        xcenter = (pretty_xmin + pretty_xmax) / 2.0;
        ycenter = (pretty_ymin + pretty_ymax) / 2.0;
        zcenter = (pretty_zmin + pretty_zmax) / 2.0;

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
        zt = (pretty_zmax - zcenter) / cscale * z_exag;
        if (zt > 1.0) {
            z_exag /= zt;
            x_exag /= zt;
            y_exag /= zt;
        }

    /*
     *  If any of the exaggerations have changed by more than .1 percent,
     *  update the scaled xyz node arrays.
     */
        dx = last_x_exag - x_exag;
        dy = last_y_exag - y_exag;
        dz = last_z_exag - z_exag;
        if (dx < 0.0) dx = -dx;
        if (dy < 0.0) dy = -dy;
        if (dz < 0.0) dz = -dz;
        xpct = x_exag * .001;
        ypct = y_exag * .001;
        zpct = z_exag * .001;

        if (dx > xpct  ||  dy > ypct  ||  dz > zpct) {
            last_x_exag = x_exag;
            last_y_exag = y_exag;
            last_z_exag = z_exag;
            updateViewNodes ();
        }

    /*
     * If any of the exagerations have changed more than 10 percent from
     * the previous values, the view normal vectors for the trimesh need
     * to be updated as well.
     */
        xpct = x_exag * 0.1;
        ypct = y_exag * 0.1;
        zpct = z_exag * 0.1;
        if (dx > xpct  ||  dy > ypct  ||  dz > zpct) {
            last_x_exag = x_exag;
            last_y_exag = y_exag;
            last_z_exag = z_exag;
            updateViewNormals (x_exag,
                               y_exag,
                               z_exag);
        }
        else {
            x_exag = last_x_exag;
            y_exag = last_y_exag;
            z_exag = last_z_exag;
        }

    /*
     * Set if color by attribute is to be done.
     */
        boolean          acolor;

        acolor = false;
        if (doAtColor == true  &&
            nodeRedArray != null  &&
            nodeGreenArray != null  &&
            nodeBlueArray != null  &&
            nodeAlphaArray != null)
        {
            acolor = true;
        }

        if (directColor == true) {
            acolor = true;
        }

    /*
     * Set up for triangle drawing.
     */
        Color            color;
        int              n1, n2, n3;
        JPoint3D         pt, pt1, pt2, pt3;
        int              nstart = 0;
        int              ntri = 0;

        panel.pushName (J3DConst.TRIANGLES_NAME);
        panel.pushName (0);

        int isolid = 1;
        if (solidFlag == false) {
            isolid = 0;
        }

    /*
     * Use the panel selectionMode flag to determine if the panel push, load and
     * pop names methods should be called.  The selection mode also skips some aspects
     * of "drawing" not needed for picking.
     */
        boolean selectionMode = panel.selectionMode;

    /*
     * Load the single solid color once for all triangles.
     */
        if (selectionMode == false) {
            jgl.DGNodeColor (drawRed,
                             drawGreen,
                             drawBlue,
                             drawAlpha);
        }
        else {
            jgl.DGNodeColor (drawRed,
                             drawGreen,
                             drawBlue,
                             1.0);
        }

    /*
     * If there is no display list to run for this surface, compile a new
     * display list and then run it.  If the "drawing" is in selectionMode
     * then no display lists are created.
     */
        int ilist;
        int dlist = solidList;
        if (solidFlag == false) {
            dlist = wireList;
        }

        if (dlist < 0  ||  selectionMode == true) {
            ilist = -dlist;
            if (selectionMode == false) {
                jgl.NewList (ilist); // uncomment to put back dlists
            }

        /*
         * The stop and start draw mode is applied to the entire trimesh
         * if the selection mode is false.  This is done to speed up
         * drawing.
         */
            if (selectionMode == false) {
                jgl.StartDrawMode ();
            }
            else {
                acolor = false;
            }

//System.out.println ("acolor at start of numtriangles loop = "+acolor);

        /*
         * Draw the individual triangles one at a time.
         */
int ndump = 0;
            for (i=0; i<numTriangles; i++) {

                if (selectionMode) {
                    panel.loadName (i);
                    panel.pushName (isolid);
                }

            /*
             * Get the node numbers for this triangle.
             */
                n1 = n1TriArray[i];
                n2 = n2TriArray[i];
                n3 = n3TriArray[i];

                if (nodeWorldZArray[n1] > 1.e20  ||
                    nodeWorldZArray[n1] < -1.e20) {
                    continue;
                }
                if (nodeWorldZArray[n2] > 1.e20  ||
                    nodeWorldZArray[n2] < -1.e20) {
                    continue;
                }
                if (nodeWorldZArray[n3] > 1.e20  ||
                    nodeWorldZArray[n3] < -1.e20) {
                    continue;
                }

                if (nodeYArray[n1] > 1.e20  ||
                    nodeYArray[n1] < -1.e20) {
                    continue;
                }
                if (nodeYArray[n2] > 1.e20  ||
                    nodeYArray[n2] < -1.e20) {
                    continue;
                }
                if (nodeYArray[n3] > 1.e20  ||
                    nodeYArray[n3] < -1.e20) {
                    continue;
                }

            /*
             * If the color is being done
             * based on an attribute value, and that attribute value
             * has no color, the triangle is skipped.
             */
                if (acolor) {
                    if (nodeRedArray[n1] < 0.0  ||
                        nodeRedArray[n2] < 0.0  ||
                        nodeRedArray[n3] < 0.0)
                    {
                        continue;
                    }
                }

                if (selectionMode) {
                    jgl.StartDrawMode ();
                }

            /*
             * Draw the three triangle nodes, color first, then normal
             * and finally the point itself.
             */
                if (selectionMode == false) {
                    if (acolor) {

/*
if (ndump < 100  &&  i % 5 == 0) {
System.out.println ("Color at node "+n1+" "+
                     nodeRedArray[n1]+" "+nodeGreenArray[n1]+" "+
                     nodeBlueArray[n1]);
ndump++;
}
*/
                        jgl.DGNodeColor (nodeRedArray[n1],
                                         nodeGreenArray[n1],
                                         nodeBlueArray[n1],
                                         nodeAlphaArray[n1]);
                    }

                    jgl.DGNodeNormal (viewXNormalArray[n1],
                                      viewYNormalArray[n1],
                                      viewZNormalArray[n1]);
                }
                jgl.DGNodeXYZ (nodeXArray[n1],
                               nodeYArray[n1],
                               nodeZArray[n1]);

                if (selectionMode == false) {
                    if (acolor) {
                        jgl.DGNodeColor (nodeRedArray[n2],
                                         nodeGreenArray[n2],
                                         nodeBlueArray[n2],
                                         nodeAlphaArray[n2]);
                    }
                    jgl.DGNodeNormal (viewXNormalArray[n2],
                                      viewYNormalArray[n2],
                                      viewZNormalArray[n2]);
                }
                jgl.DGNodeXYZ (nodeXArray[n2],
                               nodeYArray[n2],
                               nodeZArray[n2]);

                if (selectionMode == false) {
                    if (acolor) {
                        jgl.DGNodeColor (nodeRedArray[n3],
                                         nodeGreenArray[n3],
                                         nodeBlueArray[n3],
                                         nodeAlphaArray[n3]);
                    }
                    jgl.DGNodeNormal (viewXNormalArray[n3],
                                      viewYNormalArray[n3],
                                      viewZNormalArray[n3]);
                }
                jgl.DGNodeXYZ (nodeXArray[n3],
                               nodeYArray[n3],
                               nodeZArray[n3]);

                if (selectionMode) {
                    jgl.StopDrawMode ();
                    panel.popName ();
                }

            }

        /*
         * The stop and start draw mode is applied to the entire trimesh
         * if the selection mode is false.  This is done to speed up
         * drawing.
         */
            if (selectionMode == false) {
                jgl.StopDrawMode ();
            }

            if (selectionMode == false) {
                jgl.EndList ();  // uncomment to put back dlists
                jgl.CallList (ilist);
                lastList = ilist;
            }

        } // end of compile and run display list block.

    /*
     * The display list for this surface can be run without recompiling.
     */
        else {
            jgl.CallList (dlist);  // uncomment to put back dlists
        }

        panel.popName ();
        panel.popName ();

        if (!panel.isSelectionMode() &&
            labelFlag &&
            DEBUG_LABEL_TRIANGLES) {

            for (i=0; i<numTriangles; i++) {

                n1 = n1TriArray[i];
                n2 = n2TriArray[i];
                n3 = n3TriArray[i];

                xt = nodeWorldXArray[n1];
                yt = nodeWorldYArray[n1];
                zt = nodeWorldZArray[n1];
                pt1 = scalePoint (xt, yt, zt);
                if (pt1 == null) {
                    continue;
                }
                xt = nodeWorldXArray[n2];
                yt = nodeWorldYArray[n2];
                zt = nodeWorldZArray[n2];
                pt2 = scalePoint (xt, yt, zt);
                if (pt2 == null) {
                    continue;
                }
                xt = nodeWorldXArray[n3];
                yt = nodeWorldYArray[n3];
                zt = nodeWorldZArray[n3];
                pt3 = scalePoint (xt, yt, zt);
                if (pt3 == null) {
                    continue;
                }

                double xmid = (pt1.x + pt2.x + pt3.x) / 3.0;
                double ymid = (pt1.y + pt2.y + pt3.y) / 3.0;
                double zmid = (pt1.z + pt2.z + pt3.z) / 3.0;
                jgl.DrawTextXYZ (xmid, ymid, zmid,
                                 Integer.toString (i),
                                 .07);

            }

        }

    }  // end of scaleAndDrawTriangles function

/*-----------------------------------------------------------------------------*/

    private void updateViewNormals (double x_exag,
                                    double y_exag,
                                    double z_exag)
    {
        int          i, size;
        double       dx, dy, dz, d1, d2;

        if (nodeXNormalArray == null)
//          throw new IllegalStateException("nodeXNormalArray has not been initialized.");
          return;

        size = nodeXNormalArray.length;
        for (i=0; i<size; i++) {
            dx = nodeXNormalArray[i];
            dy = nodeYNormalArray[i];
            dz = nodeZNormalArray[i];
            dx /= x_exag;
            dy /= y_exag;
            dz /= z_exag;
            d1 = dx * dx + dy * dy + dz * dz;
            d2 = Math.sqrt (d1);
            if (d2 < 0.0001) d2 = 0.0001;
            dx /= d2;
            dy /= d2;
            dz /= d2;
            viewXNormalArray[i] = dx;
            viewYNormalArray[i] = dz;
            viewZNormalArray[i] = dy;
        }

        if (wireList > 0) {
            wireList = -wireList;
        }
        if (solidList > 0) {
            solidList = -solidList;
        }

        return;

    }

/*--------------------------------------------------------------------------------*/

    private JPoint3D scalePoint (double x, double y, double z)
    {
        JPoint3D            pt;

        pt = scaler.scalePoint ((float)x, (float)y, (float)z);

        return pt;

    }

/*--------------------------------------------------------------------------------*/

    private void scalePoint (double x, double y, double z, JPoint3D pt)
    {
        scaler.scalePoint (x, y, z, pt);
    }

/*---------------------------------------------------------------------------------*/

    int calcXYZHit (
        int   index,
        JPoint3D  nearin,
        JPoint3D  farin,
        JPoint3D  resultout)
    {
        double[]  x, y, z;

        resultout.x = 1.e30;
        resultout.y = 1.e30;
        resultout.z = 1.e30;

        if (index < 0  ||  index >= numTriangles) {
            return -1;
        }

        JPoint3D near =
        scaler.unscalePoint (nearin.x, nearin.y, nearin.z);
        JPoint3D far =
        scaler.unscalePoint (farin.x, farin.y, farin.z);

        x = new double[3];
        y = new double[3];
        z = new double[3];

        int n1 = n1TriArray[index];
        int n2 = n2TriArray[index];
        int n3 = n3TriArray[index];
        x[0] = nodeWorldXArray[n1];
        y[0] = nodeWorldYArray[n1];
        z[0] = nodeWorldZArray[n1];
        x[1] = nodeWorldXArray[n2];
        y[1] = nodeWorldYArray[n2];
        z[1] = nodeWorldZArray[n2];
        x[2] = nodeWorldXArray[n3];
        y[2] = nodeWorldYArray[n3];
        z[2] = nodeWorldZArray[n3];
        TriangleNormal (x, y, z);
        double tnx = tnxNorm;
        double tny = tnyNorm;
        double tnz = tnzNorm;

        JPoint3D result = new JPoint3D ();

        int istat =
        Math3D.intersectPlaneAndLine (
            tnx, tny, tnz,
            x[0], y[0], z[0],
            near, far,
            result);

        if (istat == -1) {
            return -1;
        }

        resultout.x = result.x;
        resultout.y = result.y;
        resultout.z = result.z;

        return 1;

    }

/*------------------------------------------------------------------------*/

    private void updateViewNodes ()
    {

        double        xt, yt, zt;
        JPoint3D      pt1;

        pt1 = new JPoint3D ();

        for (int i=0; i<numNodes; i++) {
            xt = nodeWorldXArray[i];
            yt = nodeWorldYArray[i];
            zt = nodeWorldZArray[i];
            scalePoint (xt, yt, zt, pt1);
            nodeXArray[i] = pt1.x;
            nodeYArray[i] = pt1.y;
            nodeZArray[i] = pt1.z;
        }

        if (wireList > 0) {
            wireList = -wireList;
        }
        if (solidList > 0) {
            solidList = -solidList;
        }

    }

}  // end of class definition
