
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.awt.Color;

/**
 * This interface must be implemented for any callback objects
 * associated with a 3d view object (surfaces, lines, points).
 * The methods provide for directly updating the callback 
 * object's select state, 3d visibility state, the line data 
 * or the point set data.  The J3D viewer does not impose a
 * data structure for any of the data model.  It simply defines
 * this interface which can be used if desired.
 */
public interface J3DCallbackObject
{

/**
 * If the 3d view object that has this callback object changes
 * selection state, this method is called to inform the callback
 * object of the new select state.
 *
 * @param selectState True if the new state is selected or false
 * if the new state is not selected.
 */
    public void J3DSelectionChanged (boolean selectState);

/**
 * If the 3d view object that has this callback object changes
 * visibility in the 3d view, the callback object is informed
 * of the new visibility via this method.
 *
 * @param visibleState True if the new state is visible or false
 * if the new state is not visible.
 */
    public void J3DVisibilityChanged (boolean visibleState);

/**
 * If the points, color or thickness of a line changed, the 
 * callback object is informed of the new state via this method.
 *
 * @param newX  Array with changed values of the line's X coordinates.
 * @param newY  Array with changed values of the line's Y coordinates.
 * @param newZ  Array with changed values of the line's Z coordinates.
 * @param npts  Number of points in the new line.
 * @param newColor  {@link Color} object with the color of the 
 * changed line.
 * @param newThick Thickness of the new line in inches as seen on the screen.
 */
    public void J3DXYZLineChanged (double[] newX,
                                   double[] newY,
                                   double[] newZ,
                                   int npts,
                                   Color newColor,
                                   double newThick);

/**
 * If the points, color or thickness of a point set changed, the 
 * callback object is informed of the new state via this method.
 *
 * @param newX  Array with new values of the point set's X coordinates.
 * @param newY  Array with new values of the point set's Y coordinates.
 * @param newZ  Array with new values of the point set's Z coordinates.
 * @param npts  Number of points in the new line.
 * @param newColor  {@link Color} object with the color of the 
 * changed point set.
 * @param newSize Thickness of the changed point set in inches as seen on the screen.
 */
    public void J3DXYZPointsChanged (double[] newX,
                                     double[] newY,
                                     double[] newZ,
                                     int npts,
                                     Color newColor,
                                     double newSize);

}  // end of the J3DCallbackObject interface definition
