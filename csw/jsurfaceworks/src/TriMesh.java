
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;

import java.io.IOException;

/**
  Store a triangular mesh representation of a surface patch.
  A TriMesh object can be calculated with a {@link JSurfaceWorks} object.
  @author Glenn Pinkerton
*/
public class TriMesh {

	private static long StaticID = 0;

/*
 * Arrays with node by node information.
 */
  private double[] nodeXArray;
  private double[] nodeYArray;
  private double[] nodeZArray;
  private int[] nodeFlagArray;
  private int numNodes;

  private double[] rdpValues;
  private int      atType = -1;

/*
 * Arrays with edge by edge information.
 */
  private int[] edgeNode0Array;
  private int[] edgeNode1Array;
  private int[] edgeTriangle0Array;
  private int[] edgeTriangle1Array;
  private int[] edgeFlagArray;
  private int numEdges;

/*
 * Arrays with triangle by triangle information.
 */
  private int[] triangleEdge0Array;
  private int[] triangleEdge1Array;
  private int[] triangleEdge2Array;
  private int[] triangleFlagArray;
  private int numTriangles;

  private double zMin = 1.e30;
  private double zMax = -1.e30;
  private double yMin = 1.e30;
  private double yMax = -1.e30;
  private double xMin = 1.e30;
  private double xMax = -1.e30;

  private double averageEdgeLength = -1.0;

  public VertBaseline   vBase = null;

  public int debug_id1;
  public int debug_id2;
  public int debug_id3;

  public double[] xBounds = null;
  public double[] yBounds = null;
  public int      nBounds = 0;

  public boolean  externalTsurf;
  public boolean  externalGrid;

  public boolean  flipFlag = false;
  public boolean  flipFlag2 = false;

/**
 Create an empty trimesh object.
*/
  public TriMesh () {
    externalTsurf = false;
    externalGrid = false;
  }

  /**
   * Construct a new TriMesh based on an existing one, with zs offset
   * a certain amount (negative zs offset above, positive below).
   *
   * @param triMesh TriMesh to copy
   * @param zOffset offset value for z array
   */
  public TriMesh(TriMesh triMesh, double zOffset) {
    if (triMesh == null)
      throw new IllegalArgumentException("triMesh is null");
    // first copy the entire trimesh to the new one
    copyFrom(triMesh);
    // now loop through z array and offset the values
    for (int index=0; index<nodeZArray.length; index++) {
      nodeZArray[index] += zOffset;
    }
  }

  /**
   * Copy Constructor
   * @param triMesh TriMesh to copy from
   */
  public TriMesh(TriMesh triMesh) {
    if (triMesh == null)
      throw new IllegalArgumentException("triMesh is null");
    // first copy the entire trimesh to the new one
    copyFrom(triMesh);
  }

/*-------------------------------------------------------------------------------*/

/**
 Set the attribute type to allow the cached attributes per
 node to be used again if possible.
*/
  public void setAtType (int ival)
  {
    atType = ival;
  }

  public int getAtType ()
  {
    return atType;
  }


/**
 Set the pernode RDP values that are currently being used
 to color this trimesh on the 3D display.
*/
  public void setRDPValues (double[] vals)
  {
    rdpValues = vals;
  }


/**
 Get the pernode RDP values that are currently being used
 to color this trimesh on the 3D display.
*/
  public double[] getRDPValues ()
  {
    return rdpValues;
  }

/*-------------------------------------------------------------------------------*/

  public void resetLimits ()
  {
    resetCachedExtents ();
  }

/*-------------------------------------------------------------------------------*/

