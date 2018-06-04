package csw.mysql.jtest.src;

import java.sql.Connection;

// Notice, do not import com.mysql.jdbc.*
// or you will have problems!

public class STSMain {

    public static void main(String[] args) {

      Connection  conn = null;

      try {
        conn = STSDB.stsConnect ();
        STSDB  stsdb = new STSDB (conn);
        stsdb.testMainStuff ();
      }
      catch (Throwable ex) {
        System.out.println ("Exception from TestMain: " +
                             ex.getMessage());
      }

    }

}
