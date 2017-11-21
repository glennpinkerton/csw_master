
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

import csw.jutils.src.XYZPolyline;

import csw.jsurfaceworks.src.JSurfaceWorks;
import csw.jsurfaceworks.src.TriMesh;

/**
 * Common functionality for line digitizing and editing.
 *
 * @author Glenn Pinkerton
 */
public abstract class J3DLineTool extends J3DTool {
//
//  private boolean isLineMode;

  private DigitizingType type;
  private DigitizingType possibleTypes;

  protected ArrayList<JPoint3D>   editPointList = new ArrayList<JPoint3D> ();
  protected J3DLine               activeLineEditPoints = null;

  protected ArrayList<XYZPolyline>   cacheDrapeList = null;

  protected TriMesh           drapeTriMesh = null;
  protected J3DSurfacePatch   drapingSurfacePatch = null;

  ArrayList<J3DLineEditListener> lineEditListenerList = 
    new ArrayList<J3DLineEditListener> (1);
  private boolean         motionInterest = false;
  private boolean         updateInterest = true;

  /**
   * Constructor
   * @param panel
   * @param jgl
   * @param possibleTypes possible types for digitizing and editing
   */
  protected J3DLineTool(J3DPanel panel, JGL jgl, DigitizingType possibleTypes) {
    super(panel, jgl);
    this.possibleTypes = possibleTypes;
  }

/*------------------------------------------------------------------------------------------*/

  protected void calcDrapedTempLine() {
    if (type == DigitizingType.POINT) {
      return;
    }
    int npoint = editPointList.size();

    if (npoint < 2) {
      cacheDrapeList = null;
      return;
    }

    ArrayList<XYZPolyline> lalist = new ArrayList<XYZPolyline> (1);
    XYZPolyline la = new XYZPolyline(npoint);
    double[] xa = la.getXArray();
    double[] ya = la.getYArray();
    double[] za = la.getZArray();

    JPoint3D p;
    for (int i = 0; i < npoint; i++) {
      p = editPointList.get(i);
      xa[i] = p.x;
      ya[i] = p.y;
      za[i] = p.z;
    }
    lalist.add(la);

    JSurfaceWorks jsw = new JSurfaceWorks();
    cacheDrapeList = jsw.calcDrapedLines(drapeTriMesh, lalist);
  }

  protected J3DLine updateEditLine(int minSize) {
    J3DLine line;

    /*
     * update for line picking mode.
     */
    if (type != DigitizingType.POINT) {
      int size = editPointList.size();
      if (size < minSize) {
        return null;
      }

      XYZPolyline la = new XYZPolyline(size);

      double[] xa = la.getXArray();
      double[] ya = la.getYArray();
      double[] za = la.getZArray();

      JPoint3D pt;

      for (int i = 0; i < size; i++) {
        pt = editPointList.get(i);
        xa[i] = pt.x;
        ya[i] = pt.y;
        za[i] = pt.z;
      }

      line = new J3DLine();

      if (pickedLine != null) {
        line.shallowCopy(pickedLine);
      }

      line.setLine(la);
      line.drapedLineList = null;
      line.drapingSurface = null;
      if (drapingSurfacePatch != null) {
        line.drapingSurface = drapingSurfacePatch;
        line.drapedLineList = cacheDrapeList;
      }

      /*
       * Make sure the line has a non null color, or it will
       * not be displayed.
       */
      if (line.getGlobalColor() == null) {

        /*
         * If the background is dark, draw in white.  If it is
         * light, draw in black.
         */
        line.setGlobalColor(panel.createContrastColor(panel.backgroundColor));

      }
    }

    /*
     * Update for point picking mode.
     */
    else {
      int size = editPointList.size();
      if (size < 1) {
        return null;
      }

      XYZPolyline la = new XYZPolyline(size);

      double[] xa = la.getXArray();
      double[] ya = la.getYArray();
      double[] za = la.getZArray();

      JPoint3D pt;

      for (int i = 0; i < size; i++) {
        pt = editPointList.get(i);
        xa[i] = pt.x;
        ya[i] = pt.y;
        za[i] = pt.z;
      }

      line = new J3DLine();

      line.setLine(la);
      line.drapedLineList = null;
      line.drapingSurface = null;
      if (drapingSurfacePatch != null) {
        line.drapingSurface = drapingSurfacePatch;
        line.drapedLineList = cacheDrapeList;
      }

      /*
       * Make sure the line has a non null color, or it will
       * not be displayed.
       */
      if (line.getGlobalColor() == null) {

        /*
         * If the background is dark, draw in white.  If it is
         * light, draw in black.
         */
        line.setGlobalColor(panel.createContrastColor(panel.backgroundColor));

      }
    }

    return line;
  }

