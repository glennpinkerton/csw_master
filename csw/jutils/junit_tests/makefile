.IGNORE:

include $(CSW_PARENT)/build/system.mk

JAVA_SRC=\
  TestJutils.java \
  Log4jTestWatcher.java \
  TestRunner.java
  
JAVA_CLASSES=\
  TestJutils.class \
  Log4jTestWatcher.class \
  TestRunner.class

PACKAGE_PATH=csw/jutils/junit_tests
JAR_FILE=junit_tests.jar

all: java_classes

java_classes: $(JAVA_CLASSES)

clean:
	$(RM) *.class *.jar *.log ff.*
