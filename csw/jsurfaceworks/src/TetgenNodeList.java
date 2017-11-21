
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

public class TetgenNodeList {


  double[] x;
  double[] y;
  double[] z;
  int[]    marks;
  int      npts;

/*------------------------------------------------------------------------------------*/

/**
 Create an empty tetgen node list object.
*/
  public TetgenNodeList () {
  }

/*------------------------------------------------------------------------------------*/

/**
 * Read nodes from a .node file produced by tetgen and fill up
 * arrays with the x, y, z and various attributes.
 */
  public void loadAsciiFile(
    String fileName
  ) throws IOException {

    BufferedReader reader = new BufferedReader(new FileReader(fileName));

    String text;

    for (;;) {
      text = reader.readLine();
      if (text == null) {
        break;
      }
      text = text.trim ();
      if (text.length() < 1) {
        continue;
      }
      if (text.startsWith ("#")) {
        continue;
      }
      else {
        break;
      }
    }

    int    ntot, idum, nattrib, markflag;

    StringTokenizer tokenizer = new StringTokenizer(text);
    ntot = Integer.parseInt (tokenizer.nextToken());
    idum = Integer.parseInt (tokenizer.nextToken());
    nattrib = Integer.parseInt (tokenizer.nextToken());
    markflag = Integer.parseInt (tokenizer.nextToken());

    if (ntot < 4) {
        reader.close ();
        throw
        new IOException ("Corrupted tetgen node file.");
    }

    ArrayList<String> alist = new ArrayList<String> (ntot);

    int nlines = 0;
    for (;;) {
      text = reader.readLine();
      if (text == null) {
        break;
      }
      text = text.trim ();
      if (text.length() < 5) {
        continue;
      }
      if (text.startsWith ("#")) {
        continue;
      }
      alist.add (text);
      nlines++;
    }

    reader.close ();

    if (nlines != ntot) {
        throw
        new IOException ("Corrupted tetgen node file.");
    }

    if (nlines < 1) {
        throw
        new IOException ("Empty tetgen node file.");
    }

    x = new double[nlines];
    y = new double[nlines];
    z = new double[nlines];
    if (markflag == 1) {
      marks = new int[nlines];
    }
    else {
      marks = null;
    }
    npts = 0;

    double      xt, yt, zt;
    int         mt;

    for (int i=0; i<nlines; i++) {

      text = alist.get (i);

      tokenizer = new StringTokenizer(text);
      idum = Integer.parseInt (tokenizer.nextToken());
      xt = Double.parseDouble (tokenizer.nextToken());
      yt = Double.parseDouble (tokenizer.nextToken());
      zt = Double.parseDouble (tokenizer.nextToken());
      if (markflag == 1) {
        mt = Integer.parseInt (tokenizer.nextToken());
        marks[npts] = mt;
      }
      x[npts] = xt;
      y[npts] = yt;
      z[npts] = zt;
      npts++;

    }

    return;

  }

/*--------------------------------------------------------------------------------*/


}  // end of TetgenNodeList class
