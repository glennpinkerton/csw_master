.IGNORE:

include $(CSW_PARENT)/build/system.mk



LIB_SRC_CC=\
 con_api.cc\
 con_calc.cc\
 con_draw.cc\
 con_triangle.cc\
 con_xsect.cc\
 grd_api.cc\
 grd_arith.cc\
 grd_calc.cc\
 grd_constraint.cc\
 grd_ctog.cc\
 grd_faults.cc\
 grd_fileio.cc\
 grd_image.cc\
 grd_stats.cc\
 grd_triangle.cc\
 grd_tsurf.cc\
 grd_utils.cc\
 grd_spatial3dtri.cc\
 grd_xyindex.cc\
 grd_xyzindex.cc\
 FaultConnect.cc\
 moller.cc\
 PadSurfaceForSim.cc\
 SealedModel.cc\
 SurfaceGroupPlane.cc\
 Vert.cc



LIB_OBJS_CC=\
 con_api$(OBJ_SUFFIX)\
 con_calc$(OBJ_SUFFIX)\
 con_draw$(OBJ_SUFFIX)\
 con_triangle$(OBJ_SUFFIX)\
 con_xsect$(OBJ_SUFFIX)\
 grd_arith$(OBJ_SUFFIX)\
 grd_api$(OBJ_SUFFIX)\
 grd_calc$(OBJ_SUFFIX)\
 grd_constraint$(OBJ_SUFFIX)\
 grd_ctog$(OBJ_SUFFIX)\
 grd_faults$(OBJ_SUFFIX)\
 grd_fileio$(OBJ_SUFFIX)\
 grd_image$(OBJ_SUFFIX)\
 grd_stats$(OBJ_SUFFIX)\
 grd_triangle$(OBJ_SUFFIX)\
 grd_tsurf$(OBJ_SUFFIX)\
 grd_utils$(OBJ_SUFFIX)\
 grd_spatial3dtri$(OBJ_SUFFIX)\
 grd_xyindex$(OBJ_SUFFIX)\
 grd_xyzindex$(OBJ_SUFFIX)\
 FaultConnect$(OBJ_SUFFIX)\
 moller$(OBJ_SUFFIX)\
 PadSurfaceForSim$(OBJ_SUFFIX)\
 SealedModel$(OBJ_SUFFIX)\
 SurfaceGroupPlane$(OBJ_SUFFIX)\
 Vert$(OBJ_SUFFIX)


LIB_FILE=$(LIB_PREFIX)surf$(LIB_SUFFIX)

$(LIB_FILE): $(ALL_LIB_OBJS)
	$(LIB_CMD)
	$(LIB_COPY)
	$(CP) ../../utils/src/utils.a utils.a

prog: $(LIB_FILE)




$(EXE_FILE): $(EXE_OBJS) $(EXE_LIBS)
	$(LINK_CMD)


clean: 
	$(RM) $(ALL_LIB_OBJS) $(LIB_FILE)
	$(RM) $(LIB_PREFIX)*$(LIB_SUFFIX) 
	$(RM) *$(OBJ_SUFFIX) 


