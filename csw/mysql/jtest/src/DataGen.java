package csw.mysql.jtest.src;

import java.lang.Math;
import java.lang.Integer;
import java.lang.Long;

import java.util.Random;
import java.util.ArrayList;

import java.io.IOException;
import java.io.EOFException;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.RandomAccessFile;
import java.io.FileWriter;



class DataGen {

    public static void main(String[] args) {

      try {
        String sfn = "/home/gpinkerton/data/";
        if (args == null  ||  args.length == 0) {
            FakeDataGen  dg = new FakeDataGen (sfn, -1, -1, 1);
            dg.done();
        }
        else {
          int  alen = args.length;
          if (args[0].equals("load")) {
            int  fid = 1;
            long  obase = 0;
            if (alen >= 2) {
              fid = Integer.parseInt (args[1]);
            }
            if (alen >= 3) {
              obase = Long.parseLong (args[2]);
            }
            FakeDataLoad dl = new FakeDataLoad (sfn, fid, obase);
            dl.done();
          }
          else if (alen >= 2) {
            int n10 = Integer.parseInt (args[0]);
            if (n10 < 1) n10 = 1;
            int np = Integer.parseInt (args[1]);
            if (np < 5) np = 5;
            int fid = 1;
            if (alen == 3) {
              fid = Integer.parseInt (args[2]);
            }
            FakeDataGen  dg = new FakeDataGen (sfn, n10, np, fid);
            dg.done();
          }
        }
      }
      catch (IOException ex) {
      }
      catch (Throwable tex) {
      }

    }

}


class PointLookup {
    int    lineid;
    int    fileid;
    long   filepos;

    PointLookup (int lid, int fid, long fpos) {
      lineid = lid;
      fid = fileid;
      filepos = fpos;
    }

}


class TableData {
    long   file_pos;
    long   object_id;
    double xmin;
    double ymin;
    double xmax;
    double ymax;
    int    file_id;
    long   line_id;
}


//
// Create polylines with somewhat random shapes and sizes.
// These are written to a binary file which hopefully can
// be used later to load into and test the glp data base
// stuff.

class FakeDataGen {

    RandomAccessFile   genraf = null;
    String             dirname = null;

    Random             ran = null;

    DataInputStream   dis = null;
    FileInputStream   fis = null;

    RandomAccessFile  raf = null;

    int    n10 = 1000;
    int    n10loop = 0;
    int    np10 = 100;
    int    nskip = 1;

    ArrayList<PointLookup> plist = null;

    FakeDataGen (String  pdirname, int nobj,
                 int nps, int fid) throws IOException {

      if (nobj > 0) n10 = nobj;
      if (nps > 0) np10 = nps;

      dirname = pdirname;

      ran = new Random (123456 * fid);
      try {
        plist = new ArrayList<PointLookup> (n10);
      }
      catch (Throwable ex) {
        System.out.println 
          ("Could not create PointLookup list in FakeDataGen.");
      }

      nskip = n10 / 100;

//long start_nano = System.nanoTime ();

      String  fname = dirname + "glp_points_" + fid + ".dat";
      try {
        genraf = new RandomAccessFile (fname, "rw");
      }
      catch (IOException ex) {
        System.out.println ();
        System.out.println ("IO exception caught in DataGen constructor.");
        System.out.println ();
        throw (ex);
      }

      generateFakePointData ();

      try {
        genraf.close();
      }
      catch (IOException ex) {};

      genraf = null;


      try {
        raf = new RandomAccessFile (fname, "r");
      }
      catch (IOException ex) {
        System.out.println ();
        System.out.println 
          ("IO exception caught in Random Access File creation.");
        System.out.println ();
        throw (ex);
      }

      confirmFakePointDataRandomAccess ();

      try {
        raf.close ();
      }
      catch (IOException  ex) {
        System.out.println ();
        System.out.println ("IO exception caught in random access close.");
        System.out.println ();
        throw (ex);
      }
      finally {
        raf = null;
      }


    }  // end of method
    
    // This dummy method is here to avois an eclipse warning.
    // The warning is about an instance of FakeDataGen not being 
    // used.  So, I call this dummy routine to fool eclipse into
    // thinking the instance is actually used for something.
    protected void done () {}



    private void generateFakePointData ()
    {

        int iskip = 2;
        int range = 500000000;
        int nloop = 1;
        n10loop = 0;
        int i1 = 0;
        if (n10 < 100000) {
          i1 = 100000 / n10 + 2;
          i1 /= 2;
          if (i1 > 6) i1 = 6;
          iskip = i1 / 2 + 1;
          if (iskip < 2) iskip = 2;
        }
        while (n10loop < n10) {
          range = 500000000;
          nloop = 2;
          for (int i=0; i<20; i+=iskip) {
            if (i < i1) {
              range /= (iskip + 1);
              continue;
            }
            if (n10loop + nloop > n10) nloop = n10 - n10loop;
            if (nloop < 2) nloop = 2;
            generateFakePointData (range, nloop);
            n10loop += nloop;
            if (n10loop >= n10) break;
            range /= (iskip + 1);
            nloop *= 2;
          }
          i1+=2;
          if (i1 > 10) i1 = 10;
        }
    }



