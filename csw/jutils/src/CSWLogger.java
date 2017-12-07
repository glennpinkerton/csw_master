package csw.jutils.src;
    
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;


// This interface gets a logger from the log4j2 LogManager.
// No attempt to make this "general" is made.  You can
// always use log4j2 directly if you want.  All this does
// is make a logger to the csw_run logger and return that
// logger when the getMyLogger method is called.    

public class CSWLogger {
    
    public static Logger getMyLogger () {
        Logger logger = LogManager.getLogger("csw_run");
        return logger;
    }
    
}
