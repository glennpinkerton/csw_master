
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.awt.event.InputEvent;
import java.awt.event.MouseEvent;

import csw.jutils.src.XYZPolyline;

/**
 * Tool to perform selection.
 *
 * @author Glenn Pinkerton
 */
public class J3DSelectionTool extends J3DTool {

/**
  * Constructor
  *
  */
public J3DSelectionTool(J3DPanel panel, JGL jgl)
{
  super(panel, jgl);
}

/* (non-Javadoc)
 * @see csw.j3d.src.J3DTool#drawSelect()
 */
protected void drawComponents()
{
  jgl.SetDrawDepthTest (1);
  jgl.UpdateMatrices ();

  jgl.SetDrawLighting (0);
  jgl.SetDrawShading (0);

  panel.totalTriangles = 0;

  /*
   * Draw the opaque components.
   */
  panel.opaqueFlag = true;
  panel.drawFaults();
  panel.drawHorizons();

  /*
   * Draw the lines.
   */
  panel.drawLines();
}

  /* (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB1LinePick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[], int)
   */
protected boolean processB1LinePick(
  boolean aflag,
  boolean cflag,
  JPoint3D near,
  JPoint3D far,
  int[] names
)
{
  if (panel.lineList == null) {
    return false;
  }

  int size = panel.lineList.size ();
  if (size < 1) {
      return false;
  }

  if (names[1] < 0  ||  names[1] >= size) {
      return false;
  }

  J3DLine lp = panel.lineList.get (names[1]);
  if (lp == null) {
      return false;
  }

  JPoint3D pt = findPickedPointOnLine(lp, near, far);
  clearPickInfo ();
  if (pt == null)
      return false;

  if (aflag == false) {
      pickedLine = lp;
  }

  if (cflag == false) {
    panel.unselectAllObjects();
  }

  pickedX = pt.x;
  pickedY = pt.y;
  pickedZ = pt.z;


  if (panel.targetZoomFlag == false) {
      panel.callSelectListeners ();
  }
  panel.hideSelectedLines();
  return true;
}

/* (non-Javadoc)
 * @see csw.j3d.src.J3DTool#processB1HorizonPick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[], int)
 */
protected boolean processB1HorizonPick(
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  )
{
  if (panel.horizonList == null) {
      return false;
  }

  int size = panel.horizonList.size();
  if (size < 1) {
    return false;
  }

  if (names[1] < 0 || names[1] >= size) {
    return false;
  }

  J3DHorizonPatch hp = panel.horizonList.get(names[1]);
  if (hp == null) {
    return false;
  }

  TriMeshDraw3D tm3d = hp.tm3d;
  if (tm3d == null) {
    return false;
  }

  JPoint3D pt = new JPoint3D();
  int istat = tm3d.calcXYZHit(names[3], near, far, pt);
  if (istat == -1) {
    return false;
  }

  clearPickInfo();

  if (aflag == false) {
    pickedHorizon = hp;
  }

  if (cflag == false) {
    panel.unselectAllObjects();
  }

  pickedTriangleIndex = names[3];
  pickedX = pt.x;
  pickedY = pt.y;
  pickedZ = pt.z;

  if (panel.targetZoomFlag == false) {
    panel.callSelectListeners();
  }

  return true;
}

