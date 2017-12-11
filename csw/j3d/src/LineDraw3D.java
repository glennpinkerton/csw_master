/*
 */

package csw.j3d.src;

import java.awt.Color;

import csw.jutils.src.XYZPolyline;

/**
 This class stores information needed to draw a {@link XYZPolyline} in the
 3d display.  The j3d package is the only code that needs access to this
 code so it is declared with package scope.
 */
class LineDraw3D
{

    int              pointFlag = 0;

    private double[] nodeXArray,
                     nodeYArray,
                     nodeZArray;

    private double[] nodeAttributeArray;
    private float[]  nodeRedArray;
    private float[]  nodeGreenArray;
    private float[]  nodeBlueArray;
    private float[]  nodeAlphaArray;
    private boolean  colorByAttribute = false;
    private AtColorLookup    colorLookup;

    private boolean  doAtColor = false;

    private int      numNodes;

    private Color    globalColor;

    private Scaler   scaler;

    private double   lineWidth = 1.5;
    private boolean  dashFlag = false;

/*--------------------------------------------------------------------------------*/

  /**
   This constructor takes the current contents of a {@link XYZPolyline} object
   and transfers it to this LineDraw3D object.  If the specified line is null,
   the object is created empty.
   */
    LineDraw3D (XYZPolyline line)
    {
        setLine (line);
    }

/*--------------------------------------------------------------------------------*/

  /**
   Set the internal node arrays to the Line values.  If the specified line is null,
   the object is emptied.
  */
    void setLine (XYZPolyline line)
    {
        double[] nodex, nodey, nodez;
        int[]    edge1, edge2, tri1, tri2, tri3;
        int      nnode, nedge, ntri, i, size;

        nodeXArray = null;
        nodeYArray = null;
        nodeZArray = null;

        nodeAttributeArray = null;
        nodeRedArray = null;
        nodeGreenArray = null;
        nodeBlueArray = null;
        nodeAlphaArray = null;
        colorLookup = null;
        colorByAttribute = false;
        doAtColor = false;

        if (line == null) {
            return;
        }

    /*
     * Get the nodes, edges, and triangles from the trimesh.
     */
        nodex = line.getXArray ();
        nodey = line.getYArray ();
        nodez = line.getZArray ();

        if (nodex == null  ||  nodey == null  ||  nodez == null) {
            return;
        }

        nnode = line.size ();
        if (nnode < 1) {
            return;
        }

    /*
     * Copy the nodes to this object's node arrays
     */
        size = nodex.length;
        nodeXArray = new double[size];
        nodeYArray = new double[size];
        nodeZArray = new double[size];

        for (i=0; i<nnode; i++) {
            nodeXArray[i] = nodex[i];
            nodeYArray[i] = nodey[i];
            nodeZArray[i] = nodez[i];
        }

        numNodes = nnode;

    }

/*--------------------------------------------------------------------------------*/

    void setLineWidth (double width)
    {
        if (width < 1.0) {
            return;
        }

        lineWidth = width;
    }

    double getLineWidth ()
    {
        return lineWidth;
    }

/*--------------------------------------------------------------------------------*/

    void setDashed (boolean bval)
    {
        dashFlag = bval;
    }

    boolean isDashed ()
    {
        return dashFlag;
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
        if (nodeXArray == null) {
            return;
        }

        int        num_nodes, i;
        double     zt;
        JColor4D   clrd;
        Color      color;

        num_nodes = nodeXArray.length;

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

        return;

    }

/*-----------------------------------------------------------------------------*/

    //private boolean          labelFlag = false;

