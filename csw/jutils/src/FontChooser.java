
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;


import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GraphicsEnvironment;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * This class provides a panel for the user to choose a font from
 * the available fonts on the system.  The user can also specify size,
 * bold and italic for the font via the panel.  An instance of this class can be
 * put into another dialog, or the application can use the {@link #createDialog}
 * static method to present the font chooser panel to the user.  The
 * {@link #chooseFont} static method allows a still simpler way to
 * get a font by displaying a modal default dialog and returning the
 * chosen font.
 <p>
 * This class maintains a list of most recently chosen fonts.  This list
 * integrates the fonts chosen from all instances of the font chooser panel.
 * You can get this list via the static {@link #getRecentFontNames} method.
 * You can set the list, for example if it has been loaded from a project
 * file, via the {@link #setRecentFontNames} method.
 *
 * @author Glenn Pinkerton
 */
public class FontChooser extends JPanel {

	private static final long serialVersionUID = 1L;

/*-------------------------------------------------------------------------------

  This section has static data and methods.

---------------------------------------------------------------------------------*/

/**
 The size units are points, or about 1/100 of an inch.
*/
  public static final int POINTS = 0;
/**
 The size units are inches.
*/
  public static final int INCHES = 1;
/**
 The size units are millimeters.
*/
  public static final int MILLIMETERS = 2;

/**
 The default font name is "Lucida Sans Regular"
*/
  public static final String DEFAULT_FONT_NAME = "Lucida Sans Regular";

  private static ArrayList<String> systemFontNames;
  private static int dpi = 96;

  private static boolean setupDone = false;

  private static ArrayList<String> recentFontNames = 
	  new ArrayList<String> (10);

  private static double[] unitsConvert = new double[3];

  private static double lastSize = .1;
  private static int lastUnits = POINTS;
  private static boolean lastBold = false;
  private static boolean lastItalic = false;

/*-------------------------------------------------------------------------------*/

/**
 Return the last size displayed by the last instance of the font chooser.
 This is intended for storage in a project file so the font chooser can
 be initialized as it was when the program exited.
 */
  public static double getLastSize ()
  {
    return lastSize;
  }

/**
 Return the last units displayed by the last instance of the font chooser.
 This is intended for storage in a project file so the font chooser can
 be initialized as it was when the program exited.
 */
  public static int getLastUnits ()
  {
    return lastUnits;
  }

/**
 Return the last bold flag displayed by the last instance of the font chooser.
 This is intended for storage in a project file so the font chooser can
 be initialized as it was when the program exited.
 */
  public static boolean getLastBold ()
  {
    return lastBold;
  }

/**
 Return the last italic flag displayed by the last instance of the font chooser.
 This is intended for storage in a project file so the font chooser can
 be initialized as it was when the program exited.
 */
  public static boolean getLastItalic ()
  {
    return lastItalic;
  }

/**
 Set the size to a value that was previously gotten via getLastSize.  This is intended
 for restoring the chooser state from a file of some sort.
 */
  public static void setLastSize (double size)
  {
    if (size <= 0.01) {
      size = .1;
    }
    lastSize = size;
  }

/**
 Set the units to a value that was previously gotten via getLastUnits.  This is intended
 for restoring the chooser state from a file of some sort.
 */
  public static void setLastUnits (int units)
  {
    if (units < 0  ||  units > 2) {
      units = 0;
    }
    lastUnits = units;
  }

/**
 Set the bold flag to a value that was previously gotten via getLastBold.  This is intended
 for restoring the chooser state from a file of some sort.
 */
  public static void setLastBold (boolean flag)
  {
    lastBold = flag;
  }

/**
 Set the italic to a value that was previously gotten via getLastItalic.  This is intended
 for restoring the chooser state from a file of some sort.
 */
  public static void setLastItalic (boolean flag)
  {
    lastItalic = flag;
  }


/*-------------------------------------------------------------------------------*/

/*
  Static initializer block called when class is loaded.
*/
  static {
    setupSystemFontNames ();
    unitsConvert[0] = 100.0;
    unitsConvert[1] = 1.0;
    unitsConvert[2] = 25.4;
  }

/*-------------------------------------------------------------------------------*/

/**
 This method should be called only once, when this class is initially loaded.  It gets
 all the font family names from the system and puts them in a static list.
 Subsequent calls to this method are ignored.
*/
  private static void setupSystemFontNames ()
  {
    if (setupDone == true) {
      return;
    }

    String[] fnames;

    fnames =
      GraphicsEnvironment.
      getLocalGraphicsEnvironment().
      getAvailableFontFamilyNames ();

    int size = fnames.length;
    systemFontNames = new ArrayList<String> (size);

    for (int i=0; i<size; i++) {
      systemFontNames.add (fnames[i]);
    }

    dpi = Toolkit.getDefaultToolkit().getScreenResolution();
    unitsConvert[0] = dpi;

    setupDone = true;
  }

/*--------------------------------------------------------------------------*/

/**
 Call this to load recent font names that were saved in a project
 or elsewhere so they can be used for more than one program run.
 */
  public static void setRecentFontNames (String[] names)
  {
    recentFontNames.clear ();

    if (names == null) {
       return;
    }

    int size = names.length;
    if (size < 1) {
      return;
    }

    if (size > 12) size = 12;

    for (int i=0; i<size; i++) {
      if (names[i] != null) {
        recentFontNames.add (names[i]);
      }
    }

    return;

  }

/*-------------------------------------------------------------------------------*/

/**
 Get the list of recently chosen font names.  This list is modified whenever
 a font is chosen and applied.  Whenever a call to {@link #getChooserFont} or
 {@link #getFontName} is made, the recent font name list is updated.  This list
 reflects fonts chosen from any instances of the font chooser.  If you want to
 store the recent names in a file and use them in another program run, you can
 retrieve the names with this method.
@return An array of strings with the most recent font names.
*/
  public static String[] getRecentFontNames ()
  {
    int size = recentFontNames.size ();
    if (size < 1) {
      return null;
    }

    String[] sarray = new String[size];

    for (int i=0; i<size; i++) {
      sarray[i] = recentFontNames.get (i);
    }

    return sarray;
  }



/*--------------------------------------------------------------------------*/

/*
 * The modalAction and modalDialog members are used only
 * in conjunction with the chooseFont static method.
 */
  private static int modalAction = 0;
  private static JDialog modalDialog = null;

/**
 Display a modal dialog with a {@link FontChooser} panel inside of
 it.  When the Ok or Cancel is clicked on the modal dialog, the
 dialog disappears and this function returns.  If Ok is clicked, the
 font displayed on the panel when Ok was clicked is returned.  If
 Cancel is clicked, null is returned.
@param parent The owner of the dialog.  The dialog will also be centered
 on the specified parent.  If this is null, the dialog is centered on the
 screen.
@param title The title for the dialog.  If this is null, "Choose a Font" is
used as the title.
@param defaultFont The default font to initially show on the dialog.  If
this is null, a system default font is shown.
*/
  public static Font chooseFont (
    JFrame    parent,
    String    title,
    Font      defaultFont
  )
  {

  /*
   * Create a font chooser panel and
   * set its font to the specified default font.
   */
    FontChooser    fc = new FontChooser ();
    if (defaultFont != null) {
      fc.setChooserFont (defaultFont);
    }

  /*
   * If the ok button is clicked, set the action to 1 and dispose the dialog.
   */
    ActionListener okListener = new ActionListener () {
      public void actionPerformed(ActionEvent ae){
        FontChooser.modalAction = 1;
        FontChooser.modalDialog.dispose ();
      }
    };

  /*
   * If the cancel button is clicked, set the action to 1 and dispose the dialog.
   */
    ActionListener cancelListener = new ActionListener () {
      public void actionPerformed(ActionEvent ae){
        FontChooser.modalAction = 0;
        FontChooser.modalDialog.dispose ();
      }
    };

    JDialog dialog = createDialog (
      parent,
      title,
      true,
      fc,
      okListener,
      null,
      null,
      cancelListener
    );

    modalDialog = dialog;

    dialog.setLocationRelativeTo (parent);

  /*
   * This show method call will not return until ok or cancel
   * is clicked.
   */
    dialog.setVisible (true);

    modalDialog = null;

    if (modalAction == 0) {
      return null;
    }

    modalAction = 0;

    Font font = fc.getChooserFont ();

    return font;

  }

/*--------------------------------------------------------------------------*/

/**
 Create a dialog using the specified FontChooser object as its main panel.
 The dialog can have up to four buttons at the bottom, OK, Apply, Reset and
 Cancel.  The OK and Cancel buttons will always appear.  The Apply and Reset
 buttons will only appear if they have non null listeners.
<p>
 The various listeners should call appropriate methods on the specified
 {@link FontChooser} object to get the chosen font information.
@param parent The parent frame for the dialog.
@param title The title for the dialog.  If this is null, "Choose a Font" is used.
@param modal Set to True for a modal dialog or to false for a modeless dialog.
@param chooserPanel The {@link FontChooser} object to place inside the dialog.
@param okListener The {@link ActionListener} to invoke when the OK button is
selected.
@param applyListener The {@link ActionListener} to invoke when the Apply button is
selected.  If this is null, no Apply button will appear on the dialog.
@param resetListener The {@link ActionListener} to invoke when the OK button is
selected.  If this is null, no Reset button will appear on the dialog.
@param cancelListener The {@link ActionListener} to invoke when the OK button is
selected.
*/
  public static JDialog createDialog (
    JFrame         parent,
    String         title,
    boolean        modal,
    FontChooser    chooserPanel,
    ActionListener okListener,
    ActionListener applyListener,
    ActionListener resetListener,
    ActionListener cancelListener
  )
  {

  /*
   * Create a dialog that cannot be resized.
   */
    JDialog        dialog = new JDialog (parent, title, modal);
    dialog.setResizable (true);
    dialog.setSize (450, 270);

    if (title == null) {
      title = "Choose a Font";
    }
    dialog.setTitle (title);

  /*
   * The dialog's content pane will have two children, the
   * chooser panel in the CENTER of a border layout and the
   * panel with buttons in the SOUTH.
   */
    Container content = dialog.getContentPane ();
    content.setLayout (new BorderLayout ());

    content.add (chooserPanel, BorderLayout.CENTER);

  /*
   * Use a flow layout with custom horizontal gap for the
   * button panel.
   */
    FlowLayout flow = new FlowLayout ();
    int hgap = 20;
    if (applyListener == null  ||  modal == true) {
      hgap += 10;
    }
    if (resetListener == null) {
      hgap += 10;
    }
    flow.setHgap (hgap);

    JPanel buttonPanel = new JPanel (true);
    buttonPanel.setLayout (flow);
    content.add (buttonPanel, BorderLayout.SOUTH);

  /*
   * Create the needed buttons.
   */
    JButton  okButton = null;
    JButton  applyButton = null;
    JButton  resetButton = null;
    JButton  cancelButton = null;

    okButton = new JButton (
      ("Ok")
    );
    applyButton = new JButton (
      ("Apply")
    );
    resetButton = new JButton (
      ("Reset")
    );
    cancelButton = new JButton (
      ("Cancel")
    );

  /*
   * Add the buttons to the button panel
   * from left to right using the flow layout.
   */
    buttonPanel.add (okButton);
    if (modal == false  &&  applyListener != null) {
      buttonPanel.add (applyButton);
    }
    if (resetListener != null) {
      buttonPanel.add (resetButton);
    }
    buttonPanel.add (cancelButton);

  /*
   * Use the input action listeners as listeners for the
   * buttons.  If an input listener is null, do not use it.
   */
    if (okListener != null) {
      okButton.addActionListener (okListener);
    }
    if (applyListener != null  &&  modal == false) {
      applyButton.addActionListener (applyListener);
    }
    if (resetListener != null) {
      resetButton.addActionListener (resetListener);
    }
    if (cancelListener != null) {
      cancelButton.addActionListener (cancelListener);
    }

    return dialog;

  }

/*--------------------------------------------------------------------------*/

/*
 * Instance variables and instance methods take up the rest of
 * the class definition
 */
  private Font        defaultFont;
  private int         defaultSizeUnits;
  private double      defaultFontSize;
  private boolean     defaultBold,
                      defaultItalic;
  private String      defaultFontName;

  private Font        currentFont;
  private int         currentSizeUnits;
  private double      currentFontSize;
  private boolean     currentBold,
                      currentItalic;
  private String      currentFontName = DEFAULT_FONT_NAME;

  private boolean     updateFlag = false;

/*-------------------------------------------------------------------------------*/

  /**
   * Create a default font chooser panel.  The default font,
   * Lucida Sans Regular, is displayed as the font name.
   * The default style is PLAIN and the default size is
   * 10 points.
   */
  public FontChooser () {
    createFields ();
    init ();
  }

/*-------------------------------------------------------------------------------*/

/*
 * This method sets the defaults to the hard coded defaults and
 * also sets the current variables to these values.
 */
  private void init() {
    defaultFont = new Font(DEFAULT_FONT_NAME, Font.PLAIN, 12);
    defaultSizeUnits = lastUnits;
    defaultFontSize = lastSize;
    defaultBold = lastItalic;
    defaultItalic = lastBold;
    defaultFontName = DEFAULT_FONT_NAME;

    setCurrentToDefaults ();

  }

/*-------------------------------------------------------------------------------*/

/*
 * Reset the current variables to the default values.
 * The default values can be either hard coded defaults
 * assigned at construction time or may be set by the
 * application via the setDefault... methods.
 */
  private void setCurrentToDefaults ()
  {
    currentFont = defaultFont;
    currentSizeUnits = defaultSizeUnits;
    currentFontSize = defaultFontSize;
    currentBold = defaultBold;
    currentItalic = defaultItalic;
    currentFontName = defaultFontName;
    updateNameList ();
    updateFields ();
  }

/*-------------------------------------------------------------------------------*/

/**
 Set the font name to display in the font name field.  A null name
 is ignored.
*/
  public void setFontName (String name)
  {
    if (name == null) {
      return;
    }
    defaultFontName = name;
    currentFontName = defaultFontName;
    updateNameList ();
    updateFields ();
  }

/*-------------------------------------------------------------------------------*/

/**
 Get the current font name, as displayed in the font name field.
*/
  public String getFontName ()
  {
    lastUnits = currentSizeUnits;
    lastSize = currentFontSize;
    lastItalic = currentItalic;
    lastBold = currentBold;
    addToRecent (currentFontName);
    return currentFontName;
  }

/*-------------------------------------------------------------------------------*/

/**
 Set the size units flag to either {@link #POINTS}, {@link #INCHES} or
 {@link #MILLIMETERS}.  If a value other than these is
 specified, INCHES will be used.  The displayed value in the font
 size field will use these values.  However, the reported font size
 from {@link #getFontSize} will always be in inches.
*/
  public void setSizeUnits (int units)
  {
    defaultSizeUnits = INCHES;
    if (units == MILLIMETERS) {
      defaultSizeUnits = units;
    }
    currentSizeUnits = defaultSizeUnits;
    updateFields ();
  }

/*-------------------------------------------------------------------------------*/

/**
 Get the current font size units, which will be {@link #POINTS}, {@link #INCHES}
 or {@link #MILLIMETERS}.
*/
  public int getSizeUnits ()
  {
    return currentSizeUnits;
  }

/*-------------------------------------------------------------------------------*/

/**
 Set the font size in inches.  It will be displayed in the
 units specified by the {@link #setSizeUnits} method.
 If the size is less than or equal to zero, it is ignored.
*/
  public void setFontSize (double size)
  {
    if (size <= 0.0) {
      return;
    }

    defaultFontSize = size;
    currentFontSize = size;
    updateFields ();
  }

/*-------------------------------------------------------------------------------*/

/**
 Set the font size in the units specified by the units parameter.
 The units can be {@link #POINTS}, {@link #INCHES} or {@link #MILLIMETERS}.
 If the size is less than or equal to zero, it is ignored.
*/
  public void setFontSize (double size, int units)
  {
    if (size <= 0.0) {
      return;
    }

    if (units < 0  ||  units > 2) {
        units = 1;
    }

    size /= unitsConvert[units];
    defaultFontSize = size;
    currentFontSize = size;
    updateFields ();
  }

/*-------------------------------------------------------------------------------*/

/**
 Get the current font size, always in inches.
*/
  public double getFontSize ()
  {
    return currentFontSize;
  }

/*-------------------------------------------------------------------------------*/


/**
 Set the font style.  The style is either Font.PLAIN, Font.BOLD or Font.ITALIC,
 or a logical or of Font.BOLD|Font.ITALIC.  These constants are from the {@link Font}
 class.
*/
  public void setFontStyle (int style)
  {
    if (style == Font.PLAIN) {
      defaultBold = false;
      defaultItalic = false;
    }
    else {
      if ((style & Font.BOLD)  != 0) {
        defaultBold = true;
      }
      if ((style & Font.ITALIC)  != 0) {
        defaultItalic = true;
      }
    }

    currentBold = defaultBold;
    currentItalic = defaultItalic;

    updateFields ();

  }

/*-------------------------------------------------------------------------------*/

/**
 Return the current font style, which reflects the state of the bold and italic
 check boxes.
*/
  int getFontStyle ()
  {
    int        style;

    if (currentBold == false  &&  currentItalic == false) {
      style = Font.PLAIN;
    }
    else {
      style = 0;
      if (currentBold) {
        style |= Font.BOLD;
      }
      if (currentItalic) {
        style |= Font.ITALIC;
      }
    }

    return style;

  }

/*-------------------------------------------------------------------------------*/

/**
 Set the actual font for the panel.  The name, style and size are extracted
 from the font and used to populate the fields.
*/
  public void setChooserFont (Font font)
  {
    if (font == null) {
      return;
    }

    currentFont = font;
    defaultFont = font;

    String name = font.getFamily ();
    double size = (double)font.getSize() / dpi;
    int style = font.getStyle ();

    updateFlag = false;

    setFontName (name);
    setFontStyle (style);
    setFontSize (size);

    addToRecent (currentFontName);

    updateFlag = true;
    updateFields ();
    updateNameList ();

  }

/*-------------------------------------------------------------------------------*/

/**
 Get the current font, which reflects the current font name, style and size.
*/
  public Font getChooserFont ()
  {
    lastUnits = currentSizeUnits;
    lastSize = currentFontSize;
    lastItalic = currentItalic;
    lastBold = currentBold;
    addToRecent (currentFontName);
    return currentFont;
  }


/*-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------*/

  JLabel          nameLabel,
                  sizeLabel,
                  sampleLabel;
  JTextField      sizeText;
  JCheckBox       boldCheck,
                  italicCheck;
  JComboBox<String>       nameCombo,
                          unitsCombo;


/*-------------------------------------------------------------------------------*/

  private void createFields ()
  {

    setBorder (new EmptyBorder (15, 10, 10, 10));
    setLayout (new BorderLayout ());

    JPanel dataPanel = new JPanel ();

    nameLabel = new JLabel ();
    sizeLabel = new JLabel ();
    sampleLabel = new JLabel ();
    nameLabel.setBorder (new EmptyBorder (3, 2, 3, 3));
    sizeLabel.setBorder (new EmptyBorder (3, 2, 3, 3));
    sampleLabel.setBorder (new EmptyBorder (5, 5, 5, 0));

    sizeText = new JTextField (5);

    boldCheck = new JCheckBox ();
    italicCheck = new JCheckBox ();

    nameCombo = new JComboBox <String> ();
    nameCombo.setLightWeightPopupEnabled (false);
    nameCombo.setMaximumRowCount (12);

    updateNameList ();

    unitsCombo = new JComboBox <String> ();
    unitsCombo.setLightWeightPopupEnabled (false);
    unitsCombo.setMaximumRowCount (3);

    translateAll ();

  /*
   * Layout the the fields in each row using a GridBag.  The grid bag basically has
   * 2 rows and 4 columns.
   */
    GridBagLayout gridbag = new GridBagLayout ();
    dataPanel.setLayout (gridbag);
    GridBagConstraints c = new GridBagConstraints ();

    c.weightx = 0.1;
    c.weighty = 0.1;
    c.insets = new Insets (2, 0, 0, 0);

  /*
   * Layout the name label, name combo and bold check
   * in the first row.
   */
    c.gridy = 0;
    c.insets = new Insets (4, 0, 4, 0);

    c.gridx = 0;
    c.anchor = GridBagConstraints.EAST;
    gridbag.setConstraints (nameLabel, c);
    dataPanel.add (nameLabel);

    c.gridx = 1;
    c.anchor = GridBagConstraints.WEST;
    c.gridwidth = 2;
    gridbag.setConstraints (nameCombo, c);
    dataPanel.add (nameCombo);

    c.gridx = 3;
    c.gridwidth = GridBagConstraints.REMAINDER;
    gridbag.setConstraints (boldCheck, c);
    dataPanel.add (boldCheck);

  /*
   * Layout the size label, size text, units label and
   * italic check box in the second row.
   */
    c.gridy = 1;

    c.gridx = 0;
    c.anchor = GridBagConstraints.EAST;
    c.gridwidth = 1;
    gridbag.setConstraints (sizeLabel, c);
    dataPanel.add (sizeLabel);

    c.gridx = 1;
    c.anchor = GridBagConstraints.WEST;
    c.fill = GridBagConstraints.HORIZONTAL;
    gridbag.setConstraints (sizeText, c);
    dataPanel.add (sizeText);

    c.fill = GridBagConstraints.NONE;
    c.gridx = 2;
    gridbag.setConstraints (unitsCombo, c);
    dataPanel.add (unitsCombo);

    c.gridx = 3;
    gridbag.setConstraints (italicCheck, c);
    dataPanel.add (italicCheck);

    Border bline = BorderFactory.createLineBorder (Color.BLACK, 1);
    Border cborder = BorderFactory.createCompoundBorder (
        bline,
        new EmptyBorder (10, 10, 10, 10)
    );
    dataPanel.setBorder (cborder);

    add (BorderLayout.NORTH, dataPanel);

  /*
   * Put the sample label into a panel below the data panel.
   */
    JPanel sp = new JPanel ();
    GridBagLayout gridbag2 = new GridBagLayout ();
    sp.setLayout (gridbag2);
    sp.setSize (new Dimension (500, 100));
    sp.setBorder (new EmptyBorder (5, 0, 0, 0));

    c.gridx = 0;
    c.gridwidth = GridBagConstraints.REMAINDER;
    c.gridheight = 3;
    gridbag.setConstraints (sampleLabel, c);
    sp.add (sampleLabel);

    add (BorderLayout.CENTER, sp);

  /*
   * Setup the listeners.
   */
    nameCombo.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent ae){
        FontChooser.this.updateCurrentValues ();
      }
    });

    unitsCombo.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent ae){
        FontChooser.this.updateSizeUnits ();
        FontChooser.this.updateCurrentValues ();
      }
    });

    sizeText.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent ae){
        FontChooser.this.updateCurrentValues ();
      }
    });

    boldCheck.addChangeListener(new ChangeListener() {
      public void stateChanged(ChangeEvent ce){
        FontChooser.this.updateCurrentValues ();
      }
    });

    italicCheck.addChangeListener(new ChangeListener() {
      public void stateChanged(ChangeEvent ce){
        FontChooser.this.updateCurrentValues ();
      }
    });

    unitsCombo.setSelectedIndex (lastUnits);
    italicCheck.setSelected (lastItalic);
    boldCheck.setSelected (lastBold);

    updateFlag = true;

  }