  /*
   *  (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB1ExternalPick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
protected boolean processB1ExternalPick(
      boolean aflag,
      boolean cflag,
      JPoint3D near,
      JPoint3D far,
      int[] names)
{
  return false;
}

/* (non-Javadoc)
 * @see csw.j3d.src.J3DTool#processB1FaultPick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[], int)
 */
protected boolean processB1FaultPick(
  boolean aflag,
  boolean cflag,
  JPoint3D near,
  JPoint3D far,
  int[] names)
{
  if (panel.faultList == null) {
    return false;
  }

  int size = panel.faultList.size ();
  if (size < 1) {
    return false;
  }

  if (names[1] < 0  ||  names[1] >= size) {
    return false;
  }

  J3DFault fp = panel.faultList.get (names[1]);
  if (fp == null) {
    return false;
  }

  TriMeshDraw3D tm3d = fp.tm3d;
  if (tm3d == null) {
      return false;
  }

  JPoint3D pt = new JPoint3D ();
  int istat = tm3d.calcXYZHit (
      names[3],
      near,
      far,
      pt);
  if (istat == -1) {
      return false;
  }

  clearPickInfo ();

  if (aflag == false) {
      pickedFault = fp;
  }

  if (cflag == false) {
      panel.unselectAllObjects();
  }

  pickedTriangleIndex = names[3];
  pickedX = pt.x;
  pickedY = pt.y;
  pickedZ = pt.z;


  if (panel.targetZoomFlag == false) {
      panel.callSelectListeners ();
  }

  return true;
}

/* (non-Javadoc)
 * @see csw.j3d.src.J3DTool#processB2LinePick(csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[], int)
 */
protected boolean processB2LinePick(
  JPoint3D near,
  JPoint3D far,
  int[] names)
{
  return false;
}

/* (non-Javadoc)
 * @see csw.j3d.src.J3DTool#processB2HorizonPick(csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[], int)
 */
protected boolean processB2HorizonPick(
  JPoint3D near,
  JPoint3D far,
  int[] names)
{
  return false;
}

/* (non-Javadoc)
 * @see csw.j3d.src.J3DTool#processB2FaultPick(csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[], int)
 */
protected boolean processB2FaultPick(
  JPoint3D near,
  JPoint3D far,
  int[] names)
{
  return false;
}

/* (non-Javadoc)
 * @see csw.j3d.src.J3DTool#processB3LinePick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[], int)
 */
protected boolean processB3LinePick(
  boolean aflag,
  boolean cflag,
  JPoint3D near,
  JPoint3D far,
  int[] names)
{
  if (panel.lineList == null) {
    return false;
  }

  int size = panel.lineList.size();
  if (size < 1) {
    return false;
  }

  if (names[1] < 0 || names[1] >= size) {
    return false;
  }

  J3DLine lp = panel.lineList.get(names[1]);
  if (lp == null) {
    return false;
  }

  JPoint3D pt = findPickedPointOnLine(lp, near, far);
  clearPickInfo();
  if (pt == null) {
    return false;
  }

  if (!lp.isSelected()) {
    if (cflag == false) {
      panel.unselectAllObjects();
    }
    panel.callSelectListeners();
  }

  pickedX = pt.x;
  pickedY = pt.y;
  pickedZ = pt.z;

  if (aflag == false) {
    pickedLine = lp;
  }
  callRightClickListeners();

  return true;
}

/* (non-Javadoc)
 * @see csw.j3d.src.J3DTool#processB3HorizonPick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[], int)
 */
protected boolean processB3HorizonPick(
  boolean aflag,
  boolean cflag,
  JPoint3D near,
  JPoint3D far,
  int[] names)
{
  if (panel.horizonList == null) {
    return false;
  }

  int size = panel.horizonList.size();
  if (size < 1) {
    return false;
  }

  if (names[1] < 0 || names[1] >= size) {
    return false;
  }

  J3DHorizonPatch hp = panel.horizonList.get(names[1]);
  if (hp == null) {
    return false;
  }

  TriMeshDraw3D tm3d = hp.tm3d;
  if (tm3d == null) {
    return false;
  }

  JPoint3D pt = new JPoint3D();
  pt.x = 1.e30;
  pt.y = 1.e30;
  pt.z = 1.e30;
  int istat = tm3d.calcXYZHit(names[3], near, far, pt);
  if (istat == -1) {
    return false;
  }

  clearPickInfo();

  if (aflag == false) {
    pickedHorizon = hp;
  }

  if (!hp.isSelected()) {
    if (cflag == false) {
      panel.unselectAllObjects();
    }
    panel.callSelectListeners();
  }

  pickedTriangleIndex = names[3];
  pickedX = pt.x;
  pickedY = pt.y;
  pickedZ = pt.z;

  callRightClickListeners();

  return true;
}

/* (non-Javadoc)
 * @see csw.j3d.src.J3DTool#processB3FaultPick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[], int)
 */
protected boolean processB3FaultPick(
  boolean aflag,
  boolean cflag,
  JPoint3D near,
  JPoint3D far,
  int[] names)
{
  if (panel.faultList == null) {
    return false;
  }

  int size = panel.faultList.size();
  if (size < 1) {
    return false;
  }

  if (names[1] < 0 || names[1] >= size) {
    return false;
  }

  J3DFault fp = panel.faultList.get(names[1]);
  if (fp == null) {
    return false;
  }

  TriMeshDraw3D tm3d = fp.tm3d;
  if (tm3d == null) {
    return false;
  }

  JPoint3D pt = new JPoint3D();
  pt.x = 1.e30;
  pt.y = 1.e30;
  pt.z = 1.e30;
  int istat = tm3d.calcXYZHit(names[3], near, far, pt);
  if (istat == -1) {
    return false;
  }

  clearPickInfo();

  if (aflag == false) {
    pickedFault = fp;
  }

  if (!fp.isSelected()) {
    if (cflag == false) {
      panel.unselectAllObjects();
    }
    panel.callSelectListeners();
  }

  pickedTriangleIndex = names[3];
  pickedX = pt.x;
  pickedY = pt.y;
  pickedZ = pt.z;

  callRightClickListeners();

  return true;
}

/*
 *  (non-Javadoc)
 * @see csw.j3d.src.J3DTool#processB3ExternalPick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
 */
protected boolean processB3ExternalPick(
  boolean aflag,
  boolean cflag,
  JPoint3D near,
  JPoint3D far,
  int[] names)
{
  return false;
}


/* (non-Javadoc)
 * @see csw.j3d.src.J3DTool#processEmptyPick(java.awt.event.MouseEvent)
 */
protected void processEmptyPick(MouseEvent e) {
  clearPickInfo ();
  pickedX =
  pickedScreenX = e.getX();
  pickedY =
  pickedScreenY = e.getY();
  int button_3 = e.getModifiers() & InputEvent.BUTTON3_MASK;
  if (button_3 != 0) {
    callRightClickListeners();
  }
  int button_1 = e.getModifiers() & InputEvent.BUTTON1_MASK;
  if (button_1 != 0 && !e.isControlDown()) {
    panel.unselectAllObjects();
  }
}

private JPoint3D findPickedPointOnLine(
    J3DLine lp,
    JPoint3D near,
    JPoint3D far)
{
  LineDraw3D l3d;
  JPoint3D pt = new JPoint3D ();
  int size;
  int istat = 0;

  // Already found, the selected line is a point.
  if (lp.getLine().getXArray().length == 1) {
    pt.x = lp.getLine().getXArray()[0];
    pt.y = lp.getLine().getYArray()[0];
    pt.z = lp.getLine().getZArray()[0];
    return pt;
  }

  // Pick from the draped line, if available
  if (lp.drapedLineList != null) {
    size = lp.drapedLineList.size ();
    for (int i=0; i<size; i++) {
      XYZPolyline atmp = lp.drapedLineList.get (i);
      if (atmp == null) {
        continue;
      }
      l3d = new LineDraw3D (atmp);
      l3d.setScaler (panel.scaler);
      istat = l3d.calcXYZHit (
          near,
          far,
          pt);
      if (istat == 1) {
        break;
      }
    }
  }

  // pick from the original line, if available and no draped line
  else if (lp.originalLine != null) {
    l3d = new LineDraw3D (lp.originalLine);
    l3d.setScaler (panel.scaler);
    istat = l3d.calcXYZHit (
      near,
      far,
      pt);
  }

  // If a line point is found return it.
  if (istat == 1)
    return pt;

  // Nothing found
    return null;
  }

}
