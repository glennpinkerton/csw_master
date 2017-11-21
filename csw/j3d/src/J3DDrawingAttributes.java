
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.awt.Font;

import csw.jutils.src.View3DProperties;
import csw.jutils.src.LightData;

/**
 * Drawing attributes for the J3DPanel
 *
 * @author Glenn Pinkerton
 */
public class J3DDrawingAttributes {

  private double            zExag = 10.0;
  private boolean           doBox = true;
  private boolean           doSides = true;
  private boolean           doAxes = false;
  private boolean           doAxesLabels = false;
  private boolean           doAxesNumbers = false;
  private boolean           doSolidFill = true;
  private boolean           doWireFrame = false;
  private boolean           doLighting = true;
  private boolean           doShading = true;
  private boolean           doDrawLights = false;
  private boolean           doDrawTitles = false;
  private boolean           doDrawExaggeration = false;
  private boolean           doDrawMoveAzimuthDisc = false;
  private boolean           doCompass = false;
  private boolean           motionRedraw = true;
  private View3DProperties  view3DProperties;
  private String            titleString = null;

  // Fonts
  private Font titlesFont;
  private Font axisLabelsFont;
  private Font axisNumbersFont;

  private J3DPanel          panel;

  /**
   * Constructor
   *
   */
  public J3DDrawingAttributes(J3DPanel panel) {
    super();
    this.panel = panel;
  }

/**
Set the title to draw in the upper left of the 3D view
when the user chooses to draw titles.  This single line
string is drawn and the 3d display xy and z unit are
drawn below the title.  The 3rd line is the vertical exaggeration.
*/
  public void setTitleString (String s)
  {
      titleString = s;
  }

