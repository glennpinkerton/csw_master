
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jtest;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.WindowConstants;

import csw.jutils.src.FontChooser;

import csw.jeasyx.src.*;

/*
 * Test various java fonts and also the FontChooser stuff from Jutils.
 */
public class TestFont extends JFrame
{
	private static final long serialVersionUID = 1L;

    private String                 fileName;
    private JDisplayList           dl;

    JTextField       nameField, angleField;
    JButton          b1, b2;

    Font             chosenFont = null;

    public TestFont ()
    {

        setTitle ("Test Java Fonts");
        setSize (900, 700);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        Container contentPane = getContentPane ();

        b1 = new JButton ("Show Font");
        b1.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            TestFont.this.showFont ();
          }
        });

        b2 = new JButton ("Choose Font");
        b2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            TestFont.this.chooseFont ();
          }
        });

        nameField = new JTextField (20);
        nameField.setText ("Unknown");
        angleField = new JTextField (10);
        angleField.setText ("0.0");

        JPanel lpanel = new JPanel ();
        lpanel.setLayout (new GridLayout (1, 4));
        lpanel.add (b2);
        lpanel.add (nameField);
        lpanel.add (angleField);
        lpanel.add (b1);

        JPanel cpanel = new JPanel ();
        cpanel.setLayout (new BorderLayout ());
        cpanel.add (lpanel, BorderLayout.NORTH);

        JEasyXGraphicsPanel gpanel = new JEasyXGraphicsPanel ();
        cpanel.add (gpanel, BorderLayout.CENTER);

        contentPane.add (cpanel);

        dl = gpanel.getDisplayList ();

        initPlot();

    }

    private void chooseFont ()
    {
        Font font =
        FontChooser.chooseFont (
            this,
            null,
            null);

        if (font != null) {
            chosenFont = font;
            nameField.setText (font.getName());
        }
    }
        
        

    private void initPlot ()
    {

        dl.beginPlot ("debug surface splitting",
                      0.0, 0.0, 100.0, 100.0);

        dl.setFrameClip (1);

        dl.createFrame ("debug surface test frame",
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       1,
                       0,
                       0,
                       DLConst.FRAME_PLAIN_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);

        repaint ();

    }

    private void clearData ()
    {
        dl.clear ();
        initPlot ();
    }

    private void showFont ()
    {
        if (chosenFont == null) {
            return;
        }

        clearData ();

        String fname = chosenFont.getName ();
        String aname = angleField.getText ();
        int style = chosenFont.getStyle ();

        double angle = Double.parseDouble (aname);
         
        dl.setFont (fname, style);

        String text;

        dl.setTextAnchor (1);
        dl.addSymbol (10.0, 12.0, 2.0, 0.0, 7);
        text = "Sample of "+fname+" at 1.0 inches";
        dl.addText (10.0, 12.0, 1.0, angle, text);

        dl.addSymbol (10.0, 35.0, 1.4, 0.0, 7);
        text = "Sample of "+fname+" at 0.7 inches";
        dl.addText (10.0, 35.0, 0.7, angle, text);

        dl.addSymbol (10.0, 50.0, 1.0, 0.0, 7);
        text = "Sample of "+fname+" at 0.5 inches";
        dl.addText (10.0, 50.0, 0.5, angle, text);

        dl.setTextAnchor (9);
        dl.addSymbol (90.0, 65.0, 0.6, 0.0, 7);
        text = "Sample of "+fname+" at 0.3 inches 123";
        dl.addText (90.0, 65.0, 0.3, angle, text);

        dl.setTextAnchor (6);
        dl.addSymbol (90.0, 75.0, 0.4, 0.0, 7);
        text = "Sample of "+fname+" at 0.2 inches 123";
        dl.addText (90.0, 75.0, 0.2, angle, text);

        dl.setTextAnchor (3);
        dl.addSymbol (90.0, 83.0, 0.3, 0.0, 7);
        text = "Sample of "+fname+" at 0.15 inches";
        dl.addText (90.0, 83.0, 0.15, angle, text);

        dl.addSymbol (90.0, 90.0, 0.2, 0.0, 7);
        text = "Sample of "+fname+" at 0.1 inches";
        dl.addText (90.0, 90.0, 0.1, angle, text);

        repaint ();

    }

};
