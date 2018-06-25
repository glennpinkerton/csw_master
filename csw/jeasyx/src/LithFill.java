
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

// java imports
import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.net.URL;

import javax.swing.ImageIcon;

import csw.images.FillImages;

public final class LithFill {

  public static final int LITHOLOGY_TYPE_SANDSTONE = 0;
  public static final int LITHOLOGY_TYPE_SILTSTONE = 1;
  public static final int LITHOLOGY_TYPE_SHALE = 2;
  public static final int LITHOLOGY_TYPE_LIMESTONE = 3;
  public static final int LITHOLOGY_TYPE_DOLOMITE = 4;
  public static final int LITHOLOGY_TYPE_EVAPORITE = 5;
  public static final int LITHOLOGY_TYPE_CONGLOMERATE = 6;
  public static final int LITHOLOGY_TYPE_IGNEOUS = 7;
  public static final int LITHOLOGY_TYPE_SALT = 8;
  public static final int N_LITHOLOGY_TYPES = 9;

  public static final String[] lithologyTypes = {
    "Sandstone",
    "Siltstone",
    "Shale",
    "Limestone",
    "Dolomite",
    "Evaporite",
    "Conglomerate",
    "Igneous",
    "Salt"
  };

  private static final Color Sand = new Color(220, 220,   0); // Sandstone -- Dark Yellow
  private static final Color Silt = new Color(201, 171, 144); // Siltstone -- Sand
  private static final Color Shal = new Color(150, 170,  75); // Shale -- Spring Green
  private static final Color Lime = new Color(105, 130, 255); // Limestone -- True Blue
  private static final Color Dolo = new Color(135,   0, 220); // Dolomite -- Purple
  private static final Color Evap = new Color(  0, 100,   0); // Evaporite -- Dark Green if Salt
  private static final Color Cong = new Color(  0,   0,   0); // Conglometate -- Black
  private static final Color Igns = new Color(216, 118,  78); // Igneous -- Poppy Red
  private static final Color Salt = new Color(0, 100, 0);     // Salt -- dark green

  public static final Color[] lithologyColors = {
    Sand,
    Silt,
    Shal,
    Lime,
    Dolo,
    Evap,
    Cong,
    Igns,
    Salt
  };




// The "resources" are generally files that stay constant during the
// life time of the java application.  In the case of the CSW stuff,
// there are some gif files that fit this "resource" definition.  The
// get Resource call basically returns the file name of the gif file.

// It seems that resources can be retrieved either by the class of the class 
// loader.  In my experimenting, if the resource (in this case the gif file)
// is in the package directory, then getting the resource using just the
// filename works with the class, but not with the class loader.

// For a gradle build, if the gif files were put into the src/main/resource
// directory, then the classLoader version would work and the class version
// would not, at least in my experimentation.

/*
  public static final URL[] lithologyURL = {
      FillImages.class.getClassLoader().getResource(FillImages.SANDSTONE),
      FillImages.class.getClassLoader().getResource(FillImages.SILTSTONE),
      FillImages.class.getClassLoader().getResource(FillImages.SHALE),
      FillImages.class.getClassLoader().getResource(FillImages.LIMESTONE),
      FillImages.class.getClassLoader().getResource(FillImages.DOLOMITE),
      FillImages.class.getClassLoader().getResource(FillImages.EVAPORITE),
      FillImages.class.getClassLoader().getResource(FillImages.CONGLOMERATE),
      FillImages.class.getClassLoader().getResource(FillImages.IGNEOUS),
      FillImages.class.getClassLoader().getResource(FillImages.SALT)
  };
*/

  public static final URL[] lithologyURL = {
      FillImages.class.getResource(FillImages.SANDSTONE),
      FillImages.class.getResource(FillImages.SILTSTONE),
      FillImages.class.getResource(FillImages.SHALE),
      FillImages.class.getResource(FillImages.LIMESTONE),
      FillImages.class.getResource(FillImages.DOLOMITE),
      FillImages.class.getResource(FillImages.EVAPORITE),
      FillImages.class.getResource(FillImages.CONGLOMERATE),
      FillImages.class.getResource(FillImages.IGNEOUS),
      FillImages.class.getResource(FillImages.SALT)
  };

  public static int getTypeFromName(String name) {
    for (int i = 0; i < N_LITHOLOGY_TYPES; i++) {
      if (name.compareTo(lithologyTypes[i]) == 0)
        return(i);
    }
    return(-1);
  }

  private static BufferedImage createBI (URL url)
  {
    ImageIcon           icon;
    Image               image;
    BufferedImage       bi;

    icon = new ImageIcon (url);
    image = icon.getImage ();

    bi = new BufferedImage (image.getWidth(null),
                            image.getHeight(null),
                            BufferedImage.TYPE_INT_ARGB);
    Graphics2D g2 = bi.createGraphics ();
    g2.drawImage (image, null, null);

  /*
   * Convert white in the image to transparent.
   */
    int nrow = bi.getHeight();
    int ncol = bi.getWidth();
    int i, j, rgb, a, r, g, b;

    for (i=0; i<nrow; i++) {
      for (j=0; j<ncol; j++) {
        rgb = bi.getRGB (j, i);
        a = rgb >> 24;
        r = rgb & 0x00ff0000;
        r = r >> 16;
        g = rgb & 0x0000ff00;
        g = g >> 8;
        b = rgb & 0x000000ff;
        if (r >= 254  &&  g >= 254  &&  b >= 254) {
          a = 0;
          r = (r << 16) & 0x00ff0000;
          g = (g << 8) & 0x0000ff00;
          b = b & 0x000000ff;
          rgb = a | r | g | b;
          bi.setRGB (j, i, rgb);
        }
        else {
          a = 255 << 24;
          r = (r << 16) & 0x00ff0000;
          g = (g << 8) & 0x0000ff00;
          b = b & 0x000000ff;
          rgb = a | r | g | b;
          bi.setRGB (j, i, rgb);
        }
      }
    }

    return bi;

  }

  private static final BufferedImage sandBI = createBI(lithologyURL[0]);
  private static final BufferedImage siltBI = createBI(lithologyURL[1]);
  private static final BufferedImage shalBI = createBI(lithologyURL[2]);
  private static final BufferedImage limeBI = createBI(lithologyURL[3]);
  private static final BufferedImage doloBI = createBI(lithologyURL[4]);
  private static final BufferedImage evapBI = createBI(lithologyURL[5]);
  private static final BufferedImage congBI = createBI(lithologyURL[6]);
  private static final BufferedImage ignsBI = createBI(lithologyURL[7]);
  private static final BufferedImage saltBI = createBI(lithologyURL[8]);

  public static final BufferedImage[] lithologyImages = {
    sandBI,
    siltBI,
    shalBI,
    limeBI,
    doloBI,
    evapBI,
    congBI,
    ignsBI,
    saltBI
  };

  public LithFill() {
  }

}
