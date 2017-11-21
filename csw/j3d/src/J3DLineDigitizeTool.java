
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

/**
 * Line (or point) digitizing.  Used to create a new line or point, or
 * extend an existing one.
 *
 * @author Glenn Pinkerton
 */
public class J3DLineDigitizeTool extends J3DLineTool {

  private boolean isAppendMode = true;

  /**
   * Constructor for a new line.
   * @param panel
   * @param jgl
   * @param possibleTypes possible types for digitizing
   */
  protected J3DLineDigitizeTool(
    J3DPanel panel,
    JGL jgl,
    DigitizingType possibleTypes
  ) {
    super(panel, jgl, possibleTypes);
    DigitizingType type = possibleTypes;
    if (possibleTypes == DigitizingType.POLYLINE_OR_POLYGON)
      type = DigitizingType.POLYLINE;
    this.setDigitizingType(type);

  }

  /**
   * Constructor from a {@link J3DLineEditTool}.
   * @param editTool
   * @param isAppendMode true for append mode, false for prepend mode
   */
  protected J3DLineDigitizeTool(
    J3DLineEditTool editTool,
    boolean isAppendMode
  ) {
    super(editTool.panel, editTool.jgl, editTool.getPossibleDigitizingTypes());
    setDigitizingType(editTool.getDigitizingType());
    this.editPointList = editTool.editPointList;
    this.isAppendMode = isAppendMode;
    removeAllLineEditListeners();
    addLineEditListener(
      editTool.lineEditListenerList.get(0)
    );
  }

  /*
   *  (non-Javadoc)
   * @see csw.j3d.src.J3DTool#drawComponents()
   */
  protected void drawComponents() {
    int n, i;
    J3DFault fp;
    J3DHorizonPatch hp;

    jgl.SetDrawDepthTest(1);
    jgl.UpdateMatrices();
    jgl.SetDrawLighting(0);
    jgl.SetDrawShading(0);

    panel.totalTriangles = 0;

    panel.opaqueFlag = true;

    /*
     * Draw opaque faults.
     */
    panel.initNames();
    panel.pushName(J3DConst.FAULT_SURFACE_NAME);
    panel.pushName(0);
    if (panel.faultList != null) {
      n = panel.faultList.size();
      for (i = 0; i < n; i++) {
        fp = panel.faultList.get(i);
        if (fp == null ||
        // If a draping surface patch has already been selected,
            // then only include it in selection (name stack).
            (drapingSurfacePatch != null && fp != drapingSurfacePatch)) {
          continue;
        }
        if (fp.isVisible() == false) {
          continue;
        }
        panel.loadName(i);
        panel.drawFaultSurface(fp);
      }
    }

    /*
     * Draw opaque horizons.
     */

    if (panel.horizonList != null) {
      panel.initNames();
      panel.pushName(J3DConst.HORIZON_SURFACE_NAME);
      panel.pushName(0);
      n = panel.horizonList.size();
      for (i = 0; i < n; i++) {
        hp = panel.horizonList.get(i);
        if (hp == null ||
        // If a draping surface patch has already been selected,
            // then only include in selection (name stack).
            (drapingSurfacePatch != null && hp != drapingSurfacePatch)) {
          continue;
        }
        if (hp.isVisible() == false) {
          continue;
        }
        panel.loadName(i);
        panel.drawHorizonSurface(hp);
      }
    }

    panel.opaqueFlag = false;
    /*
     * Draw transparent faults.
     */
    if (panel.faultList != null) {
      panel.initNames();
      panel.pushName(J3DConst.FAULT_SURFACE_NAME);
      panel.pushName(0);
      n = panel.faultList.size();
      for (i = 0; i < n; i++) {
        fp = panel.faultList.get(i);
        if (fp == null ||
        // If a draping surface patch has already been selected,
            // then only include it in selection (name stack).
            (drapingSurfacePatch != null && fp != drapingSurfacePatch)) {
          continue;
        }
        if (fp.isVisible() == false) {
          continue;
        }
        panel.loadName(i);
        panel.drawFaultSurface(fp);
      }
    }

    /*
     * Draw transparent horizons.
     */
    if (panel.horizonList != null) {
      panel.initNames();
      panel.pushName(J3DConst.HORIZON_SURFACE_NAME);
      panel.pushName(0);
      n = panel.horizonList.size();
      for (i = 0; i < n; i++) {
        hp = panel.horizonList.get(i);
        if (hp == null ||
        // If a draping surface patch has already been selected,
            // then only include in selection (name stack).
            (drapingSurfacePatch != null && hp != drapingSurfacePatch)) {
          continue;
        }
        if (hp.isVisible() == false) {
          continue;
        }
        panel.loadName(i);
        panel.drawHorizonSurface(hp);
      }
    }

  }

  /*
   * (non-Javadoc)
   *
   * @see csw.j3d.src.J3DTool#processB1LinePick(boolean, boolean,
   *      csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB1LinePick(
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  ) {
    return false;
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
      addEditPoint(pickedX, pickedY, pickedZ);
      if (isPointMode())
        finishLinePick();
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
    return false;
  }

  /* (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB2HorizonPick(csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB2HorizonPick(
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

    pickedHorizon = hp;
    pickedTriangleIndex = names[3];
    pickedX = pt.x;
    pickedY = pt.y;
    pickedZ = pt.z;

    boolean flag = removeEditPoint(pickedX, pickedY, pickedZ);
    if (isLineMode()) {
      calcDrapedTempLine();
    }
    return flag;
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
  protected boolean processB3LinePick(boolean aflag, boolean cflag,
      JPoint3D near, JPoint3D far, int[] names) {
    return false;
  }

  /* (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB3HorizonPick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB3HorizonPick(boolean aflag, boolean cflag,
      JPoint3D near, JPoint3D far, int[] names) {
    callRightClickListeners();
    return true;
  }

  /* (non-Javadoc)
   * @see csw.j3d.src.J3DTool#processB3FaultPick(boolean, boolean, csw.j3d.src.JPoint3D, csw.j3d.src.JPoint3D, int[])
   */
  protected boolean processB3FaultPick(boolean aflag, boolean cflag,
      JPoint3D near, JPoint3D far, int[] names) {
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

  private void addEditPoint(double x, double y, double z) {
    JPoint3D p = new JPoint3D(x, y, z);

    // Only change the draping surface for a new line or point.
    if (drapingSurfacePatch == null) {
      if (pickedFault != null) {
        drapeTriMesh = pickedFault.triMesh;
        drapingSurfacePatch = pickedFault;
      } else if (pickedHorizon != null) {
        drapeTriMesh = pickedHorizon.triMesh;
        drapingSurfacePatch = pickedHorizon;
      }
    }

    if (drapeTriMesh == null) {
      return;
    }

    if (isAppendMode) {
      editPointList.add(p);
    } else {
      editPointList.add(0, p);
    }


    callUpdateListeners();

    if (isLineMode()) {
      calcDrapedTempLine();
    }

    return;
  }

}
