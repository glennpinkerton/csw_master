/*
 */
package csw.j3d.src;

import java.awt.BorderLayout;
import java.awt.Frame;
//import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JDialog;
import javax.swing.JTabbedPane;

import csw.jutils.src.View3DProperties;
import csw.jutils.src.TranslationManager;

/**
 * Dialog to change the 3-D view options.
 *
 * @author Glenn Pinkerton
 */
public class View3DPropertiesDialog extends JDialog {

  public static final long  serialVersionUID = 7002349;

  private View3DProperties view3DProperties;
  private View3DOptionsPanel panel;
  private J3DPanel     j3dpanel;

  public View3DPropertiesDialog(Frame owner,
                                J3DPanel  p3d,
                                View3DProperties vp) {
    super(owner, true);
    this.setTitle(TranslationManager.translate("3-D View Properties"));
    this.view3DProperties = vp;
    j3dpanel = p3d;
    buildDialog();
  }

  public void setViewProperties (View3DProperties vp)
  {
    this.view3DProperties = vp;
  }

  private void buildDialog() {
    panel = new View3DOptionsPanel();
    setContentPane(panel);
    setResizable(false);
    pack();
  }

  private class View3DOptionsPanel extends JPanel {

    private View3DLightingOptionsPanel lightingPanel;
    private View3DPropertiesPanel generalPanel;

    public static final long  serialVersionUID = 3492349;

    private View3DOptionsPanel() {
      super(new BorderLayout());
      buildPanel();
      standardizePanelWidths();
    }

    private void buildPanel() {
      JTabbedPane tabbedPane = new JTabbedPane();
      generalPanel = buildGeneralPanel();
      generalPanel.setBorder(BorderFactory.createEmptyBorder(3, 3, 3, 3));
      lightingPanel = buildLightingPanel();
      lightingPanel.setBorder(BorderFactory.createEmptyBorder(3, 3, 3, 3));
      tabbedPane.add(TranslationManager.translate("General"), generalPanel);
      tabbedPane.add(TranslationManager.translate("Lighting"), lightingPanel);
      this.add(tabbedPane, BorderLayout.CENTER);
      JPanel buttonPanel = buildButtonPanel();
      this.add(buttonPanel, BorderLayout.SOUTH);
    }

    private void standardizePanelWidths() {
      int width = Math.max(
        generalPanel.getMaximumWidth(),
        lightingPanel.getMaximumWidth()
      );
      generalPanel.setPanelWidth(width);
      lightingPanel.setPanelWidth(width);
    }

    private View3DLightingOptionsPanel buildLightingPanel() {
      return new View3DLightingOptionsPanel(view3DProperties);
    }

    private View3DPropertiesPanel buildGeneralPanel() {
      return new View3DPropertiesPanel(view3DProperties);
    }

    private JPanel buildButtonPanel() {
      JPanel buttonPanel = new JPanel();

      JButton okButton = new JButton (TranslationManager.translate("OK"));
      buttonPanel.add(okButton);

      JButton applyButton = new JButton (TranslationManager.translate("Apply"));
      buttonPanel.add(applyButton);

      JButton cancelButton = new JButton (TranslationManager.translate("Cancel"));
      buttonPanel.add(cancelButton);

      JButton resetButton = new JButton (TranslationManager.translate("Reset"));
      buttonPanel.add(resetButton);

      okButton.addActionListener(
        new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            apply();
            View3DPropertiesDialog.this.dispose();
          }
        }
      );

      applyButton.addActionListener(
        new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            apply();
          }
        }
      );

      cancelButton.addActionListener(
        new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            View3DPropertiesDialog.this.dispose();
          }
        }
      );

      resetButton.addActionListener(
        new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            reset();
          }
        }
      );

      return buttonPanel;
    }

    private void apply() {
      generalPanel.apply();
      lightingPanel.apply();
      j3dpanel.setIsPropertyApply (true);
      j3dpanel.redrawAllImmediately ();
      j3dpanel.setIsPropertyApply (false);
    }

    private void reset() {
      generalPanel.reset();
      lightingPanel.reset();
      j3dpanel.setIsPropertyApply (true);
      j3dpanel.redrawAllImmediately ();
      j3dpanel.setIsPropertyApply (false);
    }

  }

}
