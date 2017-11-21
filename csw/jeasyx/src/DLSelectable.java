
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

import java.util.ArrayList;

/**
 This class is used to connect graphical picture elements with
 the data used to produce them.  It also acts as a collection
 mechanism for primitives that refer to the same application data.
 When drawing into a {@link JDisplayList} object, the application
 sets a DLSelectable and then draws primitives.  All of the primitives
 drawn after seting the DLSelectable and up to changing the DLSelectable
 are implicitly grouped together.  
 */

public class DLSelectable {

  /*
   * The variables have package scope so the JDisplayList can
   * access them directly.
   */
    Object                  applicationData;
    ArrayList<DLLine>       lineList;
    ArrayList<DLFill>       fillList;
    ArrayList<DLText>       textList;
    ArrayList<DLSymb>       symbList;
    ArrayList<DLArc>        arcList;
    ArrayList<DLRectangle>  rectangleList;
    ArrayList<DLContour>    contourList;
    ArrayList<DLAxis>       axisList;

    int             nativeIndex;

    boolean         isSelected;

/**
 Create an empty DLSelectable object.
 */
    public DLSelectable () {
        nativeIndex = -1;
        isSelected = false;
    }

    /**
    Return true if this selectable is currently selected or false if
    it is not currently selected.
    */
    public boolean getIsSelected ()
    {
        return isSelected;
    }

    /**
    Return the pointer to the application data object.  The application
    will probably need to cast this to the actual object type.
     */
    public Object getApplicationData() {
        return applicationData;
    }

    /**
    Get the list of DLContour objects in this selectable.  If there are no contours,
    null is returned.
     */
    public ArrayList<DLContour> getContourList() {
        return contourList;
    }

    /**
    Get the list of DLAxis objects in this selectable.  If there are no axes,
    null is returned.
     */
    public ArrayList<DLAxis> getAxisList() {
        return axisList;
    }

    /**
    Get the list of DLFill objects in this selectable.  If there are no fills, 
    null is returned.
     */
    public ArrayList<DLFill> getFillList() {
        return fillList;
    }

    /**
    Get the list of DLLine objects in this selectable.  If there are no lines, 
    null is returned.
     */
    public ArrayList<DLLine> getLineList() {
        return lineList;
    }

    /**
    Get the list of DLArc objects in this selectable.  If there are no arcs, 
    null is returned.
     */
    public ArrayList<DLArc> getArcList() {
        return arcList;
    }

    /**
    Get the list of DLRectangle objects in this selectable.  If there are no rectangles, 
    null is returned.
     */
    public ArrayList<DLRectangle> getRectangleList() {
        return rectangleList;
    }

    /**
    Get the list of DLSymb objects in this selectable.  If there are no symbols, 
    null is returned.
     */
    public ArrayList<DLSymb> getSymbList() {
        return symbList;
    }

    /**
    Get the list of DLText objects in this selectable.  If there are no texts, 
    null is returned.
     */
    public ArrayList<DLText> getTextList() {
        return textList;
    }

    /**
     Set the application data object connected with this selectable.  All of the
     graphic primitives associated with the selecftable should be createable from
     this data object.
     * @param object Application data object.
     */
    public void setApplicationData(Object object) {
        applicationData = object;
    }

    /*
     * The methods used to set primitive objects into the selectable
     * have package scope.  The application cannot add primitives
     * to the selectable via these functions.
     */

    /*
     * The methods to get and set the native index are also package scope.
     */
    int getNativeIndex ()
    {
        return nativeIndex;
    }

    void setNativeIndex (int i)
    {
        nativeIndex = i;
    }

}
    
