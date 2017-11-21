
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
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.image.BufferedImage;
import java.util.ArrayList;

import javax.swing.JPanel;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;

/**
 This class provides a swing component that can be drawn to as part
 of the graphic properties edit dialogs.  This does not use any
 {@link JDisplayList} methods.  It is designed for very simple
 drawings associated with previewing graphic attribute changes.
*/
public class PreviewPanel extends JPanel
{

	private static final long serialVersionUID = 1L;

    private ArrayList<NativePrim.Line>       lineList = 
		new ArrayList<NativePrim.Line> ();
    private ArrayList<NativePrim.Fill>       fillList = 
		new ArrayList<NativePrim.Fill> ();
    private ArrayList<NativePrim.Text>       textList = 
		new ArrayList<NativePrim.Text> ();
    private ArrayList<NativePrim.Arc>        arcList = 
		new ArrayList<NativePrim.Arc> ();
    private ArrayList<NativePrim.FilledArc>  filledArcList = 
		new ArrayList<NativePrim.FilledArc> ();

/*---------------------------------------------------------------------------*/

    public PreviewPanel () {
        BevelBorder b = new BevelBorder (BevelBorder.LOWERED);
        setBorder (b);
    }

    public PreviewPanel(Color bgColor, Border border) {
      setBackground(bgColor);
      setBorder(border);
    }
/*---------------------------------------------------------------------------*/

    public void clearData ()
    {
        lineList.clear ();
        fillList.clear ();
        textList.clear ();
        arcList.clear ();
        filledArcList.clear ();
    }

/*---------------------------------------------------------------------------*/

    public void addLine (NativePrim.Line line)
    {
        lineList.add (line);
    }

    public void addLineList (ArrayList<NativePrim.Line> list)
    {
        if (list == null) {
            return;
        }

        NativePrim.Line line;

        int size = list.size ();
        for (int i=0; i<size; i++) {
            line = list.get (i);
            lineList.add (line);
        }

    }


/*---------------------------------------------------------------------------*/

    public void addFill (NativePrim.Fill fill)
    {
        fillList.add (fill);
    }

    public void addFillList (ArrayList<NativePrim.Fill> list)
    {
        if (list == null) {
            return;
        }

        NativePrim.Fill fill;

        int size = list.size ();
        for (int i=0; i<size; i++) {
            fill = list.get (i);
            fillList.add (fill);
        }

    }


/*---------------------------------------------------------------------------*/

    public void addText (NativePrim.Text text)
    {
        textList.add (text);
    }

    public void addTextList (ArrayList<NativePrim.Text> list)
    {
        if (list == null) {
            return;
        }

        NativePrim.Text text;

        int size = list.size ();
        for (int i=0; i<size; i++) {
            text = list.get (i);
            textList.add (text);
        }

    }


/*---------------------------------------------------------------------------*/

    public void addArc (NativePrim.Arc arc)
    {
        arcList.add (arc);
    }

    public void addArcList (ArrayList<NativePrim.Arc> list)
    {
        if (list == null) {
            return;
        }

        NativePrim.Arc arc;

        int size = list.size ();
        for (int i=0; i<size; i++) {
            arc = list.get (i);
            arcList.add (arc);
        }

    }


/*---------------------------------------------------------------------------*/

    public void addFilledArc (NativePrim.FilledArc farc)
    {
        filledArcList.add (farc);
    }

    public void addFilledArcList (ArrayList<NativePrim.FilledArc> list)
    {
        if (list == null) {
            return;
        }

        NativePrim.FilledArc farc;

        int size = list.size ();
        for (int i=0; i<size; i++) {
            farc = list.get (i);
            filledArcList.add (farc);
        }

    }

/*----------------------------------------------------------------------------*/

    double xTrans = 0;
    double yTrans = 0;

/*----------------------------------------------------------------------------*/

    public void paintComponent (Graphics g)
    {
        Graphics2D g2d = (Graphics2D)g;

        calcTranslations ();

        localDrawFunc (g2d);

    }

/*----------------------------------------------------------------------------*/

    private boolean imageFlag = false;

    public BufferedImage paintBufferedImage ()
    {
        int width = getWidth();
        int height = getHeight();

        BufferedImage bi = new BufferedImage (
            width, height,
            BufferedImage.TYPE_INT_ARGB);

        Graphics2D g2d = bi.createGraphics ();

        calcTranslations ();

        imageFlag = true;
        localDrawFunc (g2d);
        imageFlag = false;

        return bi;

    }

/*--------------------------------------------------------------------------*/

