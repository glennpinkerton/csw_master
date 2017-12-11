.IGNORE:

include $(CSW_PARENT)/build/system.mk

JAVA_SRC=\
 AtColorLookup.java\
 AttributeDef.java\
 AttributeDefList.java\
 BoundingBoxDraw3D.java\
 J3DAtToolBar.java\
 J3DAttribute.java\
 J3DCallbackObject.java\
 J3DConst.java\
 J3DDrawingAttributes.java\
 J3DElement.java\
 J3DFault.java\
 J3DLineEditAdapter.java \
 J3DLineEditListener.java \
 J3DHorizonPatch.java\
 J3DLine.java\
 J3DPanel.java\
 J3DRightClickAdapter.java\
 J3DRightClickInfo.java\
 J3DRightClickListener.java\
 J3DSelectAdapter.java\
 J3DSelectionChangedInfo.java\
 J3DSelectListener.java\
 J3DSurfacePatch.java\
 J3DText.java\
 J3DToolBar.java\
 J3DVisibilityChangedInfo.java\
 J3DVisibleAdapter.java\
 J3DVisibleListener.java\
 JColor4D.java\
 JGL.java\
 JPoint2D.java\
 JPoint3D.java\
 LastTextData.java\
 LineDraw3D.java\
 Math3D.java\
 Scaler.java\
 TextPositionFilter.java\
 TriMeshDraw3D.java\
 View3DLightingOptionsPanel.java\
 View3DPropertiesPanel.java\
 View3DPropertiesDialog.java

JAVA_CLASSES=\
 AttributeDef.class\
 AttributeDefList.class\
 BoundingBoxDraw3D.class\
 J3DAtToolBar.class\
 J3DAttribute.class\
 J3DCallbackObject.class\
 J3DConst.class\
 J3DDrawingAttributes.class\
 J3DElement.class\
 J3DFault.class\
 J3DLineEditAdapter.class \
 J3DLineEditListener.class \
 J3DHorizonPatch.class\
 J3DLine.class\
 J3DPanel.class\
 J3DRightClickAdapter.class\
 J3DRightClickInfo.class\
 J3DRightClickListener.class\
 J3DSelectAdapter.class\
 J3DSelectionChangedInfo.class\
 J3DSelectListener.class\
 J3DSurfacePatch.class\
 J3DText.class\
 J3DToolBar.class\
 J3DVisibilityChangedInfo.class\
 J3DVisibleAdapter.class\
 J3DVisibleListener.class\
 JColor4D.class\
 JGL.class\
 JPoint2D.class\
 JPoint3D.class\
 LastTextData.class\
 LineDraw3D.class\
 Math3D.class\
 Scaler.class\
 TextPositionFilter.class\
 TriMeshDraw3D.class\
 View3DLightingOptionsPanel.class\
 View3DPropertiesPanel.class\
 View3DPropertiesDialog.class


JAR_FILE=j3d.jar
PACKAGE_PATH=csw/j3d/src

all: prog

java_classes: $(JAVA_CLASSES)

prog: java_classes
	$(JAR_CMD)

clean:
	$(RM) $(JAVA_CLASSES) $(JAR_FILE)
	$(RM) $(LIB_PREFIX)*$(LIB_SUFFIX) 
	$(RM) *$(OBJ_SUFFIX) *.jar *.class