    private void generateFakePointData (int range, int nloop)
    {

      int     xanc, yanc, size, npts, nbytes;
      int     ix, iy, ix0 = 0, iy0 = 0;
      double  rad, r2, ang, dang, dx, dy;

      PointLookup   plook = null;

      long fpos;
      int  lid;
      int  fid = 1;
      int  ipoly = 0;
      boolean  bpoly;
      int  ip = 0;

      for (int i=0; i<nloop; i++) {

        lid = i;

        xanc = ran.nextInt (2000000000);
        yanc = ran.nextInt (1000000000);
        xanc -= 1000000000;
        xanc *= 2;
        yanc -= 500000000;
        yanc *= 2;
        size = ran.nextInt (range);
        size += range / 2;
        size /= 2;
        if (size < 10000) size = 10000;

        if (nloop == 1) {
            xanc = 0;
            yanc = 0;
            size = range;
        }

        ipoly = ran.nextInt (10);
        bpoly = false;
        ip = 0;
        if (ipoly < 4) {
          bpoly = true;
          ip = 1;
        }

    // make sure the generated shape is all inside the "universe"
        int  size15 = (int)(size * 1.5);
        if (nloop == 1)  size15 = 0;
        if (xanc < -2000000000 + size15) xanc = -2000000000 + size15;
        if (yanc < -1000000000 + size15) yanc = -1000000000 + size15;
        if (xanc > 2000000000 - size15) xanc = 2000000000 - size15;
        if (yanc > 1000000000 - size15) yanc = 1000000000 - size15;

        npts = ran.nextInt (np10);
        npts += 5;
        if (bpoly) npts++;
        nbytes = npts * 8 + 12;
        rad = (double)size / 2.0;

        if (nloop == 1) {
          npts = np10;
        }

        try {
          fpos = genraf.getFilePointer();
          genraf.writeInt (nbytes);
          genraf.writeInt (1);
          genraf.writeInt (npts);
          dang = Math.PI / (double)(npts-ip);
          dang *= 2.0;
          for (int j=0; j<npts; j++) {
            r2 = rad * (ran.nextDouble() + .49);
            ang = dang * j;
            dx = r2 * Math.cos (ang);
            dy = r2 * Math.sin (ang);
            ix = (int)dx + xanc;
            iy = (int)dy + yanc;
            if (bpoly) {
              if (j == 0) {
                ix0 = ix;
                iy0 = iy;
              }
              else if (j == npts - 1) {
                ix = ix0;
                iy = iy0;
              }
            }
            genraf.writeInt (ix);
            genraf.writeInt (iy);
          }
        }
        catch (IOException ex) {
          System.out.println ("exception writing data.");
          break;
        }
        try {
          plook = new PointLookup (lid, fid, fpos);
          plist.add (plook);
        }
        catch (Throwable ex) {
          System.out.println ("Exception creating or adding plook:  " +
                              ex.getMessage());
          break;
        }
      }

      return;

    }



    private void confirmFakePointDataRandomAccess ()
    {

      boolean  sout = false;

      int  nfound = 0;
      int  i = 0;
      
      int  idum, npts, nbytes, ix, iy;
      
      for (int ido=0; ido<100; ido++) {

        i = ran.nextInt (n10loop);

        if (sout == false) continue;

        PointLookup pl = plist.get (i);
        long lfp = pl.filepos;

        try {
          raf.seek (lfp);
          nbytes = raf.readInt ();
          idum = raf.readInt ();
          npts = raf.readInt ();
          for (int j=0; j<npts; j++) {
            ix = raf.readInt ();
            iy = raf.readInt ();
          }
          nfound++;
        }
        catch (IOException ex) {
          System.out.println ("exception reading data for confirmation.");
          System.out.println (ex.getMessage());
          return;
        }
      }

    }

}  // end of FakeDataGen class




// Read binary point file created by a previous run of DataGen
// and create tab separated files that can be loaded into the
// glp database.

class FakeDataLoad {

    String             dirname = null;
    RandomAccessFile   raf = null;

    long               ob_id_base = 0;

    ArrayList<TableData> tlist = null;

    FakeDataLoad (String pdirname, int fid, long obase)
      throws IOException
    {

      ob_id_base = obase;
      dirname = pdirname;
      String  fname = dirname + "glp_points_" + fid + ".dat";

      try {
        raf = new RandomAccessFile (fname, "r");
      }
      catch (IOException ex) {
        System.out.println ();
        System.out.println
          ("IO exception caught in Random Access File creation.");
        System.out.println ();
        throw (ex);
      }

      try {
        tlist = new ArrayList<TableData> (1000);
      }
      catch (Throwable ex) {
        System.out.println 
          ("Could not create TableData list in FakeDataLoad.");
      }

      loadFakePointData (fid);

      try {
        raf.close ();
      }
      catch (IOException  ex) {
        System.out.println ();
        System.out.println ("IO exception caught in DataGen load close.");
        System.out.println ();
        throw (ex);
      }
      finally {
        raf = null;
      }

      OutputLoadFiles (fid);

    }  // end of FakeDataLoad constructor
    


