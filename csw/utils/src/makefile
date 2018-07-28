.IGNORE:

include $(CSW_PARENT)/build/system.mk

# gpf_fontlen.cc\
# ply_calc.cc\
# ply_drivers.cc\

LIB_SRC_CC=\
 csw_timer.cc\
 csw_fileio.cc\
 csw_filenam.cc\
 csw_fileno.cc\
 csw_fsync.cc\
 csw_heap.cc\
 csw_hostname.cc\
 csw_memmgt.cc\
 csw_putenv.cc\
 csw_stread.cc\
 csw_string.cc\
 csw_time.cc\
 csw_utils.cc\
 gpf_calcdraw.cc\
 gpf_color.cc\
 gpf_font.cc\
 gpf_graph.cc\
 gpf_image.cc\
 gpf_ldash.cc\
 gpf_lines.cc\
 gpf_shapes.cc\
 gpf_spline.cc\
 gpf_symbol.cc\
 gpf_thinxy.cc\
 gpf_utils.cc\
 ply_compdata.cc\
 ply_cutline.cc\
 ply_edit.cc\
 ply_graph.cc\
 ply_gridvec.cc\
 ply_patfill.cc\
 ply_tiledata.cc\
 ply_traverse.cc\
 ply_utils.cc

LIB_SRC_C_UNIX=\
 TextBounds.c\
 csw_cuserid.c\
 csw_errno.c

LIB_OBJS_C_UNIX=\
 TextBounds$(OBJ_SUFFIX)\
 csw_cuserid$(OBJ_SUFFIX)\
 csw_errno$(OBJ_SUFFIX)

# gpf_fontlen$(OBJ_SUFFIX)\

LIB_OBJS_CC=\
 csw_timer$(OBJ_SUFFIX)\
 csw_fileio$(OBJ_SUFFIX)\
 csw_filenam$(OBJ_SUFFIX)\
 csw_fileno$(OBJ_SUFFIX)\
 csw_fsync$(OBJ_SUFFIX)\
 csw_heap$(OBJ_SUFFIX)\
 csw_hostname$(OBJ_SUFFIX)\
 csw_memmgt$(OBJ_SUFFIX)\
 csw_putenv$(OBJ_SUFFIX)\
 csw_stread$(OBJ_SUFFIX)\
 csw_string$(OBJ_SUFFIX)\
 csw_time$(OBJ_SUFFIX)\
 csw_utils$(OBJ_SUFFIX)\
 gpf_calcdraw$(OBJ_SUFFIX)\
 gpf_color$(OBJ_SUFFIX)\
 gpf_font$(OBJ_SUFFIX)\
 gpf_graph$(OBJ_SUFFIX)\
 gpf_image$(OBJ_SUFFIX)\
 gpf_ldash$(OBJ_SUFFIX)\
 gpf_lines$(OBJ_SUFFIX)\
 gpf_shapes$(OBJ_SUFFIX)\
 gpf_spline$(OBJ_SUFFIX)\
 gpf_symbol$(OBJ_SUFFIX)\
 gpf_thinxy$(OBJ_SUFFIX)\
 gpf_utils$(OBJ_SUFFIX)\
 ply_calc$(OBJ_SUFFIX)\
 ply_compdata$(OBJ_SUFFIX)\
 ply_cutline$(OBJ_SUFFIX)\
 ply_drivers$(OBJ_SUFFIX)\
 ply_edit$(OBJ_SUFFIX)\
 ply_graph$(OBJ_SUFFIX)\
 ply_gridvec$(OBJ_SUFFIX)\
 ply_patfill$(OBJ_SUFFIX)\
 ply_tiledata$(OBJ_SUFFIX)\
 ply_traverse$(OBJ_SUFFIX)\
 ply_utils$(OBJ_SUFFIX)\
 csw_errno$(OBJ_SUFFIX)


CC  =  gcc

LIB_FILE=$(LIB_PREFIX)utils$(LIB_SUFFIX)

all: prog

$(LIB_FILE): $(ALL_LIB_OBJS)
	$(LIB_CMD)
	$(LIB_COPY)

clean:
	$(RM) $(ALL_LIB_OBJS) $(LIB_FILE)
	$(RM) $(LIB_PREFIX)*$(LIB_SUFFIX) 
	$(RM) *$(OBJ_SUFFIX) 


prog: $(LIB_FILE)