  public double getAverageEdgeLength ()
  {
    if (averageEdgeLength > 0.0) {
      return averageEdgeLength;
    }

    if (nodeXArray == null  ||
        nodeYArray == null  ||
        nodeZArray == null  ||
        numNodes < 3  ||
        edgeNode0Array == null  ||
        edgeNode1Array == null  ||
        numEdges < 3) {
      return -1.0;
    }

    int        i, n1, n2;
    double     dx, dy, dz, dist;
    double     sum1, sum2;

    sum1 = 0.0;
    sum2 = 0.0;

    for (i=0; i<numEdges; i++) {
        n1 = edgeNode0Array[i];
        n2 = edgeNode1Array[i];
        dx = nodeXArray[n1] - nodeXArray[n2];
        dy = nodeYArray[n1] - nodeYArray[n2];
        dz = nodeZArray[n1] - nodeZArray[n2];
        dist = dx * dx + dy * dy + dz * dz;
        dist = Math.sqrt (dist);
        sum1 += dist;
        sum2++;
    }

    averageEdgeLength = sum1 / sum2;
    averageEdgeLength /= 1.13;

    return averageEdgeLength;

  }

/*-------------------------------------------------------------------------------*/


  /*
   * Reset the extents to the defaults so they will be recalculated next
   * time they are needed.
   */
  private void resetCachedExtents() {
    yMin = 1.e30;
    yMax = -1.e30;
    xMin = 1.e30;
    xMax = -1.e30;
    zMin = 1.e30;
    zMax = -1.e30;
    averageEdgeLength = -1.0;
  }

/**
 Return the external tsurf flag.  If this trimesh object was created by
 using the {@link JSurfaceWorks#convertTriMesh} method, then this flag
 is true.  For all other cases, the flag is false.
*/
  public boolean isExternalTsurf ()
  {
    return externalTsurf;
  }

/**
 Return the external grid flag.  If this trimesh object was created by
 using the {@link JSurfaceWorks#gridToTriMesh} method, then this flag
 is true.  For all other cases, the flag is false.
*/
  public boolean isExternalGrid ()
  {
    return externalGrid;
  }

/**
 Print ou the x, y, z limits of the trimesh.
 */
  public void dumpLimits ()
  {
    System.out.println ("x limits: "+xMin+" "+xMax);
    System.out.println ("y limits: "+yMin+" "+yMax);
    System.out.println ("z limits: "+zMin+" "+zMax);
  }

/**
 Print all the node locations to standard output.  This is for debug
 purposes only.
*/
  public void dump ()
  {
    dump (numNodes);
  }

/**
 Print a specified number of node locations to standard output.  This
 is for debug purposes only.  If the specified number is larger than
 the total number of available nodes, the total number of available
 nodes is used.
*/
  public void dump (int nItemsToDump) {
    System.out.println ();
    System.out.println ("Trimesh dump");
    System.out.println ("num nodes = " + numNodes);
    System.out.println ("num edges = " + numEdges);
    System.out.println ("num triangles = " + numTriangles);
    int nitems = nItemsToDump;
    if (nitems > numNodes)
      nitems = numNodes;
    System.out.println ("Node list (dumping "+nitems+"):");
    for (int i=0; i<nitems; i++) {
      System.out.println (i +": " + nodeXArray[i] + " " + nodeYArray[i] + " " + nodeZArray[i]);
    }
    System.out.println ();
  }

/**
 Print the number of nodes, edges, and triangles to standard output.
 This is used for debugging purposes.
*/
  public void dumpBasics ()
  {
    System.out.println ("numNodes = "+numNodes+" numEdges = "+numEdges+
                        " numTriangles = "+numTriangles);
  }

/**
 Add node data to the trimesh.
@param nodex Array of the node X coordinates.
@param nodey Array of the node Y coordinates.
@param nodez Array of the node Z coordinates.
@param flag Array of flags for nodes.
@param n Number of nodes.
*/
  public void setNodeArrays (double[] nodex,
                      double[] nodey,
                      double[] nodez,
                      int[] flag,
                      int n)
  {
    nodeXArray = nodex;
    nodeYArray = nodey;
    nodeZArray = nodez;
    nodeFlagArray = flag;
    numNodes = n;
    averageEdgeLength = -1.0;
    calcMinMax ();
  }

/**
 Return a reference to the node x coordinates array.
*/
  public double[] getNodeXArray ()
  {
    return nodeXArray;
  }

/**
 Return a reference to the node y coordinates array.
*/
  public double[] getNodeYArray ()
  {
    return nodeYArray;
  }

/**
 Return a reference to the node z coordinates array.
*/
  public double[] getNodeZArray ()
  {
    return nodeZArray;
  }

/**
 Return a reference to the node flag coordinates array.
*/
  public int[] getNodeFlagArray ()
  {
    return nodeFlagArray;
  }



/**
 Return the number of nodes.
*/
  public int getNumNodes ()
  {
    return numNodes;
  }



