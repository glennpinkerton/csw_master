
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

public class DBPointList {

  public double[] x;
  public double[] y;
  public double[] z;
  public int      npts;

/**
 Create an empty point list object.
*/
  public DBPointList () {
  }

/*------------------------------------------------------------------------------------*/

/*
 * Read data from a debug trimesh file into the DBPointList object.
 * This is intended for debigging only, and it is therefore
 * set to package scope.  The unit test program can use it to
 * draw a trimesh (nodes, edges, triangles) to help debug.
 */
  public void loadAsciiFile(
    String fileName
  ) throws IOException {

    BufferedReader reader = new BufferedReader(new FileReader(fileName));

    String text;

    ArrayList<String> alist = new ArrayList<String> (1000);

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

    x = new double[nlines];
    y = new double[nlines];
    z = new double[nlines];
    npts = 0;

    double      xt, yt, zt;

    for (int i=0; i<nlines; i++) {

      text = alist.get (i);

      StringTokenizer tokenizer = new StringTokenizer(text);
      xt = Double.parseDouble (tokenizer.nextToken());
      yt = Double.parseDouble (tokenizer.nextToken());
      zt = Double.parseDouble (tokenizer.nextToken());
      x[npts] = xt;
      y[npts] = yt;
      z[npts] = zt;
      npts++;
    }

    return;

  }


}  // end of DBPointList class
