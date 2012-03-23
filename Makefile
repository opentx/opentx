# !!!! BETA Makefile !!!!
# !!!! Use at own risk !!!!
#----------------------------------------------------------------------------
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF.
#
# make extcoff = Convert ELF to AVR Extended COFF.
#
# make program = Download the hex file to the device, using avrdude.
#                Please customize the avrdude settings below first!
#
# make debug = Start either simulavr or avarice as specified for debugging,
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------

#----------- BUILD OPTIONS ---------------------------

#gruvin: PCB version -- OVERRIDES the following settings if not STD
# Values: STD, V4, ARM
PCB = STD

# Following options for PCB=STD only (ignored otherwise) ...

# Enable JETI-Telemetry or FrSky Telemetry reception on UART0
# For this option you need to modify your hardware!
# More information at [insertURLhere]
# Values = STD, FRSKY, JETI, NMEA, ARDUPILOT, MAVLINK
EXT = STD

# Enable heli menu
# Values = YES, NO
HELI = NO

# Enable templates menu
# Values = YES, NO
TEMPLATES = YES

# Enable navigation with Pots / Rotary encoders
# Values = NO
#          POT1 (only POT1 available for fields modification),
#          POT2 (menus scroll),
#          POT3 (cursor down/up),
#          POTS (POT1, POT2, POT3),
#          RE1  (Rotary encoder 1, on V4 boards)
ifeq ($(PCB), V4)
NAVIGATION = RE1
else
NAVIGATION = POTS
endif

# AUDIO Mods
# Values = YES, NO 
AUDIO = NO

# HAPTIC Mods
# Values = YES, NO 
HAPTIC = NO

# SPLASH on START
SPLASH = YES

# Decimals display in the main view (PPM calibration, 
# Values = YES, NO
DECIMALS = YES

# DISPLAY_USER_DATA to display on screen data from FrSky module (testing/demo purpose)
# Values = YES, NO
DISPLAY_USER_DATA = NO

# FrSky Hub 
# Values = YES, NO
FRSKY_HUB = YES

# WS HowHigh 
# Values = YES, NO
WS_HOW_HIGH = YES

# SDCARD Logs
# Values = YES, NO
LOGS = YES

# PXX (FrSky PCM) protocol
PXX = NO

# PPM16
PPM16 = NO

# DSM2 (Spektrum) protocol
DSM2 = NO

# SOMO-14D module
SOMO = NO

# TRANSLATIONS
# Values = en, 
TRANSLATIONS = EN

# DEBUG mode
DEBUG = NO

#------- END BUILD OPTIONS ---------------------------

# Define programs and commands.
SHELL = sh
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
REMOVEDIR = rm -rf
COPY = cp
WINSHELL = cmd

IMG2LBM = python ../util/img2lbm.py
REV = $(shell sh -c "svnversion | egrep -o '[[:digit:]]+[[:alpha:]]*$$'")

# MCU name
ifeq ($(PCB), STD)
  CC = avr-gcc
  MCU = atmega64
  BOARDSRC = board_stock.cpp
  EEPROMSRC = eeprom_avr.cpp  
  CPPDEFS = -DF_CPU=$(F_CPU)UL
endif
ifeq ($(PCB), V4)
  CC = avr-gcc
  MCU = atmega2560
  BOARDSRC = board_gruvin9x.cpp
  EEPROMSRC = eeprom_avr.cpp
  CPPDEFS = -DF_CPU=$(F_CPU)UL
endif
ifeq ($(PCB), ARM)
  TRGT = arm-none-eabi-
  CC   = $(TRGT)gcc
  CP   = $(TRGT)objcopy
  CLSS = $(TRGT)objdump
  AS   = $(TRGT)gcc -x assembler-with-cpp
  BIN  = $(CP) -O ihex 
  BINX = $(CP) -O binary 
  MCU  = cortex-m3
  BOARDSRC = board_ersky9x.cpp
  CPPDEFS = 
endif

# Processor frequency.
F_CPU = 16000000

# Output format. (can be srec, ihex, binary)
FORMAT = ihex