    void scaleAndDrawLine (
        double pretty_xmin,
        double pretty_ymin,
        double pretty_zmin,
        double pretty_xmax,
        double pretty_ymax,
        double pretty_zmax,
        double raw_z_exag,
        Color  globalColor,
        double lineWidth,
        boolean dashFlag,
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

        if (globalColor == null) {
            return;
        }

        drawRed = (double)globalColor.getRed();
        drawGreen = (double)globalColor.getGreen();
        drawBlue = (double)globalColor.getBlue();
        drawAlpha = (double)globalColor.getAlpha();
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

        boolean          acolor;

        acolor = false;
        if (doAtColor == true) {
            acolor = true;
        }

        int              n1, n2, nend;
        JPoint3D         pt, pt1, pt2;

    /*
     * Draw as lines.
     */
        if (pointFlag == 0) {
            if (numNodes > 1) {
                jgl.SetDrawMode (JGL.DG_LINES);
                jgl.SetDrawStyle (JGL.DG_WIREFRAME);
                jgl.SetDrawLineWidth (lineWidth);
                jgl.SetDrawLineDash (dashFlag);
                jgl.SetDrawCulling (0);
                jgl.SetDrawLighting (0);

                jgl.DGNodeColor (drawRed,
                                 drawGreen,
                                 drawBlue,
                                 drawAlpha);

                for (i=0; i<numNodes-1; i++) {

                    n1 = i;
                    n2 = i + 1;

                /*
                 * If either end node cannot be colored, skip the segment.
                 */
                    if (acolor) {
                        if (nodeRedArray[n1] < 0.0  ||
                            nodeRedArray[n2] < 0.0) {
                            continue;
                        }
                    }

                /*
                 * If either point cannot be scaled, skip the segment draw.
                 */
                    xt = nodeXArray[n1];
                    yt = nodeYArray[n1];
                    zt = nodeZArray[n1];
                    pt1 = scalePoint (xt, yt, zt);
                    if (pt1 == null) {
                        continue;
                    }

                    xt = nodeXArray[n2];
                    yt = nodeYArray[n2];
                    zt = nodeZArray[n2];
                    pt2 = scalePoint (xt, yt, zt);
                    if (pt2 == null) {
                        continue;
                    }

                    jgl.StartDrawMode ();

                /*
                 * Draw the two line segment nodes.
                 */
                    if (acolor) {
                        jgl.DGNodeColor (nodeRedArray[n1],
                                         nodeGreenArray[n1],
                                         nodeBlueArray[n1],
                                         nodeAlphaArray[n1]);
                    }
                    jgl.DGNodeXYZ (pt1.x, pt1.y, pt1.z);

                    if (acolor) {
                        jgl.DGNodeColor (nodeRedArray[n2],
                                         nodeGreenArray[n2],
                                         nodeBlueArray[n2],
                                         nodeAlphaArray[n2]);
                    }
                    jgl.DGNodeXYZ (pt2.x, pt2.y, pt2.z);

                    jgl.StopDrawMode ();

                }  // end of numNodes loop

            }  // end of numNodes > 1 if block

        }  // end of pointflag == 0 if block

    /*
     * Draw as points.
     */
        else {

            jgl.SetDrawMode (JGL.DG_POINTS);
            double psize;
            psize = lineWidth * 5.0;
            if (psize < 5.0) psize = 5.0;
            jgl.SetDrawPointSize (psize);
            jgl.SetDrawStyle (JGL.DG_MESH);

            jgl.DGNodeColor (drawRed,
                             drawGreen,
                             drawBlue,
                             drawAlpha);

            panel.pushName (J3DConst.POINT_NAME);
            panel.pushName (0);

            for (i=0; i<numNodes; i++) {

               n1 = i;

           /*
            * If either end node cannot be colored, skip the segment.
            */
               if (acolor) {
                   if (nodeRedArray[n1] < 0.0) {
                       continue;
                   }
               }

           /*
            * If either point cannot be scaled, skip the segment draw.
            */
               xt = nodeXArray[n1];
               yt = nodeYArray[n1];
               zt = nodeZArray[n1];
               pt1 = scalePoint (xt, yt, zt);
               if (pt1 == null) {
                   continue;
               }

               panel.loadName (i);
               jgl.StartDrawMode ();

           /*
            * Draw the point.
            */
               if (acolor) {
                   jgl.DGNodeColor (nodeRedArray[n1],
                                    nodeGreenArray[n1],
                                    nodeBlueArray[n1],
                                    nodeAlphaArray[n1]);
               }
               jgl.DGNodeXYZ (pt1.x, pt1.y, pt1.z);

               jgl.StopDrawMode ();

            }

            panel.popName();
            panel.popName();

        }  // end of else block

    }  // end of scaleAndDrawLine function

/*--------------------------------------------------------------------------------*/

    private JPoint3D scalePoint (double x, double y, double z)
    {
        JPoint3D            pt;

        pt = scaler.scalePoint ((float)x, (float)y, (float)z);

        return pt;

    }

/*---------------------------------------------------------------------------------*/

  /*
   * Calculate the closest point on the line to the view point ray.
   * If the view point ray is not close to any line segment, zero is
   * returned.  If the object is not set up correctly for the calculation,
   * -1 is returned.  On success 1 is returned.  The resultout is filled
   * with the xyz of the closest point on success or filled with 1.e30
   * on failure.
   */
    int calcXYZHit (
        JPoint3D  nearin,
        JPoint3D  farin,
        JPoint3D  resultout)
    {
        double    x1, y1, z1,
                  x2, y2, z2,
                  xt, yt, zt;
        double    dist, dmin, dx, dy, dz;
        int       i, size;

        resultout.x = 1.e30;
        resultout.y = 1.e30;
        resultout.z = 1.e30;

        if (nodeXArray == null) {
            return -1;
        }

        xt = 1.e30;
        yt = 1.e30;
        zt = 1.e30;

        JPoint3D near =
        scaler.unscalePoint (nearin.x, nearin.y, nearin.z);
        JPoint3D far =
        scaler.unscalePoint (farin.x, farin.y, farin.z);

        JPoint3D result1 = new JPoint3D ();
        JPoint3D result2 = new JPoint3D ();

        size = nodeXArray.length;

        dmin = 1.e30;

        for (i=0; i<size-1; i++) {

            x1 = nodeXArray[i];
            y1 = nodeYArray[i];
            z1 = nodeZArray[i];
            x2 = nodeXArray[i+1];
            y2 = nodeYArray[i+1];
            z2 = nodeZArray[i+1];

            int istat =
            Math3D.intersect3DLineSegments (
                x1, y1, z1,
                x2, y2, z2,
                near, far,
                result1, result2);

            if (istat == -1) {
                continue;
            }

            dx = result1.x - result2.x;
            dy = result1.y - result2.y;
            dz = result1.z - result2.z;

            dist = dx * dx + dy * dy + dz * dz;
            dist = Math.sqrt (dist);

            if (dist < dmin) {
                dmin = dist;
                xt = result1.x;
                yt = result1.y;
                zt = result1.z;
            }

        }

        resultout.x = xt;
        resultout.y = yt;
        resultout.z = zt;

        if (xt >= 1.e30) {
            return 0;
        }

        return 1;

    }

