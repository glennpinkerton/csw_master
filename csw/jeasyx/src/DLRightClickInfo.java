
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
 An instance of this class provides the information associated with a
 {@link DLRightClickListener} object.  Information includes the object
 that is clicked on and the location of the click.
<p>
 An instance of this object can only be created by a class within the
 csw.jeasyx.src package.  The rest of the world can only read
 the information contained here.
*/
public class DLRightClickInfo
{

/*
 * members have package scope so they can be directly
 * accessed by the csw.jeasyx.src package classes.
 */
    double    xPick;
    double    yPick;
    double    zPick;

    int       xScreen;
    int       yScreen;

    ArrayList<DLSelectable> selectableList;

/*----------------------------------------------------------------------*/

/**
 Print out the values in the object.
 */
    public void dump ()
    {
        System.out.println ("xPick = "+xPick);
        System.out.println ("yPick = "+yPick);
        System.out.println ("zPick = "+zPick);
        System.out.println ("xScreen = "+xScreen);
        System.out.println ("yScreen = "+yScreen);

        if (selectableList == null) {
            System.out.println ("Number of Selected Items = 0");
            return;
        }

        int i;
        int n = 0;
        int size = selectableList.size();
        DLSelectable  dls;
        String str;

        for (i=0; i<size; i++) {
            dls = selectableList.get(i);
            if (dls.isSelected) {
                n++;
            }
        }

        System.out.println ("Number of Selected Items = "+n);
        if (n > 0) {
            n = 0;
            for (i=0; i<size; i++) {
                dls = selectableList.get(i);
                if (dls.isSelected) {
                    if (dls.applicationData == null) {
                        str = "No application data available.";
                    }
                    else {
                        str = dls.applicationData.toString();
                    }
                    System.out.println ("Item "+n+": "+str);
                    n++;
                }
            }
        }

    }

/*----------------------------------------------------------------------*/

/*
 * The constructor has package scope.
 */
    DLRightClickInfo ()
    {
        xPick = 1.e30;
        yPick = 1.e30;
        zPick = 1.e30;
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
  Return the world z coordinate for the mouse click.  If the click is on
  a point that has no z value, 1.e30 is returned.
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
  Return the {@link DLSelectable} that was under the mouse when the
  right click was made.
  */
    public ArrayList<DLSelectable> getSelectableList ()
    {
        return selectableList;
    }

/*-----------------------------------------------------------------------*/

}