    // This dummy method is here to avoid an eclipse warning.
    // The warning is about an instance of FakeDataLoad not being 
    // used.  So, I call this dummy routine to fool eclipse into
    // thinking the instance is actually used for something.
    protected void done () {}




    private void loadFakePointData (int fid)
    {
      int     npts, ix, iy;
      long    ixmin, iymin, ixmax, iymax;
      long    fpos;

      TableData  trow;

      double  dxmin, dymin, dxmax, dymax;
      double  dxscl, dyscl;

      dxscl = 180.0 / 2000000000.0;
      dyscl = 90.0 / 1000000000.0;

      try {
        raf.seek (0);
      }
      catch (IOException ex) {
        System.out.println (ex.getMessage());
        return;
      }

      long nline = 0;
      int  nbytes = 0;
      int  idum = 0;

int  ncount = 0;

      while (true) 
      {
        try {
          fpos = raf.getFilePointer ();
          nbytes = raf.readInt ();
          idum = raf.readInt ();
          npts = raf.readInt ();
          ixmin = 2000000000;
          iymin = 1000000000;
          ixmax = -2000000000;
          iymax = -1000000000;
          for (int j=0; j<npts; j++) {
            ix = raf.readInt ();
            iy = raf.readInt ();
            if ((long)ix < ixmin) ixmin = (long)ix;
            if ((long)iy < iymin) iymin = (long)iy;
            if ((long)ix > ixmax) ixmax = (long)ix;
            if ((long)iy > iymax) iymax = (long)iy;
          }
          try {
            trow = new TableData ();
            trow.file_pos = fpos;
            trow.file_id = fid;

            dxmin = ((double)ixmin + 2000000000.) * dxscl - 180.0;
            dxmax = ((double)ixmax + 2000000000.) * dxscl - 180.0;
            dymin = ((double)iymin + 1000000000.) * dyscl - 90.0;
            dymax = ((double)iymax + 1000000000.) * dyscl - 90.0;

            trow.xmin = dxmin;
            trow.ymin = dymin;
            trow.xmax = dxmax;
            trow.ymax = dymax;
            trow.line_id = nline + ob_id_base;
            trow.object_id = nline + ob_id_base;

            tlist.add (trow);
            nline++;
          }
          catch (Throwable ex) {
          }
        }
        catch (EOFException ex) {
          break;
        }
        catch (IOException ex) {
          System.out.println ("exception reading data for load.");
          System.out.println (ex.getMessage());
        }
      }

      return;

    }


    // Traverse the tlist collection and write load files.

    private void OutputLoadFiles (int fid)
    {

      final String t = "\t";
      final String t1 = "1";
      final String l = "1";
      final String lf = "\n";
      final String sf = "%.7f";

      TableData td;
      String s1, s2, s3;
      
      FileWriter fw1 = null;
      FileWriter fw2 = null;
      FileWriter fw3 = null;

      String  dirn = "/home/gpinkerton/data/load/";

      try {
        fw1 = new FileWriter (dirn + "load_points_lookup_" + fid + ".dat");
        fw2 = new FileWriter (dirn + "load_id_lookup_" + fid + ".dat");
        fw3 = new FileWriter (dirn + "load_objects_" + fid + ".dat");
      }
      catch (IOException ex) {
        System.out.println ("Exception creating load file writers.");
        System.out.println (ex.getMessage());
        try {
          if (fw1 != null) fw1.close();
          if (fw2 != null) fw2.close();
        }
        catch (IOException ex2) {};
        return;
      }

      int   ndo = tlist.size();

      for (int i=0; i<ndo; i++) {
        td = tlist.get (i);
        s1 = td.line_id + t + td.file_id + t + td.file_pos + lf;
        s2 = td.object_id + t + td.line_id + lf;
        s3 = td.object_id + t +
             t1 + t +
             l + t +
             String.format(sf,td.xmin) + t +
             String.format(sf,td.ymin) + t +
             String.format(sf,td.xmax) + t +
             String.format(sf,td.ymax) + lf;
        try {
            if (fw1 != null) fw1.write (s1); 
            if (fw2 != null) fw2.write (s2); 
            if (fw3 != null) fw3.write (s3); 
        }
        catch (IOException ex) {
          System.out.println ("Exception writing to load files.");
          System.out.println (ex.getMessage());
          try {
            fw1.close();
            fw2.close();
            fw3.close();
          }
          catch (IOException ex2) {};
          return;
        }
      }

      try {
        fw1.close();
        fw2.close();
        fw3.close();
      }
      catch (IOException ex) {
      }

      return;

    }

}  // end of FakeDataLoad class
