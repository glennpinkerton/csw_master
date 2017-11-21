
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;

import java.awt.Color;
import java.awt.Font;

/**
 * Display properties for 3-D View.
 */
public class View3DProperties {
  // constants
  private static final Color WIREFRAME_DEFAULT_COLOR = Color.BLACK;
  private static final Color BG_DEFAULT_COLOR = Color.LIGHT_GRAY.brighter();
  private static final double DEFAULT_VERTICAL_EXAG_VALUE = 5.0;
  private static final double INVALID_VERTICAL_EXAG_VALUE = -1.0;
  // instance variables
  private boolean showWireframe;
  private Color wireframeColor;
  private boolean solidFill;
  // lighting options
  private boolean smoothShading;
  private boolean lightingNShadowing;
  private Color backgroundColor;
  private boolean showLights;
  private LightData light1;
  private LightData light2;
  private LightData light3;
  private LightData light4;
  // global options
  private boolean showBoundingBox;
  private boolean showWalls;
  private boolean showAxes;
  private boolean showNames;
  private boolean showNumbers;
  private boolean showTitles;
  private boolean showExaggeration;
  private boolean showCompass;
  private boolean drawWhileRotating;
  private double verticalExag;
  private double transparency;

  private boolean showScaleBar;
  private boolean showColorScale;

  // Fonts
  private Font titlesFont;
  private Font axisNamesFont;
  private Font axisNumbersFont;

  /**
   * Constructor.
   */
  public View3DProperties() {
    init();
  }

  /**
   * Copy constructor
   * @param other
   */
/*
  public View3DProperties(View3DProperties other) {
    init();
    showWireframe = other.showWireframe();
    wireframeColor = other.wireframeColor;
    solidFill = other.solidFill;
    // lighting options
    smoothShading = other.smoothShading;
    lightingNShadowing = other.lightingNShadowing;
    backgroundColor = other.backgroundColor;
    showLights = other.showLights;
    // global options
    showBoundingBox = other.showBoundingBox;
    showWalls = other.showWalls;
    showAxes = other.showAxes;
    showNames = other.showNames;
    showNumbers = other.showNumbers;
    showTitles = other.showTitles;
    showCompass = other.showCompass;
    drawWhileRotating = other.drawWhileRotating;
    verticalExag = other.verticalExag;
    showExaggeration = other.showExaggeration;
    transparency = other.transparency;
    // fonts
    titlesFont = new Font(
      other.titlesFont.getFontName(),
      other.titlesFont.getStyle(),
      other.titlesFont.getSize()
    );
    axisNamesFont = new Font(
      other.axisNamesFont.getFontName(),
      other.axisNamesFont.getStyle(),
      other.axisNamesFont.getSize()
    );
    axisNumbersFont = new Font(
      other.axisNumbersFont.getFontName(),
      other.axisNumbersFont.getStyle(),
      other.axisNumbersFont.getSize()
    );
  }
*/

  private void init() {
    showWireframe = false;
    wireframeColor = WIREFRAME_DEFAULT_COLOR;
    solidFill = true;
    // lighting options
    smoothShading = true;
    lightingNShadowing = true;
    backgroundColor = BG_DEFAULT_COLOR;
    showLights = false;
    // global options
    showBoundingBox = true;
    showWalls = true;
    showAxes = true; // suggested default by Phil Levine
    showNames = true; // suggested default by Phil Levine
    showNumbers = false;
    showTitles = false;
    drawWhileRotating = true;
    verticalExag = 10.0;
    transparency = 100.0;
    defaultLights();

    // fonts
    titlesFont = new Font("SansSerif", Font.PLAIN, 18);
    axisNamesFont = new Font("SansSerif", Font.PLAIN, 18);
    axisNumbersFont = new Font("SansSerif", Font.PLAIN, 14);
  }

  private void defaultLights() {
    light1 = defaultLight1();
    light2 = defaultLight2();
    light3 = defaultLight3();
    light4 = defaultLight4();
  }

  private LightData defaultLight1() {
    LightData ld = new LightData(.866, .5, .5, true, 50);
    return(ld);
  }

