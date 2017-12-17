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

public class GLPDB {

    String   g_dirname;
    double   g_xmin, g_ymin, g_xmax, g_ymax;
    int      g_ixmin, g_iymin, g_ixmax, g_iymax;
    Statement stmt = null;
    long     spi_min = 0, spi_max = 0;

// I have tried several things to get mysql to accept multiple
// rows (with spatial data in each row) in the same insert.  I
// have given it up as a lost cause.  I write a separate insert
// for each row in the objects load file.

    final int MAX_FOR_INSERT = 1;

    ResultSet  rs = null;

    public static Connection glpConnect () throws Throwable {

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
              "jdbc:mysql://localhost/glp?" +
              "user=glenn&password=Dog4-J3ssi3&useSSL=false");
      }
      catch (SQLException ex) {
        System.out.println ("Exception in getConnection stuff.");
        throw (ex);
      }

      return conn;
    }




    Connection  conn = null;

    GLPDB (Connection myconn) throws Throwable {
        if (myconn == null) {
            throw (new Exception ("Cannot use null connection."));
        }
        conn = myconn;
    }

//
// Fill the mysql spatial data values in the objects_geom table.
//
    public void fillSpIndex ()
    {

      String fname = "csw/mysql/jtest/src/ld2.dat";

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
              putIntoTable (alist);
              alist.clear ();
              n = 0;
            }
        }

    // Should see this after end of file
        catch (NoSuchElementException ex) {
          putIntoTable (alist);
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

    }



    private void putIntoTable (ArrayList<SpatialObj> alist)
    {

      if (alist.size() < 1) {
        return;
      }

      final String  s1 = "insert into objects_geom values ( \n";
      final String  b = " ";
      final String  l = "(";
      final String  r = ")";
      final String  c = ", ";
      final String  sst = "ST_PolygonFromText(";
      final String  ply = "'polygon((";
      final String  nl = "\n";

      int    ix1, iy1, ix2, iy2;
      long   id;
      SpatialObj  sob;

      String sq = new String (s1);
      String sl, sg, spt;
      
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
          sg = sst + spt + r + c + "\n";
        }
        else {
          sg = sst + spt + r + "\n";
        }
        sl = id + c + sg;
        sq = sq + sl;
      }

      sq = sq + r + ";";

    }



    public void testMainStuff ()
    {
      doStuff ();
    }


    private void doStuff ()
    {

      String    sq, bbs;

// get global stuff
        
      sq = "select * from global_stuff";
      do_query (sq);

      getGlobalRsData (rs);
      try {
        rs.close ();
        rs = null;
        stmt.close ();
        stmt = null;
      }
      catch (SQLException exstmt) {
      }

      double x1, y1, x2, y2;
      Scanner scan = new Scanner( System.in );

long  stime, etime;
double  dtime;
 
      while (true) 
      {
       
        System.out.println (); 
        System.out.print ("Enter bbox: ");

        try {
          if (scan.hasNextDouble() == false) break;
          x1 = scan.nextDouble();
          y1 = scan.nextDouble();
          x2 = scan.nextDouble();
          y2 = scan.nextDouble();
        }
        catch (Throwable ex) {
          break;
        }

stime = System.nanoTime ();

        bbs = createBboxString (x1, y1, x2, y2);

        sq = createSelectQuery (bbs);

        do_query (sq);
        getRsData (rs);
        try {
          rs.close ();
          rs = null;
          stmt.close ();
          stmt = null;
        }
        catch (SQLException exstmt) {
        }

etime = System.nanoTime();

dtime = ((double)etime - (double)stime) / 1000000000.0;
System.out.println ("elapsed time = " + dtime);
System.out.println ();

      }

      return;

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



    private void getGlobalRsData (ResultSet rs)
    {

      int        oid = 0;
      double     xmin, ymin, xmax, ymax;

      try {
        rs.first ();

    // retrieve the single line from the global stuff table
        
        g_dirname = rs.getString ("dirname");
        g_xmin = rs.getDouble ("xmin");
        g_ymin = rs.getDouble ("ymin");
        g_xmax = rs.getDouble ("xmax");
        g_ymax = rs.getDouble ("ymax");
        g_ixmin = rs.getInt ("ixmin");
        g_iymin = rs.getInt ("iymin");
        g_ixmax = rs.getInt ("ixmax");
        g_iymax = rs.getInt ("iymax");

      }
      catch (SQLException ex) { }

    }




    private void getRsData (ResultSet rs)
    {

      long       obid = 0;
      int        line_id, file_id, file_pos;

      try {
        rs.first ();

    // Do the first line before nexting the row cursor

        obid = rs.getLong ("object_id");
        line_id = rs.getInt ("line_id");
        file_id = rs.getInt ("file_id");
        file_pos = rs.getInt ("file_pos");
        processObj (obid, line_id, file_id, file_pos);

    // Do the rest of the result set.

        while (rs.next ()) {
          obid = rs.getLong ("object_id");
          line_id = rs.getInt ("line_id");
          file_id = rs.getInt ("file_id");
          file_pos = rs.getInt ("file_pos");
          processObj (obid, line_id, file_id, file_pos);
        }
      }
      catch (SQLException ex) { }

      try {
        int count = 0;
        rs.last();
        count = rs.getRow();
        rs.beforeFirst();
System.out.println ();
System.out.println ("Number of results = " + count);
System.out.println ();
      }
      catch (SQLException ex)
      {
      }

    }


    private void processObj (long obid,
                             int  line_id,
                             int  file_id,
                             int  file_pos)
    {
System.out.println ();
System.out.println ("object id: " + obid);
System.out.println ("  line_id = " + line_id);
System.out.println ("  file_id = " + file_id);
System.out.println ("  file_pos = " + file_pos);
System.out.println ();
    }




/**
 * Build a string of sql that will return objects inside the
 * specified bounding box.
 */
    private String  createBboxString (double    bxmin, 
                                      double    bymin,
                                      double    bxmax,
                                      double    bymax)
    {
      String  s;

      int  ix1 = (int)(bxmin * 1000000.0);
      int  iy1 = (int)(bymin * 1000000.0);
      int  ix2 = (int)(bxmax * 1000000.0);
      int  iy2 = (int)(bymax * 1000000.0);

      s = 
        "'polygon ((\n" +
           ix1 + " " + iy1 + " , \n" +
           ix1 + " " + iy2 + " , \n" +
           ix2 + " " + iy2 + " , \n" +
           ix2 + " " + iy1 + " , \n" +
           ix1 + " " + iy1 + " \n" + "))'";

      return s;   
    }

/**
 * Build a string of sql that will return objects inside the
 * specified bounding box.
 */
    private String  createSelectQuery (String bbs)
    {
      String  s;

      s = "select objects.object_id,pline_points_lookup.* from \n" +
          "objects_geom,objects,pline_id_lookup,pline_points_lookup \n" +
          " where \n" +
          "(MBRIntersects" + "\n" +
             "(ST_GeomFromText\n(" +
               bbs + "),\n objects_geom.bbox)) \n" +
          "  and \n" +
          "(objects.object_id = objects_geom.object_id) \n" +
          "  and \n" +
          "(pline_id_lookup.object_id = objects.object_id) \n" +
          "  and \n" +
          "(pline_points_lookup.line_id = pline_id_lookup.line_id);";

      return s;   
    }


} // end of GLPDB class
