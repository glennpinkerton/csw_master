
# This file has make variable definitions for the Ubuntu Linux
# distribution.  Copy this file to a file named system.mk, in this
# same directory, to build csw software on Ubuntu systems.
#

OBJ_SUFFIX=.o

#
# Define the prefix and suffix for library files.
#
LIB_PREFIX=
LIB_SUFFIX=.a
SHARED_LIB_PREFIX=lib
SHARED_LIB_SUFFIX=.so

EXE_SUFFIX=

RM=rm -f
CP=cp -f
JAR=jar cf

#
# Define the c and c++ compiler
#
CC         = gcc
CXX         = g++

#
# define flags for debug and non debug compile.
# If the NODEBUG environment variable is set,
# the code will compile optimized, without debug.
#
#                 -I../../..

ifndef NODEBUG

# compile with debug and profiling
  ifndef NOPROF
    CFLAGS     = -g\
                 -fPIC \
                 -Wall\
                 -pg \
                 -c \
                 -D_CSW_FUNC_PROTOS \
                 -DPRIVATE_HEADERS_OK \
                 -I"$(CSW_PARENT)"\
                 -I"$(JAVA_HOME)/include"\
                 -I"$(JAVA_HOME)/include/linux"

# compile with debug and without profiling
  else
    CFLAGS     = -g\
                 -fPIC \
                 -Wall\
                 -c \
                 -D_CSW_FUNC_PROTOS \
                 -DPRIVATE_HEADERS_OK \
                 -I"$(CSW_PARENT)"\
                 -I"$(JAVA_HOME)/include"\
                 -I"$(JAVA_HOME)/include/linux"
  endif

#compile without debug and without profiling
else

  CFLAGS     = -O \
               -fPIC \
               -Wall\
               -c \
               -D_CSW_FUNC_PROTOS \
               -DPRIVATE_HEADERS_OK \
               -I"$(CSW_PARENT)"\
               -I"$(JAVA_HOME)/include"\
               -I"$(JAVA_HOME)/include/linux"

endif

#
# Options needed to compile .cc files
# (as opposed to .cpp or .cxx files)
#
CXX_OPTIONS = -std=c++11

#
# Define the linker
#
LINK       = gcc

#
# define flags for debug and non debug link.
#
ifndef NODEBUG
LINK_FLAGS = 
else
LINK_FLAGS = 
endif

#
#Define flags for shared library linking
#
ifndef NODEBUG
SHL_FLAGS =  -shared
else
SHL_FLAGS =  -shared
endif

#
# define debug and nondebug c libraries and cc libraries
#
ifndef NODEBUG
LIBC_MT = -lm -lc -lstdc++
else
LIBC_MT = -lm -lc -lstdc++
endif

#
# define libraries used by many csw applications.
#
CSW_LIBS_2 = \
	$(CSW_PARENT)/csw/surfaceworks/src/surf.a \
	$(CSW_PARENT)/csw/utils/src/utils.a

CSW_LIBS = $(CSW_PARENT)/csw/hlevutils/src/hlutil.a \
	$(CSW_PARENT)/csw/surfaceworks/src/surf.a \
	$(CSW_PARENT)/csw/utils/src/utils.a

#
# define the java compiler and flags for it
#
CSW_CLASS_PATH = $(CSW_PARENT):$(CSW_PARENT)/csw/jar/jogl-all.jar:$(CSW_PARENT)/csw/jar/gluegen-rt.jar

JAVA_C      = javac
JAVA_C_OPTIONS  = -classpath $(CSW_CLASS_PATH) -Xlint

#
# create a new list of suffixes
#
.SUFFIXES:
.SUFFIXES:\
 .o .c .cc \
 .class .java

#
# rules for compiling java, c and c++ files
#
.java.class:
	$(JAVA_C) $(JAVA_C_OPTIONS) $<

.c.o:
	$(CC)  $(CFLAGS) $(LOCAL_FLAGS) -c $<

.cc.o:
	$(CXX) $(CXX_OPTIONS) $(CFLAGS) $(LOCAL_FLAGS) -c $<

#
# define object library command.
#
ALL_LIB_OBJS = $(LIB_OBJS_C) \
               $(LIB_OBJS_CC) \
               $(LIB_OBJS_C_UNIX) \
               $(LIB_OBJS_CC_UNIX)

LIB_CMD = ar rc $(LIB_FILE) $(ALL_LIB_OBJS)
LIB_COPY = $(CP) $(LIB_FILE) $(CSW_PARENT)/csw/lib

#
# Define java archiver command.
#
JAR_CMD = cd $(CSW_PARENT); jar cf $(PACKAGE_PATH)/$(JAR_FILE) $(PACKAGE_PATH)/*.class
JAR_CMD_GIF = cd $(CSW_PARENT); jar cf $(PACKAGE_PATH)/$(JAR_FILE) $(PACKAGE_PATH)/*.class $(PACKAGE_PATH)/*.gif

#
# define shared library build command
#
SHL_CMD = $(LINK) -o $(SHL_FILE) $(SHL_FLAGS) \
           $(SHL_BOOT) $(SHL_OBJS) $(SHL_LIBS) $(EXTRA_SHL_LIBS) $(LIBC_MT)
SHL_COPY = $(CP) $(SHL_FILE) $(CSW_PARENT)/csw/lib

JS_LIB=$(CSW_PARENT)/csw/jeasyx/src/jeasyx.a
#
# define the regular, non shared, link commands
#
LINK_CMD = $(LINK) -o $(EXE_FILE) $(LINK_FLAGS)\
            $(EXE_OBJS) $(EXE_LIBS) $(LIBC_MT)

LINK_CMD2 = $(LINK) -o $(EXE_FILE2) $(LINK_FLAGS)\
            $(EXE_OBJS2) $(EXE_LIBS) $(LIBC_MT)

LINK_CMD3 = $(LINK) -o $(EXE_FILE3) $(LINK_FLAGS)\
            $(EXE_OBJS3) $(EXE_LIBS) $(LIBC_MT)

LINK_CMD4 = $(LINK) -o $(EXE_FILE4) $(LINK_FLAGS)\
            $(EXE_OBJS4) $(EXE_LIBS) $(LIBC_MT)

LINK_CMD5 = $(LINK) -o $(EXE_FILE5) $(LINK_FLAGS)\
            $(EXE_OBJS5) $(EXE_LIBS) $(LIBC_MT)

LINK_CMD6 = $(LINK) -o $(EXE_FILE6) $(LINK_FLAGS)\
            $(EXE_OBJS6) $(EXE_LIBS) $(LIBC_MT)

LINK_CMD7 = $(LINK) -o $(EXE_FILE7) $(LINK_FLAGS)\
            $(EXE_OBJS7) $(EXE_LIBS) $(LIBC_MT)

LINK_CMD8 = $(LINK) -o $(EXE_FILE8) $(LINK_FLAGS)\
            $(EXE_OBJS8) $(EXE_LIBS) $(LIBC_MT)

LINK_CMD9 = $(LINK) -o $(EXE_FILE9) $(LINK_FLAGS)\
            $(EXE_OBJS9) $(EXE_LIBS) $(LIBC_MT)