    private int localDrawFunc (Graphics2D g2d) {

        int             status;
        Graphics2D      g2d_fg, g2d_bg;
        Insets          iset;
        int             width, height;

        if (imageFlag == false) {
            iset = getInsets ();
            width = getWidth() - iset.left - iset.right;
            height = getHeight() - iset.top - iset.bottom;
        }
        else {
            iset = new Insets (0, 0, 0, 0);
            width = getWidth();
            height = getHeight();
        }

    /*
     * Create graphic contexts for each image.
     */
        g2d_fg = g2d;
        g2d_bg = g2d;

        Color bgColor = this.getBackground();
        g2d_bg.setColor (bgColor);
        g2d_bg.fillRect (iset.left,
                         iset.top,
                         width,
                         height);

        g2d.translate (xTrans, yTrans);

    /*
     * Create a LocalDraw object.  This will draw the lists
     * of native primitives to the buffered image.
     */
        LocalDraw local_draw = new LocalDraw ();
        local_draw.setNativeArrays (
            lineList,
            fillList,
            textList,
            arcList,
            filledArcList,
            null,
            null);

        local_draw.setProperties (
            getWidth(),
            getHeight(),
            bgColor.getRed(),
            bgColor.getGreen(),
            bgColor.getBlue(),
            bgColor.getAlpha()
        );

     /*
      * Do the drawing via the local draw object.
      */
        status = local_draw.localDraw (g2d_bg, g2d_fg);

        g2d.translate (-xTrans, -yTrans);

        return status;

    }

/*--------------------------------------------------------------------------*/

    private void calcTranslations ()
    {
        int         size, i, j;
        float       x1, y1, x2, y2, xt, yt, rt;
        int         width, height;

        width = getWidth ();
        height = getHeight ();

        NativePrim.Line       line;
        NativePrim.Fill       fill;
        NativePrim.Text       text;
        NativePrim.Arc        arc;
        NativePrim.FilledArc  farc;

        x1 = 10000f;
        y1 = 10000f;
        x2 = -10000f;
        y2 = -10000f;

        if (lineList != null) {
            size = lineList.size();
            for (i=0; i<size; i++) {
                line = lineList.get(i);
                if (line == null) continue;
                for (j=0; j<line.npts*2; j+=2) {
                    xt = line.xy[j];
                    yt = line.xy[j+1];
                    if (xt < x1) x1 = xt;
                    if (yt < y1) y1 = yt;
                    if (xt > x2) x2 = xt;
                    if (yt > y2) y2 = yt;
                }
            }
        }

        if (fillList != null) {
            size = fillList.size();
            for (i=0; i<size; i++) {
                fill = fillList.get(i);
                if (fill == null) continue;
                for (j=0; j<fill.npts*2; j+=2) {
                    xt = fill.xy[j];
                    yt = fill.xy[j+1];
                    if (xt < x1) x1 = xt;
                    if (yt < y1) y1 = yt;
                    if (xt > x2) x2 = xt;
                    if (yt > y2) y2 = yt;
                }
            }
        }

        if (textList != null) {
            size = textList.size();
            for (i=0; i<size; i++) {
                text = textList.get(i);
                if (text == null) continue;
                xt = text.x;
                yt = text.y;
                if (xt < x1) x1 = xt;
                if (yt < y1) y1 = yt;
                if (xt > x2) x2 = xt;
                if (yt > y2) y2 = yt;
            }
        }

        if (arcList != null) {
            size = arcList.size();
            for (i=0; i<size; i++) {
                arc = arcList.get(i);
                if (arc == null) continue;
                rt = arc.r1;
                if (arc.r2 > rt) rt = arc.r2;
                xt = arc.x - rt;
                yt = arc.y - rt;
                if (xt < x1) x1 = xt;
                if (yt < y1) y1 = yt;
                xt = arc.x + rt;
                yt = arc.y + rt;
                if (xt > x2) x2 = xt;
                if (yt > y2) y2 = yt;
            }
        }

        if (filledArcList != null) {
            size = filledArcList.size();
            for (i=0; i<size; i++) {
                farc = filledArcList.get(i);
                if (farc == null) continue;
                rt = farc.r1;
                if (farc.r2 > rt) rt = farc.r2;
                xt = farc.x - rt;
                yt = farc.y - rt;
                if (xt < x1) x1 = xt;
                if (yt < y1) y1 = yt;
                xt = farc.x + rt;
                yt = farc.y + rt;
                if (xt > x2) x2 = xt;
                if (yt > y2) y2 = yt;
            }
        }

        if (x1 > x2  ||  y1 > y2) {
            xTrans = 0.0;
            yTrans = 0.0;
            return;
        }

        xt = (x1 + x2) / 2.0f;
        yt = (y1 + y2) / 2.0f;

        double dwidth = width / 2.0;
        double dheight = height / 2.0;

        xTrans = (dwidth - xt);
        yTrans = (dheight - yt);

        return;

    }


}  // end of PreviewPanel class
