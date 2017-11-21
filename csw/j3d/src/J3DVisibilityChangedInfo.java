/*
 */

package csw.j3d.src;

import java.util.ArrayList;

/**
 An instance of this class provides the information associated with a
 {@link J3DVisibleListener} object.  Information includes lists of visible
 and invisible objects.
<p>
 An instance of this object can only be created by a class within the
 csw.j3d.src package.  The rest of the world can only read
 the information contained here.
*/
public class J3DVisibilityChangedInfo
{

/*
 * members have package scope so they can be directly
 * accessed by the J3DPanel class.
 */
    ArrayList<J3DFault>    visibleFaults;
    ArrayList<J3DHorizonPatch>  visibleHorizons;
    ArrayList<J3DLine>     visibleLines;

    ArrayList<J3DFault>    invisibleFaults;
    ArrayList<J3DHorizonPatch>  invisibleHorizons;
    ArrayList<J3DLine>     invisibleLines;

/*
 * Points are not yet implemented.
    ArrayList visiblePoints;
    ArrayList invisiblePoints;
 */


/*----------------------------------------------------------------------*/

/*
 * The constructor has package scope.
 */
    J3DVisibilityChangedInfo ()
    {
    }

/*-----------------------------------------------------------------------*/

  /**
  Return an {@link ArrayList} object with the currently visible faults.
  If the list is null or empty, no faults are currently visible.  All
  the values in the list will be {@link J3DFault} objects.
  */
    public ArrayList<J3DFault> getVisibleFaults ()
    {
        return visibleFaults;
    }

/*-----------------------------------------------------------------------*/

  /**
  Return an {@link ArrayList} object with the currently visible horizons.
  If the list is null or empty, no horizons are currently visible.  All
  the values in the list will be {@link J3DHorizonPatch} objects.
  */
    public ArrayList<J3DHorizonPatch> getVisibleHorizons ()
    {
        return visibleHorizons;
    }

/*-----------------------------------------------------------------------*/

  /**
  Return an {@link ArrayList} object with the currently visible lines.
  If the list is null or empty, no lines are currently visible.  All the
  values in the list will be {@link J3DLine} objects.
  */
    public ArrayList<J3DLine> getVisibleLines ()
    {
        return visibleLines;
    }

/*-----------------------------------------------------------------------*/

  /**
  Points are not yet implemented.
  Return an {@link ArrayList} object with the currently visible points.
  If the list is null or empty, no points are currently visible.  The
  values in the list will be yet to be implemented J3DPoint objects.
    public ArrayList getVisiblePoints ()
    {
        return visiblePoints;
    }
  */

/*-----------------------------------------------------------------------*/

  /**
  Return an {@link ArrayList} object with the currently ininvisible faults.
  If the list is null or empty, no faults are currently invisible.  All
  the values in the list will be {@link J3DFault} objects.
  */
    public ArrayList<J3DFault> getInvisibleFaults ()
    {
        return invisibleFaults;
    }

/*-----------------------------------------------------------------------*/

  /**
  Return an {@link ArrayList} object with the currently invisible horizons.
  If the list is null or empty, no horizons are currently invisible.  All
  the values in the list will be {@link J3DHorizonPatch} objects.
  */
    public ArrayList<J3DHorizonPatch> getInvisibleHorizons ()
    {
        return invisibleHorizons;
    }

/*-----------------------------------------------------------------------*/

  /**
  Return an {@link ArrayList} object with the currently invisible lines.
  If the list is null or empty, no lines are currently invisible.  All the
  values in the list will be {@link J3DLine} objects.
  */
    public ArrayList<J3DLine> getInvisibleLines ()
    {
        return invisibleLines;
    }

/*-----------------------------------------------------------------------*/

  /**
  Points are not yet implemented.
  Return an {@link ArrayList} object with the currently invisible points.
  If the list is null or empty, no points are currently invisible.  The
  values in the list will be yet to be implemented J3DPoint objects.
    public ArrayList getInvisiblePoints ()
    {
        return invisiblePoints;
    }
  */

/*-----------------------------------------------------------------------*/

}

