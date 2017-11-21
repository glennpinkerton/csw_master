
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
import java.util.ArrayList;
import java.util.Iterator;

import csw.jutils.src.XYZPoint;

import csw.jsurfaceworks.src.JSurfaceWorks;
import csw.jsurfaceworks.src.TriMesh;
import csw.jutils.src.XYZPolyline;

/**
 * Line (or Point) editing.
 *
 * @author Glenn Pinkerton
 */
public class J3DLineEditTool extends J3DLineTool {
  boolean dragPickScheduled = false;
  boolean dragPointMode = false;
  boolean dragLineMode = false;

  /**
   * Constructor for line mode.
   * @param panel
   * @param jgl
   * @param lineEditListener the line edit listener
   * @param pointList the line's points in program coordinates
   * @param triMesh the draping trimesh.
   * @param drapingPatch
   */
  protected J3DLineEditTool(
    J3DPanel panel,
    JGL jgl,
    J3DLineEditListener lineEditListener,
    ArrayList<XYZPoint> pointList,
    TriMesh triMesh,
    J3DSurfacePatch drapingPatch,
    DigitizingType possibleTypes
  ) {
    super(panel, jgl, possibleTypes);
    this.drapingSurfacePatch = drapingPatch;
    removeAllLineEditListeners();
    addLineEditListener(lineEditListener);
    drapeTriMesh = triMesh;
    Iterator<XYZPoint> it = pointList.iterator();
    while (it.hasNext()) {
      XYZPoint xyzPoint = it.next();
      JPoint3D j3dPoint =
        new JPoint3D(xyzPoint.getX(), xyzPoint.getY(), xyzPoint.getZ());
      editPointList.add(j3dPoint);
    }
    calcDrapedTempLine();
    DigitizingType type;
    if (isEditPointsClosed()) {
      type = DigitizingType.POLYGON;
    } else {
      type = DigitizingType.POLYLINE;
    }
    setDigitizingType(type);
  }

  /**
   *
   * Constructor for point mode.
   *
   * @param panel
   * @param jgl
   * @param lineEditListener the line edit listener
   * @param xyzPoint the line's point in program coordinates
   * @param triMesh the draping trimesh.
   * @param drapingPatch
   */
  protected J3DLineEditTool(
    J3DPanel panel,
    JGL jgl,
    J3DLineEditListener lineEditListener,
    XYZPoint xyzPoint,
    TriMesh triMesh,
    J3DSurfacePatch drapingPatch
  ) {
    super(panel, jgl, DigitizingType.POINT);
    setDigitizingType(DigitizingType.POINT);
    this.drapingSurfacePatch = drapingPatch;
    removeAllLineEditListeners();
    addLineEditListener(lineEditListener);
    drapeTriMesh = triMesh;
    JPoint3D j3dPoint =
        new JPoint3D(xyzPoint.getX(), xyzPoint.getY(), xyzPoint.getZ());
    editPointList.add(j3dPoint);
  }

  /**
   * Constructor from a {@link J3DLineDigitizeTool}.
   */
  protected J3DLineEditTool(
    J3DLineDigitizeTool digitizeTool
  ) {
    super(
      digitizeTool.panel,
      digitizeTool.jgl,
      digitizeTool.getPossibleDigitizingTypes()
    );
    this.drapingSurfacePatch = digitizeTool.drapingSurfacePatch;
    removeAllLineEditListeners();
    addLineEditListener(digitizeTool.lineEditListenerList.get(0));
    drapeTriMesh = digitizeTool.drapeTriMesh;
    this.editPointList = digitizeTool.editPointList;
    calcDrapedTempLine();
    DigitizingType type;
    if (isEditPointsClosed()) {
      type = DigitizingType.POLYGON;
    } else {
      type = DigitizingType.POLYLINE;
    }
    setDigitizingType(type);
  }

