package csw.mysql.jtest.src;

import java.sql.Connection;

public class LgLoad {

    public static void main(String[] args) {

      Connection  conn = null;

      try {
        conn = GLPDB.glpConnect ();
        GLPDB  glpdb = new GLPDB (conn);
        glpdb.fillSpIndex ();
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
