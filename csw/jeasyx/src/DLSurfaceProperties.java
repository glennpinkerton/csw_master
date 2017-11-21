
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

import java.awt.Color;

/*--------------------------------------------------------------------------*/

  /**
   This class stores graphical properties used to draw contours or color filled
   contours on the
   {@link JDisplayList} object.  All of the properties can be publically accessed
   via the appropriate set and get methods of the class.  In addition, the classes
   in the csw.jeasyx.src package can directly access the properties.
  <p>
   When the addGrid or addTriMesh method is called, an instance of this class
   can be specified to instruct how the grid or trimesh is to be visualized.
   */

public class DLSurfaceProperties {

/*--------------------------------------------------------------------------*/

  /*
   * The instance variables have package scope so the JDisplayList can
   * access them directly.
   */
    boolean        showContours,
                   showColorFills,
                   showNodes,
                   showNodeValues,
                   showCellEdges,
                   showFaultLines;
    boolean        showOutline;
    boolean        isThickness;
    double         contourMinValue,
                   contourMaxValue,
                   contourInterval,
                   dataLogBase,
                   fillMinValue,
                   fillMaxValue;
    int            majorInterval;
    boolean        labelMajor,
                   labelMinor;
    int            tickMajor,
                   tickMinor;
    Color          majorColor,
                   minorColor,
                   nodeColor,
                   nodeValueColor,
                   cellEdgeColor,
                   faultLineColor;
    Color          outlineColor;
    double         minorThickness,
                   majorThickness,
                   cellEdgeThickness,
                   faultThickness;
    int            contourFont,
                   nodeFont;
    double         minorLabelSize,
                   majorLabelSize,
                   minorLabelSpacing,
                   majorLabelSpacing,
                   minorTickLength,
                   majorTickLength,
                   minorTickSpacing,
                   majorTickSpacing;
    int            nodeSymbol;
    double         nodeSymbolSize,
                   nodeValueSize;
    double         hardMin,
                   hardMax;
    int            contourSmoothing;
    double         zUnitsConversionFactor;

/*--------------------------------------------------------------------------*/

  /**
   Create a new object with default contour property values.
  */
    public DLSurfaceProperties ()
    {
        setToDefaults ();
        return;
    }

/*--------------------------------------------------------------------------*/

