.IGNORE:

include $(CSW_PARENT)/build/system.mk

JAVA_SRC=\
  TestMain.java\
  LgLoad.java\
  DataGen.java\
  SpatialObj.java\
  GLPDB.java

JAVA_CLASSES=\
  TestMain.class\
  LgLoad.class\
  DataGen.class\
  SpatialObj.class\
  GLPDB.class

PACKAGE_PATH=csw/mysql/jtest/src
JAR_FILE=mysql.jar

all: java_classes

java_classes: $(JAVA_CLASSES)

clean:
	$(RM)  *.class *.jar
	$(RM) *$(OBJ_SUFFIX) *.log
