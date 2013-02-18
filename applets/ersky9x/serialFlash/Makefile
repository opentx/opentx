# ----------------------------------------------------------------------------
#         ATMEL Microcontroller Software Support 
# ----------------------------------------------------------------------------
# Copyright (c) 2011, Atmel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice,
# this list of conditions and the disclaimer below.
#
# Atmel's name may not be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

#   Makefile for compiling the applet-serial flash project

# Chip & board used for compilation
# (can be overriden by adding CHIP=chip and BOARD=board to the command-line)
CHIP  = atsam3s4
BOARD = atsam3s4_ek
DYN = OFF

# Makefile for compiling applet-serialflash applet.

ifeq (atsam3s4,$(CHIP))
BOARD_DIR = at91sam3s4-ek
endif

ifeq (atsam3s2,$(CHIP))
BOARD_DIR = at91sam3s2-ek
endif

ifeq (sam9g15,$(CHIP))
BOARD =sam9g15_ek
BOARD_DIR = at91sam9g15-ek
endif

ifeq (sam9g25,$(CHIP))
BOARD =sam9g25_ek
BOARD_DIR = at91sam9g25-ek
endif

ifeq (sam9g35,$(CHIP))
BOARD =sam9g35_ek
BOARD_DIR = at91sam9g35-ek
endif

ifeq (sam9x25,$(CHIP))
BOARD =sam9x25_ek
BOARD_DIR = at91sam9x25-ek
endif

ifeq (sam9x35,$(CHIP))
BOARD =sam9x35_ek
BOARD_DIR = at91sam9x35-ek
endif

# Defines which are the available memory targets for the AT91SAM3S-EK board.
MEMORIES = sram

# Trace level used for compilation
# (can be overriden by adding TRACE_LEVEL=#number to the command-line)
# TRACE_LEVEL_DEBUG      5
# TRACE_LEVEL_INFO       4
# TRACE_LEVEL_WARNING    3
# TRACE_LEVEL_ERROR      2
# TRACE_LEVEL_FATAL      1
# TRACE_LEVEL_NO_TRACE   0

# Info traces allowed by default
TR_LEV = 0

# Optimization level, put in comment for debugging
OPTIMIZATION = -Os

# Output file basename
OUTPUT = applet-serialflash-$(CHIP)

# Output directories
BIN = bin
OBJ = obj

#-------------------------------------------------------------------------------
#		Tools
#-------------------------------------------------------------------------------

# Tool suffix when cross-compiling
CROSS_COMPILE = arm-none-eabi-

# Libraries
LIBRARIES = ../../libraries
# Chip library directory
CHIP_LIB = $(LIBRARIES)/libchip_atsam3s
# Board library directory
BOARD_LIB = $(LIBRARIES)/libboard_atsam3s-ek
# Spiflash library directory
#SPIFLASH_LIB = $(LIBRARIES)/libspiflash

# ISP directory
ISP = ..
# tcl_lib directory

#INSTALLDIR = "../../../../tcl_lib/$(BOARD_DIR)/"

LIBS = -Wl,--start-group -lgcc -lc -Wl,--end-group

#-lchip_$(CHIP)_gcc_rel -lboard_$(BOARD)_gcc_rel -lspiflash_gcc_rel -Wl,--end-group

LIB_PATH = -L$(CHIP_LIB)/lib
LIB_PATH += -L$(BOARD_LIB)/lib
#LIB_PATH += -L$(SPIFLASH_LIB)/lib
#LIB_PATH+=-L=/lib
#LIB_PATH+=-L=/../lib/gcc/arm-none-eabi/4.4.1

# Compilation tools
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
SIZE = $(CROSS_COMPILE)size
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
GDB = $(CROSS_COMPILE)gdb
NM = $(CROSS_COMPILE)nm
CLSS = $(CROSS_COMPILE)objdump

# Flags
INCLUDES  = -I$(CHIP_LIB)
INCLUDES += -I$(BOARD_LIB)
INCLUDES += -I$(SPIFLASH_LIB)
INCLUDES += -I$(LIBRARIES)