# Target file name (without extension).
TARGET = open9x

# Object files directory
OBJDIR = obj

# List C++ source files here. (C dependencies are automatically generated.)
CPPSRC = open9x.cpp pulses.cpp stamp.cpp menus.cpp model_menus.cpp general_menus.cpp main_views.cpp statistics_views.cpp $(EEPROMSRC) lcd.cpp drivers.cpp o9xstrings.cpp

ifeq ($(EXT), JETI)
 CPPSRC += jeti.cpp
endif

ifeq ($(EXT), ARDUPILOT)
 CPPSRC += ardupilot.cpp
endif

ifeq ($(EXT), NMEA)
 CPPSRC += nmea.cpp
endif

# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DBGFMT = dwarf-2


# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRAINCDIRS = . translations

# NOTE: PCB version now overrides all the earlier individual settings
#       These individual settings work only for PCB=STD

CPPDEFS += -DTRANSLATIONS_$(TRANSLATIONS)

# If POT1/POTS/RE1 is used for fields modification
ifeq ($(NAVIGATION), POT1)
  CPPDEFS += -DNAVIGATION_POT1
endif
ifeq ($(NAVIGATION), POT2)
  CPPDEFS += -DNAVIGATION_POT2
endif
ifeq ($(NAVIGATION), POT3)
  CPPDEFS += -DNAVIGATION_POT3
endif
ifeq ($(NAVIGATION), POTS)
  CPPDEFS += -DNAVIGATION_POT1 -DNAVIGATION_POT2 -DNAVIGATION_POT3
endif

ifeq ($(SPLASH), YES)
  CPPDEFS += -DSPLASH
endif

ifeq ($(HAPTIC), YES)
  CPPDEFS += -DHAPTIC
endif

# If ARDUPILOT-Support is enabled
ifeq ($(EXT), ARDUPILOT)
  CPPDEFS += -DARDUPILOT
endif

# If NMEA-Support is enabled
ifeq ($(EXT), NMEA)
  CPPDEFS += -DNMEA
endif

# If JETI-Support is enabled
ifeq ($(EXT), JETI)
  MODS:=${MODS}J
  CPPDEFS += -DJETI
endif

ifeq ($(EXT), MAVLINK)
 MODS:=${MODS}M
 CPPDEFS += -DMAVLINK
 CPPSRC += mavlink.cpp rotarysw.cpp serial.cpp
endif

# If FRSKY-Support is enabled
ifeq ($(EXT), FRSKY)
  MODS:=${MODS}F
  CPPDEFS += -DFRSKY
  CPPSRC += frsky.cpp
  # If FRSKY-Hub is enabled
  ifeq ($(FRSKY_HUB), YES)
    CPPDEFS += -DFRSKY_HUB
  endif
  # If WS HowHigh is enabled
  ifeq ($(WS_HOW_HIGH), YES)
    MODS:=${MODS}W
    CPPDEFS += -DWS_HOW_HIGH
  endif
endif

ifeq ($(DEBUG), YES)
  CPPDEFS += -DDEBUG
endif

ifeq ($(PCB), ARM)
  # V4 ARM, so ...
  OPT = 2
  CPPDEFS += -DPCBARM
  EXTRAINCDIRS += ersky9x
  CPPSRC += ersky9x/core_cm3.c
  CPPSRC += ersky9x/board_lowlevel.c
  CPPSRC += ersky9x/crt.c
  CPPSRC += ersky9x/vectors_sam3s.c
  CPPSRC += ersky9x/sound.cpp
  CPPSRC += beeper.cpp
endif

ifeq ($(PCB), V4)
  # V4 PCB, so ...
  OPT = 2
  CPPDEFS += -DPCBV4 -DAUDIO
  EXTRAINCDIRS += gruvin9x
  CPPSRC += audio.cpp
  CPPSRC += gruvin9x/gtime.cpp
  CPPSRC += gruvin9x/rtc.cpp
  CPPSRC += gruvin9x/ff.cpp
  CPPSRC += gruvin9x/diskio.cpp
   

  ifeq ($(NAVIGATION), RE1)
    CPPDEFS += -DNAVIGATION_RE1
  endif

  ifeq ($(LOGS), YES)
    CPPSRC += gruvin9x/logs.cpp
    CPPDEFS += -DLOGS
    MODS:=${MODS}L
  endif
    
  ifeq ($(SOMO), YES)
    CPPSRC += gruvin9x/somo14d.cpp
    CPPDEFS += -DSOMO
  endif
