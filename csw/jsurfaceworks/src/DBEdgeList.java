
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

public class DBEdgeList {


  int      numEdges;

  int[]    edgeNum;
  int[]    edgeFlag;

  double[] x1;
  double[] y1;
  double[] x2;
  double[] y2;

  double vx1;
  double vy1;
  double vx2;
  double vy2;

  double xmin;
  double ymin;
  double xmax;
  double ymax;

/**
 Create an empty line list object.
*/
  public DBEdgeList () {
  }

/*------------------------------------------------------------------------------------*/

/*
 * Read data from a debug edge file into the DBEdgeList object.
 * This is intended for debigging only, and it is therefore
 * set to package scope.  The unit test program can use it to
 * draw a trimesh (nodes, edges, triangles) to help debug.
 */
  public void loadAsciiFile(
    String fileName
  ) throws IOException {

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

    numEdges = nlines - 1;

    x1 = new double[nlines];
    y1 = new double[nlines];
    x2 = new double[nlines];
    y2 = new double[nlines];
    edgeNum = new int[nlines];
    edgeFlag = new int[nlines];

    double      xt1, yt1, xt2, yt2;

    int n = 0;
    int i;

    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;

    text = alist.get (0);
    StringTokenizer
    tokenizer = new StringTokenizer(text);
    vx1 = Double.parseDouble (tokenizer.nextToken());
    vy1 = Double.parseDouble (tokenizer.nextToken());
    vx2 = Double.parseDouble (tokenizer.nextToken());
    vy2 = Double.parseDouble (tokenizer.nextToken());

    if (vx1 < xmin) xmin = vx1;
    if (vx2 < xmin) xmin = vx2;
    if (vx1 > xmax) xmax = vx1;
    if (vx2 > xmax) xmax = vx2;
    if (vy1 < ymin) ymin = vy1;
    if (vy2 < ymin) ymin = vy2;
    if (vy1 > ymax) ymax = vy1;
    if (vy2 > ymax) ymax = vy2;

    for (i=1; i<nlines; i++) {

      text = alist.get (i);
      tokenizer = new StringTokenizer(text);
      xt1 = Double.parseDouble (tokenizer.nextToken());
      yt1 = Double.parseDouble (tokenizer.nextToken());
      xt2 = Double.parseDouble (tokenizer.nextToken());
      yt2 = Double.parseDouble (tokenizer.nextToken());

      if (xt1 < xmin) xmin = xt1;
      if (xt2 < xmin) xmin = xt2;
      if (xt1 > xmax) xmax = xt1;
      if (xt2 > xmax) xmax = xt2;
      if (yt1 < ymin) ymin = yt1;
      if (yt2 < ymin) ymin = yt2;
      if (yt1 > ymax) ymax = yt1;
      if (yt2 > ymax) ymax = yt2;

      x1[n] = xt1;
      y1[n] = yt1;
      x2[n] = xt2;
      y2[n] = yt2;
      edgeNum[n] = (int)(Double.parseDouble (tokenizer.nextToken()));
      edgeFlag[n] = (int)(Double.parseDouble (tokenizer.nextToken()));
      n++;

    }

    return;

  }

  /*
   * Accessor (get) functions
   */
  public double getXmin () 
  {
	  return xmin;
  }

  public double getYmin () 
  {
	  return ymin;
  }

  public double getXmax () 
  {
	  return xmax;
  }

  public double getYmax () 
  {
	  return ymax;
  }

  public double getVx1 ()
  {
	  return vx1;
  }

  public double getVy1 ()
  {
	  return vy1;
  }

  public double getVx2 ()
  {
	  return vx2;
  }

  public double getVy2 ()
  {
	  return vy2;
  }

  public double[] getX1 ()
  {
	  return x1;
  }

  public double[] getY1 ()
  {
	  return y1;
  }

  public double[] getX2 ()
  {
	  return x2;
  }

  public double[] getY2 ()
  {
	  return y2;
  }

  public int[] getEdgeNum ()
  {
	  return edgeNum;
  }

  public int[] getEdgeFlag ()
  {
	  return edgeFlag;
  }

  public int getNumEdges ()
  {
	  return numEdges;
  }

}  // end of DBEdgeList class
