/*
 */

package csw.j3d.src;

/**
This interface provides a method to query whether the 3d text should be
drawn in its position.  It can be used for some rudimentary overposting
avoidance when drawing text to the 3d display.
*/
interface TextPositionFilter 
{

/**
This method is called by the JGL DrawTextXYZ method that uses the text
anchor position.  If the bounding box is not in a position that you want to
draw (for example it is inside the cube for axis labels) then this method
should return false.  If it is ok to draw, the method should return true.
*/
    public boolean checkTextPosition (
        double    x1,
        double    y1,
        double    x2,
        double    y2);

}  // end of interface definition
