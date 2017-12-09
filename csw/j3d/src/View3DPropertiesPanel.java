/*
 */
package  csw.j3d.src;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusAdapter;
import java.awt.event.FocusEvent;
import java.text.DecimalFormat;
import java.util.Hashtable;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.SwingConstants;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import csw.jutils.src.View3DProperties;
import csw.jutils.src.TranslationManager;

//import   csw.uistuff.*;



public class View3DPropertiesPanel extends JPanel 
  implements csw.uistuff.ValueChangeListener
{

  public static final long  serialVersionUID = 20092371;

  private View3DProperties view3DProperties;

  // Main panels
  private Box generalBox;
  private Box modelBox;
  private Box axesBox;
  private Box veBox;

  // General
  private JButton bgColorButton;
  private csw.uistuff.TextCheckBox lightsCheck;
  private csw.uistuff.TextCheckBox northArrowCheck;
  private csw.uistuff.TextCheckBox scaleBarCheck;
  private csw.uistuff.TextCheckBox colorScaleCheck;

  // Model
  private csw.uistuff.TextCheckBox titlesCheck;
  private csw.uistuff.TextCheckBox wallsCheck;
  private csw.uistuff.TextCheckBox boundingBoxCheck;
  private csw.uistuff.FontName titlesFontNameControl;
  private csw.uistuff.DoubleField titlesFontSizeControl;
  private csw.uistuff.TextToggleButton titlesBoldControl;
  private csw.uistuff.TextToggleButton titlesItalicControl;

  // Cache user choices for the titles font
  private String titlesFontName;
  private int titlesFontSize;
  private boolean isTitlesFontBold;
  private boolean isTitlesFontItalic;

  // Axes
  private csw.uistuff.TextCheckBox axesColorCheck;
  private csw.uistuff.TextCheckBox axesLabelsCheck;
  private csw.uistuff.TextCheckBox axesValuesCheck;

  // Axes fonts
  private csw.uistuff.FontName axesLabelsFontNameControl;
  private csw.uistuff.DoubleField axesLabelsFontSizeControl;
  private csw.uistuff.TextToggleButton axesLabelsBoldControl;
  private csw.uistuff.TextToggleButton axesLabelsItalicControl;

  private csw.uistuff.FontName axesValuesFontNameControl;
  private csw.uistuff.DoubleField axesValuesFontSizeControl;
  private csw.uistuff.TextToggleButton axesValuesBoldControl;
  private csw.uistuff.TextToggleButton axesValuesItalicControl;

  // Cache user choices for the axes labels font
  private String axesLabelsFontName;
  private int axesLabelsFontSize;
  private boolean isAxesLabelsFontBold;
  private boolean isAxesLabelsFontItalic;

  // Cache user choices for the axes values font
  private String axesValuesFontName;
  private int axesValuesFontSize;
  private boolean isAxesValuesFontBold;
  private boolean isAxesValuesFontItalic;

  // Vertical exaggeration
  private csw.uistuff.DoubleField veText;
  private JSlider veSlider;
  private csw.uistuff.TextCheckBox showVeCheck;
  private static final double constant = Math.pow(10, 4);
  private static final int SLIDER_MIN = (int) ((-1) * constant);
  private static final int SLIDER_MAX = (int) (2 * constant);

  public View3DPropertiesPanel(View3DProperties props) {
    super (new GridBagLayout());
    this.view3DProperties = props;
    buildPanel();
    setControlValues();
  }

  private void buildPanel() {
    generalBox = buildGeneralOptionsBox();
    modelBox = buildModelOptionsBox();
    axesBox = buildAxesOptionsBox();
    veBox = buildVerticalExaggerationBox();

    GridBagConstraints gbc = new GridBagConstraints();
    gbc.gridx = 0;
    gbc.gridy = 0;
    gbc.anchor = GridBagConstraints.WEST;
    this.add(generalBox, gbc);

    gbc.gridx = 0;
    gbc.gridy = 1;
    this.add(modelBox, gbc);

    gbc.gridx = 0;
    gbc.gridy = 2;
    this.add(axesBox, gbc);

    gbc.gridx = 0;
    gbc.gridy = 3;
    this.add(veBox, gbc);
  }

  /**
   * Returns the maximum width for of all of the main components.
   */
  public int getMaximumWidth() {
    int maxWidth = Math.max(
      generalBox.getPreferredSize().width,
      modelBox.getPreferredSize().width
    );
    maxWidth = Math.max(maxWidth, axesBox.getPreferredSize().width);
    maxWidth = Math.max(maxWidth, veBox.getPreferredSize().width);
    return maxWidth;
  }

  /**
   * Sets the width of the main components to the specified width.
   * @param width specified width
   */
  public void setPanelWidth(int width) {
    Dimension d = new Dimension(width, generalBox.getPreferredSize().height);
    generalBox.setPreferredSize(d);
    generalBox.setMaximumSize(d);
    d = new Dimension(width, modelBox.getPreferredSize().height);
    modelBox.setPreferredSize(d);
    modelBox.setMaximumSize(d);
    d = new Dimension(width, axesBox.getPreferredSize().height);
    axesBox.setPreferredSize(d);
    axesBox.setMaximumSize(d);
    d = new Dimension(width, veBox.getPreferredSize().height);
    veBox.setPreferredSize(d);
    veBox.setMaximumSize(d);
  }

  private Box buildGeneralOptionsBox() {

    // Color button and label
    Box hBox = Box.createHorizontalBox();
    hBox.setAlignmentX(LEFT_ALIGNMENT);
    bgColorButton = new JButton();
    bgColorButton.setMaximumSize(new Dimension(40, 40));
    hBox.add(bgColorButton);
    bgColorButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        Color newColor = JColorChooser.showDialog(
          bgColorButton,
          "Choose 3D View Background Color",
          bgColorButton.getBackground()
        );
        if (newColor != null) {
          bgColorButton.setBackground(newColor);
        }
      }
    });
    hBox.add(Box.createHorizontalStrut(5));
    JLabel bgColorLabel =
      new JLabel(TranslationManager.translate("Background Color"));
    hBox.add(bgColorLabel);

    // Lights
    lightsCheck = new csw.uistuff.TextCheckBox(this,
                        TranslationManager.translate("Lights"),
                        view3DProperties.showLights());
    lightsCheck.setAlignmentX(LEFT_ALIGNMENT);

    // North arrow
    northArrowCheck =
      new csw.uistuff.TextCheckBox(this,
                       TranslationManager.translate("North arrow"),
                       view3DProperties.showCompass());
    northArrowCheck.setAlignmentX(LEFT_ALIGNMENT);

    // Scale bar
    scaleBarCheck = new csw.uistuff.TextCheckBox(this,
                  TranslationManager.translate("Scale bar"),
                  view3DProperties.showScaleBar());
    scaleBarCheck.setAlignmentX(LEFT_ALIGNMENT);

    // Color scale
    colorScaleCheck =
      new csw.uistuff.TextCheckBox(this,
                   TranslationManager.translate("Color scale"),
                   view3DProperties.showColorScale());
    colorScaleCheck.setAlignmentX(LEFT_ALIGNMENT);

    // General options box
    Box box = Box.createVerticalBox();
    box.add(hBox);
    box.add(lightsCheck);
    box.add(northArrowCheck);
    box.add(scaleBarCheck);
    box.add(colorScaleCheck);

    box.setBorder(
      BorderFactory.createTitledBorder(TranslationManager.translate("General"))
    );
    return box;
  }

  private Box buildModelOptionsBox() {
    // titles
    titlesCheck = new csw.uistuff.TextCheckBox(this,
                  TranslationManager.translate("Titles"),
                  view3DProperties.showTitles());
    titlesCheck.setAlignmentX(LEFT_ALIGNMENT);

    titlesFontName = view3DProperties.getTitlesFont().getFamily();
    titlesFontNameControl = new csw.uistuff.FontName(this, titlesFontName);

    titlesFontSize = view3DProperties.getTitlesFont().getSize();
    titlesFontSizeControl = new csw.uistuff.DoubleField(this, titlesFontSize, 0.0, 100.0);
    titlesFontSizeControl.setColumns (5);

    isTitlesFontBold = view3DProperties.getTitlesFont().isBold();
    titlesBoldControl = new csw.uistuff.TextToggleButton(this, "Bold", isTitlesFontBold);

    isTitlesFontItalic = view3DProperties.getTitlesFont().isItalic();
    titlesItalicControl = new csw.uistuff.TextToggleButton(this, "Italic", isTitlesFontItalic);

    Box titlesLine = Box.createHorizontalBox();
    titlesLine.add(titlesCheck);
    titlesLine.add(titlesFontNameControl.getComboBox());
    titlesLine.add(titlesFontSizeControl);
    titlesLine.add(titlesBoldControl);
    titlesLine.add(titlesItalicControl);
    titlesLine.setAlignmentX(LEFT_ALIGNMENT);

    // Walls
    wallsCheck = new csw.uistuff.TextCheckBox(this,
                 TranslationManager.translate("Walls"),
                 view3DProperties.showWalls());
    wallsCheck.setAlignmentX(LEFT_ALIGNMENT);

    // Bounding box
    boundingBoxCheck =
      new csw.uistuff.TextCheckBox(this,
                                   TranslationManager.translate("Bounding Box"),
                                   view3DProperties.showBoundingBox());
    boundingBoxCheck.setAlignmentX(LEFT_ALIGNMENT);

    // Model options box
    Box box = Box.createVerticalBox();
    box.add(titlesLine);
    box.add(wallsCheck);
    box.add(boundingBoxCheck);

    box.setBorder(
      BorderFactory.createTitledBorder(TranslationManager.translate("Model"))
    );
    return box;
  }

  private Box buildAxesOptionsBox() {
    // Axes color
    axesColorCheck = new csw.uistuff.TextCheckBox(this,
                     TranslationManager.translate("Color"),
                     view3DProperties.showAxes());
    axesColorCheck.setAlignmentX(LEFT_ALIGNMENT);

    // Axes labels
    axesLabelsCheck =
      new csw.uistuff.TextCheckBox(this,
           TranslationManager.translate("Labels"),
           view3DProperties.showNames());
    axesLabelsCheck.setAlignmentX(LEFT_ALIGNMENT);

    axesLabelsFontName = view3DProperties.getAxisNamesFont().getFamily();
    axesLabelsFontNameControl =
      new csw.uistuff.FontName(this, axesLabelsFontName);

    axesLabelsFontSize = view3DProperties.getAxisNamesFont().getSize();
    axesLabelsFontSizeControl =
      new csw.uistuff.DoubleField(this, axesLabelsFontSize, 0.0, 100.0);
    axesLabelsFontSizeControl.setColumns (5);

    isAxesLabelsFontBold = view3DProperties.getAxisNamesFont().isBold();
    axesLabelsBoldControl =
      new csw.uistuff.TextToggleButton(this, "Bold", isAxesLabelsFontBold);

    isAxesLabelsFontItalic = view3DProperties.getAxisNamesFont().isItalic();
    axesLabelsItalicControl =
      new csw.uistuff.TextToggleButton(this, "Italic", isAxesLabelsFontItalic);

    Box axesLabelsLine = Box.createHorizontalBox();
    axesLabelsLine.add(axesLabelsCheck);
    axesLabelsLine.add(axesLabelsFontNameControl.getComboBox());
    axesLabelsLine.add(axesLabelsFontSizeControl);
    axesLabelsLine.add(axesLabelsBoldControl);
    axesLabelsLine.add(axesLabelsItalicControl);
    axesLabelsLine.setAlignmentX(LEFT_ALIGNMENT);

    // Axes values
    axesValuesCheck = new csw.uistuff.TextCheckBox(this,
                      TranslationManager.translate("Values"),
                      view3DProperties.showNumbers());
    axesValuesCheck.setAlignmentX(LEFT_ALIGNMENT);

    axesValuesFontName = view3DProperties.getAxisNumbersFont().getFamily();
    axesValuesFontNameControl = new csw.uistuff.FontName(this, axesValuesFontName);

    axesValuesFontSize = view3DProperties.getAxisNumbersFont().getSize();
    axesValuesFontSizeControl = new csw.uistuff.DoubleField(this, axesValuesFontSize, 0.0, 100.0);
    axesValuesFontSizeControl.setColumns (5);

    isAxesValuesFontBold = view3DProperties.getAxisNumbersFont().isBold();
    axesValuesBoldControl = new csw.uistuff.TextToggleButton(this, "Bold", isAxesValuesFontBold);

    isAxesValuesFontItalic = view3DProperties.getAxisNumbersFont().isItalic();
    axesValuesItalicControl =
      new csw.uistuff.TextToggleButton(this, "Italic", isAxesValuesFontItalic);

    Box axesValuesLine = Box.createHorizontalBox();
    axesValuesLine.add(axesValuesCheck);
    axesValuesLine.add(axesValuesFontNameControl.getComboBox());
    axesValuesLine.add(axesValuesFontSizeControl);
    axesValuesLine.add(axesValuesBoldControl);
    axesValuesLine.add(axesValuesItalicControl);
    axesValuesLine.setAlignmentX(LEFT_ALIGNMENT);

    // Axes options box
    Box box = Box.createVerticalBox();
    box.add(axesColorCheck);
    box.add(axesLabelsLine);
    box.add(axesValuesLine);

    box.setBorder(
      BorderFactory.createTitledBorder(TranslationManager.translate("Axes"))
    );
    return box;
  }

  private Box buildVerticalExaggerationBox() {

    double vertExag = view3DProperties.getVerticalExaggeration();

    showVeCheck = new csw.uistuff.TextCheckBox(this, TranslationManager.translate("Show"));
    showVeCheck.setAlignmentX(LEFT_ALIGNMENT);
    showVeCheck.setSelectedValue(view3DProperties.showExaggeration());

    veText = new csw.uistuff.DoubleField(this, vertExag, .01, 1000.0);
    veText.setAlignmentX(RIGHT_ALIGNMENT);
    veText.setMaximumSize(veText.getPreferredSize());
    veText.setColumns (5);

    Box veLine = Box.createHorizontalBox();
    veLine.add(showVeCheck);
    veLine.add(Box.createHorizontalGlue());
    veLine.add(veText);
    veLine.setAlignmentX(CENTER_ALIGNMENT);

    int verticalExageration = (int)(Math.log(vertExag) / Math.log(10) * constant);
    veSlider = new JSlider(
      SwingConstants.HORIZONTAL,
      SLIDER_MIN,
      SLIDER_MAX,
      verticalExageration
    );
    veSlider.setMajorTickSpacing(10000);

    Hashtable<Integer, JLabel> labels = new Hashtable<Integer, JLabel>();
    labels.put(new Integer(-10000), new JLabel("0.1"));

    labels.put(new Integer(0), new JLabel("1"));

    labels.put(new Integer(10000), new JLabel("10"));

    labels.put(new Integer(20000), new JLabel("100"));
    veSlider.setLabelTable(labels);

    veSlider.setPaintLabels(true);
    veSlider.setPaintTicks(true);
    veSlider.setPaintTrack(true);

    veSlider.addChangeListener(new ChangeListener() {
      public void stateChanged(ChangeEvent e) {
        double exag = Math.pow(10, (veSlider.getValue() / constant));
        if (exag  >= 1) {
          exag = Math.round(exag);
        }
        veText.setVal(exag);
      }
    });

    veText.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          try {
            double exag = Math.log(Double.parseDouble(veText.getText())) / Math.log(10) * constant;
            if (exag >= SLIDER_MIN && exag <= SLIDER_MAX)
              veSlider.setValue((int)exag);
            else
              veText.setText(String.valueOf(view3DProperties.getVerticalExaggeration()));
          }
          catch (NumberFormatException nfe) {
            veText.setText(String.valueOf(view3DProperties.getVerticalExaggeration()));
          }
        }
      }
    );
    /* The FocusListener is needed here. If the exaggeration is changed by typing in the
     * text field, and then the focus is moved to somewhere else instead of a carriage return key
     * entered, the slider bar will reflect the new value.
     */

    veText.addFocusListener(
      new FocusAdapter() {
        public void focusLost(FocusEvent e) {
          try {
            double exag = Math.log(Double.parseDouble(veText.getText())) / Math.log(10) * constant;
            if (exag >= SLIDER_MIN && exag <= SLIDER_MAX)
              veSlider.setValue((int)exag);
            else
              veText.setText(view3DProperties.getVerticalExaggeration() + "");
          }
          catch (NumberFormatException nfe) {
            veText.setText(view3DProperties.getVerticalExaggeration() + "");
          }
        }
      }
    );
    veSlider.setAlignmentX(CENTER_ALIGNMENT);

    Box veSliderLine = Box.createHorizontalBox();
    veSliderLine.add(Box.createHorizontalGlue());
    veSliderLine.add(veSlider);
    veSliderLine.add(Box.createHorizontalGlue());
    veSliderLine.setAlignmentX(CENTER_ALIGNMENT);

    // Vertical exaggeration options box
    Box box = Box.createVerticalBox();
    box.add(veLine);
    box.add(veSliderLine);

    box.setBorder(
      BorderFactory.createTitledBorder(
        TranslationManager.translate("Vertical Exaggeration")
      )
    );
    return box;
  }

  private void setControlValues() {
    bgColorButton.setBackground(view3DProperties.getBackgroundColor());
    lightsCheck.setSelectedValue(view3DProperties.showLights());
    northArrowCheck.setSelectedValue(view3DProperties.showCompass());

    titlesCheck.setSelectedValue(view3DProperties.showTitles());
    wallsCheck.setSelectedValue(view3DProperties.showWalls());
    boundingBoxCheck.setSelectedValue(view3DProperties.showBoundingBox());

    axesColorCheck.setSelectedValue(view3DProperties.showAxes());
    axesLabelsCheck.setSelectedValue(view3DProperties.showNames());
    axesValuesCheck.setSelectedValue(view3DProperties.showNumbers());
  }


