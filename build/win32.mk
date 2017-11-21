
#
# This file has make variable definitions for ms windows 32
# systems.  Copy this file to a file named system.mk, in this
# same directory, to build csw software on win32 systems.
#

OBJ_SUFFIX=.obj

#
# Define the prefix and suffix for library files.
#
LIB_PREFIX=
LIB_SUFFIX=.lib
SHARED_LIB_PREFIX=
SHARED_LIB_SUFFIX=.dll

EXE_SUFFIX=.exe

RM=del /Q

#
# Define the c and c++ compiler
#
CC         = cl

#
# define flags for debug and non debug compile.
# If the NODEBUG environment variable is set,
# the code will compile optimized, without debug.
#
ifndef NODEBUG
CFLAGS     = -Z7 -Gy -Od -Os \
             -nologo \
             -DWINNT \
             -D_CSW_FUNC_PROTOS \
             -DPRIVATE_HEADERS_OK \
             -I../../.. \
             -I"$(CSW_PARENT)"\
             -I"$(JAVA_HOME)/include"\
             -I"$(JAVA_HOME)/include/win32"
else

CFLAGS     = -Ox \
             -nologo \
             -DWINNT \
             -D_CSW_FUNC_PROTOS \
             -DPRIVATE_HEADERS_OK \
             -I../../.. \
             -I"$(CSW_PARENT)"\
             -I"$(JAVA_HOME)/include"\
             -I"$(JAVA_HOME)/include/win32"
endif

#
# Options needed to compile .cc files
# (as opposed to .cpp or .cxx files)
#
CXX_OPTIONS = -GX -TP -W3

#
# Define the linker
#
LINK       = "c:\Program Files\Microsoft Visual Studio\VC98\bin\link"

#
# define flags for debug and non debug link.
#
ifndef NODEBUG
LINK_FLAGS = -debug:full -debugtype:cv
else
LINK_FLAGS = /RELEASE
endif

ifndef NODEBUG
SHL_FLAGS =  -debug:full \
             -debugtype:cv \
             /NODEFAULTLIB \
             /FIXED:NO \
             /INCREMENTAL:NO \
             /PDB:NONE \
             /NOLOGO \
             /LIBPATH:"c:\Program Files\Microsoft Visual Studio\VC98\lib" \
             -entry:_DllMainCRTStartup@12 \
             -dll \
             -opt:noref 
else
SHL_FLAGS =  /NODEFAULTLIB \
             /FIXED:NO \
             /INCREMENTAL:NO \
             /PDB:NONE \
             /NOLOGO \
             /LIBPATH:"c:\Program Files\Microsoft Visual Studio\VC98\lib" \
             -entry:_DllMainCRTStartup@12 \
             -dll \
             -opt:noref 
endif

#
# define debug and nondebug c libraries and cc libraries
#
ifndef NODEBUG
LIBC_MT = libcmtd.lib kernel32.lib ws2_32.lib mswsock.lib advapi32.lib user32.lib \
          gdi32.lib comctl32.lib comdlg32.lib msimg32.lib winspool.lib
else
LIBC_MT = libcmt.lib kernel32.lib ws2_32.lib mswsock.lib advapi32.lib user32.lib \
          gdi32.lib comctl32.lib comdlg32.lib msimg32.lib winspool.lib
endif

#
# define libraries used by many csw applications.
#
CSW_LIBS = $(CSW_PARENT)/csw/surfaceworks/src/surf.lib \
           $(CSW_PARENT)/csw/utils/src/utils.lib

#
# define the java compiler and flags for it
#
JAVA_C      = javac
JAVA_C_OPTIONS  = -classpath $(CSW_PARENT) -Xlint

#
# create a new list of suffixes
#
.SUFFIXES:
.SUFFIXES:\
 .obj .c .cc\
 .class .java

#
# rules for compiling java, c and c++ files
#
.java.class:
	$(JAVA_C) $(JAVA_C_OPTIONS) $<

.c.obj:
	$(CC) $(CFLAGS) $(LOCAL_FLAGS) -c $<

.cc.obj:
	$(CC) $(CXX_OPTIONS) $(CFLAGS) $(LOCAL_FLAGS) -c $<

#
# define object library command.
#
ALL_LIB_OBJS = $(LIB_OBJS_C) \
               $(LIB_OBJS_CC) \
               $(LIB_OBJS_C_OS_MSWIN) \
               $(LIB_OBJS_CC_OS_MSWIN)

LIB_CMD = lib /NOLOGO /out:$(LIB_FILE) $(ALL_LIB_OBJS)

#
# define shared library build command
#
SHL_CMD = $(LINK) /NOLOGO -out:$(SHL_FILE) $(SHL_FLAGS) \
           $(SHL_BOOT) $(SHL_OBJS) $(SHL_LIBS) $(LIBC_MT)

#
# define the regular, non shared, link command
#
LINK_CMD = $(LINK) /NOLOGO -out:$(EXE_FILE) $(LINK_FLAGS)\
            $(EXE_OBJS) $(EXE_LIBS) $(LIBC_MT)