endif

ifeq ($(PCB), STD)
  # STD PCB, so ...
  OPT = s
  CPPDEFS += -DPCBSTD
   
  ifeq ($(AUDIO), YES)
    CPPDEFS += -DAUDIO
    CPPSRC += audio.cpp
  else
    CPPSRC += beeper.cpp 
  endif
endif

### Global Build-Option Directives ###

ifeq ($(DECIMALS), YES)
  CPPDEFS += -DDECIMALS_DISPLAYED
endif

ifeq ($(HELI), YES)
 MODS:=${MODS}H
 CPPDEFS += -DHELI
endif

ifeq ($(TEMPLATES), YES)
 CPPDEFS += -DTEMPLATES
 CPPSRC += templates.cpp
endif

ifeq ($(DISPLAY_USER_DATA), YES)
 CPPDEFS += -DDISPLAY_USER_DATA
endif

ifeq ($(PXX), YES)
  CPPDEFS += -DPXX
endif

ifeq ($(PPM16), YES)
  CPPDEFS += -DPPM16
endif

ifeq ($(DSM2), SERIAL)
  CPPDEFS += -DDSM2 -DDSM2_SERIAL
endif

ifeq ($(DSM2), PPM)
  CPPDEFS += -DDSM2 -DDSM2_PPM
endif

#---------------- Compiler Options C++ ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CPPFLAGS = -g$(DBGFMT)
CPPFLAGS += $(CPPDEFS)
CPPFLAGS += -O$(OPT)
#CPPFLAGS += -mint8
#CPPFLAGS += -mshort-calls
#CPPFLAGS += -funsigned-char
#CPPFLAGS += -funsigned-bitfields
#CPPFLAGS += -fpack-struct
#CPPFLAGS += -fshort-enums
#CPPFLAGS += -fno-exceptions
#CPPFLAGS += -fno-unit-at-a-time
CPPFLAGS += -Wall
CPPFLAGS += -fno-exceptions
CPPFLAGS += -Wno-strict-aliasing
#CPPFLAGS += -Wstrict-prototypes
#CPPFLAGS += -Wunreachable-code
#CPPFLAGS += -Wsign-compare
#CPPFLAGS += -Wa,-adhlns=$(<:%.cpp=$(OBJDIR)/%.lst)
CPPFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

ifneq ($(PCB), ARM)
  GCCVERSIONGTE462 := $(shell expr 4.6.2 \<= `$(CC) -dumpversion`)
  ifeq ($(GCCVERSIONGTE462),1)
    CPPFLAGS += -flto
  endif
  CPPFLAGS += -fno-inline-small-functions
endif



#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
ASFLAGS = -Wa,-adhlns=$(<:%.S=$(OBJDIR)/%.lst),-gstabs


#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB =
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)


# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB =
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)


MATH_LIB = -lm



#---------------- External Memory Options ----------------

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =



#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)
#LDFLAGS += -T linker_script.x



#---------------- Programming Options (avrdude) ----------------

# Programming hardware: alf avr910 avrisp bascom bsd
# dt006 pavr picoweb pony-stk200 sp12 stk200 stk500
#
# Type: avrdude -c ?
# to get a full listing.
#
AVRDUDE_PROGRAMMER = avrispmkII


# com1 = serial port. Use lpt1 to connect to parallel port.
AVRDUDE_PORT = usb

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex:a
AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).bin:a
AVRDUDE_READ_FLASH = -U flash:r:$(TARGET).hex:r
AVRDUDE_READ_EEPROM = -U eeprom:r:$(TARGET).bin:r



# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude>
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

#AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS = -B0.25 -p $(MCU) -c $(AVRDUDE_PROGRAMMER) -P $(AVRDUDE_PORT)
#AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)

ifeq ($(ERAZE), NO)
  AVRDUDE_FLAGS += -D
endif

#---------------- Debugging Options ----------------

# For simulavr only - target MCU frequency.
DEBUG_MFREQ = $(F_CPU)

# Set the DEBUG_UI to either gdb or insight.
DEBUG_UI = gdb
# DEBUG_UI = insight

# Set the debugging back-end to either avarice, simulavr.
#DEBUG_BACKEND = avarice
DEBUG_BACKEND = simulavr

# GDB Init Filename.
GDBINIT_FILE = __avr_gdbinit

# When using avarice settings for the JTAG
JTAG_DEV = /dev/com1

# Debugging port used to communicate between GDB / avarice / simulavr.
DEBUG_PORT = 4242

# Debugging host used to communicate between GDB / avarice / simulavr, normally
#     just set to localhost unless doing some sort of crazy debugging when
#     avarice is running on a different computer.
DEBUG_HOST = localhost

# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_COMPILING = Compiling C++:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:
MSG_CREATING_LIBRARY = Creating library:

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.

ifeq ($(PCB), ARM)
  ALL_CPPFLAGS = -c -mcpu=$(MCU) -mthumb -fomit-frame-pointer -fverbose-asm -Wa,-ahlms=open9x.lst -Dat91sam3s4 -DRUN_FROM_FLASH=1 $(CPPFLAGS) $(GENDEPFLAGS)
else
  ALL_CPPFLAGS = -mmcu=$(MCU) -I. -x c++ $(CPPFLAGS) $(GENDEPFLAGS) -fwhole-program
endif

MAJ_VER = ${shell sh -c "grep \"MAJ_VERS\" open9x.h | cut -d\  -f3 | egrep -o \"[[:digit:]]\""}
MIN_VER = ${shell sh -c "grep \"MIN_VERS\" open9x.h | cut -d\  -f3"}

# Default target.
all: begin gccversion sizebefore build sizeafter end

# Change the build target to build a HEX file or a library.
build: stamp_header font.lbm font_dblsize.lbm sticks.lbm s9xsplash.lbm allsrc.cpp elf remallsrc bin hex eep lss sym

elf: $(TARGET).elf
bin: $(TARGET).bin
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym

# Build stamp-file
stamp_header:
	@echo
	@echo "Generate Version-stamp:"
	@echo "//Automatically generated file (Makefile) - do not edit" > stamp-open9x.h
	@echo "#define DATE_STR \"`date +%Y-%m-%d`\"" >> stamp-open9x.h
	@echo "#define TIME_STR \"`date +%H:%I:%S`\"" >> stamp-open9x.h
	@echo "#define VERS_STR \"$(MAJ_VER).$(MIN_VER)-$(MODS)\"" >> stamp-open9x.h
	@echo "#define SVN_STR  \"open9x-r$(REV)\"" >> stamp-open9x.h
	@cat stamp-open9x.h
	
stamp:
	@echo "#define DATE_STR \"`date +%Y-%m-%d`\"" > ../stamp-open9x.txt
	@echo "#define TIME_STR \"`date +%H:%I:%S`\"" >> ../stamp-open9x.txt
	@echo "#define VERS_STR \"$(MAJ_VER).$(MIN_VER)\"" >> ../stamp-open9x.txt
	@echo "#define SVN_VERS  \"open9x-r$(REV)\"" >> ../stamp-open9x.txt
	@cat ../stamp-open9x.txt
 
font.lbm: font_6x1.xbm
	@echo
	@echo "Convert font from xbm to lbm:"
	$(IMG2LBM) $< $@ char
	$(IMG2LBM) translations/font_se.xbm translations/font_se.lbm char

font_dblsize.lbm: font_dblsize.xbm
	@echo
	@echo "Convert font from xbm to lbm:"
	$(IMG2LBM) $< $@ dblsize
	$(IMG2LBM) translations/font_dblsize_se.xbm translations/font_dblsize_se.lbm dblsize

