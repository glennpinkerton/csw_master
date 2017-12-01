.IGNORE:

include $(CSW_PARENT)/build/system.mk

JAVA_SRC=\
 AttributeTriMesh.java\
 CSWGrid.java\
 DBContactLine.java\
 DBDrapeLine.java\
 DBEdgeList.java\
 DBLineList.java\
 DBPoint.java\
 DBPointList.java\
 FaultConstraint.java\
 FaultMajorMinor.java\
 GridCalcOptions.java\
 GridGeometry.java\
 Grid.java\
 JSBorderSegment.java\
 JSResult2D.java\
 JSResult3D.java\
 JSurfaceWorksBase.java\
 JSurfaceWorks.java\
 PatchLine.java\
 ProtoPatch.java\
 TetgenFace.java\
 TetgenFaceList.java\
 TetgenNode.java\
 TetgenNodeList.java\
 TriangleIndex3D.java\
 TriangleIndex3DResult.java\
 TriangleIndex3DResultList.java\
 TriMeshConstraint.java\
 TriMesh.java\
 VertBaseline.java

JAVA_CLASSES=\
 AttributeTriMesh.class\
 CSWGrid.class\
 DBContactLine.class\
 DBDrapeLine.class\
 DBEdgeList.class\
 DBLineList.class\
 DBPoint.class\
 DBPointList.class\
 FaultConstraint.class\
 FaultMajorMinor.class\
 GridCalcOptions.class\
 GridGeometry.class\
 Grid.class\
 JSBorderSegment.class\
 JSResult2D.class\
 JSResult3D.class\
 JSurfaceWorksBase.class\
 JSurfaceWorks.class\
 PatchLine.class\
 ProtoPatch.class\
 TetgenFace.class\
 TetgenFaceList.class\
 TetgenNode.class\
 TetgenNodeList.class\
 TriangleIndex3D.class\
 TriangleIndex3DResult.class\
 TriangleIndex3DResultList.class\
 TriMeshConstraint.class\
 TriMesh.class\
 VertBaseline.class

JAR_FILE=jsurf.jar
PACKAGE_PATH=csw/jsurfaceworks/src

LIB_SRC_C=\
 SurfaceWorksJNI.c

LIB_SRC_CC=\
 JVert.cc\
 SWCalc.cc\
 SWCommand.cc\
 shr_lib_link.cc\
 PatchSplit.cc

LIB_OBJS_C=\
 SurfaceWorksJNI$(OBJ_SUFFIX)\

LIB_OBJS_CC=\
 JVert$(OBJ_SUFFIX)\
 SWCalc$(OBJ_SUFFIX)\
 SWCommand$(OBJ_SUFFIX)\
 shr_lib_link$(OBJ_SUFFIX)\
 PatchSplit$(OBJ_SUFFIX) 

LIB_FILE=$(LIB_PREFIX)jsurf$(LIB_SUFFIX)

EXE_LIBS=\
  $(CSW_PARENT)/csw/lib/jsurf.a \
  $(CSW_PARENT)/csw/lib/jeasyx.a \
  $(CSW_PARENT)/csw/lib/hlutil.a \
  $(CSW_PARENT)/csw/lib/jeasyx.a \
  $(CSW_PARENT)/csw/lib/surf.a \
  $(CSW_PARENT)/csw/lib/utils.a

EXE_OBJS=\
 sw_test.o

EXE_FILE=\
 sw_test$(EXE_SUFFIX)

all: prog

java_classes: $(JAVA_CLASSES)

lib: $(ALL_LIB_OBJS) $(JAVA_CLASSES)
	$(LIB_CMD)
	$(LIB_COPY)

archive: lib

executable: $(EXE_OBJS)
	$(LINK_CMD)
	$(CP) $(EXE_FILE) $(CSW_PARENT)/csw/jtest

prog: java_classes lib executable

clean:
	$(RM) $(ALL_LIB_OBJS) $(LIB_FILE) $(JAVA_CLASSES) $(EXE_OBJS) $(EXE_FILE)
	$(RM) $(LIB_PREFIX)*$(LIB_SUFFIX) 
	$(RM) *$(OBJ_SUFFIX) *.class *.jar

