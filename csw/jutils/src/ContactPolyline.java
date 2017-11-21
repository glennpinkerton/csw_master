
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;

import java.util.Iterator;

/**
 * Represents a contact polyine generated by the sealing and fault connect
 * processes.
 */
public class ContactPolyline 
{

    private boolean hangingWall = false;
    private boolean footWall = false;
    private boolean detachment = false;
    private long    patch1ID;
    private long    patch2ID;
    private XYZPolyline polyline;

  /**
   * Constructor
   *
   * @param title title to use
   * @param size XYZPolyline size
   */
    public ContactPolyline(String title, int size) {
        polyline = new XYZPolyline(size);
    }

  /**
   * Copy constructor
   *
   * @param contactPolyline ContactPolyline to copy
   */
    public ContactPolyline(ContactPolyline contactPolyline) {
      hangingWall = contactPolyline.hangingWall;
      footWall = contactPolyline.footWall;
      detachment = contactPolyline.detachment;
      patch1ID = contactPolyline.patch1ID;
      patch2ID = contactPolyline.patch2ID;
      polyline = contactPolyline.polyline;
    }

  /**
   * Gets the geometry of this contact line as a {@link XYZPolyline}.
   */
    public XYZPolyline getPolyline() {
        return polyline;
    }

  /**
   * @return true if this contact polyline represents the hangingWall-wall
   * side of the fault, false if it is not the hanging wall.
   */
    public boolean isHangingWall() {
        return hangingWall;
    }

  /**
   * @param b value of true means that this polyline is on the hangingWall wall
   * side of a fault, false means it is not on the hanging wall side.
   */
    public void setHangingWall(boolean b) {
        hangingWall = b;
    }

  /**
   * @return true if this contact polyline represents the hangingWall-wall
   * side of the fault, false if it is not the hanging wall.
   */
    public boolean isFootWall() {
        return footWall;
    }

  /**
   * @param b value of true means that this polyline is on the hangingWall wall
   * side of the fault, false means it is not on the hanging wall side.
   */
    public void setFootWall(boolean b) {
        footWall = b;
    }

  /**
   Set this to true if the contact is between a fault and its detachment.
   By default this is false.
  */
    public void setDetachment (boolean b)
    {
        detachment = b;
    }

  /**
  Returns true if the contact is between a fault and its detachment
  or false otherwise.
  */
    public boolean isDetachment ()
    {
        return detachment;
    }

  /**
  Set the unique id values for the two patches that share this contact line.
  The first and second patch id's are arbitrary.
  */
    public void setPatchIDs (long id1, long id2)
    {
        patch1ID = id1;
        patch2ID = id2;
    }

    public void setPatch1ID (long id)
    {
        patch1ID = id;
    }

    public void setPatch2ID (long id)
    {
        patch2ID = id;
    }

  /**
  Return the unique id of a the first patch that the contact line is shared by.
  First and second patches are arbitrary.
  */
    public long getPatch1ID ()
    {
        return patch1ID;
    }

  /**
  Return the unique id of a the second patch that the contact line is shared by.
  First and second patches are arbitrary.
  */
    public long getPatch2ID ()
    {
        return patch2ID;
    }

}