sticks.lbm: sticks_4x1.xbm
	@echo
	@echo "Convert sticks from xbm to lbm:"
	$(IMG2LBM) $< $@ img 4

s9xsplash.lbm: s9xsplash.xbm
	@echo
	@echo "Convert font from xbm to lbm:"
	$(IMG2LBM) $< $@ img

# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.

begin:
	@echo
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)
	@echo



# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) --mcu=$(MCU) --format=avr $(TARGET).elf
AVRMEM = avr-mem.sh $(TARGET).elf $(MCU)

ifeq ($(PCB), ARM)
sizebefore:
	
sizeafter:
	
else
sizebefore:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); \
	$(AVRMEM) 2>/dev/null; echo; fi

sizeafter:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); \
	$(AVRMEM) 2>/dev/null; echo; fi
endif

# Display compiler version information.
gccversion :
	@$(CC) --version



# Program the device.
wflash: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

weeprom: $(TARGET).bin
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_EEPROM)

# Write flash and eeprom
wfe: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_EEPROM)

rflash: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_READ_FLASH)

reeprom: $(TARGET).bin
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_READ_EEPROM)


# Generate avr-gdb config/init file which does the following:
#     define the reset signal, load the target file, connect to target, and set
#     a breakpoint at main().
gdb-config:
	@$(REMOVE) $(GDBINIT_FILE)
	@echo define reset >> $(GDBINIT_FILE)
	@echo SIGNAL SIGHUP >> $(GDBINIT_FILE)
	@echo end >> $(GDBINIT_FILE)
	@echo file $(TARGET).elf >> $(GDBINIT_FILE)
	@echo target remote $(DEBUG_HOST):$(DEBUG_PORT)  >> $(GDBINIT_FILE)
ifeq ($(DEBUG_BACKEND),simulavr)
	@echo load  >> $(GDBINIT_FILE)
endif
	@echo break main >> $(GDBINIT_FILE)

# gruvin: added extra include and lib paths to get simu working on my Mac
FOXINC=-I/usr/local/include/fox-1.6 -I/usr/include/fox-1.6 \
       -I$(FOXPATH)/include \
       -I/opt/local/include/fox-1.6
FOXLIB=-L/usr/local/lib \
       -L$(FOXPATH)/src/.libs \
       -L/opt/local/lib \
       -lFOX-1.6 \
       -Wl,-rpath,$(FOXPATH)/src/.libs

simu: $(BOARDSRC) $(CPPSRC) Makefile simu.cpp simpgmspace.cpp *.h *.lbm eeprom.bin
	g++ simu.cpp $(BOARDSRC) $(CPPFLAGS) $(CPPSRC) simpgmspace.cpp $(ARCH) -MD -DSIMU -o simu $(FOXINC) $(FOXLIB) -pthread

eeprom.bin:
	dd if=/dev/zero of=$@ bs=1 count=2048

debug: gdb-config $(TARGET).elf
ifeq ($(DEBUG_BACKEND),avarice)
	@echo Starting AVaRICE - Press enter when "waiting to connect" message displays.
	@$(WINSHELL) /c start avarice --jtag $(JTAG_DEV) --erase --program --file \
	  $(TARGET).elf $(DEBUG_HOST):$(DEBUG_PORT)
	@$(WINSHELL) /c pause
else
	@$(WINSHELL) /c start simulavr --gdbserver --device $(MCU) --clock-freq \
	  $(DEBUG_MFREQ) --port $(DEBUG_PORT)
endif
	@$(WINSHELL) /c start avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)




# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
	COFFCONVERT = $(OBJCOPY) --debugging
	COFFCONVERT += --change-section-address .data-0x800000
	COFFCONVERT += --change-section-address .bss-0x800000
	COFFCONVERT += --change-section-address .noinit-0x800000
	COFFCONVERT += --change-section-address .eeprom-0x810000