  public String getTitleString ()
  {
      return titleString;
  }

/*----------------------------------------------------------------------------*/

/**
Retrieve the view properties object currently being used by the panel.
*/
  public View3DProperties getViewProperties () {
    return(view3DProperties);
  }

/*---------------------------------------------------------------------------*/

/**
Set the vertical exageration to be used in subsequent redraws.  By default
the vertical exageration is 10.0.
*/
  public void setVerticalExageration (double value)
  {
      if (value < 0.001 ||  value > 100000.0) {
          return;
      }

      double  oldz = zExag;

      zExag = value;

      if (value < 0.99 * oldz  ||  value > 1.01 * oldz) {
          panel.visibleLimitsUpdateNeeded = true;
      }
  }

/**
Return the current vertical exageration.
*/
  public double getVerticalExageration ()
  {
      return zExag;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true if you want surfaces to be solid fill colored (as opposed to wireframe).
By default this is true.
*/
  public void setDoSolidFill (boolean bval)
  {
      doSolidFill = bval;
  }

  public boolean getDoSolidFill ()
  {
      return doSolidFill;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true if you want the surfaces to be drawn in wireframe.  By default
this is false.
*/
  public void setDoWireFrame (boolean bval)
  {
      doWireFrame = bval;
  }

  public boolean getDoWireFrame ()
  {
      return doWireFrame;
  }

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/**
Set this to true if you want the bounding box edges to be drawn or set to false
if you don't want the bounding box edges drawn.  By default this is true.
*/
  public void setDoBox (boolean bval)
  {
      doBox = bval;
  }
  public boolean getDoBox ()
  {
      return doBox;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true if you want the sides of the bounding box drawn or set to
false if you don't want the sides drawn.  By default this is true.
*/
  public void setDoSides (boolean bval)
  {
      doSides = bval;
  }
  public boolean getDoSides ()
  {
      return doSides;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true if you want the contents of the model to be drawn
while rotating of panning.  Set to false if only the boundary cube
is drawn while rotating or panning.
*/
  public void setMotionRedraw (boolean bval)
  {
      motionRedraw = bval;
  }
  public boolean getMotionRedraw ()
  {
      return motionRedraw;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true if you want the x, y and z axes colored and drawn.
By default this is false.
*/
  public void setDoAxes (boolean bval)
  {
      doAxes = bval;
  }
  public boolean getDoAxes ()
  {
      return doAxes;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true to draw the axis names at the open ends of the axes.
By default this is false.
*/
  public void setDoAxesLabels (boolean bval)
  {
      doAxesLabels = bval;
  }
  public boolean getDoAxesLabels ()
  {
      return doAxesLabels;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true to label the major ticks on the axes.
By default this is false.
*/
  public void setDoAxesNumbers (boolean bval)
  {
      doAxesNumbers = bval;
  }
  public boolean getDoAxesNumbers ()
  {
      return doAxesNumbers;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true to have surfaces (horizons and faults) drawn with lighting
and shadowing effects.  By default this is true.
*/
  public void setDoLighting (boolean bval)
  {
      if (bval != doLighting) {
          panel.unsetDisplayLists();
      }
      doLighting = bval;
  }
  public boolean getDoLighting ()
  {
      return doLighting;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true to have smooth shading inside surface triangles.  By default
this is true.
*/
  public void setDoShading (boolean bval)
  {
      if (bval != doShading) {
          panel.unsetDisplayLists ();
      }
      doShading = bval;
  }
  public boolean getDoShading ()
  {
      return doShading;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true to draw the locations of enabled lights.  By default this is false.
*/
  public void setDoDrawLights (boolean bval)
  {
      doDrawLights = bval;
  }
  public boolean getDoDrawLights ()
  {
      return doDrawLights;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true to draw the title strings on the 3d view.  Title strings include
the actual title and the xy units and the z units.
*/
  public void setDoDrawTitles (boolean bval)
  {
      doDrawTitles = bval;
  }
  public boolean getDoDrawTitles ()
  {
      return doDrawTitles;
  }

  /*---------------------------------------------------------------------------*/

  /**
   *Set this to true to draw the vertical exaggeration strings on the 3d view.
   */
    public void setDoDrawExaggeration (boolean bval)
    {
        doDrawExaggeration = bval;
    }
    public boolean getDoDrawExagerration ()
    {
        return doDrawExaggeration;
    }

/*---------------------------------------------------------------------------*/

/**
Set this to true to draw the move azimuth disk on the 3d view.
*/
  public void setDoDrawMoveAzimuthDisc (boolean bval)
  {
      doDrawMoveAzimuthDisc = bval;
  }
  public boolean getDoDrawMoveAzimuthDisc ()
  {
      return doDrawMoveAzimuthDisc;
  }

/*---------------------------------------------------------------------------*/

/**
Set this to true to draw the compass on the 3d view.
*/
  public void setDoCompass (boolean bval)
  {
      doCompass = bval;
  }
  public boolean getDoCompass ()
  {
      return doCompass;
  }

  void setViewProperties(View3DProperties vp) {

    if (vp == null) {
      throw new IllegalArgumentException("View3DProperties is null.");
    }

    view3DProperties = vp;
    setDoSolidFill(vp.isSolidFill());
    setDoWireFrame(vp.showWireframe());
    setDoShading (vp.isSmoothShading());
    setDoLighting (vp.isLightingNShadowing());
    setDoDrawLights (vp.showLights());
    setDoBox (vp.showBoundingBox());
    setDoSides (vp.showWalls ());
    setDoAxes (vp.showAxes());
    setDoAxesLabels (vp.showNames());
    setDoAxesNumbers (vp.showNumbers ());
    setVerticalExageration (vp.getVerticalExaggeration ());
    setDoDrawTitles (vp.showTitles());
    setDoDrawExaggeration(vp.showExaggeration());
    setDoCompass (vp.showCompass());
    titlesFont = vp.getTitlesFont();
    axisLabelsFont = vp.getAxisNamesFont();
    axisNumbersFont = vp.getAxisNumbersFont();
  }

  /**
  Set all the lights to their default positions, brightness and
  enabled states.  By default, light 1 is above the scene and to the northeast.
  Light 1 is enabled and it's brightness is set to 50 (at half intensity).
  All other lights are disabled by default.
  */
//    public void setLightsToDefaults ()
//    {
//        defaultLights ();
//    }


  /**
  Set the position, enabled state and brightness for light 1.
  The position values are in "unit cube" coordinates.  They
  must be between -1 and 1.  The brightness can range from 0 to 100,
  with 0 being dimmer and 100 being brighter.
  @param x X position of light (-1 to 1).
  @param y Y position of light (-1 to 1).
  @param z Z position of light (-1 to 1).
  @param enabled Set to true to use this light or to false to not use this light.
  @param brightness Brightness value from 0 (dim) to 100 (bright).
  */
//    public void setLight1Data (
//        double x,
//        double y,
//        double z,
//        boolean enabled,
//        int brightness
//    )
//    {
//        light1.x = x;
//        light1.y = y;
//        light1.z = z;
//        light1.enabled = enabled;
//        light1.brightness = brightness;
//    }

  /**
  Set the position, enabled state and brightness for light 1.  This
  version specifies a {@link LightData} object that has the data.
  */
    public void setLight1Data (LightData ldata)
    {
      view3DProperties.setLight1(ldata);
    }

  /**
  Return the position, enabled state and brightness of light1 encapsulated
  in a {@link LightData} object.  This returns a copy of the internal
  light1 data object.
  */
    public LightData getLight1Data ()
    {
        return (view3DProperties.getLight1());
    }

    public LightData getLight1Default ()
    {
        return (view3DProperties.getLight1Default());
    }


  /**
  Set the position, enabled state and brightness for light 2.
  The position values are in "unit cube" coordinates.  They
  must be between -1 and 1.  The brightness can range from 0 to 100,
  with 0 being dimmer and 100 being brighter.
  @param x X position of light (-1 to 1).
  @param y Y position of light (-1 to 1).
  @param z Z position of light (-1 to 1).
  @param enabled Set to true to use this light or to false to not use this light.
  @param brightness Brightness value from 0 (dim) to 100 (bright).
  */
//    public void setLight2Data (
//        double x,
//        double y,
//        double z,
//        boolean enabled,
//        int brightness
//    )
//    {
//        light2.x = x;
//        light2.y = y;
//        light2.z = z;
//        light2.enabled = enabled;
//        light2.brightness = brightness;
//    }

  /**
  Set the position, enabled state and brightness for light 1.  This
  version specifies a {@link LightData} object that has the data.
  */
    public void setLight2Data (LightData ldata)
    {
        view3DProperties.setLight2(ldata);
    }

  /**
  Return the position, enabled state and brightness of light 2 encapsulated
  in a {@link LightData} object.  This is the same object as is used internally
  for the light data.
  */
    public LightData getLight2Data ()
    {
        return (view3DProperties.getLight2());
    }

    public LightData getLight2Default ()
    {
        return (view3DProperties.getLight2Default());
    }


  /**
  Set the position, enabled state and brightness for light 3.
  The position values are in "unit cube" coordinates.  They
  must be between -1 and 1.  The brightness can range from 0 to 100,
  with 0 being dimmer and 100 being brighter.
  @param x X position of light (-1 to 1).
  @param y Y position of light (-1 to 1).
  @param z Z position of light (-1 to 1).
  @param enabled Set to true to use this light or to false to not use this light.
  @param brightness Brightness value from 0 (dim) to 100 (bright).
  */
//    public void setLight3Data (
//        double x,
//        double y,
//        double z,
//        boolean enabled,
//        int brightness
//    )
//    {
//        light3.x = x;
//        light3.y = y;
//        light3.z = z;
//        light3.enabled = enabled;
//        light3.brightness = brightness;
//    }

  /**
  Set the position, enabled state and brightness for light 1.  This
  version specifies a {@link LightData} object that has the data.
  */
    public void setLight3Data (LightData ldata)
    {
      view3DProperties.setLight3(ldata);
    }

  /**
  Return the position, enabled state and brightness of light 3 encapsulated
  in a {@link LightData} object.  This is the same object as is used internally
  for the light data.
  */
    public LightData getLight3Data ()
    {
        return (view3DProperties.getLight3());
    }

    public LightData getLight3Default ()
    {
        return (view3DProperties.getLight3Default());
    }


  /**
  Set the position, enabled state and brightness for light 4.
  The position values are in "unit cube" coordinates.  They
  must be between -1 and 1.  The brightness can range from 0 to 100,
  with 0 being dimmer and 100 being brighter.
  @param x X position of light (-1 to 1).
  @param y Y position of light (-1 to 1).
  @param z Z position of light (-1 to 1).
  @param enabled Set to true to use this light or to false to not use this light.
  @param brightness Brightness value from 0 (dim) to 100 (bright).
  */
//    public void setLight4Data (
//        double x,
//        double y,
//        double z,
//        boolean enabled,
//        int brightness
//    )
//    {
//        light4.x = x;
//        light4.y = y;
//        light4.z = z;
//        light4.enabled = enabled;
//        light4.brightness = brightness;
//    }

  /**
  Set the position, enabled state and brightness for light 1.  This
  version specifies a {@link LightData} object that has the data.
  */
    public void setLight4Data (LightData ldata)
    {
      view3DProperties.setLight4(ldata);
    }

  /**
  Return the position, enabled state and brightness of light 4 encapsulated
  in a {@link LightData} object.  This is the same object as is used internally
  for the light data.
  */
    public LightData getLight4Data ()
    {
        return (view3DProperties.getLight4());
    }

    public LightData getLight4Default ()
    {
        return (view3DProperties.getLight4Default());
    }

    /**
     * Returns the axis labels (names) font.
     */
    public Font getAxisLabelsFont() {
      return axisLabelsFont;
    }

    /**
     * Returns the axis numbers (values) font.
     */
    public Font getAxisNumbersFont() {
      return axisNumbersFont;
    }

    /**
     * Returns the titles font.
     */
    public Font getTitlesFont() {
      return titlesFont;
    }

}
