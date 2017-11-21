.IGNORE:

include $(CSW_PARENT)/build/system.mk

JAVA_SRC=\
 JEasyXTest.java\
 JSWUnitTest.java\
 SplitTest2D.java\
 TestFont.java
# SplitTest.java\
# SplitTestR.java

JAVA_CLASSES=\
 JEasyXTest.class\
 JSWUnitTest.class\
 SplitTest2D.class\
 TestFont.class
# SplitTest.class\
# SplitTestR.class\

PACKAGE_PATH=csw/jtest
JAR_FILE=jtest.jar

all: java_classes

java_classes: $(JAVA_CLASSES)

clean:
	$(RM)  *.class *.jar
	$(RM) ezx_test$(EXE_SUFFIX) sw_test$(EXE_SUFFIX) gendata$(EXE_SUFFIX)
	$(RM) $(LIB_PREFIX)*$(LIB_SUFFIX) 
	$(RM) *$(OBJ_SUFFIX) *.log utest.*
