
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/


package csw.jeasyx.src;

import java.awt.Dimension;
import java.awt.image.BufferedImage;

import javax.swing.ImageIcon;
import javax.swing.JButton;

/**
 * Creates a button with a csw symbol on it.
 */
public class SymbolButton {
  // instance variables
  private JButton button;
  private ImageIcon icon;

  /**
   * Constructor
   *
   * @param symbolType csw symbol type (@see DLSymb}
   * @param width preferred button width
   * @param height preferred button height
   */
  public SymbolButton(int symbolType, int width, int height) {
    if (symbolType < DLSymb.SYMBOL_MIN || symbolType > DLSymb.SYMBOL_MAX)
      throw new IllegalArgumentException("symbolType="+symbolType+" is out of range");

    PreviewPanel pPanel = new PreviewPanel();
    pPanel.clearData();
    pPanel.setSize(width-1, height-1);

    SymbolParts symbolParts = JDisplayList.getSymbolParts (
      symbolType,
      0.2,  // symbol size in inches
      0.0   // symbol angle in degrees
    );

    pPanel.addLineList(symbolParts.lineList);
    pPanel.addFillList(symbolParts.fillList);
    pPanel.addTextList(symbolParts.textList);
    pPanel.addArcList(symbolParts.arcList);
    pPanel.addFilledArcList(symbolParts.filledArcList);

    /*
     * This method will draw an image that is the same size as the PreviewPanel,
     * in this case width by height.
     */
    BufferedImage image = pPanel.paintBufferedImage();

    // now create the icon
    icon = new ImageIcon(image);

    // now create the button with the icon, and set its preferred size
    button = new JButton(icon);
    button.setMinimumSize(new Dimension(width, height));
    button.setSize(new Dimension(width, height));
    button.setPreferredSize(new Dimension(width, height));
  }

  /**
   * @return the {@link JButton}
   */
  public JButton getButton() {
    return(button);
  }

  /**
   * @return the {@link ImageIcon}
   */
  public ImageIcon getImageIcon() {
    return(icon);
  }
}