  private void calcMinMax ()
  {

    double zmin = 1.e30;
    double zmax = -1.e30;
    double xmin = 1.e30;
    double xmax = -1.e30;
    double ymin = 1.e30;
    double ymax = -1.e30;

    int i;

    for (i=0; i<numNodes; i++) {
      if (nodeZArray[i] > 1.e20  ||
          nodeZArray[i] < -1.e20) {
        continue;
      }
      if (nodeZArray[i] < zmin) zmin = nodeZArray[i];
      if (nodeZArray[i] > zmax) zmax = nodeZArray[i];
    }

    zMin = zmin;
    zMax = zmax;

    for (i=0; i<numNodes; i++) {
      if (nodeXArray[i] < xmin) xmin = nodeXArray[i];
      if (nodeXArray[i] > xmax) xmax = nodeXArray[i];
    }

    xMin = xmin;
    xMax = xmax;

    for (i=0; i<numNodes; i++) {
      if (nodeYArray[i] < ymin) ymin = nodeYArray[i];
      if (nodeYArray[i] > ymax) ymax = nodeYArray[i];
    }

    yMin = ymin;
    yMax = ymax;

  }


/**
 * return the vertical baseline data for the trimesh.
 */
  public VertBaseline getVertBaseline ()
  {
    return vBase;
  }


/**
 Return the minimum node z value.
*/
  public double getZMin ()
  {
    if (zMin > zMax) {
      calcMinMax ();
    }
    return zMin;
  }

/**
 Return the maximum node z value.
*/
  public double getZMax ()
  {
    if (zMin > zMax) {
      calcMinMax ();
    }
    return zMax;
  }


/**
 Return the minimum node x value.
*/
  public double getXMin ()
  {
    if (xMin > xMax) {
      calcMinMax ();
    }
    return xMin;
  }

/**
 Return the maximum node x value.
*/
  public double getXMax ()
  {
    if (xMin > xMax) {
      calcMinMax ();
    }
    return xMax;
  }

/**
 Return the minimum node y value.
*/
  public double getYMin ()
  {
    if (yMin > yMax) {
      calcMinMax ();
    }
    return yMin;
  }

/**
 Return the maximum node y value.
*/
  public double getYMax ()
  {
    if (yMin > yMax) {
      calcMinMax ();
    }
    return yMax;
  }


/**
 Add edge data to the trimesh.
@param node0 Array of first edge endpoint node indices.
@param node1 Array of second edge endpoint node indices.
@param tri0 Array of triangle indices of first triangle using the edge.
@param tri1 Array of triangle indices for second triangle using the edge.
@param flag Array of flags for edges.
@param n Number of edges.
*/
  public void setEdgeArrays (int[] node0,
                      int[] node1,
                      int[] tri0,
                      int[] tri1,
                      int[] flag,
                      int n)
  {
    edgeNode0Array = node0;
    edgeNode1Array = node1;
    edgeTriangle0Array = tri0;
    edgeTriangle1Array = tri1;
    edgeFlagArray = flag;
    averageEdgeLength = -1.0;
    numEdges = n;
  }

/**
 Return a reference to the edge node0 array.
*/
  public int[] getEdgeNode0Array ()
  {
    return edgeNode0Array;
  }

/**
 Return a reference to the edge node1 array.
*/
  public int[] getEdgeNode1Array ()
  {
    return edgeNode1Array;
  }

/**
 Return a reference to the edge triangle0 array.
*/
  public int[] getEdgeTriangle0Array ()
  {
    return edgeTriangle0Array;
  }

/**
 Return a reference to the edge triangle1 array.
*/
  public int[] getEdgeTriangle1Array ()
  {
    return edgeTriangle1Array;
  }

/**
 Return a reference to the edge flag array.
*/
  public int[] getEdgeFlagArray ()
  {
    return edgeFlagArray;
  }

/**
 Return the number of edges in the trimesh.
*/
  public int getNumEdges ()
  {
    return numEdges;
  }

/**
 Add triangle data to the trimesh.
@param edge0 Array of edge indices for first triangle edge.
@param edge1 Array of edge indices for second triangle edge.
@param edge2 Array of triangle indices for trhird triangle edge.
@param flag Array of triangle flags.
@param n Number of triangles.
*/
  public void setTriangleArrays (
                          int[] edge0,
                          int[] edge1,
                          int[] edge2,
                          int[] flag,
                          int n)
  {
    triangleEdge0Array = edge0;
    triangleEdge1Array = edge1;
    triangleEdge2Array = edge2;
    triangleFlagArray = flag;
    numTriangles = n;
  }

/**
 Return a reference to the triangle edge 0 array
*/
  public int[] getTriangleEdge0Array ()
  {
    return triangleEdge0Array;
  }

/**
 Return a reference to the triangle edge 1 array
*/
  public int[] getTriangleEdge1Array ()
  {
    return triangleEdge1Array;
  }

/**
 Return a reference to the triangle edge 2 array
*/
  public int[] getTriangleEdge2Array ()
  {
    return triangleEdge2Array;
  }

/**
 Return a reference to the triangle flag array
*/
  public int[] getTriangleFlagArray ()
  {
    return triangleFlagArray;
  }

/**
 Return the number of triangles in the trimesh.
*/
  public int getNumTriangles ()
  {
    return numTriangles;
  }