  private LightData defaultLight2() {
    LightData ld = new LightData(-.866, .5, .5, false, 50);
    return(ld);
  }

  private LightData defaultLight3() {
    LightData ld = new LightData(-.866, .5, -.5, false, 50);
    return(ld);
  }

  private LightData defaultLight4() {
    LightData ld = new LightData(.866, .5, -.5, false, 50);
    return(ld);
  }

  /**
   * @return showWireframe flag
   */
  public boolean showWireframe() {
    return showWireframe;
  }

  /**
   * @param b
   */
  public void setShowWireframe(boolean b) {
    if (showWireframe == b)
      return;
    showWireframe = b;
  }

  /**
   * @return wireframe color
   */
  public Color getWireframeColor() {
    return wireframeColor;
  }

  /**
   * @param color
   */
  public void setWireframeColor(Color color) {
    if (color == null)
      throw new IllegalArgumentException("color is null");
    if (wireframeColor.getRGB() == color.getRGB())
      return;
    wireframeColor = color;
  }

  public boolean isSolidFill() {
    return solidFill;
  }

  public void setSolidFill(boolean s) {
    if (solidFill == s)
      return;
    solidFill = s;
  }

  public boolean isSmoothShading() {
    return smoothShading;
  }

  public void setSmoothShading(boolean s) {
    if (smoothShading == s)
      return;
    smoothShading = s;
  }

  public boolean isLightingNShadowing() {
    return lightingNShadowing;
  }

  public void setLightingNShadowing(boolean l) {
    if (lightingNShadowing == l) {
      return;
    }
    lightingNShadowing = l;
  }

  public boolean showLights() {
    return showLights;
  }

  public void setShowLights(boolean s) {
    if (showLights == s) {
      return;
    }
    showLights = s;
  }

  public boolean showScaleBar ()
  {
    return showScaleBar;
  }

  public void setShowScaleBar (boolean b)
  {
    showScaleBar = b;
  }

  public boolean showColorScale ()
  {
    return showColorScale;
  }

  public void setShowColorScale (boolean b)
  {
    showColorScale = b;
  }

  public boolean showBoundingBox() {
    return showBoundingBox;
  }

  public void setShowBoundingBox(boolean b) {
    if (showBoundingBox == b)
      return;
    showBoundingBox = b;
  }

  public boolean showWalls() {
    return showWalls;
  }

  public void setShowWalls(boolean w) {
    if (showWalls == w)
      return;
    showWalls = w;
  }

  public boolean showAxes() {
    return showAxes;
  }

  public void setShowAxes(boolean a) {
    if (showAxes == a)
      return;
    showAxes = a;
  }

  public boolean showNames() {
    return showNames;
  }

  public void setShowNames(boolean n) {
    if (showNames == n)
      return;
    showNames = n;
  }

  public boolean showNumbers() {
    return showNumbers;
  }

  public void setShowNumbers(boolean n) {
    if (showNumbers == n)
      return;
    showNumbers = n;
  }

  public boolean showTitles() {
    return showTitles;
  }

  public void setShowTitles(boolean t) {
    if (showTitles == t)
      return;
    showTitles = t;
  }

  public boolean showExaggeration() {
    return showExaggeration;
  }

  public void setShowExaggeration(boolean t) {
    if (showExaggeration == t)
      return;
    showExaggeration = t;
  }

  public boolean showCompass() {
    return showCompass;
  }

  public void setShowCompass(boolean t) {
    if (showCompass == t)
      return;
    showCompass = t;
  }

  public boolean drawWhileRotating() {
    return drawWhileRotating;
  }

  public void setDrawWhileRotating(boolean d) {
    if (drawWhileRotating == d)
      return;
    drawWhileRotating = d;
  }

  public double getVerticalExaggeration() {
    // could throw exception, but that may be too brutal
    if (isVerticalExaggerationInvalid())
      return(DEFAULT_VERTICAL_EXAG_VALUE); //don't set verticalExag, just return this value
    return verticalExag;
  }

  public void setVerticalExaggeration(double v) {
    if (verticalExag == v)
      return;
    verticalExag = v;
  }