/*-------------------------------------------------------------------------------*/

  private void translateAll ()
  {
    String     str;

    str = ("Font Name") + ":";
    nameLabel.setText (str);

    str = ("Font Size") + ":";
    sizeLabel.setText (str);

    str = ("Points");
    unitsCombo.addItem (str);

    str = ("Inches");
    unitsCombo.addItem (str);

    str = ("Millimeters");
    unitsCombo.addItem (str);

    str = ("Bold") + ":";
    boldCheck.setText (str);

    str = ("Italic") + ":";
    italicCheck.setText (str);

  }


/*-------------------------------------------------------------------------------*/

  private void updateUnitsLabel ()
  {
    unitsCombo.setSelectedIndex (currentSizeUnits);
  }

/*-------------------------------------------------------------------------------*/

  private boolean useLast = true;

  private void updateSizeUnits ()
  {
    int isize;

    if (useLast) {
      isize = lastUnits;
      currentFontSize = lastSize;
      useLast = false;
    }
    else {
      isize = unitsCombo.getSelectedIndex ();
      if (isize < 0  ||  isize > 2) {
        isize = 0;
      }
    }

    currentSizeUnits = isize;

    String str = null;

    double fsize = currentFontSize * unitsConvert[isize];

  /*
   * Format points as integer.
   */
    if (isize == 0) {
      str = String.valueOf ((int)(fsize + .5));
    }

  /*
   * Format inches with 2 decimal places.
   */
    else if (isize == 1) {
      int itmp = (int)(fsize * 100.0 + .5);
      double d = itmp / 100.0;
      str = String.valueOf ((float)d);
    }

  /*
   * Format millimeters with 1 decimal place;
   */
    else if (isize == 2) {
      int itmp = (int)(fsize * 10.0 + .5);
      double d = itmp / 10.0;
      str = String.valueOf ((float)d);
    }

    if (str != null) {
      sizeText.setText (str);
    }

    return;

}




