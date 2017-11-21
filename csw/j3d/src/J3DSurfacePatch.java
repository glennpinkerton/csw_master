
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.util.ArrayList;

import csw.jsurfaceworks.src.TriMesh;

import csw.jutils.src.XYZNode;
import csw.jutils.src.XYZPolyline;

/**
 This is the common base class for surfaces that can be displayed
 on the J3DPanel object.  Known extensions are {@link J3DFault}
 and {@link J3DHorizonPatch}.

*/
public class J3DSurfacePatch extends J3DElement {

/**
 The surface is a horizon.
*/
    public static final int HORIZON = 1;
/**
 The surface is a fault.
*/
    public static final int FAULT = 2;

    protected int   type;

/*
 * members have package scope so they can be directly
 * accessed by the J3DPanel class.
 */
    String          surfaceName;
    int             surfaceID;
    int             patchID;
    private boolean solidFill,
                    wireFrame;
    boolean         lighted;
    boolean         pickable;

    int             wireListID;
    int             solidListID;

  /*
   * The original values.
   */
    ArrayList<XYZNode>       originalNodes;
    ArrayList<XYZPolyline>   originalLineList;
    TriMesh         triMesh;

  /*
   * Cached drawable trimesh
   */
    TriMeshDraw3D   tm3d;

/**
 Create an empty surface patch object for use in the 3D display.  Default
 values are set for all the
 members.  Unless you fill this with data via the various set calls, the
 surface will not be drawn.
*/
  public J3DSurfacePatch () {
    super();
    init();
  }

/**
 Constructor that takes arrays of node x,y,z coordinates and
 arrays of triangle node1, node2 and node3 numbers.  This is
 specifically meant for adding patches via the simulation
 results viewer.
 @param nodeX Array of triangle node x coordinates.
 @param nodeY Array of triangle node y coordinates.
 @param nodeZ Array of triangle node z coordinates.
 @param triNode1 Array of node 1 numbers for each triangle.
 @param triNode2 Array of node 2 numbers for each triangle.
 @param triNode3 Array of node 3 numbers for each triangle.
*/
  public J3DSurfacePatch (
    double[] nodeX,
    double[] nodeY,
    double[] nodeZ,
    int[] triNode1,
    int[] triNode2,
    int[] triNode3)
  {
    int   l1, l2, l3;

    if (nodeX == null  ||  nodeY == null  ||  nodeZ == null  ||
        triNode1 == null  ||  triNode2 == null  ||  triNode3 == null) {
      throw new IllegalArgumentException
        ("null arrays passed to J3DSurfacePatch constructor");
    }

    l1 = nodeX.length;
    l2 = nodeY.length;
    l3 = nodeZ.length;

    if (l1 != l2  ||  l1 != l3) {
      throw new IllegalArgumentException
        ("The x,y,z arrays passed to the J3DSurfacePatch "+
         "constructor are not the same length");
    }

    l1 = triNode1.length;
    l2 = triNode2.length;
    l3 = triNode3.length;

    if (l1 != l2  ||  l1 != l3) {
      throw new IllegalArgumentException
        ("The node number arrays passed to the J3DSurfacePatch "+
         "constructor are not the same length");
    }

    init ();

    tm3d = new TriMeshDraw3D (nodeX, nodeY, nodeZ,
                              triNode1, triNode2, triNode3);

  }


  private void init() {
    surfaceName = null;
    surfaceID = 0;
    patchID = 0;
    triMesh = null;
    solidFill = true;
    wireFrame = false;

    lighted = true;
    pickable = true;

    type = 0;

    tm3d = null;

    originalNodes = null;
    originalLineList = null;

    wireListID = -1;
    solidListID = -1;
  }

/**
 Return the type {@link #HORIZON} or {@link #FAULT} of this surface patch.
*/
    public int getType ()
    {
        return type;
    }

    /**
     Specify the original pre edit nodes.  This should be the nodes used to
     calculate the trimesh.  By default, this is null.
    @param na A {@link ArrayList} object with the points used to calculate the
    trimesh for the surface.
    */
    public void setOriginalNodes (ArrayList<XYZNode> na)
    {
         originalNodes = na;
    }

    /**
     Specify the original pre edit lines.  This should be the lines used to
     calculate the trimesh.  By default, this is null.
    @param la A {@link ArrayList} object with the lines used to calculate the
    trimesh for the surface.
    */
    public void setOriginalLines (ArrayList<XYZPolyline> la)
    {
         originalLineList = la;
    }

