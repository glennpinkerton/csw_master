
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.StringTokenizer;

public class DBLineList {


  public int[]    linePts;
  public double[] x;
  public double[] y;
  public double[] z;
  public int      numLines;
  public int      vflag = 0;

  private boolean flipFlag = false;
  private boolean flipFlag2 = false;

/**
 Create an empty line list object.
*/
  public DBLineList () {
  }

/*------------------------------------------------------------------------------------*/

  public void setYZFlip (boolean bval)
  {
    flipFlag = bval;
    if (bval) {
      flipFlag2 = false;
    }
  }

  public void setXZFlip (boolean bval)
  {
    flipFlag2 = bval;
    if (bval) {
      flipFlag = false;
    }
  }

/*
 * Read data from a debug trimesh file into the DBLineList object.
 * This is intended for debigging only, and it is therefore
 * set to package scope.  The unit test program can use it to
 * draw a trimesh (nodes, edges, triangles) to help debug.
 */
  public void loadAsciiFile(
    String fileName
  ) throws IOException {

    boolean vectorFlag = false;
    vflag = 0;

    FileReader fread = new FileReader (fileName);
    BufferedReader reader = new BufferedReader(fread);

    String text;

    ArrayList<String> alist = new ArrayList<String> (1000);

    int nlines = 0;
    for (;;) {
      text = reader.readLine();
      if (text == null) {
        break;
      }
      text = text.trim ();
      if (text.startsWith ("//")) {
        continue;
      }
      alist.add (text);
      nlines++;
    }

    reader.close ();

    if (nlines < 1) {
        throw
        new IOException ("Empty file");
    }

    text = alist.get (0);
    StringTokenizer tokenizer = new StringTokenizer(text);
    numLines = Integer.parseInt (tokenizer.nextToken());

    if (numLines < 0) {
        vectorFlag = true;
        vflag = 1;
        numLines = nlines - 1;
    }

    x = new double[nlines*2];
    y = new double[nlines*2];
    z = new double[nlines*2];
    linePts = new int[numLines];

    double      xt, yt, zt;

    int npts = 0;
    int n = 0;
    int i;
    int nl = 1;

    for (int ido=0; ido<numLines; ido++) {

      text = alist.get (nl);
      nl++;
      tokenizer = new StringTokenizer(text);

      if (vectorFlag) {
        xt = Double.parseDouble (tokenizer.nextToken());
        yt = Double.parseDouble (tokenizer.nextToken());
        x[n] = xt;
        y[n] = yt;
        z[n] = 1.e30;
        n++;
        xt = Double.parseDouble (tokenizer.nextToken());
        yt = Double.parseDouble (tokenizer.nextToken());
        x[n] = xt;
        y[n] = yt;
        z[n] = 1.e30;
        n++;
        linePts[ido] = 2;
      }

      else {
        npts = Integer.parseInt (tokenizer.nextToken());
        linePts[ido] = npts;

        for (i=0; i<npts; i++) {
          text = alist.get (nl);
          nl++;
          tokenizer = new StringTokenizer(text);
          xt = Double.parseDouble (tokenizer.nextToken());
          yt = Double.parseDouble (tokenizer.nextToken());
          zt = Double.parseDouble (tokenizer.nextToken());
          x[n] = xt;
          y[n] = yt;
          z[n] = zt;
          if (flipFlag) {
            xt = y[n];
            y[n] = z[n];
            z[n] = xt;
          }
          else if (flipFlag2) {
            xt = x[n];
            x[n] = z[n];
            z[n] = xt;
          }

          n++;
        }
      }

    }

    return;

  }


}  // end of DBLineList class