CFLAGS += -Wall -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int
CFLAGS += -Werror-implicit-function-declaration -Wmain -Wparentheses
CFLAGS += -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused
CFLAGS += -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef
CFLAGS += -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings
CFLAGS += -Wsign-compare -Waggregate-return -Wstrict-prototypes
CFLAGS += -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations
CFLAGS += -Wpacked -Wredundant-decls -Wnested-externs -Winline -Wlong-long
CFLAGS += -Wunreachable-code
CFLAGS += -Wcast-align
#CFLAGS += -Wmissing-noreturn
#CFLAGS += -Wconversion

# To reduce application size use only integer printf function.
CFLAGS += -Dprintf=iprintf
TRACE_LEVEL = 4
#CFLAGS += -DTRACE_LEVEL=$(TR_LEV)
 #-DDYN_TRACES
# -mlong-calls  -Wall
CFLAGS += -ffunction-sections  -D$(MEMORIES)
#CFLAGS += -DNDEBUG
CFLAGS += -mthumb -g -gdwarf-2 $(OPTIMIZATION) $(INCLUDES) -D$(CHIP) -DTRACE_LEVEL=$(TRACE_LEVEL) -DDYN_TRACES
ASFLAGS = -Wall -g $(OPTIMIZATION) $(INCLUDES) -D$(CHIP) -D__ASSEMBLY__
LDFLAGS= -nostartfiles -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--entry=entry -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -Wl,--warn-unresolved-symbols
LD_OPTIONAL=-Wl,--print-gc-sections -Wl,--stats

#-------------------------------------------------------------------------------
#		Files
#-------------------------------------------------------------------------------

# Directories where source files can be found

VPATH += ../..
VPATH += $(ISP)/common


# Objects built from Asm source files
//ASM_OBJECTS = applet_cstartup.o

# Objects built from C source files
C_OBJECTS += main.o
C_OBJECTS += applet_cstartup.o

# Append OBJ and BIN directories to output filename
OUTPUT := $(BIN)/$(OUTPUT)

#-------------------------------------------------------------------------------
#		Rules
#-------------------------------------------------------------------------------

all: $(BIN) $(OBJ) $(MEMORIES)

$(BIN) $(OBJ):
	mkdir $@

define RULES
C_OBJECTS_$(1) = $(addprefix $(OBJ)/$(1)_, $(C_OBJECTS))
ASM_OBJECTS_$(1) = $(addprefix $(OBJ)/$(1)_, $(ASM_OBJECTS))

$(1): $$(ASM_OBJECTS_$(1)) $$(C_OBJECTS_$(1))
	$(CC) $(LIB_PATH) $(LDFLAGS) $(LD_OPTIONAL) -T"$(BOARD_LIB)/resources/gcc/$(CHIP)/$$@_samba.lds" -Wl,-Map,$(OUTPUT).map -o $(OUTPUT).elf $$^ $(LIBS) 
	$(NM) $(OUTPUT).elf >$(OUTPUT).elf.txt 
	$(OBJCOPY) -O binary $(OUTPUT).elf $(OUTPUT).bin 
	$(SIZE) $$^ $(OUTPUT).elf 
	$(CLSS) -h -S $(OUTPUT).elf >$(OUTPUT).lss
#	@[ -e $(INSTALLDIR) ] || mkdir $(INSTALLDIR)
#	@cp -f $(OUTPUT).bin $(INSTALLDIR)
	
$$(C_OBJECTS_$(1)): $(OBJ)/$(1)_%.o: %.c Makefile $(OBJ) $(BIN)
	$(CC) $(CFLAGS) -D$(1) -c -o $$@ $$<

$$(ASM_OBJECTS_$(1)): $(OBJ)/$(1)_%.o: %.S Makefile $(OBJ) $(BIN)
	$(CC) $(ASFLAGS) -D$(1) -c -o $$@ $$<

debug_$(1): $(1)
	$(GDB) -x "$(BOARD_LIB)/resources/gcc/$(BOARD)_$(1).gdb" -ex "reset" -readnow -se $(OUTPUT)-$(1).elf
endef

$(foreach MEMORY, $(MEMORIES), $(eval $(call RULES,$(MEMORY))))

clean:
	-del $(OBJ)\*.o $(BIN)\*.bin $(BIN)\*.elf $(BIN)\*.map

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S $< > $@