  /** Returns an array of triangle node 0
   */
  public int[] getTriangleNode0Array() {

    int nodeArray[] = new int[numTriangles];

    for (int loop = 0; loop < numTriangles; loop++) {
      nodeArray[loop] = edgeNode0Array[triangleEdge0Array[loop]];
    }

    return(nodeArray);
  }

  /** Returns an array of triangle node 1
   */
  public int[] getTriangleNode1Array() {

    int nodeArray[] = new int[numTriangles];

    for (int loop = 0; loop < numTriangles; loop++) {
      nodeArray[loop] = edgeNode1Array[triangleEdge0Array[loop]];
    }

    return(nodeArray);
  }

  /** Returns an array of triangle node 2
   */
  public int[] getTriangleNode2Array() {

    int nodeArray[] = new int[numTriangles];

    for (int loop = 0; loop < numTriangles; loop++) {
      int node0 = edgeNode0Array[triangleEdge0Array[loop]];
      int node1 = edgeNode1Array[triangleEdge0Array[loop]];

      int node2 = edgeNode0Array[triangleEdge1Array[loop]];
      if (node2 == node0 || node2 == node1)
        node2 = edgeNode1Array[triangleEdge1Array[loop]];

      nodeArray[loop] = node2;
    }

    return(nodeArray);
  }