  /**
   * Invalidate the vertical exaggeration.  This method may be used to force
   * the application to figure out a good default vertical exaggeration based
   * upon the data.  For instance, when a rendering panel is first created,
   * a call to this method is made.  Later, when the rendering panel attempts
   * to draw some data, call isVerticalExaggerationInvalid() and figure an
   * optimal vertical exaggeration based upon the data, then call
   * setVerticalExaggeration(), passing in your new value.
   */
  public void invalidateVerticalExaggeration() {
    verticalExag = INVALID_VERTICAL_EXAG_VALUE;
  }

  /**
   * @return true if a prior call to invalidateVerticalExaggeration() has
   * been made without a subsequent call to setVerticalExaggeration(), false
   * otherwise.
   */
  public boolean isVerticalExaggerationInvalid() {
    return(verticalExag <= INVALID_VERTICAL_EXAG_VALUE);
  }

  public double getTransparency() {
    return transparency;
  }

  public void setTransparency(double t) {
    if (transparency == t)
      return;
    transparency = t;
  }
  /**
   * @return the background color
   */
  public Color getBackgroundColor() {
    return backgroundColor;
  }

  /**
   * @param color the background color to set
   */
  public void setBackgroundColor(Color color) {
    if (color == null)
      throw new IllegalArgumentException("color is null");
    if (backgroundColor.getRGB() == color.getRGB())
      return;
    backgroundColor = color;
  }

  /**
   * Set multiple ligting properties.
   *
   * @param smoothShading smooth shading.
   * @param lightingNShadowing lighting and shadowing.
   * @param light1 data for light 1.
   * @param light2 data for light 2.
   * @param light3 data for light 3.
   * @param light4 data for light 4.
   *
   */
  public void setLightingProperties(
    boolean smoothShading,
    boolean lightingNShadowing,
    LightData light1,
    LightData light2,
    LightData light3,
    LightData light4
  ) {
    this.smoothShading = smoothShading;
    this.lightingNShadowing = lightingNShadowing;
    this.light1.setValues(light1);
    this.light2.setValues(light2);
    this.light3.setValues(light3);
    this.light4.setValues(light4);
  }
  /**
   * @return Light 1.
   */
  public LightData getLight1() {
    return light1;
  }

  /**
   * @return Light 1 default.
   */
  public LightData getLight1Default() {
    return defaultLight1();
  }

  /**
   * @return Light 2.
   */
  public LightData getLight2() {
    return light2;
  }

  /**
   * @return Light 2 default.
   */
  public LightData getLight2Default() {
    return defaultLight2();
  }

  /**
   * @return Light 3.
   */
  public LightData getLight3() {
    return light3;
  }

  /**
   * @return Light 3 default.
   */
  public LightData getLight3Default() {
    return defaultLight3();
  }

  /**
   * @return Light 4.
   */
  public LightData getLight4() {
    return light4;
  }

  /**
   * @return Light 4 default.
   */
  public LightData getLight4Default() {
    return defaultLight4();
  }

  /**
   * @param data LightData to copy.
   */
  public void setLight1(LightData data) {
    light1.setValues(data);
  }

  /**
   * @param data LightData to copy.
   */
  public void setLight2(LightData data) {
    light2.setValues(data);
  }

  /**
   * @param data LightData to copy.
   */
  public void setLight3(LightData data) {
    light3.setValues(data);
  }

  /**
   * @param data LightData to copy.
   */
  public void setLight4(LightData data) {
    light4.setValues(data);
  }

  public Font getAxisNamesFont() {
    return axisNamesFont;
  }

  public void setAxisNamesFont(Font axisNamesFont) {
    this.axisNamesFont = axisNamesFont;
  }

  public Font getAxisNumbersFont() {
    return axisNumbersFont;
  }

  public void setAxisNumbersFont(Font axisNumbersFont) {
    this.axisNumbersFont = axisNumbersFont;
  }

  public Font getTitlesFont() {
    return titlesFont;
  }

  public void setTitlesFont(Font titlesFont) {
    this.titlesFont = titlesFont;
  }

  public void print_lighting ()
  {
System.out.println ("from print_lighting = "+lightingNShadowing);
  }

}
