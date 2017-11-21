
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

public class TetgenFaceList {


  int[] n1;
  int[] n2;
  int[] n3;
  int[] marks;
  int      npts;

/*------------------------------------------------------------------------------------*/

/**
 Create an empty tetgen node list object.
*/
  public TetgenFaceList () {
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

    int    ntot, markflag;

    StringTokenizer tokenizer = new StringTokenizer(text);
    ntot = Integer.parseInt (tokenizer.nextToken());
    markflag = Integer.parseInt (tokenizer.nextToken());

    if (ntot < 4) {
        reader.close ();
        throw
        new IOException ("Corrupted tetgen face file.");
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
        new IOException ("Corrupted tetgen face file.");
    }

    if (nlines < 1) {
        throw
        new IOException ("Empty tetgen face file.");
    }

    n1 = new int[nlines];
    n2 = new int[nlines];
    n3 = new int[nlines];
    if (markflag == 1) {
      marks = new int[nlines];
    }
    else {
      marks = null;
    }

    int   idum;

    for (int i=0; i<nlines; i++) {

      text = alist.get (i);

      tokenizer = new StringTokenizer(text);
      idum = Integer.parseInt (tokenizer.nextToken());
      n1[i] = Integer.parseInt (tokenizer.nextToken());
      n2[i] = Integer.parseInt (tokenizer.nextToken());
      n3[i] = Integer.parseInt (tokenizer.nextToken());
      if (markflag == 1) {
        marks[i] = Integer.parseInt (tokenizer.nextToken());
      }

    }

    return;

  }

/*--------------------------------------------------------------------------------*/


}  // end of TetgenFaceList class
