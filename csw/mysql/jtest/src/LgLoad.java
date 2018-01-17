package csw.mysql.jtest.src;

import java.sql.Connection;

public class LgLoad {

    public static void main(String[] args) {

      Connection  conn = null;
      int  fid = 1;

      try {
        conn = GLPDB.glpConnect ();
        GLPDB  glpdb = new GLPDB (conn);
        int alen = args.length;
        if (alen > 0) {
          fid = Integer.parseInt (args[0]);
        }
        glpdb.fillSpIndex (fid);
        conn.close();
        conn = null;
      }
      catch (Throwable ex) {
        System.out.println ("Exception from TestMain: " +
                             ex.getMessage());
      }
      finally {
        try {
          if (conn != null) conn.close();
        }
        catch (Throwable ex) {
        }
      }

    }

}