/*-------------------------------------------------------------------------------*/

  private void updateSampleLabel ()
  {

    int style = 0;
    if (currentBold == false  &&  currentItalic == false) {
      style = Font.PLAIN;
    }
    else {
      if (currentBold) {
        style |= Font.BOLD;
      }
      if (currentItalic) {
        style |= Font.ITALIC;
      }
    }

    int size = (int)(currentFontSize * dpi + .5);

    Font font = new Font (currentFontName, style, size);

    sampleLabel.setFont (font);

    String str =
      ("Sample of ")
      +currentFontName;

    sampleLabel.setText (str);

    currentFont = font;

  }

/*-------------------------------------------------------------------------------*/

  private void updateCurrentValues ()
  {
    if (updateFlag == false) {
      return;
    }

    currentFontName = (String)nameCombo.getSelectedItem ();
    String str = sizeText.getText ();
    try
    {
      currentFontSize = Double.parseDouble (str);
    }
    catch (Exception e)
    {
      currentFontSize = .1 * unitsConvert[currentSizeUnits];
    }
    currentFontSize /= unitsConvert[currentSizeUnits];
    currentBold = boldCheck.isSelected ();
    currentItalic = italicCheck.isSelected ();

    updateSampleLabel ();

  }

/*-------------------------------------------------------------------------------*/

  private void updateFields ()
  {
    if (updateFlag == false) {
      return;
    }

    updateSizeUnits ();
    updateUnitsLabel ();

    boldCheck.setSelected (currentBold);
    italicCheck.setSelected (currentItalic);

    updateFlag = false;
    nameCombo.setSelectedItem (currentFontName);
    updateFlag = true;

    updateSampleLabel ();

    return;
  }

