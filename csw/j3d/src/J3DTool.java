/*
 */
package csw.j3d.src;

import java.awt.event.InputEvent;
import java.awt.event.MouseEvent;
import java.util.Date;

/**
 * J3DPanel tool for selection, digitizing, and editing.
 *
 * @author Glenn Pinkerton
 */
public abstract class J3DTool {
  protected J3DPanel panel;
  protected JGL jgl;

  J3DFault                      pickedFault;
  J3DHorizonPatch               pickedHorizon;
  J3DLine                       pickedLine;
  JPoint3D                      pickedPoint;
  int                           pickedTriangleIndex;
  double                        pickedX,
                                pickedY,
                                pickedZ;
  int                           pickedScreenX,
                                pickedScreenY;
  int                           pickedLineIndex;
  int                           pickedPointIndex;

  /**
   * Constructor
   *
   */
  protected J3DTool(J3DPanel panel, JGL jgl) {
    super();
    this.panel = panel;
    this.jgl = jgl;
    // TODO Auto-generated constructor stub
  }

  protected void clearPickInfo() {
    pickedFault = null;
    pickedHorizon = null;
    pickedLine = null;
    pickedPoint = null;
    pickedTriangleIndex = -1;
    pickedX = 1.e30;
    pickedY = 1.e30;
    pickedZ = 1.e30;
    pickedLineIndex = -1;
    pickedPointIndex = -1;
  }

  protected boolean pickObject(MouseEvent e, double aspect_ratio) {
    int ix = e.getX();
    int iy = e.getY();

    int size = 5;
    int found = 0;

    int[] names = new int[100];
    int n;

    boolean target = false;

    for (int i = 0; i < 3; i++) {

      /*
       * First, try to select a line object.
       */
      panel.selectionMode = true;
      jgl.MatrixModeProjection();
      jgl.LoadIdentity();
      jgl.SelectModelObjectSetup(
        ix,
        iy,
        2 * size,
        panel.projectionMode,
        aspect_ratio
      );
      panel.linePickFlag = true;
      drawSelect();
      panel.linePickFlag = false;
      jgl.FlushGX();
      jgl.FinishGX();

      /*
       * retrieve the "names" of selected lines.
       */
      found = jgl.SelectModelObject();

      /*
       * No lines were found, so try again with all objects.
       */
      if (found == 0) {
        jgl.MatrixModeProjection();
        jgl.LoadIdentity();
        jgl.SelectModelObjectSetup(ix, iy, size, panel.projectionMode, aspect_ratio);
        drawComponents();
        jgl.FlushGX();
        jgl.FinishGX();
        found = jgl.SelectModelObject();
      }

      panel.initNames();
      panel.selectionMode = false;
      if (found == 0) {
        if (panel.targetZoomFlag) {
          panel.targetDrawFlag = true;
          jgl.MatrixModeProjection();
          jgl.LoadIdentity();
          jgl.SelectModelObjectSetup(ix, iy, size, panel.projectionMode, aspect_ratio);
          drawComponents();
          jgl.FlushGX();
          jgl.FinishGX();
          found = jgl.SelectModelObject();
          panel.targetDrawFlag = false;
          break;
        }
        size *= 2;
        continue;
      }
      break;
    }

    /*
     * If nothing is found even now, return false.
     */
    if (found == 0) {
      processEmptyPick(e);
      return false;
    }

    /*
     * Prior to processing the pick result, set the opengl matrices to the
     * values used for drawing. This is needed so the unproject functions will
     * convert properly from mouse position to xyz coordinates.
     */
    panel.setDrawMatrices();

    n = jgl.GetSelectedStack(names, 100);

    boolean bval = processPickResult(e, names, n);

    return bval;
  }

  private void drawSelect() {
    Date date = new Date();
    long t1 = date.getTime();

    jgl.ClearDepthBuffer();
    //jgl.ClearColorAndDepthBuffers ();

    jgl.MatrixModeModel();
    jgl.LoadIdentity();
    jgl.TranslateD(panel.xTrans, panel.yTrans, panel.zTrans);
    jgl.RotateD(panel.xRot, 1.0, 0.0, 0.0);
    jgl.RotateD(panel.yRot, 0.0, 1.0, 0.0);
    jgl.RotateD(panel.zRot, 0.0, 0.0, 1.0);
    jgl.ScaleD(panel.xScale, panel.yScale, panel.zScale);

    drawComponents();

    if (panel.targetDrawFlag) {
      panel.drawZoomTargets();
    }

    date = new Date();
    long t2 = date.getTime();

    t2 -= t1;

    //System.out.println ("picking time in milliseconds = "+t2);
  }