coff: $(TARGET).elf
	@echo
	@echo $(MSG_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-avr $< $(TARGET).cof


extcoff: $(TARGET).elf
	@echo
	@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof

# Create final output files (.hex, .eep) from ELF output file.
ifeq ($(PCB), ARM)
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	arm-none-eabi-objcopy -O ihex open9x.elf open9x.hex
else
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@
endif

ifeq ($(PCB), ARM)
%.bin: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	arm-none-eabi-objcopy -O binary  open9x.elf open9x.bin
endif

#arm-none-eabi-gcc -c -mcpu=cortex-m3 -O2 -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fverbose-asm -Wa,-ahlms=open9x.lst  -Dat91sam3s4 -DRUN_FROM_FLASH=1  -DPCBARM -DDECIMALS_DISPLAYED -DSPLASH -MD -MP -MF .dep/open9x.o.d -fno-exceptions -I . -I./ersky9x  open9x.cpp -o open9x.o
#arm-none-eabi-gcc  ersky9x/core_cm3.o ersky9x/board_lowlevel.o ersky9x/crt.o ersky9x/vectors_sam3s.o drivers.o ersky9x\sound.o lcd.o menus.o main_views.o statistics_views.o model_menus.o general_menus.o open9x.o beeper.o o9xstrings.o board_ersky9x.o -mcpu=cortex-m3 -mthumb -nostartfiles -Tersky9x/sam3s2c_flash.ld -Wl,-Map=open9x_rom.map,--cref,--no-warn-mismatch    -o open9x_rom.elf
#arm-none-eabi-objcopy -O ihex  open9x_rom.elf open9x_rom.hex
#arm-none-eabi-objdump -h -S open9x_rom.elf > open9x_rom.lss
#arm-none-eabi-objcopy -O binary  open9x_rom.elf open9x_rom.bin

ifeq ($(PCB), ARM)
%.eep:
	
%.sym:
	
else
%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@
endif

# Create extended listing file from ELF output file.
ifeq ($(PCB), ARM)
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	arm-none-eabi-objdump -h -S open9x.elf > open9x.lss
else
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S $< > $@
endif

# Concatenate all sources files in one big file to optimize size
allsrc.cpp: $(BOARDSRC) $(CPPSRC)
	@echo -n > allsrc.cpp
	for f in $(BOARDSRC) $(CPPSRC); do echo "# 1 \"$$f\"" >> allsrc.cpp; cat "$$f" >> allsrc.cpp; done
	
remallsrc:
	$(REMOVE) allsrc.cpp
    
# Link: create ELF output file from object files.
ifeq ($(PCB), ARM)
%.elf: allsrc.cpp
	@echo
	@echo $(MSG_COMPILING) $@
	$(CC) $(ALL_CPPFLAGS) $< -o allsrc.o
	arm-none-eabi-gcc allsrc.o -mcpu=cortex-m3 -mthumb -nostartfiles -Tersky9x/sam3s2c_flash.ld -Wl,-Map=$(TARGET).map,--cref,--no-warn-mismatch -o $@
else
%.elf: allsrc.cpp
	@echo
	@echo $(MSG_COMPILING) $@
	$(CC) $(ALL_CPPFLAGS) $< --output $@ $(LDFLAGS)
endif
	
# Target: clean project.
clean: begin clean_list end

clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) simu
	$(REMOVE) gtests
	$(REMOVE) gtest.a
	$(REMOVE) gtest_main.a
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).eep
	$(REMOVE) $(TARGET).cof
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lss
	$(REMOVEDIR) $(OBJDIR)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) *.o
	$(REMOVE) *.d
	$(REMOVE) *.lst
	$(REMOVE) allsrc.cpp
	$(REMOVEDIR) .dep

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

#### GOOGLE TESTS 

GTEST_DIR = ../gtest-1.6.0

# Where to find user code.
USER_DIR = ./

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# House-keeping build targets.

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.

gtests: $(CPPSRC) gtests.cpp simpgmspace.cpp *.h gtest_main.a
	g++ $(CPPSRC) gtests.cpp simpgmspace.cpp $(CPPFLAGS) -I$(GTEST_DIR) $(ARCH) -o gtests -lpthread -MD -DSIMU gtest_main.a 
	
