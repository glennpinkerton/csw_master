
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/


package csw.j3d.src;

import java.awt.Color;

/**
 * Base graphic element on a J3DPanel.
 *
 */
public abstract class J3DElement {
  private boolean visible;
  private boolean selected;
  private Color globalColor;
  boolean markedForDelete;

  /**
   *
   */
  public J3DElement() {
    super();
    init();
  }

/*
  public J3DElement(Object cbData) {
    init();
    setCallbackData(cbData);
  }
*/

  private void init() {
    visible = true;
    selected = false;
    globalColor = null;
    markedForDelete = false;
  }

  /**
   * @return true if this element is currently visible, false if it is
   * not currently visible.
   */
  public boolean isVisible() {
    return(visible);
  }

  /**
   * Set whether the element is to be visible or not.  Set to true to make the
   * element visible or set to false to make the element invisible.  By default,
   * the element is visible.
   *
   * @param flag the new visible flag.
   */
  public void setVisible(boolean flag) {
    visible = flag;
  }

  /**
   * @return true if this element is currently selected, false if it is
   * currently unselected.
   */
  public boolean isSelected() {
    return(selected);
  }

  /**
   * Set the selected flag to true or false.
   */
  public void setSelected(boolean flag) {
    selected = flag;
  }


  /**
   * @return the color used for solid color fill of the surface.  If
   * this has not been set, null is returned.
   */
  public Color getGlobalColor() {
    return globalColor;
  }

  /**
   * Set the single color to be used for this surface.  This color is used
   * when the surface patch is colored all the same color.  In the future,
   * variable color by attribute will also be available.  The default is
   * null, and by default the element will not be drawn.
   *
   * @param color {@link Color} object defining the surface patch color.
   */
  public void setGlobalColor(Color color) {
    globalColor = color;
  }

  /**
  Change the opacity of the global color.  A percent of 1.0 is opaque and
  a percent of 0.0 is transparent.  The red, green and blue colors are not
  affected.
  */
  public void setOpaque (double percent)
  {
    if (globalColor == null) {
      return;
    }
    int   r = globalColor.getRed ();
    int   g = globalColor.getGreen ();
    int   b = globalColor.getBlue ();
    int   a = (int)(255 * percent);
    globalColor = new Color (r, g, b, a);

  }

  /**
   * This will explicitly make the element not deleteable in the 
   * next mark and sweep.
   */
   public void unmarkForDeletion ()
   {
       markedForDelete = false;
   }


}
