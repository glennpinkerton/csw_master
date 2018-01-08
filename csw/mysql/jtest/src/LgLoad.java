package csw.mysql.jtest.src;

import java.sql.Connection;
//import java.sql.DriverManager;
//import java.sql.SQLException;

// Notice, do not import com.mysql.jdbc.*
// or you will have problems!

public class LgLoad {

    public static void main(String[] args) {

      Connection  conn = null;

      try {
        conn = GLPDB.glpConnect ();
        GLPDB  glpdb = new GLPDB (conn);
        glpdb.fillSpIndex ();
      }
      catch (Throwable ex) {
        System.out.println ("Exception from TestMain: " +
                             ex.getMessage());
      }

    }

}