  protected JPoint3D findEditPoint(double x, double y, double z) {
    return findEditPoint(x, y, z, 5);
  }

  protected JPoint3D findEditPoint(double x, double y, double z,
      int toleranceFactor) {
    int i, n;
    double dx, dy, dz, d1, d2;
    double dmin, dt;
    int imin;
    JPoint3D p;

    n = editPointList.size();
    if (n < 1) {
      return null;
    }

    // Is the point one of the edit points?
    imin = -1;
    dmin = 1.e30;
    for (i = 0; i < n; i++) {
      p = editPointList.get(i);
      dx = p.x - x;
      dy = p.y - y;
      dz = p.z - z;
      d1 = dx * dx + dy * dy + dz * dz;
      d1 = Math.sqrt(d1);
      if (d1 < dmin) {
        dmin = d1;
        imin = i;
      }
    }

    dt = panel.getTolerance(toleranceFactor);
    if (dmin <= dt && imin >= 0) {
      pickedPointIndex = imin;
      return (editPointList.get(imin));
    }
    return null;
  }

  protected boolean isEditPointsClosed() {
    JPoint3D firstPoint = editPointList.get(0);
    int last = editPointList.size() - 1;
    JPoint3D lastPoint = editPointList.get(last);
    if (
      firstPoint.x == lastPoint.x &&
      firstPoint.y == lastPoint.y
    ) {
      return true;
    }
    return false;
  }

  protected void setDigitizingType(DigitizingType type) {
    if (type == DigitizingType.POLYLINE_OR_POLYGON) {
      throw new IllegalStateException("type must be one of the single types");
    }
    this.type = type;
  }

/*--------------------------Public methods-----------------------------------*/

  public DigitizingType getDigitizingType() {
    return type;
  }

  public DigitizingType getPossibleDigitizingTypes() {
    return possibleTypes;
  }

  public boolean removeEditPoint(double x, double y, double z) {
    JPoint3D point = findEditPoint(x, y, z);
    if (point == null) {
      return false;
    }

    editPointList.remove(point);
    callUpdateListeners();
    return true;

  }

  /**
   * Return the number of edit points currently displayed.
   */
  public int getNumEditPoints() {
    if (editPointList != null) {
      return editPointList.size();
    }
    return 0;
  }

  /**
   Set the current active line pick to the finished state.  This is
   meant to be used by editors that need to get the results of the current
   line pick back in order to calculate a surface.  It is also used internally
   when a line pick is finished by the user.
   */
  public void finishLinePick() {
    callEndListeners();
    clearEditPointsAndLines();
    panel.localRepaint();
  }

  /**
   * Remove all edit points and lines from the display.
   */
  public void clearEditPointsAndLines() {
    // Clear out everything that has to do with digitizing.
    if (editPointList != null) {
      editPointList.clear();
    }

    drapingSurfacePatch = null;
    drapeTriMesh = null;
    activeLineEditPoints = null;
  }

  /**
   * Removes the last digitized point.
   */
  public void removeLastDigitizedPoint() {
    int n;

    n = editPointList.size();
    // Nothing on list, so can't remove last point
    if (n < 1) {
      return;
      // One point on list, so start over
    } else if (n == 1) {
      clearEditPointsAndLines();
      return;
    }
    // remove the last point
    editPointList.remove(n - 1);

    if (type != DigitizingType.POINT) {
      calcDrapedTempLine();
    }
  }