  /*
   *  (non-Javadoc)
   * @see csw.j3d.src.J3DTool#drawComponents()
   */
  protected void drawComponents() {
    {
      int              n, i;
      J3DFault    fp;
      J3DHorizonPatch  hp;

      jgl.SetDrawDepthTest (1);
      jgl.UpdateMatrices ();
      jgl.SetDrawLighting (0);
      jgl.SetDrawShading (0);

      panel.totalTriangles = 0;

      panel.opaqueFlag = true;

     /* If editing a line, don't include horizons or faults in selection
      * (name stack), unless dragging the point or line.
      */
      if (isDragMode()) {
          /*
           *  Draw opaque faults.
           */
        panel.initNames ();
        panel.pushName (J3DConst.FAULT_SURFACE_NAME);
        panel.pushName (0);
          if (panel.faultList != null) {
              n = panel.faultList.size();
              for (i=0; i<n; i++) {
                  fp = panel.faultList.get(i);
                  if (
                    fp == null ||
                    // If a draping surface patch has already been selected,
                    // then only include it in selection (name stack).
                    (
                       drapingSurfacePatch != null &&
                       fp != drapingSurfacePatch
                    )
                  ) {
                      continue;
                  }
                  if (fp.isVisible() == false) {
                      continue;
                  }
                  panel.loadName (i);
                  panel.drawFaultSurface (fp);
              }
          }

          /*
           * Draw opaque horizons.
           */


          if (panel.horizonList != null) {
            panel.initNames ();
            panel.pushName (J3DConst.HORIZON_SURFACE_NAME);
            panel.pushName (0);
              n = panel.horizonList.size();
              for (i=0; i<n; i++) {
                  hp = panel.horizonList.get(i);
                  if (
                    hp == null ||
                    // If a draping surface patch has already been selected,
                    // then only include in selection (name stack).
                    (
                       drapingSurfacePatch != null &&
                       hp != drapingSurfacePatch
                    )
                  ) {
                      continue;
                  }
                  if (hp.isVisible() == false) {
                      continue;
                  }
                  panel.loadName (i);
                  panel.drawHorizonSurface (hp);
              }
          }

          panel.opaqueFlag = false;
         /*
          *  Draw transparent faults.
          */
          if (panel.faultList != null) {
            panel.initNames ();
            panel.pushName (J3DConst.FAULT_SURFACE_NAME);
            panel.pushName (0);
              n = panel.faultList.size();
              for (i=0; i<n; i++) {
                  fp = panel.faultList.get(i);
                  if (
                      fp == null ||
                      // If a draping surface patch has already been selected,
                      // then only include it in selection (name stack).
                      (
                         drapingSurfacePatch != null &&
                         fp != drapingSurfacePatch
                      )
                  ) {
                      continue;
                  }
                  if (fp.isVisible() == false) {
                      continue;
                  }
                  panel.loadName (i);
                  panel.drawFaultSurface (fp);
              }
          }

         /*
          * Draw transparent horizons.
          */
          if (panel.horizonList != null) {
            panel.initNames ();
            panel.pushName (J3DConst.HORIZON_SURFACE_NAME);
            panel.pushName (0);
              n = panel.horizonList.size();
              for (i=0; i<n; i++) {
                  hp = panel.horizonList.get(i);
                  if (
                      hp == null ||
                      // If a draping surface patch has already been selected,
                      // then only include in selection (name stack).
                      (
                         drapingSurfacePatch != null &&
                         hp != drapingSurfacePatch
                      )
                  ) {
                      continue;
                  }
                  if (hp.isVisible() == false) {
                      continue;
                  }
                  panel.loadName (i);
                  panel. drawHorizonSurface (hp);
              }
          }
      }

      if ( !isDragMode()) {
           drawEditPoints ();
      }
    }

  }

  /*
   *  (non-Javadoc)
   * @see csw.j3d.src.J3DTool#clearPickInfo()
   */
  protected void clearPickInfo() {
//     Don't clear if dragging a point or line.
    if (isDragMode())
        return;
    super.clearPickInfo();
  }

