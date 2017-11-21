.IGNORE:

include $(CSW_PARENT)/build/system.mk

LOCAL_FLAGS=-D_REENTRANT

SCOPE_FILE=scopetest$(EXE_SUFFIX)
SCOPE_OBJS=scopetest$(OBJ_SUFFIX)

EXE_FILE=$(SCOPE_FILE)
EXE_OBJS=$(SCOPE_OBJS)

all: $(SCOPE_FILE)

$(SCOPE_FILE): $(SCOPE_OBJS) $(SCOPE_LIBS)
	$(LINK_CMD)

clean:
	$(RM) *.jar *.class *.o