    /**
     Return the original pre edit nodes.  This should be the nodes used to
     calculate the original trimesh.  By default, this is null.
    */
    public ArrayList<XYZNode> getOriginalNodes ()
    {
         return originalNodes;
    }

    /**
     Return the original pre edit lines.  This should be the lines used to
     calculate the original trimesh.  By default, this is null.  If both the original
     nodes and original lines are null, the surface will not be editable
     in the 3d display.
    */
    public ArrayList<XYZPolyline> getOriginalLines ()
    {
         return originalLineList;
    }

    /**
    Return the id number of the surface.  If this has not been set,
    zero is returned.
     */
    public int getSurfaceID() {
        return surfaceID;
    }

    /**
    Return the name of the surface.  If this has not been set,
    null is returned.
     */
    public String getSurfaceName() {
        return surfaceName;
    }

    /**
    Return the patch ID of this particular surface patch.  If this has
    not been set, zero is returned.
     */
    public int getPatchID() {
        return patchID;
    }

    /**
    Return true if the patch is to use solid fill triangles for drawing.  Return
    false if no solid fill triangles are to be drawn.
     */
    public boolean isSolidFill() {
        return solidFill;
    }

    /**
    Return the triMesh object used for the patch.  If no triMesh has been set,
    null is returned.
     */
    public TriMesh getTriMesh() {
        return triMesh;
    }

    /**
    Return true if the patch outlines triangles.  Return false if no triangle outline
    is to be done for the patch.
     */
    public boolean isWireFrame() {
        return wireFrame;
    }

    /**
    Set a unique id for the surface.  If this is used, I suspect it should be the
    same for all patches belonging to the same surface.  By default this is zero.
    This is optional.  The surface patch will still be drawn regardless of the id value.
     * @param i  Optional unique identifier for the surface.
     */
    public void setSurfaceID(int i) {
        surfaceID = i;
    }

    /**
    Set the name for the surface.  This is optional, but it is strongly recommended.
    All patches from the same surface should have the same surface name.  By default,
    this is null.  The patch will be drawn regardless of the surface name.
     * @param string
     */
    public void setSurfaceName(String string) {
        surfaceName = string;
    }

    /**
    Set the optional patch id.  If there are several patches for a particular surface,
    you will probably want to uniquely identify each patch.  This is optional.  The
    patch will be drawn regardless of the patch id value.
     * @param i Unique id for the patch, within the surface.
     */
    public void setPatchID(int i) {
        patchID = i;
    }

    /**
    Set whether the triangles are solid filled when drawn.  Set this to true to draw
    the triangles as solid fill colors.  The color set with {@link #setGlobalColor} is
    used for the solid fill.  Set this to false to not draw solid filled triangles.
    You must set this to true and specify a valid color via setGlobalColor to get
    solid filled triangles on the drawing.
     * @param b  Solid fill flag.
     */
    public void setSolidFill(boolean b) {
        solidFill = b;
    }

    /**
    Set the {@link TriMesh} object that has the geometry of the surface patch.  By default,
    this is null, and the patch is not drawn.
     * @param mesh {@link TriMesh} object with the patch geometry.
     */
    public void setTriMesh(TriMesh mesh) {
        if (mesh != triMesh) {
            triMesh = mesh;
            tm3d = null;
            disableLists ();
        }
    }

    /**
    Set whether the triangle outlines are to be drawn.  Set to true to draw the outlines or
    set to false to not draw the outlines.  The color of the outline is automatically
    calculated to contrast with the solid color fill and/or the background color of
    the drawing.
     * @param b Wire frame flag.
     */
    public void setWireFrame(boolean b) {
        wireFrame = b;
    }

  /**
  Disable the display lists associated with this surface.  This should be
  called by the 3d view controller when any property of the surface is changed
  and the change would require a redraw of each point in the surface's trimesh.
  Examples of properties requiring lists to be disabled are: a new trimesh is
  needed and color by attribute changes.  Examples that do
  not need lists disabled include: solid color changes; solidfill or wireframe mode
  changes; selection state changes; name of surface changes.
  */
    public void disableLists ()
    {
        if (wireListID > 0) {
            wireListID = -wireListID;
        }
        if (solidListID > 0) {
            solidListID = -solidListID;
        }
    }

/*--------------------------------------------------------------------------*/

}  // end of class definition