  /* (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB1LinePick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB1LinePick(
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  ) {
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

  // If in editing mode, don't go further unless a drag pick has
  // been scheduled.
  if (!dragPickScheduled) {
      return false;
  }

  J3DLine lp = null;
  if (activeLineEditPoints != null) {
      lp = activeLineEditPoints;
  } else {
      lp = panel.lineList.get (names[1]);
  }
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

  pickedX = pt.x;
  pickedY = pt.y;
  pickedZ = pt.z;

  if (aflag == false) {
    if (isLineMode()) {
      pickedPoint = findEditPoint(pt.x, pt.y, pt.z);
      if (pickedPoint != null) {
        pickedX = pickedPoint.x;
        pickedY = pickedPoint.y;
        pickedZ = pickedPoint.z;
        startDragMode();
      } else {
        LineDraw3D l3d = new LineDraw3D(lp.originalLine);
        l3d.setScaler(panel.scaler);
        int index = l3d.findLineSegment(near, far, panel.getTolerance(2));
        if (index >= 0) {
          pickedX = pt.x;
          pickedY = pt.y;
          pickedZ = pt.z;
          XYZPoint p = new XYZPoint(pickedX, pickedY, pickedZ);
          ArrayList <XYZPoint> list = new ArrayList<XYZPoint>();
          list.add(p);
          JSurfaceWorks jsw = new JSurfaceWorks();
          list = jsw.calcDrapedPointsFromPoints (drapeTriMesh, list);
          if (list != null && list.size() > 0) {
            pickedZ = list.get(0).getZ();
          }
          pickedLineIndex = index;
          pickedPoint = new JPoint3D(pickedX, pickedY, pickedZ);
          startDragMode();
        }
      }
      endDragPickMode();
    } else if (isPointMode()) {
      pickedPoint = findEditPoint(pt.x, pt.y, pt.z);
      if (pickedPoint != null) {
        pickedX = pickedPoint.x;
        pickedY = pickedPoint.y;
        pickedZ = pickedPoint.z;
        startDragMode();
      }
      endDragPickMode(); // drag pick completed
    }
  }

  panel.hideSelectedLines();
  return true;
  }

  /*
   * (non-Javadoc)
   *
   * @see csw.j3d.src.J3DTool#processB1HorizonPick(boolean, boolean,
   *      csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB1HorizonPick(
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  ) {
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

    pickedTriangleIndex = names[3];
    pickedX = pt.x;
    pickedY = pt.y;
    pickedZ = pt.z;

    if (aflag == false) {
      replaceEditPoint(pickedX, pickedY, pickedZ);
    }

    return true;
  }

  /* (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB1FaultPick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB1FaultPick(
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  ) {
    /* For now don't allow dragging of points onto a fault.
     * Currently boundary lines and fault lines can only be digitized
     * on horizons. This will have to be revisited when a line can be
     * digitized on a fault
     */
    if (isDragMode()) {
      endDragMode();
      return false;
    }
    return false;
  }

  /* (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB2LinePick(csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB2LinePick(
    JPoint3D near,
    JPoint3D far,
    int[] names
  ) {
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

    J3DLine lp = null;
    if (activeLineEditPoints != null) {
      lp = activeLineEditPoints;
    } else {
      lp = panel.lineList.get(names[1]);
    }
    if (lp == null) {
      return false;
    }

    JPoint3D pt = findPickedPointOnLine(lp, near, far);
    if (pt == null)
      return false;

    clearPickInfo();

    pickedLine = lp;
    pickedX = pt.x;
    pickedY = pt.y;
    pickedZ = pt.z;


    boolean flag = removeEditPoint(pickedX, pickedY, pickedZ);
    return flag;
  }

  /* (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB2HorizonPick(csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB2HorizonPick(
    JPoint3D near,
    JPoint3D far,
    int[] names
  ) {
    return false;
  }

  /* (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB2FaultPick(csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB2FaultPick(
    JPoint3D near,
    JPoint3D far,
    int[] names
  ) {
    return false;
  }

  /* (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB3LinePick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB3LinePick(
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  ) {
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

    J3DLine lp = null;
    if (activeLineEditPoints != null) {
      lp = activeLineEditPoints;
    } else {
      lp = panel.lineList.get(names[1]);
    }
    if (lp == null) {
      return false;
    }

    JPoint3D pt = findPickedPointOnLine(lp, near, far);
    clearPickInfo();
    if (pt == null)
      return false;


    if (isLineMode()) {
        pickedPoint = findEditPoint(pt.x, pt.y, pt.z);
        if (pickedPoint != null) {
          pickedX = pickedPoint.x;
          pickedY = pickedPoint.y;
          pickedZ = pickedPoint.z;
        } else {
          LineDraw3D l3d = new LineDraw3D(lp.originalLine);
          l3d.setScaler(panel.scaler);
          int index = l3d.findLineSegment(near, far, panel.getTolerance(2));
          if (index >= 0) {
            pickedX = pt.x;
            pickedY = pt.y;
            pickedZ = pt.z;
            XYZPoint p = new XYZPoint(pickedX, pickedY, pickedZ);
            ArrayList <XYZPoint> list = new ArrayList<XYZPoint>();
            list.add(p);
            JSurfaceWorks jsw = new JSurfaceWorks();
            list = jsw.calcDrapedPointsFromPoints(drapeTriMesh, list);
            if (list != null && list.size() > 0) {
              pickedZ = list.get(0).getZ();
            }
            pickedLineIndex = index;
          }
        }
    } else { /* Point mode */
        pickedPoint = findEditPoint(pt.x, pt.y, pt.z);
        if (pickedPoint != null) {
          pickedX = pickedPoint.x;
          pickedY = pickedPoint.y;
          pickedZ = pickedPoint.z;
        }
    }

    if (aflag == false) {
      pickedLine = lp;
    }
    callRightClickListeners();

    return true;
  }

  /*
   * (non-Javadoc)
   *
   * @see csw.j3d.src.J3DTool#processB3HorizonPick(boolean, boolean,
   *      csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB3HorizonPick(
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  ) {
    return false;
  }

  /* (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB3FaultPick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB3FaultPick(
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  ) {
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

  }

  /**
   * Draw the lines and points currently being edited.
   */
  private void drawEditPoints() {

    int npoint;

    panel.initNames();
    panel.pushName(J3DConst.LINE3D_NAME);
    panel.pushName(0);

    npoint = editPointList.size();

    if (npoint == 0) {
      return;
    }

    int n = editPointList.size();
    double[] xArray = new double[n];
    double[] yArray = new double[n];
    double[] zArray = new double[n];
    for (int index = 0; index < n; index++) {
      JPoint3D pt = editPointList.get(index);
      xArray[index] = pt.x;
      yArray[index] = pt.y;
      zArray[index] = pt.z;
    }
    XYZPolyline polyline = new XYZPolyline(xArray, yArray, zArray);

    activeLineEditPoints = new J3DLine();
    activeLineEditPoints.setLine(polyline);
    if (isPointMode()) {
      activeLineEditPoints.setDrawAsPoints(1);
    } else { /* line mode */
      activeLineEditPoints.setDrapedLineList(cacheDrapeList);
    }
    /*
     * If the background is dark, draw in white.  If it is
     * light, draw in black.
     */
    activeLineEditPoints.setGlobalColor(panel
        .createContrastColor(panel.backgroundColor));
    panel.drawJ3DLine(activeLineEditPoints);
    return;

  } // end of updateEdit method

  private JPoint3D findPickedPointOnLine(
    J3DLine lp,
    JPoint3D near,
    JPoint3D far
  ) {
    LineDraw3D l3d;
    JPoint3D pt = new JPoint3D();
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
      size = lp.drapedLineList.size();
      for (int i = 0; i < size; i++) {
        XYZPolyline atmp = lp.drapedLineList.get(i);
        if (atmp == null) {
          continue;
        }
        l3d = new LineDraw3D(atmp);
        l3d.setScaler(panel.scaler);
        istat = l3d.calcXYZHit(near, far, pt, panel.getTolerance(2));
        if (istat == 1) {
          break;
        }
      }
    }

    // pick from the original line, if available and no draped line
    else if (lp.originalLine != null) {
      l3d = new LineDraw3D(lp.originalLine);
      l3d.setScaler(panel.scaler);
        istat = l3d.calcXYZHit(near, far, pt, panel.getTolerance(2));
    }

    // Try to find an edit point, if in editing mode.
    // Original line is fine, since we are looking for an edit point.
    // Edit points are bigger, so require a slightly higher tolerance.
    l3d = new LineDraw3D(lp.originalLine);
    l3d.setScaler(panel.scaler);
    int jstat = l3d.calcXYZHit(near, far, pt, panel.getTolerance(4));
    if (jstat == 1) {
      JPoint3D editPoint = findEditPoint(pt.x, pt.y, pt.z);
      if (editPoint != null) {
        return editPoint;
      }
    }

    // Edit point not found, maybe it is a end point.  These can
    // require a larger tolerance.
    if (isLineMode()) {
      jstat = l3d.calcXYZHit(near, far, pt);
      if (jstat == 1) {
        JPoint3D editPoint = findEditPoint(pt.x, pt.y, pt.z, 8);
        int index = editPointList.indexOf(editPoint);
        if (index == 0 || index == editPointList.size()) {
          return editPoint;
        }
      }
    }

    // If a line point is found return it.  If it is an edit point it
    // has already been returned.
    if (istat == 1)
      return pt;

    // Nothing found
    return null;
  }


  private void replaceEditPoint(double x, double y, double z) {
    endDragMode();
    if (pickedPoint == null)
        return;
    if (pickedLineIndex >= 0) {
      pickedPoint = insertEditPoint(
          pickedPoint.x,
          pickedPoint.y,
          pickedPoint.z,
          pickedLineIndex
      );
    }

    double origX = pickedPoint.x;
    double origY = pickedPoint.y;

    XYZPoint p = new XYZPoint(x, y, z);
    ArrayList<XYZPoint> list = new ArrayList<XYZPoint> ();
    list.add(p);
    JSurfaceWorks jsw = new JSurfaceWorks();
    list = jsw.calcDrapedPointsFromPoints(drapeTriMesh, list);
    if (list != null && list.size() > 0) {
        pickedPoint.x = x;
        pickedPoint.y = y;
        pickedPoint.z = list.get(0).getZ();
    }  else {
        return;
    }

    // If editing a line and moving an edit point, check and make
    // sure the other end point doesn't need to be moved also.
    if (
      pickedLineIndex < 0 &&
      getDigitizingType() == DigitizingType.POLYGON &&
      editPointList.size() > 0
    ) {
        JPoint3D endPoint = editPointList.get(0);
        if (endPoint.x == origX && endPoint.y == origY) {
            endPoint.x = pickedPoint.x;
            endPoint.y = pickedPoint.y;
            endPoint.z = pickedPoint.z;
        } else {
            endPoint =
              editPointList.get(editPointList.size() - 1);
            if (endPoint.x == origX && endPoint.y == origY) {
                endPoint.x = pickedPoint.x;
                endPoint.y = pickedPoint.y;
                endPoint.z = pickedPoint.z;
            }
        }
    }

    callUpdateListeners();
    clearPickInfo();
    if (isLineMode()) {
        calcDrapedTempLine();
    }
  }