/*-------------------------------------------------------------------------------*/

  private void updateNameList ()
  {
    int            size, i;
    String         str;
    ArrayList<String>      alist = 
		new ArrayList<String> (100);

    nameCombo.removeAllItems ();

  /*
   * Put the recent names at the start of the list.
   */
    if (recentFontNames != null) {
      size = recentFontNames.size();
      for (i=0; i<size; i++) {
        str = recentFontNames.get (i);
        alist.add (str);
      }
    }

  /*
   * If there are no recent names, put the default font at the start.
   */
    if (alist.size() < 1) {
      alist.add (DEFAULT_FONT_NAME);
    }

  /*
   * Add system font names that are not already in the recent names.
   */
    if (systemFontNames != null) {
      size = systemFontNames.size ();
      for (i=0; i<size; i++) {
        str = systemFontNames.get (i);
        if (str == null) {
          continue;
        }
        if (isInRecent (str)) {
          continue;
        }
        alist.add (str);
      }
    }

  /*
   * Put the names in the combo box list.
   */
    size = alist.size();
    for (i=0; i<size; i++) {
      str = alist.get(i);
      if (str == null) {
        continue;
      }
      nameCombo.addItem (str);
    }

  /*
   * Make the first item the selected item.
   */
    nameCombo.setSelectedIndex (0);

    return;

  }

