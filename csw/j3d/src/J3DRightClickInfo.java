
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;


/**
 An instance of this class provides the information associated with a
 {@link J3DRightClickListener} object.  Information includes the object
 that is clicked on and the location of the click.
<p>
 An instance of this object can only be created by a class within the
 csw.j3d.src package.  The rest of the world can only read
 the information contained here.
*/
public class J3DRightClickInfo
{

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

    /**
     * The last changed object is a point on a {@link J3DLine}
     */
    public static final int LINE_POINT = 5;

    /**
     * The last changed object is a line segment on a {@link J3DLine}
     */
    public static final int LINE_LINE = 6;

    /**
     * The last changed object is a {@link J3DExternalData}
     */
    public static final int EXTERNAL = 7;

/*
 * members have package scope so they can be directly
 * accessed by the J3DPanel class.
 */
    double    xPick;
    double    yPick;
    double    zPick;

    int       xScreen;
    int       yScreen;

    Object    clickedObject;
    int       clickedObjectType;

    int       lineIndex;
    int       pointIndex;

/*----------------------------------------------------------------------*/

/*
 * The constructor has package scope.
 */
    J3DRightClickInfo ()
    {
        xPick = 1.e30;
        yPick = 1.e30;
        zPick = 1.e30;
        lineIndex = -1;
        pointIndex = -1;
    }

/*----------------------------------------------------------------------*/

  /**
  Return the world x coordinate for the mouse click.
  */
    public double getXPick ()
    {
        return xPick;
    }


/*----------------------------------------------------------------------*/

  /**
  Return the world y coordinate for the mouse click.
  */
    public double getYPick ()
    {
        return yPick;
    }


/*----------------------------------------------------------------------*/

  /**
  Return the world z coordinate for the mouse click.
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

/*----------------------------------------------------------------------*/

  /**
  Return the object that was under the mouse when the right click occurred.
  You also need to call the {@link #getClickedObjectType} method to find
  out if the object is a fault, horizon, line or point.  If this is null,
  the right click was not over an object in the view.
  */
    public Object getClickedObject ()
    {
        return clickedObject;
    }

/*-----------------------------------------------------------------------*/

  /**
  Return the type of the object that was under the mouse when the right
  click occurred.
  Types can be
  {@link #FAULT}, {@link #HORIZON}, {@link #LINE}, {@link #POINT},
  {@link #LINE_POINT}, or {@link #LINE_LINE}.
  */
    public int getClickedObjectType ()
    {
        return clickedObjectType;
    }

/*-----------------------------------------------------------------------*/

    /**
     * Return the line index for the mouse click.
     */
    public int getLineIndex()
    {
        return lineIndex;
    }

    /**
     * Return the point index for the mouse click.
     */
    public int getPointIndex() {
      return pointIndex;
    }
}