  /**
   * Copy array references from another TriMesh to
   * this TriMesh (useful for post-read, because
   * readTriMeshFile() returns a TriMesh).
   */
  public void copyFrom(TriMesh copyFromTriMesh) {
    if (copyFromTriMesh == null) {
      return;
    }
    externalGrid = copyFromTriMesh.externalGrid;
    externalTsurf = copyFromTriMesh.externalTsurf;

    numNodes = copyFromTriMesh.numNodes;
    // allocate all numNodes arrays
    nodeXArray = new double[numNodes];
    nodeYArray = new double[numNodes];
    nodeZArray = new double[numNodes];
    nodeFlagArray = new int[numNodes];
    System.arraycopy(copyFromTriMesh.nodeXArray, 0, nodeXArray, 0, numNodes);
    System.arraycopy(copyFromTriMesh.nodeYArray, 0, nodeYArray, 0, numNodes);
    System.arraycopy(copyFromTriMesh.nodeZArray, 0, nodeZArray, 0, numNodes);
    System.arraycopy(copyFromTriMesh.nodeFlagArray, 0, nodeFlagArray, 0, numNodes);

    numEdges = copyFromTriMesh.numEdges;
    // allocate all numEdges arrays
    edgeNode0Array = new int[numEdges];
    edgeNode1Array = new int[numEdges];
    edgeTriangle0Array = new int[numEdges];
    edgeTriangle1Array = new int[numEdges];
    edgeFlagArray = new int[numEdges];
    System.arraycopy(copyFromTriMesh.edgeNode0Array, 0, edgeNode0Array, 0, numEdges);
    System.arraycopy(copyFromTriMesh.edgeNode1Array, 0, edgeNode1Array, 0, numEdges);
    System.arraycopy(copyFromTriMesh.edgeTriangle0Array, 0, edgeTriangle0Array, 0, numEdges);
    System.arraycopy(copyFromTriMesh.edgeTriangle1Array, 0, edgeTriangle1Array, 0, numEdges);
    System.arraycopy(copyFromTriMesh.edgeFlagArray, 0, edgeFlagArray, 0, numEdges);

    numTriangles = copyFromTriMesh.numTriangles;
    // allocate all numTriangles arrays
    triangleEdge0Array = new int[numTriangles];
    triangleEdge1Array = new int[numTriangles];
    triangleEdge2Array = new int[numTriangles];
    triangleFlagArray = new int[numTriangles];
    System.arraycopy(copyFromTriMesh.triangleEdge0Array, 0, triangleEdge0Array, 0, numTriangles);
    System.arraycopy(copyFromTriMesh.triangleEdge1Array, 0, triangleEdge1Array, 0, numTriangles);
    System.arraycopy(copyFromTriMesh.triangleEdge2Array, 0, triangleEdge2Array, 0, numTriangles);
    System.arraycopy(copyFromTriMesh.triangleFlagArray, 0, triangleFlagArray, 0, numTriangles);

    if (copyFromTriMesh.vBase == null) {
      vBase = null;
    } else {
      vBase = new VertBaseline(copyFromTriMesh.vBase);
    }
  }

/**
 Transfer the array references and scalar values of this
 trimesh to another trimesh object.  This effectively does
 a "shallow" copy of the trimesh object.
*/
  public void transfer (TriMesh newTriMesh)
  {
    if (newTriMesh == null) {
      return;
    }
    newTriMesh.setNodeArrays (
      nodeXArray,
      nodeYArray,
      nodeZArray,
      nodeFlagArray,
      numNodes
    );
    newTriMesh.setEdgeArrays (
      edgeNode0Array,
      edgeNode1Array,
      edgeTriangle0Array,
      edgeTriangle1Array,
      edgeFlagArray,
      numEdges
    );
    newTriMesh.setTriangleArrays (
      triangleEdge0Array,
      triangleEdge1Array,
      triangleEdge2Array,
      triangleFlagArray,
      numTriangles
    );
    newTriMesh.averageEdgeLength = averageEdgeLength;

  }


/*------------------------------------------------------------------------------------*/

  public static TriMesh createFromAsciiDebugFile (String fname)
  {
    if (fname == null) {
      return null;
    }

    TriMesh tmesh = new TriMesh ();

    try {
      tmesh.loadAsciiDebugFile (fname);
    }
    catch (Exception e) {
      return null;
    }

    return tmesh;

  }


/*------------------------------------------------------------------------------------*/

  public void setYZFlip (boolean bval)
  {
    flipFlag = bval;
    if (bval) {
      flipFlag2 = false;
    }
  }