  public boolean isLineMode() {
    return (type != DigitizingType.POINT);
  }

  public boolean isPointMode() {
    return (type == DigitizingType.POINT);
  }

  public void closeLine() {
    if (editPointList.size() < 1) {
      return;
    }
    JPoint3D firstPoint = editPointList.get(0);
    JPoint3D lastPoint = new JPoint3D(firstPoint.x, firstPoint.y, firstPoint.z);
    editPointList.add(lastPoint);
    setDigitizingType(DigitizingType.POLYGON);
    calcDrapedTempLine();
    panel.redrawAllImmediately();
  }

  public boolean isAppendMode(int index) {
    if (index < 0) {
      return false;
    }
    int lastIndex = editPointList.size() - 1;
    if (index == lastIndex && !isEditPointsClosed()) {
      return true;
    }
    return false;
  }

  public boolean isPrependMode(int index) {
    if (index == 0 && !isEditPointsClosed()) {
      return true;
    }
    return false;
  }

/*-----------------------Line Edit Listeners------------------------------*/
  public void setMotionInterest (boolean bval)
  {
      motionInterest = bval;
  }

  public void setUpdateInterest (boolean bval)
  {
      updateInterest = bval;
  }

  public boolean getMotionInterest ()
  {
      return motionInterest;
  }

  public boolean getUpdateInterest ()
  {
      return updateInterest;
  }

  public void addLineEditListener (J3DLineEditListener listener)
  {
      int index = lineEditListenerList.indexOf (listener);
      if (index >= 0) {
          return;
      }

      lineEditListenerList.add (listener);
  }

  public void removeLineEditListener (J3DLineEditListener listener)
  {
      int index = lineEditListenerList.indexOf (listener);
      if (index < 0) {
          return;
      }

      lineEditListenerList.remove (index);

      return;
  }

  public void removeAllLineEditListeners ()
  {

      lineEditListenerList.clear ();

      return;
  }

  protected void callMotionListeners ()
  {
      if (motionInterest == false) {
          return;
      }

      int                    i, size;
      J3DLineEditListener    el;

      J3DLine line = updateEditLine (2);
      if (line == null) {
          return;
      }

      size = lineEditListenerList.size ();
      for (i=0; i<size; i++) {
          el = lineEditListenerList.get (i);
          if (el == null) {
              continue;
          }
          el.lineMoved (line);
      }
  }

  protected void callUpdateListeners ()
  {
      if (updateInterest == false) {
          return;
      }

      int                    i, size;
      J3DLineEditListener    el;

      J3DLine line = updateEditLine (1);
      if (line == null) {
          return;
      }

      size = lineEditListenerList.size ();
      for (i=0; i<size; i++) {
          el = lineEditListenerList.get (i);
          if (el == null) {
              continue;
          }
          el.lineChanged (line);
      }
  }

  protected void callEndListeners ()
  {
      int                    i, size;
      J3DLineEditListener    el;

      J3DLine line = updateEditLine (2);
      if (line == null) {
          return;
      }
      if (drapingSurfacePatch instanceof J3DHorizonPatch) {
          line.type = ((J3DHorizonPatch) drapingSurfacePatch).pickedLineType;
      }

      size = lineEditListenerList.size ();
      for (i=0; i<size; i++) {
          el = lineEditListenerList.get (i);
          if (el == null) {
              continue;
          }
          el.lineFinished (line);
      }
  }

  protected void callCompletelyFinishedListeners ()
  {
      int                    i, size;
      J3DLineEditListener    el;

      J3DLine line = updateEditLine (2);
      if (line == null) {
          return;
      }

      size = lineEditListenerList.size ();
      for (i=0; i<size; i++) {
          el = lineEditListenerList.get (i);
          if (el == null) {
              continue;
          }
          el.editCompletelyFinished ();
      }

  }

}
