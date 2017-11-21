
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.images;

import java.awt.Image;
import java.net.URL;

import javax.swing.ImageIcon;

/**
 * Class with static functions to get fill pattern images 
 * for easyx polygon fills and gui buttons.
 */
public final class FillImages extends Object {

  /**
   * Static class--cannot instantiate
   */
    private FillImages() {
    }

    /**
     * @return a static Image.
     */
    public Image getImage(String imgFilename) {

      URL url = FillImages.class.getResource(imgFilename);
      if (url == null) {
        throw new IllegalArgumentException(
          "Could not find resource for filename:" + imgFilename
        );
	  }

      Image image = new ImageIcon(url).getImage();

      return(image);
    }

    public static final String BLANK_WHITE = new String(
      "Blank.gif"
    );
    public static final String CONGLOMERATE = new String(
      "cong1.gif"
    );
    public static final String DOLOMITE = new String(
      "Dolomite.gif"
    );
    public static final String EVAPORITE = new String(
      "Evaporite.gif"
    );
    public static final String IGNEOUS = new String(
      "Igneous.gif"
    );
    public static final String LIMESTONE = new String(
      "Limestone.gif"
    );
    public static final String SALT = new String(
      "salt.gif"
    );
    public static final String SANDSTONE = new String(
      "Sandstone.gif"
    );
    public static final String SILTSTONE = new String(
      "Siltstone.gif"
    );
    public static final String SHALE = new String(
      "Shale.gif"
    );

}
