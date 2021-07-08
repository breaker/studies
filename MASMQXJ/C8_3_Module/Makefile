################################################################################
# File  : Makefile
# Desc  :
#     The master makefile
#
# Author: Breaker Zhao <breakerzhao@qq.com>
# Date  : 2010.10
################################################################################

################################################################################
# Toolchain
################################################################################

AS = ml
CXX = cl
LD = link
RM = del /a /q

################################################################################
# Build Options
################################################################################

!if "$(config)"==""
config = Debug
!endif

!if !exist("$(config).mak")
!error Require config-specific makefile: $(config).mak
!endif

!message Config-specific makefile: $(config).mak
!include $(config).mak

SRC_DIR = TestModule
DEPENDS_DIR = Depends

INCLUDES = /I. /I$(SRC_DIR) /I$(DEPENDS_DIR)
AFLAGS = /nologo /c /coff /Zi $(INCLUDES) $(AFLAGS)
CXXFLAGS = /nologo /c /MD /W3 /Zi /D "WIN32" /D "_CONSOLE" $(INCLUDES) $(CXXFLAGS)

LIBS =
LDFLAGS = /NOLOGO /SUBSYSTEM:CONSOLE /DEBUG /LIBPATH:$(DEPENDS_DIR) $(LDFLAGS)

################################################################################
# Names
################################################################################

BIN = TestModule.exe
TARGET_DIR = $(config)
TARGET = $(TARGET_DIR)\$(BIN)

OBJS = \
$(TARGET_DIR)\StdAfx.obj \
$(TARGET_DIR)\StdStream.obj \
$(TARGET_DIR)\StrHelper.obj \
$(TARGET_DIR)\Euclid.obj \
$(TARGET_DIR)\TestModule.obj

# Additional and intermediate output files
# such as .lst (Assembled Code Listing), .pdb (Program Database), .ilk (Incremental Linker File), .idb (Minimal Rebuild), .pch (Precompiled Header)
INTS = \
$(TARGET_DIR)\TestModule.lst \
$(TARGET_DIR)\TestModule.pdb \
$(TARGET_DIR)\TestModule.ilk \
$(TARGET_DIR)\Euclid.lst \
$(TARGET_DIR)\vc*.idb \
$(TARGET_DIR)\vc*.pdb \
$(TARGET_DIR)\vc*.pch

################################################################################
# Build Rules
################################################################################

all: pre-build $(TARGET)
	@echo Build: $(BIN) [config=$(config)]

pre-build:
	@echo Pre-build:
	@echo Checking target dir: $(TARGET_DIR)
	@if not exist $(TARGET_DIR) mkdir $(TARGET_DIR) && echo mkdir $(TARGET_DIR)
	@echo.

$(TARGET): $(OBJS)
	@echo Linking: $(OBJS)
	$(LD) $(LDFLAGS) /OUT:"$@" $(OBJS) $(LIBS)
	@echo.

$(TARGET_DIR)\StdAfx.obj: $(DEPENDS_DIR)\StdAfx.cpp
	@echo Compiling: $(DEPENDS_DIR)\StdAfx.cpp
	$(CXX) $(CXXFLAGS) /Fo"$@" /Yc"StdAfx.h" /Fp"$(TARGET_DIR)\\" /Fd"$(TARGET_DIR)\\" "$(DEPENDS_DIR)\StdAfx.cpp"
	@echo.

{$(DEPENDS_DIR)\}.cpp{$(TARGET_DIR)\}.obj:
	@echo Compiling: $<
	$(CXX) $(CXXFLAGS) /Fo"$@" /Yu"StdAfx.h" /Fp"$(TARGET_DIR)\\" /Fd"$(TARGET_DIR)\\" "$<"
	@echo.

{$(SRC_DIR)\}.asm{$(TARGET_DIR)\}.obj:
	@echo Assembling: $<
	$(AS) $(AFLAGS) /Fl"$(TARGET_DIR)\$(@B).lst" /Fo"$@" "$<"
	@echo.

clean:
	@echo Cleaning:
	$(RM) $(TARGET) $(OBJS) $(INTS) >NUL 2>&1
