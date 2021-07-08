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

SRC_DIR = TestALU

INCLUDES = /I. /I$(SRC_DIR)
AFLAGS = /nologo /c /coff $(INCLUDES) $(AFLAGS)

LIBS =
LDFLAGS = /NOLOGO /SUBSYSTEM:CONSOLE $(LDFLAGS)

################################################################################
# Names
################################################################################

BIN = TestALU.exe
TARGET_DIR = $(config)
TARGET = $(TARGET_DIR)\$(BIN)

OBJS = \
$(TARGET_DIR)\TestALU.obj

# Additional and intermediate output files
# such as .lst (Assembled Code Listing), .pdb (Program Database), .ilk (Incremental Linker File), .idb (Minimal Rebuild)
INTS = \
$(TARGET_DIR)\TestALU.lst \
$(TARGET_DIR)\TestALU.pdb \
$(TARGET_DIR)\TestALU.ilk

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

{$(SRC_DIR)\}.asm{$(TARGET_DIR)\}.obj:
	@echo Assembling: $<
	$(AS) $(AFLAGS) /Fl"$(TARGET_DIR)\$(@B).lst" /Fo"$@" "$<"
	@echo.

clean:
	@echo Cleaning:
	$(RM) $(TARGET) $(OBJS) $(INTS) >NUL 2>&1