  public void setXZFlip (boolean bval)
  {
    flipFlag2 = bval;
    if (bval) {
      flipFlag = false;
    }
  }



/*------------------------------------------------------------------------------------*/

/*
 * Use the JSurfaceWorks methods to read a text file into this trimesh.
 */
  public void loadAsciiDebugFile(
    String fileName
  ) throws IOException {

    JSurfaceWorks jsw = new JSurfaceWorks ();

    TriMesh tmesh =
    jsw.readTriMeshFile(fileName, 0l);

    if (flipFlag == true) {
      double    xt;
      for (int i=0; i<tmesh.numNodes; i++) {
        xt = tmesh.nodeZArray[i];
        tmesh.nodeZArray[i] = tmesh.nodeYArray[i];
        tmesh.nodeYArray[i] = xt;
      }
    }
    else if (flipFlag2 == true) {
      double    xt;
      for (int i=0; i<tmesh.numNodes; i++) {
        xt = tmesh.nodeZArray[i];
        tmesh.nodeZArray[i] = tmesh.nodeXArray[i];
        tmesh.nodeXArray[i] = xt;
      }
    }

    if (tmesh == null) {
        return;
    }

    tmesh.transfer (this);

    return;
  }



/*------------------------------------------------------------------------------------*/

/**
 Clean up the native side resources associated with this trimesh.
 */
  public void cleanupNativeResources ()
  {
    JSurfaceWorks.removeTriangleIndex(this);
  }


/*------------------------------------------------------------------------------------*/

/*
 The finalize method needs to make sure the native resources for the trimesh
 are cleaned up, but it cannot do it directly since the finalize is called
 from a different thread than the event dispatch thread.  The different
 thread can and probably will corrupt static variables on the native side
 and cause a program crash.
 */
  protected void finalize() throws Throwable {
    JSurfaceWorks.addFinalizedTriMesh (this);
    super.finalize();
  }

/*------------------------------------------------------------------------------------*/

/*
 * These methods are used for debugging only.
 */
  private double[]    xCenter, yCenter, zCenter;

  public void calcTriangleCenters ()
  {
    xCenter = new double[numTriangles];
    yCenter = new double[numTriangles];
    zCenter = new double[numTriangles];

    int       i, e1, e2, n1, n2, n3;
    double    x1, y1, z1, x2, y2, z2, x3, y3, z3;

    for (i=0; i<numTriangles; i++) {
      e1 = triangleEdge0Array[i];
      e2 = triangleEdge1Array[i];
      n1 = edgeNode0Array[e1];
      n2 = edgeNode1Array[e1];
      n3 = edgeNode0Array[e2];
      if (n3 == n1  ||  n3 == n2) {
        n3 = edgeNode1Array[e2];
      }
      x1 = nodeXArray[n1];
      y1 = nodeYArray[n1];
      z1 = nodeZArray[n1];
      x2 = nodeXArray[n2];
      y2 = nodeYArray[n2];
      z2 = nodeZArray[n2];
      x3 = nodeXArray[n3];
      y3 = nodeYArray[n3];
      z3 = nodeZArray[n3];

      xCenter[i] = (x1 + x2 + x3) / 3.0;
      yCenter[i] = (y1 + y2 + y3) / 3.0;
      if (z1 > 1.e20  ||  z2 > 1.e20  ||  z3 > 1.e20) {
        zCenter[i] = 0.0;
      }
      else {
        zCenter[i] = (z1 + z2 + z3) / 3.0;
      }
    }
  }

  public int getNumCenters ()
  {
    return numTriangles;
  }

  public double[] getXCenters ()
  {
    return xCenter;
  }

  public double[] getYCenters ()
  {
    return yCenter;
  }

  public double[] getZCenters ()
  {
    return zCenter;
  }

/*---------------------------------------------------------------------*/

  public void writeTextFile (String fname)
  {
    JSurfaceWorks jsw = new JSurfaceWorks ();
    jsw.writeTextTriMeshFile(this, fname);
  }

  public void writeTextFile ()
  {
    JSurfaceWorks jsw = new JSurfaceWorks ();
    jsw.writeTextTriMeshFile(this, "java_tmesh.tri");
  }


    public long getUniqueID ()
	{
		StaticID++;
		return StaticID;
	}


/*---------------------------------------------------------------------*/

}  // end of TriMesh class
