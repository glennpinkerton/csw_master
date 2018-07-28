.IGNORE:

include $(CSW_PARENT)/build/system.mk

JAVA_SRC=\
 JPolyBool.java\
 JEasyXTest.java\
 JSWUnitTest.java\
 J3DUnitTest.java\
 SplitTest2D.java\
 TestFont.java

JAVA_CLASSES=\
 JPolyBool.class\
 JEasyXTest.class\
 JSWUnitTest.class\
 J3DUnitTest.class\
 SplitTest2D.class\
 TestFont.class

PACKAGE_PATH=csw/jtest
JAR_FILE=jtest.jar

all: java_classes

java_classes: $(JAVA_CLASSES)

clean:
	$(RM)  *.class *.jar
	$(RM) ezx_test$(EXE_SUFFIX) sw_test$(EXE_SUFFIX) gendata$(EXE_SUFFIX)
	$(RM) $(LIB_PREFIX)*$(LIB_SUFFIX) 
	$(RM) *$(OBJ_SUFFIX) *.log
