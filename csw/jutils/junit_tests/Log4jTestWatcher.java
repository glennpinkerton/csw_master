	
package csw.jutils.junit_tests;
 
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.layout.PatternLayout;
import org.apache.logging.log4j.core.Appender;
import org.apache.logging.log4j.core.appender.FileAppender;
import org.apache.logging.log4j.core.LoggerContext;
import org.apache.logging.log4j.core.config.Configuration;

import org.junit.rules.TestWatcher;
import org.junit.runner.Description;


 
public class Log4jTestWatcher extends TestWatcher {
 
    private final Logger logger;
 
    public Log4jTestWatcher() {
        logger = LogManager.getLogger();
    }
 
    public Log4jTestWatcher(String loggerName) {
        logger = LogManager.getLogger(loggerName);
    }

    @Override
    protected void failed(Throwable e, Description description) {
        String s = new String
            (description.toString() + "\n  !!!!Test Failed!!!!");
        logger.error(s, e);
    }
 
    @Override
    protected void succeeded(Description description) {
        String s = new String
            (description.toString() + "\n  ****Test Succeeded****");
        logger.info(s);
    }
}