/*--------------------Drag Methods--------------------------------------*/

  boolean isDragMode(){
      return (dragLineMode || dragPointMode);
  }

  void startDragPickMode() {
    dragPickScheduled = true;
  }

  void endDragPickMode() {
    dragPickScheduled = false;
  }

  void startDragMode() {
    if (isLineMode()) {
      dragLineMode = true;
    } else { /* point mode */
      dragPointMode = true;
    }
  }

  void endDragMode() {
    dragLineMode = false;
    dragPointMode = false;
  }

  void processLineDrag(MouseEvent e) {
  }

  void processPointDrag(MouseEvent e) {
  }

/*--------------------------Public methods---------------------------------*/

  /**
   * Returns true if the point on the line can be deleted, false if not.
   */
  public boolean shouldAllowLinePointDelete() {
    // Never allow delete for point editing.
    if (isPointMode())
      return false;
    if (getDigitizingType() == DigitizingType.POLYGON) {
      return (editPointList.size() > 4);
    }
    if (getDigitizingType() == DigitizingType.POLYLINE) {
      return (editPointList.size() > 2);
    }
    return true;
  }

  public JPoint3D insertEditPoint(double x, double y, double z, int index) {
    if (index < 0)
      return null;
    JPoint3D point = new JPoint3D(x, y, z);
    editPointList.add(index, point);
    return point;
  }

  public boolean removeEditPoint (double x, double y, double z) {

      if (!shouldAllowLinePointDelete()) {
          return false;
      }

      boolean status = super.removeEditPoint(x, y, z);
      if (!status) {
        return status;
      }

      // Is it an end point?
      if (isLineMode()) {
          JPoint3D point = findEditPoint(x, y, z);
          if (point != null) {
              // It is an end point, remove the other side.
              editPointList.remove(point);
          }
          // Nothing left, start over.
          if (editPointList.size() == 0) {
            clearEditPointsAndLines();
          }
          calcDrapedTempLine();
      }
      return true;
  }

  public void splitPolygon(
    double x,
    double y,
    double z,
    int lineIndex,
    int pointIndex
  ) {
    if (getDigitizingType() != DigitizingType.POLYGON) {
      return;
    }
    // Make sure that there is enought information to split.
    if (lineIndex == -1 && pointIndex == -1)
      return;
    // Build the new edit point list.
    ArrayList<JPoint3D> newEditPointList = new ArrayList<JPoint3D>();
    int index;
    JPoint3D lastPoint;
    if (lineIndex != -1) {
      index = lineIndex;
      newEditPointList.add(new JPoint3D(x, y, z));
      lastPoint = new JPoint3D(x, y, z);
    } else {
      index = pointIndex;
      JPoint3D firstPoint = editPointList.get(pointIndex);
      lastPoint = new JPoint3D(firstPoint.x, firstPoint.y, firstPoint.z);
    }
    int n = editPointList.size() - 1;
    for (int i = index; i < n; i++) {
      newEditPointList.add(editPointList.get(i));
    }
    for (int i = 0; i < index; i++) {
      newEditPointList.add(editPointList.get(i));
    }
    newEditPointList.add(lastPoint);
    editPointList = newEditPointList;
    setDigitizingType(DigitizingType.POLYLINE);
    panel.redrawAllImmediately();
  }

}
