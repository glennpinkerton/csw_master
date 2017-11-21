
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.uistuff;

import java.net.URL;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import javax.swing.ImageIcon;
import javax.swing.JButton;

import csw.jutils.src.TranslationManager;

/**
 * This class just has static functions to make icons needed for the
 * jeasyx package.  
 */
public final class JEasyXIcons {

  /**
   * This class is not meant to be instantiated, so the constructor is
   * made private and does nothing.
   */
    private JEasyXIcons() {
    }

  /*
   * Build a list of strings for the various files needed for 
   * easyx icons.
   */
    public static final String PAN = new String(
      "Pan24.gif"
    );
    public static final String PRINT = new String(
      "Print24.gif"
    );
    public static final String REFRESH = new String(
      "Refresh24.gif"
    );
    public static final String SELECT = new String(
      "Select24.gif"
    );
    public static final String UNSELECT = new String(
      "Unselect24.gif"
    );
    public static final String ZOOM = new String(
      "Zoom24.gif"
    );
    public static final String ZOOM_FULL = new String(
      "ZoomFull24.gif"
    );
    public static final String ZOOM_IN = new String(
      "ZoomIn24.gif"
    );
    public static final String ZOOM_OUT = new String(
      "ZoomOut24.gif"
    );

    public static final String LIGHTING_OPTIONS = new String(
      "ZoomFull24.gif"
    );
    public static final String GLOBAL_OPTIONS = new String(
      "ZoomFull24.gif"
    );

// for now (may 2017) the rest of the strings are not used
// but I set them to compile
    public static final String SET_HOME = new String(
      "agt_home.gif"
    );
    public static final String GO_TO_HOME = new String(
      "go_home.gif"
    );
    public static final String VIEW_POSITION = new String(
      "ZoomOut24.gif"
    );
    public static final String ORTHOGONAL = new String(
      "ZoomIn24.gif"
    );
    public static final String PERSPECTIVE = new String(
      "ZoomIn24.gif"
    );
    public static final String ZSCALE = new String(
      "ZoomFull24.gif"
    );
    public static final String COPY_BITMAP = new String(
      "ZoomFull24.gif"
    );
    public static final String WHATS_THIS = new String(
      "ZoomFull24.gif"
    );
    public static final String AXIS_LABELS = new String(
      "ZoomFull24.gif"
    );

  /**
   * Return the URL associated with the desired image file.
   *
   * @param type the type of the desired image
   * @return a URL object that points to an image file
   * @throws IndexOutOfBoundsException if type is invalid
   */
  public static URL getURL(String type) throws IndexOutOfBoundsException {
    URL url = JEasyXIcons.class.getResource(type);
    return(url);
  }

  /**
   * Return the ImageIcon associated with the desired type.
   *
   * @param type the type of the desired image
   * @return a previously-created static ImageIcon
   * @throws IndexOutOfBoundsException if type is invalid
   */
  public static ImageIcon getImageIcon(String type) throws IndexOutOfBoundsException {
    return(new ImageIcon(getURL(type)));
  }

  /**
   * Create a new JButton and add the image referred to by
   * index to the button.
   *
   * @param type the type of the desired image
   * @return a new instance of a JButton, with an IconImage
   */
  public static JButton createJButton(String type) {
    JButton button = new JButton();
    try {
      button.setIcon(getImageIcon(type));
    }
    catch (IndexOutOfBoundsException e) {
      e.printStackTrace();
    }
    return(button);
  }


  /**
   * Create a new JButton with the specified text string drawn on the button.
   *
   * @param text the text to draw on the button
   * @return a new instance of a JButton, with the specified text
   */
  public static JButton createJButtonFromText(String text) {
    JButton button = new JButton();
    button.setText (TranslationManager.translate (text));
    return(button);
  }


}
