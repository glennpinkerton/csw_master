.IGNORE:

include $(CSW_PARENT)/build/system.mk

JAVA_SRC=\
 FillImages.java

JAVA_CLASSES=\
 FillImages.class

PACKAGE_PATH=csw/images
JAR_FILE=images.jar

all: java_classes

java_classes: $(JAVA_CLASSES)

clean:
	$(RM) $(JAVA_CLASSES)
	$(RM) $(LIB_PREFIX)*$(LIB_SUFFIX) 
	$(RM) *$(OBJ_SUFFIX) 

