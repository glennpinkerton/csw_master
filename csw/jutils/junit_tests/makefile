.IGNORE:

include $(CSW_PARENT)/build/system.mk

JAVA_SRC=\
  TestBounds2D.java \
  TestBounds3D.java \
  TestXYZPolyline.java \
  TestRunner.java
  
JAVA_CLASSES=\
  TestBounds2D.class \
  TestBounds3D.class \
  TestXYZPolyline.class \
  TestRunner.class

PACKAGE_PATH=csw/jutils/junit_tests
JAR_FILE=junit_tests.jar


# In compiling the java classes here, A warning:
#
#  warning: No processor claimed any of these annotations: org.junit.Test,org.junit.Rule
#
# is generated.  This (according to some google searching) is caused by implicit
# compilation of files in jar archives.  This is all part of the junit layers and
# I choose to do nothing about it.  Various attempts to suppress the warning do
# not work as advertised, so I am living with it.  The code seems to work as I
# expect when it is run.


all: java_classes

java_classes: $(JAVA_CLASSES)

clean:
	$(RM) *.class *.jar *.log ff.*
