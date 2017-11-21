/*
 */

package csw.j3d.src;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import csw.jutils.src.LightData;
import csw.jutils.src.View3DProperties;
import csw.jutils.src.TranslationManager;

import csw.uistuff.*;

/**
 * The dialog for lighting options used in a 3D View.
 */
public class View3DLightingOptionsPanel extends JPanel
                         implements csw.uistuff.ValueChangeListener
{

  public static final long serialVersionUID = 299456102;

  private View3DProperties view3DProperties;
  private Box optionsBox, editBox;
  private JPanel intensityPanel, positionPanel;
  private csw.uistuff.TextCheckBox smoothShading, lighting, enabled;

  private JComboBox<String> lightsCombo;
  private JLabel dimLabel, brightLabel;
  private JSlider brightnessSlider, xSlider, ySlider, zSlider;
  private LightData origLight1, origLight2, origLight3, origLight4;
  private LightData tempLight1, tempLight2, tempLight3, tempLight4;
  private ArrayList<LightData> lightDataList = new ArrayList<LightData>();

  public View3DLightingOptionsPanel(View3DProperties props) {

    view3DProperties = props;

    // construct original copies
    origLight1 = new LightData(props.getLight1());
    origLight2 = new LightData(props.getLight2());
    origLight3 = new LightData(props.getLight3());
    origLight4 = new LightData(props.getLight4());
    // construct temporary copies
    tempLight1 = new LightData(props.getLight1());
    tempLight2 = new LightData(props.getLight2());
    tempLight3 = new LightData(props.getLight3());
    tempLight4 = new LightData(props.getLight4());
    // add temporary lights to the list
    lightDataList.add(tempLight1);
    lightDataList.add(tempLight2);
    lightDataList.add(tempLight3);
    lightDataList.add(tempLight4);

    createOptionsBox();
    createEditBox();

    this.setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
    optionsBox.setAlignmentX(LEFT_ALIGNMENT);
    this.add(optionsBox);
    this.add(Box.createVerticalStrut(6));
    editBox.setAlignmentX(LEFT_ALIGNMENT);
    this.add(editBox);
    this.add(Box.createVerticalGlue());

    translateAllContents();
  }

  /**
   * Returns the maximum width for of all of the main components.
   */
  public int getMaximumWidth() {
    int maxWidth = Math.max(
      optionsBox.getPreferredSize().width,
      editBox.getPreferredSize().width
    );
    return maxWidth;
  }

  /**
   * Sets the width of the main components to the specified width.
   * @param width specified width
   */
  public void setPanelWidth(int width) {
    Dimension d = new Dimension(width, optionsBox.getPreferredSize().height);
    optionsBox.setPreferredSize(d);
    optionsBox.setMaximumSize(d);
    d = new Dimension(width, editBox.getPreferredSize().height);
    editBox.setPreferredSize(d);
    editBox.setMaximumSize(d);
  }

/*----------------------------------------------------------------------*/

  private void createOptionsBox() {
    optionsBox = Box.createVerticalBox();

    String sstrans = TranslationManager.translate ("Smooth Shading");
    smoothShading = new csw.uistuff.TextCheckBox(this,
                         sstrans,
                         view3DProperties.isSmoothShading());
    smoothShading.setAlignmentX(LEFT_ALIGNMENT);
    smoothShading.setSelected(view3DProperties.isSmoothShading());
    optionsBox.add(smoothShading);
    optionsBox.add(Box.createVerticalStrut(2));

    sstrans = TranslationManager.translate ("Lighting");
    lighting = new csw.uistuff.TextCheckBox(this,
                    sstrans,
                    view3DProperties.isLightingNShadowing());
    lighting.setAlignmentX(LEFT_ALIGNMENT);
    lighting.setSelectedValue(view3DProperties.isLightingNShadowing());
    optionsBox.add(lighting);
    optionsBox.add(Box.createVerticalStrut(2));

  }

  private void createEditBox() {
    editBox =  Box.createVerticalBox();

    Box lightsBox = Box.createHorizontalBox();
    String[] lights = {"Light 1", "Light 2", "Light 3", "Light 4"};
    lightsCombo = new JComboBox<String>(lights);
    lightsCombo.setMaximumSize(
      new Dimension(100, (int)(lightsCombo.getPreferredSize().getHeight()))
    );
    lightsCombo.setSelectedIndex(0);
    lightsBox.add(lightsCombo);
    lightsBox.add(Box.createHorizontalStrut(2));

    String  sstrans = TranslationManager.translate ("Enabled");
    enabled = new TextCheckBox(this, sstrans);
    enabled.setSelected(((lightDataList.get(0))).isEnabled());
    lightsBox.add(enabled);
    editBox.add(lightsBox);

    intensityPanel = new JPanel();
    intensityPanel.setLayout(new BorderLayout());
    dimLabel = new JLabel();
    intensityPanel.add(dimLabel, BorderLayout.WEST);
    brightnessSlider = new JSlider(
      0,
      0,
      100,
      ((lightDataList.get(0))).getBrightness()
    );
    brightnessSlider.setMajorTickSpacing(10);
    brightnessSlider.setMinorTickSpacing(5);
    brightnessSlider.setPaintLabels(true);
    brightnessSlider.setPaintTicks(true);
    brightnessSlider.setPaintTrack(true);
    brightnessSlider.addChangeListener(new ChangeListener() {
      public void stateChanged(ChangeEvent e) {
        int brightness = brightnessSlider.getValue();
        ((lightDataList.get(lightsCombo.getSelectedIndex()))).setBrightness(brightness);
      }
    });
    intensityPanel.add(brightnessSlider, BorderLayout.CENTER);
    brightLabel = new JLabel();
    intensityPanel.add(brightLabel, BorderLayout.EAST);
    editBox.add(intensityPanel);

    positionPanel = new JPanel();
    xSlider = new JSlider(
      0,
      -100,
      100,
      (int)(((lightDataList.get(0))).getX() * 100)
    );
    xSlider.setMajorTickSpacing(50);
    xSlider.setMinorTickSpacing(10);
    xSlider.setPaintLabels(false);
    xSlider.setPaintTicks(true);
    xSlider.setPaintTrack(true);
    xSlider.addChangeListener(new ChangeListener() {
      public void stateChanged(ChangeEvent e) {
        int x = xSlider.getValue();
        ((lightDataList.get(lightsCombo.getSelectedIndex()))).
          setX(x / 100.0);
      }
    });
    ySlider = new JSlider(
      0,
      -100,
      100,
      (int)(((lightDataList.get(0))).getY() * 100)
    );
    ySlider.setMajorTickSpacing(50);
    ySlider.setMinorTickSpacing(10);
    ySlider.setPaintLabels(false);
    ySlider.setPaintTicks(true);
    ySlider.setPaintTrack(true);
    ySlider.addChangeListener(new ChangeListener() {
      public void stateChanged(ChangeEvent e) {
        int y = ySlider.getValue();
        ((lightDataList.get(lightsCombo.getSelectedIndex()))).
          setY(y / 100.0);
      }
    });
    zSlider = new JSlider(
      0,
      -100,
      100,
      (int)(((lightDataList.get(0))).getZ() * 100)
    );
    zSlider.setMajorTickSpacing(50);
    zSlider.setMinorTickSpacing(10);
    zSlider.setPaintLabels(false);
    zSlider.setPaintTicks(true);
    zSlider.setPaintTrack(true);
    zSlider.addChangeListener(new ChangeListener() {
      public void stateChanged(ChangeEvent e) {
        int z = zSlider.getValue();
        ((lightDataList.get(lightsCombo.getSelectedIndex()))).setZ(z / 100.0);
      }
    });

    GridBagLayout gridbag = new GridBagLayout();
    positionPanel.setLayout(gridbag);
    GridBagConstraints c = new GridBagConstraints();
    c.gridx = 0;
    c.gridy = 0;
    gridbag.setConstraints(xSlider, c);
    positionPanel.add(xSlider);
    c.gridx = 1;
    JLabel xLabel = new JLabel("X");
    gridbag.setConstraints(xLabel, c);
    positionPanel.add(xLabel);

    c.gridx = 0;
    c.gridy = 1;
    gridbag.setConstraints(ySlider, c);
    positionPanel.add(ySlider);
    c.gridx = 1;
    JLabel yLabel = new JLabel("Y");
    gridbag.setConstraints(yLabel, c);
    positionPanel.add(yLabel);

    c.gridx = 0;
    c.gridy = 2;
    gridbag.setConstraints(zSlider, c);
    positionPanel.add(zSlider);
    c.gridx = 1;
    JLabel zLabel = new JLabel("Z");
    gridbag.setConstraints(zLabel, c);
    positionPanel.add(zLabel);

    editBox.add(positionPanel);

    lightsCombo.addActionListener(
      new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          int index = lightsCombo.getSelectedIndex();
          LightData lightData = (lightDataList.get(index));
          enabled.setSelected(lightData.isEnabled());
          brightnessSlider.setValue(lightData.getBrightness());
          int x = (int)(100 * lightData.getX());
          int y = (int)(100 * lightData.getY());
          int z = (int)(100 * lightData.getZ());
          xSlider.setValue(x);
          ySlider.setValue(y);
          zSlider.setValue(z);
        }
      }
    );
  }

  void apply() {
    view3DProperties.setLightingProperties(
      smoothShading.getSelectedValue(),
      lighting.getSelectedValue(),
      lightDataList.get(0),
      lightDataList.get(1),
      lightDataList.get(2),
      lightDataList.get(3)
    );
  }

  void reset() {
    // reset all data
    int nLights = lightsCombo.getItemCount();
    for (int lightIndex = 0; lightIndex < nLights; lightIndex++) {
      LightData origLight = null;
      if (lightIndex == 0)
        origLight = origLight1;
      else if (lightIndex == 1)
        origLight = origLight2;
      else if (lightIndex == 2)
        origLight = origLight3;
      else if (lightIndex == 3)
        origLight = origLight4;
      else
        throw new IllegalStateException("no origLight for index=="+lightIndex);

      if (origLight != null) {
        LightData ld1 = lightDataList.get(lightIndex);
        ld1.setValues(origLight);
      }
    }
    // update the dialog
    setControlValues();
  }

  private void setControlValues() {
    smoothShading.setSelected(view3DProperties.isSmoothShading());
    lighting.setSelected(view3DProperties.isLightingNShadowing());

    int lightIndex = lightsCombo.getSelectedIndex();
    LightData lightData = (lightDataList.get(lightIndex));
    enabled.setSelected(lightData.isEnabled());
    brightnessSlider.setValue(lightData.getBrightness());
    xSlider.setValue((int)(lightData.getX() * 100));
    ySlider.setValue((int)(lightData.getY() * 100));
    zSlider.setValue((int)(lightData.getZ() * 100));
  }

  public void translateAllContents() {
    setControlValues();

    // options
    optionsBox.setBorder(
      BorderFactory.createTitledBorder(
        TranslationManager.translate("Surfaces")
      )
    );

    // edit
    editBox.setBorder(
      BorderFactory.createTitledBorder(
          TranslationManager.translate("Lights")
        )
    );
    enabled.setText(TranslationManager.translate("Enabled"));

    // intensity
    intensityPanel.setBorder(
      BorderFactory.createCompoundBorder(
        BorderFactory.createTitledBorder(
          TranslationManager.translate("Intensity")
        ),
        BorderFactory.createEmptyBorder(5,5,5,5)
      )
    );
    dimLabel.setText(TranslationManager.translate("Dim"));
    brightLabel.setText(TranslationManager.translate("Bright"));

    // position
    positionPanel.setBorder(
      BorderFactory.createCompoundBorder(
        BorderFactory.createTitledBorder(
          TranslationManager.translate("Position")
        ),
        BorderFactory.createEmptyBorder(5,5,5,5)
      )
    );
  }



// Method to respond to value changes on individual controls
// in the panel.
  public void valueChange(csw.uistuff.ValueChangeEvent ev) {
    Object source = ev.getValueChangeObject();
  }

}  // end of main class