    /*
     * Calculate the closest point on the line to the view point ray, within
     * the specified tolerance. If the view point ray is not within the
     * specified tolerance to any line segment, zero is returned.  If the
     * object is not set up correctly for the calculation, -1 is returned.
     * On success 1 is returned.  The resultout is filled with the xyz of the
     * closest point on success or filled with 1.e30 on failure.
     */
      int calcXYZHit (
          JPoint3D  nearin,
          JPoint3D  farin,
          JPoint3D  resultout,
          double tolerance)
      {
          double    x1, y1, z1,
                    x2, y2, z2,
                    xt, yt, zt;
          double    dist, dmin, dx, dy, dz;
          int       i, size;

          resultout.x = 1.e30;
          resultout.y = 1.e30;
          resultout.z = 1.e30;

          if (nodeXArray == null) {
              return -1;
          }

          xt = 1.e30;
          yt = 1.e30;
          zt = 1.e30;

          JPoint3D near =
          scaler.unscalePoint (nearin.x, nearin.y, nearin.z);
          JPoint3D far =
          scaler.unscalePoint (farin.x, farin.y, farin.z);

          JPoint3D result1 = new JPoint3D ();
          JPoint3D result2 = new JPoint3D ();

          size = nodeXArray.length;

          dmin = 1.e30;

          for (i=0; i<size-1; i++) {

              x1 = nodeXArray[i];
              y1 = nodeYArray[i];
              z1 = nodeZArray[i];
              x2 = nodeXArray[i+1];
              y2 = nodeYArray[i+1];
              z2 = nodeZArray[i+1];

              int istat =
              Math3D.intersect3DLineSegments (
                  x1, y1, z1,
                  x2, y2, z2,
                  near, far,
                  result1, result2);

              if (istat <=  0) {
                  continue;
              }

              dx = result1.x - result2.x;
              dy = result1.y - result2.y;
              dz = result1.z - result2.z;

              dist = dx * dx + dy * dy + dz * dz;
              dist = Math.sqrt (dist);

              if (dist < dmin) {
                  dmin = dist;
                  xt = result1.x;
                  yt = result1.y;
                  zt = result1.z;
              }

          }

          if (xt >= 1.e30) {
              return 0;
          }

          if (dmin < tolerance) {
            resultout.x = xt;
            resultout.y = yt;
            resultout.z = zt;
            return 1;
          }
          return 0;

      }

      /**
       * Find the closest line segment within the specified tolerance.
       * @param nearin near point (viewers eye)
       * @param farin far point (a point on the other side of the screen,
       *        that is on the extension of the line defined by the near point
       *        and the picked point)
       * @param tolerance specified tolerance
       * @return line segment (starting with 1) of the specified point;
       *        or -1 if the point is not on a line segment.
       */
      int findLineSegment(
          JPoint3D  nearin,
          JPoint3D  farin,
          double tolerance)
      {
          double    x1, y1, z1,
                    x2, y2, z2;
          double    dist, dmin, dx, dy, dz;
          int       i, size;
          int       imin = -1;

          if (nodeXArray == null) {
            return -1;
          }

          JPoint3D near =
          scaler.unscalePoint (nearin.x, nearin.y, nearin.z);
          JPoint3D far =
          scaler.unscalePoint (farin.x, farin.y, farin.z);

          JPoint3D result1 = new JPoint3D ();
          JPoint3D result2 = new JPoint3D ();

          size = nodeXArray.length;

          dmin = 1.e30;

          for (i=0; i<size-1; i++) {
              x1 = nodeXArray[i];
              y1 = nodeYArray[i];
              z1 = nodeZArray[i];
              x2 = nodeXArray[i+1];
              y2 = nodeYArray[i+1];
              z2 = nodeZArray[i+1];

              int istat =
              Math3D.intersect3DLineSegments (
                  x1, y1, z1,
                  x2, y2, z2,
                  near, far,
                  result1, result2);

              if (istat <= 0) {
                  continue;
              }

              dx = result1.x - result2.x;
              dy = result1.y - result2.y;
              dz = result1.z - result2.z;

              dist = dx * dx + dy * dy + dz * dz;
              dist = Math.sqrt (dist);

              if (dist < dmin) {
                  dmin = dist;
                  imin = i;
              }
          }
          if (dmin < tolerance) {
            return (imin + 1);
          }
          return -1;
      }

}  // end of class definition
