.IGNORE:

include $(CSW_PARENT)/build/system.mk

JAVA_SRC=\
 JDisplayListBase.java\
 AxisProperties.java\
 CLine.java\
 DLArc.java\
 DLConst.java\
 DLContour.java\
 DLEditAdapter.java\
 DLEditListener.java\
 DLFill.java\
 DLLine.java\
 DLRectangle.java\
 DLRightClickAdapter.java\
 DLRightClickInfo.java\
 DLRightClickListener.java\
 DLSelectable.java\
 DLSelectAdapter.java\
 DLSelectionInfo.java\
 DLSelectListener.java\
 DLSurfaceProperties.java\
 DLSymb.java\
 DLText.java\
 DLZoomPanAdapter.java\
 DLZoomPanListener.java\
 FontUtils.java\
 JDisplayList.java\
 JDisplayListPanel.java\
 JDLEditListener.java\
 JLineEditor.java\
 LithFill.java\
 LocalDraw.java\
 NativePrim.java\
 JEasyXGraphicsPanel.java\
 SymbolButton.java\
 SymbolParts.java\
 PreviewPanel.java

JAVA_CLASSES=\
 JDisplayListBase.class\
 AxisProperties.class\
 CLine.class\
 DLArc.class\
 DLConst.class\
 DLContour.class\
 DLEditAdapter.class\
 DLEditListener.class\
 DLFill.class\
 DLLine.class\
 DLRectangle.class\
 DLRightClickAdapter.class\
 DLRightClickInfo.class\
 DLRightClickListener.class\
 DLSelectable.class\
 DLSelectAdapter.class\
 DLSelectionInfo.class\
 DLSelectListener.class\
 DLSurfaceProperties.class\
 DLSymb.class\
 DLText.class\
 DLZoomPanAdapter.class\
 DLZoomPanListener.class\
 FontUtils.class\
 JDisplayList.class\
 JDisplayListPanel.class\
 JDLEditListener.class\
 JLineEditor.class\
 LithFill.class\
 LocalDraw.class\
 NativePrim.class\
 JEasyXGraphicsPanel.class\
 SymbolButton.class\
 SymbolParts.class\
 PreviewPanel.class

JAR_FILE=jeasyx.jar
PACKAGE_PATH=csw/jeasyx/src

LIB_SRC_C=\
 shr_lib_link.c\
 DisplayListJNI.c

LIB_SRC_CC=\
 DisplayList.cc\
 DLContour.cc\
 DLSelectable.cc\
 DLSurf.cc\
 DrawPrim.cc\
 EZXCommand.cc\
 EZXJavaArea.cc

LIB_OBJS_C=\
 shr_lib_link$(OBJ_SUFFIX)\
 DisplayListJNI$(OBJ_SUFFIX)

LIB_OBJS_CC=\
 DisplayList$(OBJ_SUFFIX)\
 DLContour$(OBJ_SUFFIX)\
 DLSelectable$(OBJ_SUFFIX)\
 DLSurf$(OBJ_SUFFIX)\
 DrawPrim$(OBJ_SUFFIX)\
 EZXCommand$(OBJ_SUFFIX)\
 EZXJavaArea$(OBJ_SUFFIX)

LIB_FILE=$(LIB_PREFIX)jeasyx$(LIB_SUFFIX)

SHL_FILE=$(SHARED_LIB_PREFIX)_csw_all$(SHARED_LIB_SUFFIX)
SHL_BOOT=shr_lib_link$(OBJ_SUFFIX)

SHL_LIBS=\
 $(LIB_FILE)\
 $(CSW_LIBS)\
 $(OTHER_LIBS)

EXE_SRC_CC=\
 ezx_test.cc

EXE_OBJS=\
 ezx_test$(OBJ_SUFFIX)

EXE_FILE=\
 ezx_test$(EXE_SUFFIX)

EXE_LIBS=$(SHL_LIBS)

all: prog

java_classes: $(JAVA_CLASSES)

lib: $(ALL_LIB_OBJS) $(JAVA_CLASSES)
	$(LIB_CMD)
	$(LIB_COPY)

shared_lib: lib

executable: $(EXE_OBJS) $(EXE_OBJS2)
	$(LINK_CMD)
	$(CP) $(EXE_FILE) $(CSW_PARENT)/csw/jtest

prog: java_classes shared_lib $(EXE_OBJS) $(EXE_OBJS2)
	$(LINK_CMD)
	$(CP) $(EXE_FILE) $(CSW_PARENT)/csw/jtest

archive: lib

clean:
	$(RM) $(ALL_LIB_OBJS) $(LIB_FILE) $(SHL_FILE) $(JAVA_CLASSES) *.class
	$(RM) $(LIB_PREFIX)*$(LIB_SUFFIX) $(EXE_FILE)
	$(RM) *$(OBJ_SUFFIX) *.class *.jar