  /**
  Set all the contour properties to their default values.
  */
    public void setToDefaults() {
        showContours = true;
        showColorFills = true;
        showNodes = false;
        showNodeValues = false;
        showCellEdges = false;
        showFaultLines = true;
        showOutline = false;

        contourMinValue = 1.e30;
        contourMaxValue = -1.e30;
        contourInterval = -1.0;
        dataLogBase = 0.0;
        fillMinValue = 1.e30;
        fillMaxValue = -1.e30;

        majorInterval = -1;
        labelMajor = true;
        labelMinor = false;

        tickMajor = 0;
        tickMinor = 0;

        majorColor = Color.black;
        minorColor = Color.black;
        nodeColor = Color.black;
        nodeValueColor = Color.black;
        cellEdgeColor = Color.black;
        faultLineColor = Color.black;
        outlineColor = Color.black;

        majorThickness = .015;
        minorThickness = .005;
        cellEdgeThickness = .005;
        faultThickness = .015;

        minorLabelSize = 0.07;
        majorLabelSize = 0.10;
        minorLabelSpacing = 5.0;
        majorLabelSpacing = 5.0;
        minorTickLength = 0.05;
        majorTickLength = 0.07;
        minorTickSpacing = 0.15;
        majorTickSpacing = 0.2;

        nodeSymbol = 7;
        nodeSymbolSize = 0.05;
        nodeValueSize = 0.07;

		hardMin = -1.e30;
		hardMax = 1.e30;

        contourSmoothing = 1;

        zUnitsConversionFactor = 1.0;
    }

/*--------------------------------------------------------------------------*/

/**
  Enable or disable the drawing of contour lines.  If the flag is set to true,
  contour lines will be drawn.  If the flag is set to false, contour lines will
  not be drawn.  By default, contour lines will be drawn.
*/
    public void setShowContours (boolean flag)
    {
        showContours = flag;
    }

/**
 Return if contour line drawing is currently enabled or disabled.  If it is enabled,
 true is returned.  If disabled, false is returned.
*/
    public boolean getShowContours ()
    {
        return showContours;
    }

/*--------------------------------------------------------------------------*/

/**
  Enable or disable the drawing of contour color fills.  If the flag is set to true,
  contour color fills will be drawn.  If the flag is set to false, contour color fills will
  not be drawn.  By default, contour color fills will be drawn.
*/
    public void setShowColorFills (boolean flag)
    {
        showColorFills = flag;
    }

/**
 Return if color fill drawing is currently enabled or disabled.  If it is enabled,
 true is returned.  If disabled, false is returned.
*/
    public boolean getShowColorFills ()
    {
        return showColorFills;
    }

/*--------------------------------------------------------------------------*/

/**
  Enable or disable the drawing of node positions.  If the flag is set to true,
  node positions will be drawn.  If the flag is set to false, node positions will
  not be drawn.  By default, node positions will be drawn.
*/
    public void setShowNodes (boolean flag)
    {
        showNodes = flag;
    }

/**
 Return if node position drawing is currently enabled or disabled.  If it is enabled,
 true is returned.  If disabled, false is returned.
*/
    public boolean getShowNodes ()
    {
        return showNodes;
    }

/*--------------------------------------------------------------------------*/

/**
  Enable or disable the drawing of node values.  If the flag is set to true,
  node values will be drawn.  If the flag is set to false, node values will
  not be drawn.  By default, node values will not be drawn.
*/
    public void setShowNodeValues (boolean flag)
    {
        showNodeValues = flag;
    }

/**
 Return if node value drawing is currently enabled or disabled.  If it is enabled,
 true is returned.  If disabled, false is returned.
*/
    public boolean getShowNodeValues ()
    {
        return showNodeValues;
    }

/*--------------------------------------------------------------------------*/

/**
  Enable or disable the drawing of cell edges.  If the flag is set to true,
  cell edges will be drawn.  If the flag is set to false, cell edges will
  not be drawn.  By default, cell edges will not be drawn.
*/
    public void setShowCellEdges (boolean flag)
    {
        showCellEdges = flag;
    }

/**
 Return if cell edge drawing is currently enabled or disabled.  If it is enabled,
 true is returned.  If disabled, false is returned.
*/
    public boolean getShowCellEdges ()
    {
        return showCellEdges;
    }

/*--------------------------------------------------------------------------*/

/**
  Enable or disable the drawing of fault lines.  If the flag is set to true,
  fault lines will be drawn.  If the flag is set to false, fault lines will
  not be drawn.  By default, fault lines will be drawn.
*/
    public void setShowFaultLines (boolean flag)
    {
        showFaultLines = flag;
    }

/**
 Return if fault line drawing is currently enabled or disabled.  If it is enabled,
 true is returned.  If disabled, false is returned.
*/
    public boolean getShowFaultLines ()
    {
        return showFaultLines;
    }

/*--------------------------------------------------------------------------*/

/**
  Enable or disable the drawing of the surface outline.  If the flag is set to true,
  the outline will be drawn.  If the flag is set to false, the outline will
  not be drawn.  By default, the outline is not drawn.
*/
    public void setShowOutline (boolean flag)
    {
        showOutline = flag;
    }

/**
 Return if outline drawing is currently enabled or disabled.  If it is enabled,
 true is returned.  If disabled, false is returned.
*/
    public boolean getShowOutline ()
    {
        return showOutline;
    }

/*--------------------------------------------------------------------------*/

/**
  Set whether or not the contouring is for a thickness value.  If the data
  are thickness, set the flag to true.  If not thickness, set the flag to false.
  By default, this is false, and data are assumed to not be thickness.
*/
    public void setIsThickness (boolean flag)
    {
        isThickness = flag;
    }

/**
  Return whether or not the contouring is for a thickness value.  If the data
  are thickness, true is returned.  If not thickness, false is returned.
  By default, this is false, and data are assumed to not be thickness.
*/
    public boolean getIsThickness ()
    {
        return isThickness;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the contour smoothing level.  Zero means no smoothing and
 1, 2 and 3 mean light, medium and heavy smoothing respectively.
 The default is 1.
*/
    public void setContourSmoothing (int ival)
    {
        contourSmoothing = ival;
    }

/**
 Get the contour smoothing level.  Zero means no smoothing and
 1, 2 and 3 mean light, medium and heavy smoothing respectively.
 The default is 1.
*/
    public int getContourSmoothing ()
    {
        return contourSmoothing;
    }


/*--------------------------------------------------------------------------*/

/**
  Set the z unit conversion factor.  The trimesh node z values will
  be multiplied by this value prior to calculating any image or contour
  displays.  Also, any node values posted will reflect this conversion factor.
*/
    public void setZUnitsConversionFactor (double val)
    {
        if (val <= 0.0) {
            zUnitsConversionFactor = 1.0;
        }
        else {
            zUnitsConversionFactor = 1.0 / val;
        }
    }

/**
  Set the z unit conversion factor.  The trimesh node z values will
  be multiplied by this value prior to calculating any image or contour
  displays.  Also, any node values posted will reflect this conversion factor.
*/
    public double getZUnitsConversionFactor ()
    {
        if (zUnitsConversionFactor <= 0.0) {
            return 1.0;
        }
        else {
            return 1.0 / zUnitsConversionFactor;
        }
    }

/*--------------------------------------------------------------------------*/

/**
 Set the minimum visible contour value.  If this is greater than the maximum
 contour value or if this is greater than 1.e20, then the min and max visible
 contours are automatically determined.  The default is automatic determination.
*/
    public void setContourMinValue (double value)
    {
        contourMinValue = value;
    }

/**
 Get the minimum visible contour value.  If this is greater than the contour
 max value or if this is greater than 1.e20, then the contour minimum will be
 automatically determined.
*/
    public double getContourMinValue ()
    {
        return contourMinValue;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the maximum visible contour value.  If this is less than the minimum
 contour value or if this is greater than 1.e20, then the min and max visible
 contours are automatically determined.  The default is automatic determination.
*/
    public void setContourMaxValue (double value)
    {
        contourMaxValue = value;
    }

/**
 Get the maximum visible contour value.  If this is less than the contour
 min value or if this is greater than 1.e20, then the contour minimum will be
 automatically determined.
*/
    public double getContourMaxValue ()
    {
        return contourMaxValue;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the contour interval.  If this is set to less than or equal to zero, a
 contour interval is automatically determined.  The default is automatic determination.
*/
    public void setContourInterval (double value)
    {
        contourInterval = value;
    }

/**
 Get the contour interval.  If this is less than or equal to zero, the contour
 interval will be determined automatically.
*/
    public double getContourInterval ()
    {
        return contourInterval;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the log base value for labeling.  This is only used for labeling of contours
 and node values.  If the grid or trimesh is logarithmic, then the numbers drawn
 on contours and nodes should be the actual numbers, not the log of the numbers.
 For example, if the data are log base 10, set the DataLogBase to 10.  A contour
 at level 1 would then be labeled as 10.  If the data are not logarithmic, set this
 to any value less than or equal to 1.  By default, this is set to zero, and the
 data are assumed to not be logarithmic.
*/
    public void setDataLogBase (double value)
    {
        dataLogBase = value;
    }

/**
 Get the log base value for labeling contours and nodes.  If this is less than or
 equal to 1, the data are assumed to be non logarithmic.
*/
    public double getDataLogBase ()
    {
        return dataLogBase;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the minimum value to be visible in the color fill.  This is used to clip the
 color fill to a particular minimum value.
*/
    public void setFillMinValue (double value)
    {
        fillMinValue = value;
    }

/**
 Get the minimum value to be visible in the color fill.  This is used to clip the
 color fill to a particular minimum value.
*/
    public double getFillMinValue ()
    {
        return fillMinValue;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the maximum value to be visible in the color fill.  This is used to clip the
 color fill to a particular maximum value.
*/
    public void setFillMaxValue (double value)
    {
        fillMaxValue = value;
    }

/**
 Get the maximum value to be visible in the color fill.  This is used to clip the
 color fill to a particular maximum value.
*/
    public double getFillMaxValue ()
    {
        return fillMaxValue;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the integer major contour interval.  For example, if you want every fifth
 contour to be major, set this to 5.  If you want the major interval to be
 determined automatically, set this to -1.  By default, the major interval is
 determined automatically.
*/
    public void setMajorInterval (int value)
    {
        majorInterval = value;
    }

/**
 Get the integer major contour interval.  If the value returned is -1, then the major interval
 is determined automatically.
*/
    public int getMajorInterval ()
    {
        return majorInterval;
    }

/*--------------------------------------------------------------------------*/

/**
 Enable or disable major contour labels.  Set to true to enable or set to
 false to disable.  By default, major contours are labeled.
*/
    public void setLabelMajor (boolean flag)
    {
        labelMajor = flag;
    }

/**
 Get whether major contour labeling is enabled or disabled.  Returns true if
 it is enabled or false if disabled.
*/
    public boolean getLabelMajor ()
    {
        return labelMajor;
    }

/*--------------------------------------------------------------------------*/

/**
 Enable or disable minor contour labels.  Set to true to enable or set to
 false to disable.  By default, minor contours are not labeled.
*/
    public void setLabelMinor (boolean flag)
    {
        labelMinor = flag;
    }

/**
 Get whether minor contour labeling is enabled or disabled.  Returns true if
 it is enabled or false if disabled.
*/
    public boolean getLabelMinor ()
    {
        return labelMinor;
    }
/*--------------------------------------------------------------------------*/

/**
 Set the tick mark direction for major contours.  For downhill tick marks,
 set this to 1.  For uphill tickmarks, set to -1.  For no tickmarks, set
 to zero.  By default, this is zero and no tickmarks are drawn.  If the
 value specified in not valid, no tickmarks will be drawn.
*/
    public void setTickMajor (int value)
    {
        if (value < -1  ||  value > 1) {
            value = 0;
        }
        tickMajor = value;
    }

/**
 Get the major contour tick mark direction.  A return value of 1 means downhill
 tick marks.  A return value of -1 means uphill tick marks.  A return value of
 zero means no tick marks.
*/
    public int getTickMajor ()
    {
        return tickMajor;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the tick mark direction for minor contours.  For downhill tick marks,
 set this to 1.  For uphill tickmarks, set to -1.  For no tickmarks, set
 to zero.  By default, this is zero and no tickmarks are drawn.  If the
 value specified in not valid, no tickmarks will be drawn.
*/
    public void setTickMinor (int value)
    {
        if (value < -1  ||  value > 1) {
            value = 0;
        }
        tickMinor = value;
    }

/**
 Get the major contour tick mark direction.  A return value of 1 means downhill
 tick marks.  A return value of -1 means uphill tick marks.  A return value of
 zero means no tick marks.
*/
    public int getTickMinor ()
    {
        return tickMinor;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the color for major contour lines, labels and ticks.  If this is set to
 null, black will be used.  By default, black will be used.
*/
    public void setMajorColor (Color color)
    {
        if (color == null) {
            color = Color.black;
        }
        majorColor = color;
    }

/**
 Get the color for major contour lines, labels and ticks.
*/
    public Color getMajorColor ()
    {
        return majorColor;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the color for minor contour lines, labels and ticks.  If this is set to
 null, black will be used.  By default, black will be used.
*/
    public void setMinorColor (Color color)
    {
        if (color == null) {
            color = Color.black;
        }
        minorColor = color;
    }

/**
 Get the color for minor contour lines, labels and ticks.
*/
    public Color getMinorColor ()
    {
        return minorColor;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the color for node position symbols.  If this is set to
 null, black will be used.  By default, black will be used.
*/
    public void setNodeColor (Color color)
    {
        if (color == null) {
            color = Color.black;
        }
        nodeColor = color;
    }

/**
 Get the color for node position symbols.
*/
    public Color getNodeColor ()
    {
        return nodeColor;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the color for node value labels.  If this is set to
 null, black will be used.  By default, black will be used.
*/
    public void setNodeValueColor (Color color)
    {
        if (color == null) {
            color = Color.black;
        }
        nodeValueColor = color;
    }

/**
 Get the color for node value labels.
*/
    public Color getNodeValueColor ()
    {
        return nodeValueColor;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the color for cell edges.  If this is set to
 null, black will be used.  By default, black will be used.
*/
    public void setCellEdgeColor (Color color)
    {
        if (color == null) {
            color = Color.black;
        }
        cellEdgeColor = color;

    }

/**
 Get the color for cell edges.
*/
    public Color getCellEdgeColor ()
    {
        return cellEdgeColor;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the color for fault lines.  If this is set to
 null, black will be used.  By default, black will be used.
*/
    public void setFaultLineColor (Color color)
    {
        if (color == null) {
            color = Color.black;
        }
        faultLineColor = color;
    }

/**
 Get the color for fault lines.
*/
    public Color getFaultLineColor ()
    {
        return faultLineColor;
    }

/*--------------------------------------------------------------------------*/

/**
 Set the color for the surface outline.  If this is set to
 null, black will be used.  By default, black will be used.
*/
    public void setOutlineColor (Color color)
    {
        if (color == null) {
            color = Color.black;
        }
        outlineColor = color;
    }

/**
 Get the color for the surface outline.
*/
    public Color getOutlineColor ()
    {
        return outlineColor;
    }

/*--------------------------------------------------------------------------*/

/**
  Set the line thickness for major contour lines.  The ticks will be about half as
  thick as the line.  The valid thickness ranges from .001 to 1.0 inches.  If the
  value specified is invalid, the line is .01 inches thick.  By default,
  the line is .01 inches thick.
*/
    public void setMajorThickness (double value)
    {
        if (value < 0.001  ||  value > 1.0) {
            value = .01;
        }
        majorThickness = value;
    }

/**
 Get the current thickness of the major contour lines.
*/
    public double getMajorThickness ()
    {
        return majorThickness;
    }

/*--------------------------------------------------------------------------*/

/**
  Set the line thickness for minor contour lines.  The ticks will be about half as
  thick as the line.  The valid thickness ranges from .001 to 1.0 inches.  If the
  value specified is invalid, the line is .01 inches thick.  By default,
  the line is .01 inches thick.
*/
    public void setMinorThickness (double value)
    {
        if (value < 0.001  ||  value > 1.0) {
            value = .01;
        }
        minorThickness = value;
    }

/**
 Get the current thickness of the major contour lines.
*/
    public double getMinorThickness ()
    {
        return minorThickness;
    }

/*--------------------------------------------------------------------------*/

    /**
     * Get the thickness in inches for cell edge lines.
     */
    public double getCellEdgeThickness() {
        return cellEdgeThickness;
    }

/*--------------------------------------------------------------------------*/

    /**
     Get the thickness, in inches, for fault lines.
     */
    public double getFaultThickness() {
        return faultThickness;
    }

/*--------------------------------------------------------------------------*/

    /**
    Get the size, in inches, for major contour labels.
     */
    public double getMajorLabelSize() {
        return majorLabelSize;
    }

/*--------------------------------------------------------------------------*/

    /**
    Get the spacing, in inches along the line, for major contour labels.
     */
    public double getMajorLabelSpacing() {
        return majorLabelSpacing;
    }

/*--------------------------------------------------------------------------*/

    /**
    Get the length, in inches, for major contour tick marks.
     */
    public double getMajorTickLength() {
        return majorTickLength;
    }

/*--------------------------------------------------------------------------*/

    /**
    Get the spacing, in inches along the line, for major contour tick marks.
     */
    public double getMajorTickSpacing() {
        return majorTickSpacing;
    }

/*--------------------------------------------------------------------------*/

    /**
    Get the size, in inches, for minor contour labels.
     */
    public double getMinorLabelSize() {
        return minorLabelSize;
    }

/*--------------------------------------------------------------------------*/

    /**
    Get the spacing, in inches along the line, for minor contour labels.
     */
    public double getMinorLabelSpacing() {
        return minorLabelSpacing;
    }

/*--------------------------------------------------------------------------*/

    /**
    Get the tick length, in inches, for minor contour tick marks.
     */
    public double getMinorTickLength() {
        return minorTickLength;
    }

/*--------------------------------------------------------------------------*/

    /**
    Get the spacing, in inches along the line, for minor contour tick marks.
     */
    public double getMinorTickSpacing() {
        return minorTickSpacing;
    }

/*--------------------------------------------------------------------------*/

    /**
    Get the symbol number for node positions.
     */
    public int getNodeSymbol() {
        return nodeSymbol;
    }

/*--------------------------------------------------------------------------*/

    /**
    Get the size, in inches, for the node position symbols.
     */
    public double getNodeSymbolSize() {
        return nodeSymbolSize;
    }

/*--------------------------------------------------------------------------*/

    /**
    Get the size, in inches, for the node value labels.
     */
    public double getNodeValueSize() {
        return nodeValueSize;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the thickness for cell edge lines.  The valid thickness values are .001
    to 1.0 (inches).  If the value is not valid, .005 inches is used.  By default,
    .005 inches is used.
     */
    public void setCellEdgeThickness(double d) {
        if (d < 0.001  ||  d > 1.0) {
            d = 0.005;
        }
        cellEdgeThickness = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the thickness for fault lines.  The valid thickness values are .001
    to 1.0 (inches).  If the value is not valid, .005 inches is used.  By default,
    .015 inches is used.
     */
    public void setFaultThickness(double d) {
        if (d < 0.001  ||  d > 1.0) {
            d = 0.015;
        }
        faultThickness = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the size (in inches) for major contour labels.  Valid sizes are .01
    to 10.0 (inches).  If an invalid value is specified, .1 inches is used.
    By default, .1 inches is used.
     */
    public void setMajorLabelSize(double d) {
        if (d < .01  ||  d > 10.0) {
            d = .1;
        }
        majorLabelSize = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the spacing (in inches along the line) for major contour labels.
    This will not be exactly honored.  It is a guide for label spacing only.
    Labels may be adjusted somewhat to be drawn in smooth areas of the contour.
    By default, 6 inches is used.
     */
    public void setMajorLabelSpacing(double d) {
        majorLabelSpacing = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the tick mark length (in inches) for the major contours.  By default,
    .07 inches is used.
     */
    public void setMajorTickLength(double d) {
        majorTickLength = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the tick mark spacing (in inches along the line) for major contours.
    By default, .2 inches is used.
     */
    public void setMajorTickSpacing(double d) {
        majorTickSpacing = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the size (in inches) for minor contour labels.  Valid sizes are .01
    to 10.0 (inches).  If an invalid value is specified, .07 inches is used.
    By default, .07 inches is used.
     */
    public void setMinorLabelSize(double d) {
        if (d < .01  ||  d > 10.0) {
            d = .1;
        }
        minorLabelSize = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the spacing (in inches along the line) for minor contour labels.
    This will not be exactly honored.  It is a guide for label spacing only.
    Labels may be adjusted somewhat to be drawn in smooth areas of the contour.
    By default, 6 inches is used.
     */
    public void setMinorLabelSpacing(double d) {
        minorLabelSpacing = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the tick mark length (in inches) for minor contour tick marks.
    By default, .05 inches is used.
     */
    public void setMinorTickLength(double d) {
        minorTickLength = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the tick mark spacing (in inches along the line) for minor contours.
    By default, .15 inches is used.
     */
    public void setMinorTickSpacing(double d) {
        minorTickSpacing = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the symbol number for node positions.  Valid symbol numbers are 1 to 140.
    If an invalid symbol is specified, symbol 7 (a cross) is used.  By default,
    symbol 7 is used.
     */
    public void setNodeSymbol(int i) {
        nodeSymbol = i;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the size (in inches) for symbols used to draw node positions.
    Valid values are .01 to 10.0.  If an invalid value is specified,
    .05 inches is used.  By default, .05 inches is used.
     */
    public void setNodeSymbolSize(double d) {
        nodeSymbolSize = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the size (in inches) for node value labels.  Valid sizes are .01
    to 10.0 (inches).  If an invalid value is specified, .07 inches is used.
    By default, .07 inches is used.
     */
    public void setNodeValueSize(double d) {
        nodeValueSize = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Return the font number for contour labels.
     */
    public int getContourFont() {
        return contourFont;
    }

/*--------------------------------------------------------------------------*/

    /**
    Return the font number for node value drawing.
     */
    public int getNodeFont() {
        return nodeFont;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the font for contour label drawing.  Valid fonts numbers are 0-7 and 101-107.
    If an invalid font is specified, font 102 is used.  By default, font 102 is used.
     */
    public void setContourFont(int i) {
        if (i < 0  ||  i > 107) {
            i = 102;
        }
        else if (i > 7  &&  i < 101) {
            i = 102;
        }
        contourFont = i;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the font for node value drawing.  Valid fonts numbers are 0-7 and 101-107.
    If an invalid font is specified, font 102 is used.  By default, font 102 is used.
     */
    public void setNodeFont(int i) {
        if (i < 0  ||  i > 107) {
            i = 102;
        }
        else if (i > 7  &&  i < 101) {
            i = 102;
        }
        nodeFont = i;
    }

/*--------------------------------------------------------------------------*/

    /**
    Return the absolute maximum value that the data can possibly have.
     */
    public double getHardMax() {
        return hardMax;
    }

/*--------------------------------------------------------------------------*/

    /**
    Return the absolute minimum value that the data can possibly have.
     */
    public double getHardMin() {
        return hardMin;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the absolute maximum value that the data can possibly have.
     */
    public void setHardMax(double d) {
        hardMax = d;
    }

/*--------------------------------------------------------------------------*/

    /**
    Set the absolute minimum value that the data can possibly have.
     */
    public void setHardMin(double d) {
        hardMin = d;
    }

/*--------------------------------------------------------------------------*/

}  // end of DLSurfaceProperties class.