  private boolean processPickResult(MouseEvent e, int[] names, int n) {
    if (n < 5 || n > 7) {
      return false;
    }

    boolean bval = false;

    JPoint3D near = new JPoint3D();
    JPoint3D far = new JPoint3D();

    pickedScreenX = e.getX();
    pickedScreenY = e.getY();

    /*
     * Get the ray from the view point through the pixel location to the far
     * horizon. This is in cube coordinates. The subsequent processing will
     * translate it back into world coordinates.
     */
    jgl.CalcHitRay(e.getX(), e.getY(), near, far);

    int nclick = e.getClickCount();

    int button_1 = e.getModifiers() & InputEvent.BUTTON1_MASK;
    int button_2 = e.getModifiers() & InputEvent.BUTTON2_MASK;
    int button_3 = e.getModifiers() & InputEvent.BUTTON3_MASK;

    bval = false;
    boolean aflag = e.isAltDown();
    boolean cflag = e.isControlDown();

    clearPickInfo();

    /*
     * Various options for a button 1 click.
     */
    if (button_1 != 0) {
      if (names[0] == J3DConst.FAULT_SURFACE_NAME) {
        bval = processB1FaultPick(aflag, cflag, near, far, names);
      } else if (names[0] == J3DConst.HORIZON_SURFACE_NAME) {
        bval = processB1HorizonPick(aflag, cflag, near, far, names);
      } else if (names[0] == J3DConst.LINE3D_NAME) {
        bval = processB1LinePick(aflag, cflag, near, far, names);
      } else if (names[0] == J3DConst.EXTERNAL_SURFACE_NAME) {
        bval = processB1ExternalPick(aflag, cflag, near, far, names);
      }
      if (bval == false) {
        return false;
      }
    }

    /*
     * Various options for a button 2 click. The alt key seems to set the same
     * modifiers bit as the button 2 key. Thus, a button 2 must be distinguished
     * from an alt button 1 or alt button 3 by making sure the button 1 and
     * button 3 bits are zero.
     */
    if (button_2 != 0 && button_1 == 0 && button_3 == 0) {
      if (nclick == 1) {
        if (names[0] == J3DConst.FAULT_SURFACE_NAME) {
          bval = processB2FaultPick(near, far, names);
        } else if (names[0] == J3DConst.HORIZON_SURFACE_NAME) {
          bval = processB2HorizonPick(near, far, names);
        } else if (names[0] == J3DConst.LINE3D_NAME) {
          bval = processB2LinePick(near, far, names);
        }
        if (bval == false) {
          return false;
        }
      }
    }

    /*
     * Button 3 options.
     */
    if (button_3 != 0) {
      if (nclick == 1) {
        if (names[0] == J3DConst.FAULT_SURFACE_NAME) {
          bval = processB3FaultPick(aflag, cflag, near, far, names);
        } else if (names[0] == J3DConst.HORIZON_SURFACE_NAME) {
          bval = processB3HorizonPick(aflag, cflag, near, far, names);
        } else if (names[0] == J3DConst.LINE3D_NAME) {
          bval = processB3LinePick(aflag, cflag, near, far, names);
        } else if (names[0] == J3DConst.EXTERNAL_SURFACE_NAME) {
          bval = processB3ExternalPick(aflag, cflag, near, far, names);
        }
        if (bval == false) {
          callRightClickListeners();
        }
      }
    }

    return true;
  }

  /*-------------------------Right Click Listeners-----------------------------*/


  protected void callRightClickListeners() {

    int ix, iy, ix2, iy2;

    int size = panel.rightClickListenerList.size();
    if (size < 1) {
      return;
    }

    J3DRightClickInfo info = new J3DRightClickInfo();

    info.xPick = pickedX;
    info.yPick = pickedY;
    info.zPick = pickedZ;

    ix = pickedScreenX;
    iy = pickedScreenY;

    info.xScreen = ix;
    info.yScreen = iy;
    info.lineIndex = pickedLineIndex;
    info.pointIndex = pickedPointIndex;

    if (pickedFault != null) {
      info.clickedObject = pickedFault;
      info.clickedObjectType = J3DRightClickInfo.FAULT;
    } else if (pickedHorizon != null) {
      info.clickedObject = pickedHorizon;
      info.clickedObjectType = J3DRightClickInfo.HORIZON;
    } else if (pickedLine != null) {
      info.clickedObject = pickedLine;
      info.clickedObjectType = J3DRightClickInfo.LINE;
      if (pickedPoint != null) {
        info.clickedObjectType = J3DRightClickInfo.LINE_POINT;
      } else if (pickedLineIndex != -1) {
        info.clickedObjectType = J3DRightClickInfo.LINE_LINE;
      }
    } else {
      info.clickedObject = null;
      info.clickedObjectType = -1;
    }

    J3DRightClickListener rcl;

    for (int i = 0; i < size; i++) {
      rcl = panel.rightClickListenerList.get(i);
      if (rcl != null) {
        rcl.processRightClick(info);
      }
    }

    return;

  }

  protected abstract void drawComponents();

  protected abstract boolean processB1LinePick (
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  );

  protected abstract boolean processB1HorizonPick (
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  );

  protected abstract boolean processB1FaultPick (
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  );

  protected abstract boolean processB2LinePick (
    JPoint3D near,
    JPoint3D far,
    int[] names
  );

  protected abstract boolean processB2HorizonPick (
    JPoint3D near,
    JPoint3D far,
    int[] names
  );

  protected abstract boolean processB2FaultPick (
    JPoint3D near,
    JPoint3D far,
    int[] names
  );

  protected abstract boolean processB3LinePick (
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  );

  protected abstract boolean processB3HorizonPick (
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  );

  protected abstract boolean processB3FaultPick (
    boolean aflag,
    boolean cflag,
    JPoint3D near,
    JPoint3D far,
    int[] names
  );

  protected abstract void processEmptyPick(MouseEvent e);

  protected boolean processB1ExternalPick (
      boolean aflag,
      boolean cflag,
      JPoint3D near,
      JPoint3D far,
      int[] names
  ) {
    return false;
  }

  protected boolean processB3ExternalPick (
      boolean aflag,
      boolean cflag,
      JPoint3D near,
      JPoint3D far,
      int[] names
  ){
    return false;
  }

}