/*-------------------------------------------------------------------------------*/

  /*
   * Prepend the specified name to the recent list.  If the name
   * already exists in the recent list, delete it at its old position
   * and add it to the start of the list.
   */
  private void addToRecent (String name)
  {
    if (name == null) {
      return;
    }

    int index = getRecentIndex (name);

    if (index == 0) {
      return;
    }

    if (index > 0) {
      recentFontNames.remove (index);
    }

    recentFontNames.add (0, name);

    return;

  }



/*-------------------------------------------------------------------------------*/

  private boolean isInRecent (String str)
  {
    int              size, i;
    String           s2;

    if (str == null) {
      return false;
    }

    if (recentFontNames == null) {
      return false;
    }

    size = recentFontNames.size ();

    for (i=0; i<size; i++) {
      s2 = recentFontNames.get (i);
      if (s2 == str) {
        return true;
      }
      if (str.compareTo(s2) == 0) {
        return true;
      }
    }

    return false;

  }

/*-------------------------------------------------------------------------------*/

  private int getRecentIndex (String str)
  {
    int              size, i;
    String           s2;

    if (str == null) {
      return -1;
    }

    if (recentFontNames == null) {
      return -1;
    }

    size = recentFontNames.size ();

    for (i=0; i<size; i++) {
      s2 = recentFontNames.get (i);
      if (s2 == str) {
        return i;
      }
      if (str.compareTo(s2) == 0) {
        return i;
      }
    }

    return -1;

  }

}  // end of main class
