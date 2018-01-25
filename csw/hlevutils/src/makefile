.IGNORE:

include $(CSW_PARENT)/build/system.mk

LIB_SRC_CC=\
 simulate_new.cc\
 ThreadGuard.cc\
 GraphicsCanvasManager.cc

LIB_OBJS_CC=\
 simulate_new.o\
 ThreadGuard$(OBJ_SUFFIX)\
 GraphicsCanvasManager$(OBJ_SUFFIX)

LIB_FILE=$(LIB_PREFIX)hlutil$(LIB_SUFFIX)

all: lib

lib: $(ALL_LIB_OBJS)
	$(LIB_CMD)
	$(LIB_COPY)

clean:
	$(RM) $(ALL_LIB_OBJS) $(LIB_FILE) 
	$(RM) $(LIB_PREFIX)*$(LIB_SUFFIX) 
	$(RM) *$(OBJ_SUFFIX) 