/**
 * Method called when the Ok or apply button is clicked on the 
 * pafrent dialog of this panel.
 */
  void apply() {

    view3DProperties.setBackgroundColor(bgColorButton.getBackground());
    view3DProperties.setShowLights(lightsCheck.getSelectedValue());
    view3DProperties.setShowCompass(northArrowCheck.getSelectedValue());

    view3DProperties.setShowTitles(titlesCheck.getSelectedValue());
    view3DProperties.setShowWalls(wallsCheck.getSelectedValue());
    view3DProperties.setShowBoundingBox(boundingBoxCheck.getSelectedValue());

    view3DProperties.setShowAxes(axesColorCheck.getSelectedValue());
    view3DProperties.setShowNames(axesLabelsCheck.getSelectedValue());
    view3DProperties.setShowNumbers(axesValuesCheck.getSelectedValue());

    view3DProperties.setShowExaggeration(showVeCheck.getSelectedValue());
    view3DProperties.setVerticalExaggeration(
      veText.getVal()
    );

    titlesFontName = titlesFontNameControl.getFontName();
    titlesFontSize = (int)titlesFontSizeControl.getVal();
    isTitlesFontBold = titlesBoldControl.getSelectedValue();
    isTitlesFontItalic = titlesItalicControl.getSelectedValue();

    axesValuesFontName = axesValuesFontNameControl.getFontName();
    axesValuesFontSize = (int)axesValuesFontSizeControl.getVal();
    isAxesValuesFontBold = axesValuesBoldControl.getSelectedValue();
    isAxesValuesFontItalic = axesValuesItalicControl.getSelectedValue();

    axesLabelsFontName = axesLabelsFontNameControl.getFontName();
    axesLabelsFontSize = (int)axesLabelsFontSizeControl.getVal();
    isAxesLabelsFontBold = axesLabelsBoldControl.getSelectedValue();
    isAxesLabelsFontItalic = axesLabelsItalicControl.getSelectedValue();

    Font font = createFont(
      titlesFontName,
      titlesFontSize,
      isTitlesFontBold,
      isTitlesFontItalic
    );
    view3DProperties.setTitlesFont(font);

    font = createFont(
      axesValuesFontName,
      axesValuesFontSize,
      isAxesValuesFontBold,
      isAxesValuesFontItalic
    );
    view3DProperties.setAxisNumbersFont(font);

    font = createFont(
      axesLabelsFontName,
      axesLabelsFontSize,
      isAxesLabelsFontBold,
      isAxesLabelsFontItalic
    );
    view3DProperties.setAxisNamesFont(font);

  }

  private Font createFont(
    String fontName,
    int fontSize,
    boolean isBold,
    boolean isItalic
  ) {

    int fontStyle;
    if (isBold && isItalic) {
      fontStyle = Font.BOLD + Font.ITALIC;
    } else if (isBold) {
      fontStyle = Font.BOLD;
    } else if (isItalic) {
      fontStyle = Font.ITALIC;
    } else {
      fontStyle = Font.PLAIN;
    }
    Font font = new Font(fontName, fontStyle, fontSize);
    return font;

  }

  void reset() {
    setControlValues();
  }


// Method to respond to value changes on individual controls
// in the panel.

//  private csw.uistuff.DoubleField titlesFontSizeControl;
//  private csw.uistuff.TextToggleButton titlesBoldControl;
//  private csw.uistuff.TextToggleButton titlesItalicControl;
  public void valueChange(csw.uistuff.ValueChangeEvent ev) {
    Object source = ev.getValueChangeObject();
    if (source == titlesFontSizeControl) {
      try {
        titlesFontSizeControl.commitEdit ();
      }
      catch (Exception e) {
      }
      titlesFontSize = (int)titlesFontSizeControl.getVal ();
    }
  }

}
