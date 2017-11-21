
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;

import java.awt.Insets;

import javax.swing.JButton;
import javax.swing.JToolBar;

/**
 * Utility functions for UI components used in any csw code.
 * This is a static class that will never be instantiated.
 *
 * @author Glenn Pinkerton
 */
public final class CSW_UI 
{

    private CSW_UI () {};

    public static void setToolbarLookAndFeel (JToolBar tbar)
    {
        tbar.setMargin (new Insets(2, 2, 2, 2));
        tbar.setRollover (true);
    }

    public static void setToolbarButtonLookAndFeel (JButton button)
    {
        button.setMargin (new Insets(2, 2, 2, 2));
    }

    public static void setFeatureUnimplemented (JButton button)
    {
        button.setEnabled (false);
    }

}

