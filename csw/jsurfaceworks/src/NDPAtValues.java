
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;

import java.io.FileWriter;
import java.io.IOException;

/**
  This class is meant for transferring NDP attribute data between
  different parts of the application.  

  @author Glenn Pinkerton
*/
public class NDPAtValues {

    public String              name;
    public byte[]              v1;
    public byte[]              v2;
    public byte[]              v3;
    public byte[]              v4;
    public byte[]              p1;
    public byte[]              p2;
    public byte[]              p3;
    public byte[]              p4;
    public int                 nValues;

  public void dumpToFile (String fileName)
  throws IOException
  {
    String         lineSep, line;
    FileWriter     fw;

    fw = new FileWriter (fileName);
    lineSep = System.getProperty ("line.separator");

    for (int i=0; i<nValues; i++) {
        line = v1[i]+" "+
               v2[i]+" "+
               v3[i]+" "+
               v4[i]+" "+
               p1[i]+" "+
               p2[i]+" "+
               p3[i]+" "+
               p4[i]+" "+
               lineSep;
        fw.write (line);
    }

    fw.close ();
  }

}
