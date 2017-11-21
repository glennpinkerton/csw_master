
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

/**
 An instance of this class provides the information associated with a
 {@link J3DSelectListener} object.  Information includes report on the
 latest selection change and lists of all currently selected objects.
<p>
 An instance of this object can only be created by a class within the
 csw.j3d.src package.  The rest of the world can only read
 the information contained here.
*/
public class J3DSelectionChangedInfo
{

  /**
  The last changed object was unselected.
  */
    public static final int SELECTED = 1;

  /**
  The last changed object was selected.
  */
    public static final int UNSELECTED = 2;

  /**
  The last changed object is a {@link J3DFault}.
  */
    public static final int FAULT = 1;

  /**
  The last changed object is a {@link J3DHorizonPatch}.
  */
    public static final int HORIZON = 2;

  /**
  The last changed object is a {@link J3DLine}.
  */
    public static final int LINE = 3;

  /**
  The last changed object is a yet to be implemented J3DPoint.
  */
    public static final int POINT = 4;

/*
 * members have package scope so they can be directly
 * accessed by the J3DPanel class.
 */
    double    xPick;
    double    yPick;
    double    zPick;

    int       xScreen;
    int       yScreen;

    ArrayList<J3DFault> selectedFaults;
    ArrayList<J3DHorizonPatch> selectedHorizons;
    ArrayList<J3DLine> selectedLines;

/*
 * Points are not yet implemented.
    ArrayList selectedPoints;
 */

    Object    changedObject;
    int       changedObjectType;
    int       changeAction;


/*----------------------------------------------------------------------*/

/*
 * The constructor has package scope.
 */
    J3DSelectionChangedInfo ()
    {
        xPick = 1.e30;
        yPick = 1.e30;
        zPick = 1.e30;
    }

/*----------------------------------------------------------------------*/

  /**
  Return the world x coordinate for the mouse click that triggered
  the most recent selection change.  If the change was not triggered by
  a mouse click, the xPick is 1.e30.
  */
    public double getXPick ()
    {
        return xPick;
    }


/*----------------------------------------------------------------------*/

  /**
  Return the world y coordinate for the mouse click that triggered
  the most recent selection change.  If the change was not triggered by
  a mouse click, the yPick is 1.e30.
  */
    public double getYPick ()
    {
        return yPick;
    }


/*----------------------------------------------------------------------*/

  /**
  Return the world z coordinate for the mouse click that triggered
  the most recent selection change.  If the change was not triggered by
  a mouse click, the zPick is 1.e30.
  */
    public double getZPick ()
    {
        return zPick;
    }

/*----------------------------------------------------------------------*/

  /**
  Return the screen x coordinate for the mouse click.
  */
    public int getXScreen ()
    {
        return xScreen;
    }


/*----------------------------------------------------------------------*/

  /**
  Return the screen y coordinate for the mouse click.
  */
    public int getYScreen ()
    {
        return yScreen;
    }

/*-----------------------------------------------------------------------*/

  /**
  Return an {@link ArrayList}<J3DFault> object with the currently selected faults.
  If the list is null or empty, no faults are currently selected.  All
  the values in the list will be {@link J3DFault} objects.
  */
    public ArrayList<J3DFault> getSelectedFaults ()
    {
        return selectedFaults;
    }

/*-----------------------------------------------------------------------*/

  /**
  Return an {@link ArrayList}<J3DHorizonPatch> object with the currently selected horizons.
  If the list is null or empty, no horizons are currently selected.  All
  the values in the list will be {@link J3DHorizonPatch} objects.
  */
    public ArrayList<J3DHorizonPatch> getSelectedHorizons ()
    {
        return selectedHorizons;
    }

/*-----------------------------------------------------------------------*/

  /**
  Return an {@link ArrayList}<J3DLine> object with the currently selected lines.
  If the list is null or empty, no lines are currently selected.  All the
  values in the list will be {@link J3DLine} objects.
  */
    public ArrayList<J3DLine> getSelectedLines ()
    {
        return selectedLines;
    }

/*-----------------------------------------------------------------------*/

  /**
  Points are not yet implemented.
  Return an {@link ArrayList} object with the currently selected points.
  If the list is null or empty, no points are currently selected.  The
  values in the list will be yet to be implemented J3DPoint objects.
    public ArrayList getSelectedPoints ()
    {
        return selectedPoints;
    }
  */

/*----------------------------------------------------------------------*/

  /**
  Return the object that changed select state to trigger this listener
  call.  You need to also call {@link #getChangedObjectType} to know if
  the object is a fault, horizon, line or point.
  */
    public Object getChangedObject ()
    {
        return changedObject;
    }

/*-----------------------------------------------------------------------*/

  /**
  Return the type of the object that changed select state to trigger the
  call to this listener.  Types can be {@link #FAULT}, {@link #HORIZON},
  {@link #LINE} or {@link #POINT}.
  */
    public int getChangedObjectType ()
    {
        return changedObjectType;
    }

/*-----------------------------------------------------------------------*/

  /**
  Return the action that triggered this listener call.  The action is either
  {@link #SELECTED} or {@link #UNSELECTED}.
  */
    public int getChangeAction ()
    {
        return changeAction;
    }

/*-----------------------------------------------------------------------*/

}

