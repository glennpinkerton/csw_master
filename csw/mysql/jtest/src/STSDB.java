package csw.mysql.jtest.src;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.ResultSet;

import java.io.*;

import java.util.NoSuchElementException;
import java.util.Scanner;
import java.util.ArrayList;



// The following comment is from a tutorial.
// I take it at face value and it seems to work.

// Notice, do not import com.mysql.jdbc.*
// or you will have problems!

public class STSDB {

    String   g_dirname = null;
    double   g_xmin, g_ymin, g_xmax, g_ymax, g_xscal, g_yscal;
    long     g_ixmin, g_iymin, g_ixmax, g_iymax;
    Statement stmt = null;

// I have tried several values for the max number of rows
// per insert.  I think mysql can do more than 10000, but
// this java code seems to slow to a crawl with much more
// than 10000.

    final int MAX_FOR_INSERT = 10000;

    ResultSet  rs = null;

    public static Connection stsConnect () throws Throwable {

      try {

          // The following comment was on a tutorial site.  I
          // take it at face value and it seems to work this way,
          //
          // The newInstance() call is a work around for some
          // broken Java implementations

          Class.forName("com.mysql.jdbc.Driver").newInstance();

      }
      catch (Exception ex) {
          System.out.println ("Exception in LoadDriver main Method");
          throw (ex);
      }

      Connection  conn = null;

      try {
        conn =
           DriverManager.getConnection(
              "jdbc:mysql://localhost/stsdb?" +
              "user=glenn&password=Dog4-J3ssi3");
              //"user=glenn&password=Dog4-J3ssi3&useSSL=false");
      }
      catch (SQLException ex) {
        System.out.println ("Exception in getConnection stuff.");
        throw (ex);
      }

      return conn;
    }




    Connection  conn = null;

    STSDB (Connection myconn) throws Throwable {
        if (myconn == null) {
            throw (new Exception ("Cannot use null connection."));
        }
        conn = myconn;
    }

//
// Fill the mysql spatial data values in the objects_geom table.
//
    public void fillSpIndex (int fid)
    {

      final String fdname = "/home/gpinkerton/data/load";

      //String fname = "csw/mysql/jtest/src/ld2.dat";
      String fname = fdname + "/load_objects_" + fid + ".dat";

      FileInputStream  fis = null;
      Scanner          fscan = null;

// create a scanner for the file

      try {
        fis = new FileInputStream (fname);
        fscan = new Scanner (fis);
      }
      catch (IOException ex) {
        System.out.println (ex.getMessage());
        return;
      }

      ArrayList<SpatialObj> alist = new ArrayList<SpatialObj> ();
      SpatialObj  sob = null;

      long   obid;
      int    itype;
      int    layer_id;
      double   xmin, ymin, xmax, ymax;

      FileWriter fw = null;

      try {
        fw = new FileWriter (fdname + "/jt_" + fid + ".sql");
      }
      catch (IOException ex) {
        System.out.println ("Cannot open jt.sql file");
        System.out.println (ex.getMessage());
        return;
      }

      int    n = 0;
      while (true) {
        try {
            obid = fscan.nextLong ();
            itype = fscan.nextInt ();
            layer_id = fscan.nextInt ();
            xmin = fscan.nextDouble ();          
            ymin = fscan.nextDouble ();          
            xmax = fscan.nextDouble ();          
            ymax = fscan.nextDouble ();          
            sob = new SpatialObj (obid, itype, layer_id,
                                  xmin, ymin, xmax, ymax);
            alist.add (sob);
            n++;
            if (n >= MAX_FOR_INSERT) {
              putIntoTable (alist, fw);
              alist.clear ();
              n = 0;
            }
        }

    // Should see this after end of file
        catch (NoSuchElementException ex) {
          putIntoTable (alist, fw);
          break;
        }
    // Any other exception is a problem
        catch (Throwable ex) {
          System.out.println ();
          System.out.println ("Problem exception scanning load input file.");
          System.out.println (ex.getMessage());
          System.out.println ();
          break;
        }
      }
      fscan.close();
      fscan = null;
      fis = null;

      try {
        fw.close ();
      }
      catch (IOException ex) {}

    }



    private void putIntoTable (ArrayList<SpatialObj> alist,
                               FileWriter fw)
    {

      if (alist.size() < 1) {
        return;
      }

      final String  s1 = "insert into objects_geom values \n";
      final String  b = " ";
      final String  r = ")";
      final String  l = "(";
      final String  c = ", ";
      final String  sst = "ST_PolygonFromText(";
      final String  ply = "'polygon((";
      final String  nl = "\n";

      int    ix1, iy1, ix2, iy2;
      long   id;
      SpatialObj  sob;

      String sq = new String (s1);
      String sln, sg, spt;
      
      int ndo = alist.size();

      for (int i=0; i<ndo; i++) {

        sob = alist.get (i);
        id = sob.object_id;
        ix1 = (int) (sob.xmin * 1000000.0);
        iy1 = (int) (sob.ymin * 1000000.0);
        ix2 = (int) (sob.xmax * 1000000.0);
        iy2 = (int) (sob.ymax * 1000000.0);

        spt = ply + ix1 + b + iy1 + c + nl +
                    ix1 + b + iy2 + c + nl +
                    ix2 + b + iy2 + c + nl +
                    ix2 + b + iy2 + c + nl +
                    ix1 + b + iy1 + b + r + r + "'" + nl;
        if (i < ndo - 1) {
          sg = sst + spt + r + r + c + "\n";
        }
        else {
          sg = sst + spt + r + r + "\n";
        }
        sln = l + id + c + sg;
        sq = sq + sln;

        if (i % 1000 == 0  &&  i > 0) {
          try {
            fw.write (sq);
            fw.flush ();
            sq = "";
          }
          catch (IOException ex) {
            System.out.println ("Error writing to jt.sql");
            System.out.println (ex.getMessage());
            break;
          }
        }
      }

      sq = sq + ";";

      try {
        fw.write (sq);
        fw.flush ();
      }
      catch (IOException ex) {
        System.out.println ("Error writing jt.sql");
        System.out.println (ex.getMessage());
      }

    }



    public void testMainStuff ()
    {
System.out.println ();
System.out.println ("STS connection ok, doint stuff here.");
System.out.println ();
      doStuff ();
    }


    private void doStuff ()
    {

      String    sq, bbs;

// get employee stuff
        
      sq = "select * from employees";
      do_query (sq);

      getEmpRsData (rs);
      try {
        rs.close ();
        rs = null;
        stmt.close ();
        stmt = null;
      }
      catch (SQLException exstmt) {
      }

    }



    private void do_query (String sq)
    {

        try {
            stmt = conn.createStatement ();
            rs = stmt.executeQuery (sq);
        }
        catch (SQLException ex) {
            System.out.println ();
            System.out.println ("SQLException: " + ex.getMessage());
            System.out.println ("SQLState: " + ex.getSQLState());
            System.out.println ("VendorError: " + ex.getErrorCode());
            System.out.println ();
        }
    }



    private void getEmpRsData (ResultSet rs)
    {

      try {
        rs.last();
        int nr = rs.getRow();
        System.out.println ();
        System.out.println ("     number of employees found = " + nr + "\n");
        System.out.println ();

        rs.beforeFirst ();
        while (rs.next ()) {
          String  fn = rs.getString ("firstName");
          String  ln = rs.getString ("lastName");
          System.out.println ("    First Name: " + fn);
          System.out.println ("    Last Name:  " + ln);
        }

      }
      catch (SQLException ex) { }

    }

} // end of STSDB class
