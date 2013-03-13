# !!!! BETA Makefile !!!!
# !!!! Use at own risk !!!!
#----------------------------------------------------------------------------
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make install = Install the most recently compiled hex file.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------

#----------- INSTALL OPTIONS ---------------------------

# Data directory we write hex files to.
DATA_DIR = /tmp

# Backup the old flash (YES, NO)
BACKUP_FLASH = YES

# Backup the old EEPROM (YES, NO)
BACKUP_EEPROM = YES

# Restore EEPROM to 9x (YES, NO) (will set BACKUP_EEPROM to YES).
RESTORE_EEPROM = NO

# Double write OK (YES, NO)
# Set this *if only if* your programmer can correctly write the EEPROM
# and FLASH in one command. If it doesn't, try setting the 9x fuse to
# not erase the eeprom when you erase the flash.
DOUBLE_WRITE_OK = NO

# Command to read/write FLASH/EEPROM images. Currently
# Must have avrdude interface
AVRDUDE = /usr/local/bin/avrdude

# Device options for AVRDUDE Command. 
AVRDEVICE = -c usbasp -p m64

# Delay to start second AVRDUDE command (required by some).
DELAY = 5

#----------- BUILD OPTIONS ---------------------------

# PCB version
# Values: 9X, 9X128, 9XR, 9XR128, GRUVIN9X, SKY9X, X9D
# Old values STD and STD128 are mapped to 9X and 9X128 for compatibility
PCB = 9X

# PCB revision
# Values: REVA, REVB
PCBREV =

# Enable JETI-Telemetry or FrSky Telemetry reception on UART0
# For this option you need to modify your hardware!
# More information at [insertURLhere]
# Values = STD, FRSKY, JETI, NMEA, ARDUPILOT, MAVLINK, TELEMETREZ
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
#          ROTENC (Rotary encoder)
NAVIGATION = NO

# Switches automatic selection
# Values = YES, NO 
AUTOSWITCH = NO

# Double keys support (+/- means invert, +/up means max, -/dwn means min, up/down means reset to default)
# Values = YES, NO 
DBLKEYS = NO

# AUDIO Mods
# Values = YES, NO 
AUDIO = NO

# VOICE Mods
# Values = YES, NO 
VOICE = NO

# corrects different rounding for negative values. So instead of -99.9 you will see again -100.0
# Values = YES, NO
CORRECT_NEGATIVE_VALUES = YES

# adds additional code to prevent arithmetic overflows. MAX or MIN values stays. Not yet guaranteed!! Just improved
# Values = YES, NO
ARITHMETIC_OVERFLOW_CHECK = NO

# HAPTIC Mods
# Values = YES, NO 
HAPTIC = NO

# SmartieParts 2.2 Backlight support (inverted)
# Values = YES, NO 
SP22 = NO

# Fade Backlight using PWM
# Values = YES, NO 
PWM_BACKLIGHT = NO

# SPLASH on START
# Values = YES, NO, FIXED
SPLASH = YES

# GRAPHICS (sliders / checkboxes / bold / batt graph)
GRAPHICS = YES
BOLD = YES
BATTGRAPH = NO

# THR trace (in STATS screen)
THR_TRACE = YES

# PPM center adjustable
# Values = YES, NO
PPM_CENTER_ADJUSTABLE = NO

# PPM symetrical limits
# Values = YES, NO
PPM_LIMITS_SYMETRICAL = NO

# PPM display unit
# Values = PERCENT_PREC1, PERCENT_PREC0, US
PPM_UNIT = PERCENT_PREC1

# FrSky Hub 
# Values = YES, NO
FRSKY_HUB = YES

# WS HowHigh 
# Values = YES, NO
WS_HOW_HIGH = YES

# WS HowHigh 
# Values = YES, NO
VARIO = YES

# RTCLOCK
# Values = YES, NO
# default = YES (means enabled when available)
RTCLOCK = YES

# BLUETOOTH
# Values = YES, NO
BLUETOOTH = NO

# PXX (FrSky PCM) protocol
PXX = NO

# DSM2 (Spektrum) protocol
# Values = NO, SERIAL, PPM
DSM2 = NO

# DSM/PPM TX caddy PCB (Gruvin)
# For auto-switching switching between DSM2/X and Fr-Sky transmitter modules
# Values = YES, NO
TX_CADDY = NO

#IR protocols (SILV / TRAC09 / PICZ / SWIFT)
IRPROTOS = NO

# TRANSLATIONS
# Values = cz, de, en, fr, it, se, es, pt
TRANSLATIONS = EN

# TTS
# Values = en, fr
TTS = $(TRANSLATIONS)

# UNITS
# Values = imperial, metric 
UNITS = METRIC

# EEPROM_PROGRESS_BAR
# Values = YES, NO
EEPROM_PROGRESS_BAR = NO

# Activates the DEBUG Mode (all boards)
# On G9X V4 board, when VOICE=NO:
#  PORTH:7 --> Transistion LOW to HIGH = start of setupPulses(). Transistion HIGH to LOW = end of setupPulses().
#  PORTH:6 --> LOW->HIGH = start of mixer calcs. HIGH->LOW, end of mixer calcs.
# Values = YES, NO
DEBUG = NO

# Flight Phases
# Values = YES, NO
FLIGHT_PHASES = YES

# Curves
# Values = YES, NO
CURVES = YES

# Customizable Curves
# Values = YES, NO
XCURVES = YES

# Global Vars
# Values = YES, NO
GVARS = NO

# Enable another 3 positions switch to replace a pot
# Values = NO, POT1, POT2, POT3,
3POS = NO

#------- END BUILD OPTIONS ---------------------------

# Define programs and commands.
SHELL = sh
IMG2LBM = -python ../util/img2lbm.py
SVNREV = $(shell sh -c "svnversion | egrep -o '[[:digit:]]+[[:alpha:]]*$$'")

CPPDEFS = 

# MCU name
ifeq ($(PCB), $(filter $(PCB), STD 9X 9XR))
  ARCH = AVR
  TRGT = avr-
  MCU = atmega64  
  CPPDEFS += -DF_CPU=$(F_CPU)UL
  XCURVES = NO
  SDCARD = NO
endif

ifeq ($(PCB), $(filter $(PCB), STD128 9X128 9XR128))
  ARCH = AVR
  TRGT = avr-
  MCU = atmega128
  CPPDEFS += -DF_CPU=$(F_CPU)UL
  SDCARD = NO
endif

ifeq ($(PCB), GRUVIN9X)
  ARCH = AVR
  ifeq ($(PCBREV), REV0)
    CPPDEFS += -DREV0
  else
    CPPDEFS += -DREV1
  endif
  TRGT = avr-
  MCU = atmega2560
  CPPDEFS += -DF_CPU=$(F_CPU)UL
  SDCARD = NO
endif

ifeq ($(PCB), SKY9X)
  ARCH = ARM
  ifeq ($(PCBREV), REVA)
    CPPDEFS += -DREVA
    LDSCRIPT = sky9x/sam3s2c_flash.ld
  else ifeq ($(PCBREV), REVB)
    CPPDEFS += -DREVB
    LDSCRIPT = sky9x/sam3s4c_flash.ld
  else
    CPPDEFS += -DREVC
    LDSCRIPT = sky9x/sam3s4c_flash.ld
  endif
  TRGT = arm-none-eabi-
  MCU  = cortex-m3
  CPPDEFS += -Dat91sam3s4 
  SDCARD = YES
endif

ifeq ($(PCB), X9D)
  ARCH = ARM
  ifeq ($(PCBREV), REV3)
    CPPDEFS += -DREV3
  else
    CPPDEFS += -DREV4
  endif
  LDSCRIPT = x9d/stm32_flash.ld
  TRGT = arm-none-eabi-
  MCU  = cortex-m3
  CPPDEFS += -DHSE_VALUE=12000000
  SDCARD = YES
  EXT = FRSKY
endif

CC      = $(TRGT)gcc
AS      = $(TRGT)as
OBJCOPY = $(TRGT)objcopy
OBJDUMP = $(TRGT)objdump
SIZE    = $(TRGT)size
NM      = $(TRGT)nm

REMOVE = rm -f
REMOVEDIR = rm -rf

SLEEP = sleep

# Processor frequency.
F_CPU = 16000000

# Output format. (can be srec, ihex, binary)
FORMAT = ihex

# Target file name (without extension).
TARGET = open9x

# List C++ source files here. (C dependencies are automatically generated.)
SRC =
STD_TTS_SRC = translations/tts_$(shell sh -c "echo $(TTS) | tr '[:upper:]' '[:lower:]'").cpp
TTS_SRC = $(shell sh -c "if test -f $(STD_TTS_SRC); then echo $(STD_TTS_SRC); else echo translations/tts_en.cpp; fi")

CPPSRC = open9x.cpp $(PULSESSRC) stamp.cpp menus.cpp model_menus.cpp general_menus.cpp main_views.cpp statistics_views.cpp $(EEPROMSRC) lcd.cpp keys.cpp translations.cpp $(TTS_SRC)

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

CPPDEFS += -DTRANSLATIONS_$(TRANSLATIONS)

# TTS language
CPPDEFS += -DTTS_$(TTS)

GVARS_VARIANT  = +1 
FRSKY_VARIANT  = +2
3POS_VARIANT   = +4
EEPROM_VARIANT = 0

ifeq ($(UNITS), IMPERIAL)
  CPPDEFS += -DIMPERIAL_UNITS
endif

# If POTS are used for fields modification
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
ifeq ($(NAVIGATION), ROTENC)
  CPPDEFS += -DROTARY_ENCODER_NAVIGATION
endif

ifeq ($(AUTOSWITCH), YES)
  CPPDEFS += -DAUTOSWITCH
endif

ifeq ($(DBLKEYS), YES)
  ifeq ($(PCB), $(filter $(PCB), 9XR 9XR128))
    $(warning Sorry, DBLKEYS doesnt work on 9XR)
  else
    CPPDEFS += -DDBLKEYS
  endif
endif

ifeq ($(SPLASH), YES)
  CPPDEFS += -DSPLASH
endif

ifeq ($(SPLASH), FIXED)
  CPPDEFS += -DSPLASH -DFSPLASH
endif

ifeq ($(GRAPHICS), YES)
  CPPDEFS += -DGRAPHICS
endif

ifeq ($(BOLD), YES)
  CPPDEFS += -DBOLD_FONT
endif

ifneq ($(PCB), X9D)
  ifeq ($(BATTGRAPH), YES)
    CPPDEFS += -DBATTGRAPH
  endif
endif
  
ifeq ($(THR_TRACE), YES)
  CPPDEFS += -DTHRTRACE
endif

ifeq ($(FLIGHT_PHASES), YES)
  CPPDEFS += -DFLIGHT_PHASES
endif

ifeq ($(CURVES), YES)
  CPPDEFS += -DCURVES
endif

ifeq ($(XCURVES), YES)
  CPPDEFS += -DXCURVES
endif

ifeq ($(GVARS), YES)
  CPPDEFS += -DGVARS
  EEPROM_VARIANT += ${GVARS_VARIANT}
endif

ifeq ($(CORRECT_NEGATIVE_VALUES), YES)
  CPPDEFS += -DCORRECT_NEGATIVE_SHIFTS
endif

ifeq ($(ARITHMETIC_OVERFLOW_CHECK), YES)
  CPPDEFS += -DPREVENT_ARITHMETIC_OVERFLOW
endif

# If STD-Support is enabled
ifeq ($(EXT), STD)
  CPPDEFS += -DEXTSTD
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
  CPPDEFS += -DJETI
endif

ifeq ($(EXT), MAVLINK)
 CPPDEFS += -DMAVLINK
 CPPSRC += mavlink.cpp rotarysw.cpp serial.cpp
endif

ifeq ($(EXT), TELEMETREZ)
  CPPDEFS += -DTELEMETREZ
endif

# If FRSKY-Support is enabled
ifeq ($(EXT), $(filter $(EXT), FRSKY TELEMETREZ))
  CPPDEFS += -DFRSKY
  CPPSRC += frsky.cpp
  # If FRSKY-Hub is enabled
  ifeq ($(FRSKY_HUB), YES)
    CPPDEFS += -DFRSKY_HUB
  endif
  # If WS HowHigh is enabled
  ifeq ($(WS_HOW_HIGH), YES)
    CPPDEFS += -DWS_HOW_HIGH
  endif
  # If Vario is enabled
  ifeq ($(VARIO), YES)
    CPPDEFS += -DVARIO
  endif
  EEPROM_VARIANT += ${FRSKY_VARIANT}
endif

ifeq ($(DEBUG), YES)
  CPPDEFS += -DDEBUG
endif

ifeq ($(EEPROM_PROGRESS_BAR), YES)
  CPPDEFS += -DEEPROM_PROGRESS_BAR
endif

ifeq ($(SDCARD), YES)
  CPPDEFS += -DSDCARD
  CPPSRC += logs.cpp 
endif

RUN_FROM_FLASH = 1

ifeq ($(PCB), X9D)
  # X9D radio
  OPT = 2
  CPPDEFS += -DPCBX9D -DCPUARM -DEEPROM_VARIANT=0 -DAUDIO -DPXX
  EXTRAINCDIRS += CoOS/kernel CoOS/portable
  EXTRAINCDIRS += x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include
  EXTRAINCDIRS += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/inc x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/inc 
  EXTRAINCDIRS += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/inc x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/hid/inc 
  BOARDSRC = x9d/board_x9d.cpp
  EXTRABOARDSRC += x9d/lcd_driver.cpp x9d/aspi.cpp x9d/i2c.c x9d/pwr_driver.cpp x9d/pulses_driver.cpp x9d/keys_driver.cpp x9d/adc_driver.cpp x9d/trainer_driver.cpp x9d/audio_driver.cpp x9d/delays.cpp x9d/uart_driver.cpp x9d/sport_driver.cpp
  SRC += CoOS/kernel/core.c CoOS/kernel/hook.c CoOS/kernel/task.c CoOS/kernel/event.c CoOS/kernel/time.c CoOS/kernel/timer.c CoOS/kernel/flag.c CoOS/kernel/mutex.c CoOS/kernel/serviceReq.c CoOS/portable/GCC/port.c CoOS/portable/arch.c
  EEPROMSRC = eeprom_avr.cpp # TODO later eeprom_conversions.cpp
  PULSESSRC = pulses_arm.cpp
  CPPSRC += audio_arm.cpp
  CPPSRC += bmp.cpp monitors_views.cpp 
  SRC += x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Source/Templates/system_stm32f2xx.c
  SRC += x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/misc.c
  SRC += x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_gpio.c
  SRC += x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rcc.c
  SRC += x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_spi.c
  # SRC += x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_dma.c
  SRC += x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rtc.c
  SRC += x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_pwr.c
  SRC += x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_usart.c
  SRC += x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Source/Templates/gcc_ride7/startup_stm32f2xx.s
  SRC += x9d/usb_bsp.c x9d/usbd_desc.c x9d/usbd_usr.c x9d/usbd_storage_msd.c
  SRC += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/src/usb_core.c
  SRC += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/src/usb_dcd.c
  SRC += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/src/usb_dcd_int.c
  SRC += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_core.c
  SRC += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_ioreq.c
  SRC += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_req.c
  SRC += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_data.c
  SRC += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_scsi.c
  SRC += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_bot.c
  SRC += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_core.c
  # SRC += x9d/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/hid/src/usbd_hid_core.c
  
  ifeq ($(SDCARD), YES)
    EXTRABOARDSRC += FatFs/ff.c FatFs/fattime.c FatFs/option/ccsbcs.c x9d/diskio.cpp
    CPPDEFS += -DVOICE
    EXTRAINCDIRS += FatFs FatFs/option
  endif
    
  ifeq ($(RTCLOCK), YES)
    CPPDEFS += -DRTCLOCK
    CPPSRC += rtc.cpp x9d/rtc_driver.cpp
  endif

  ifeq ($(DEBUG), YES)
    CPPSRC += debug.cpp
  	SRC += sky9x/syscalls.c
  endif
endif

ifeq ($(PCB), SKY9X)
  # 9x radio with a sky9x replacement board
  OPT = 2
  CPPDEFS += -DPCBSKY9X -DCPUARM -DEEPROM_VARIANT=0 -DAUDIO -DHAPTIC -DPXX -DDSM2 -DDSM2_PPM -DROTARY_ENCODERS=1
  EXTRAINCDIRS += sky9x CoOS/kernel CoOS/portable
  BOARDSRC = sky9x/board_sky9x.cpp
  EXTRABOARDSRC += sky9x/lcd_driver.cpp
  SRC += sky9x/core_cm3.c sky9x/board_lowlevel.c sky9x/crt.c sky9x/vectors_sam3s.c
  SRC += CoOS/kernel/core.c CoOS/kernel/hook.c CoOS/kernel/task.c CoOS/kernel/event.c CoOS/kernel/time.c CoOS/kernel/timer.c CoOS/kernel/flag.c CoOS/kernel/mutex.c CoOS/kernel/serviceReq.c CoOS/portable/GCC/port.c CoOS/portable/arch.c
  SRC += sky9x/usb/device/core/USBD_UDP.c sky9x/usb/device/core/USBDDriver.c
  SRC += sky9x/usb/device/massstorage/MSDDriver.c sky9x/usb/device/massstorage/MSDDStateMachine.c sky9x/usb/device/massstorage/MSDLun.c sky9x/usb/device/massstorage/MSDDriverDescriptors.c sky9x/usb/device/massstorage/SBCMethods.c  
  SRC += sky9x/usb/common/core/USBEndpointDescriptor.c sky9x/usb/common/core/USBGenericRequest.c sky9x/usb/common/core/USBFeatureRequest.c sky9x/usb/common/core/USBInterfaceRequest.c sky9x/usb/common/core/USBGetDescriptorRequest.c sky9x/usb/common/core/USBSetAddressRequest.c sky9x/usb/common/core/USBSetConfigurationRequest.c sky9x/usb/common/core/USBConfigurationDescriptor.c sky9x/usb/common/core/USBGenericDescriptor.c 
  SRC += sky9x/MEDSdcard.c
  EEPROMSRC = eeprom_arm.cpp eeprom_conversions.cpp
  PULSESSRC = pulses_arm.cpp
  CPPSRC += audio_arm.cpp haptic.cpp
  CPPSRC += sky9x/pwr_driver.cpp sky9x/adc_driver.cpp sky9x/coproc_driver.cpp sky9x/rotenc_driver.cpp sky9x/eeprom_driver.cpp sky9x/pulses_driver.cpp sky9x/keys_driver.cpp sky9x/audio_driver.cpp sky9x/buzzer_driver.cpp sky9x/haptic_driver.cpp sky9x/sdcard_driver.cpp sky9x/massstorage.cpp
  
  ifeq ($(SDCARD), YES)
    EXTRAINCDIRS += FatFs FatFs/option
    EXTRABOARDSRC += FatFs/ff.c FatFs/fattime.c FatFs/option/ccsbcs.c sky9x/diskio.cpp
    CPPDEFS += -DVOICE
  endif
  
  ifeq ($(RTCLOCK), YES)
    CPPDEFS += -DRTCLOCK
    CPPSRC += rtc.cpp sky9x/rtc_driver.cpp
  endif
  
  ifeq ($(BLUETOOTH), YES)
  	CPPDEFS += -DBLUETOOTH
  	CPPSRC += sky9x/bluetooth.cpp 
  endif
  
  ifeq ($(DEBUG), YES)
    CPPSRC += debug.cpp
  	CPPSRC += sky9x/debug_driver.cpp
  	SRC += sky9x/syscalls.c
  endif
endif

ifeq ($(PCB), GRUVIN9X)
  # 9x radio with a gruvin9x replacement board
  OPT = 2
  CPPDEFS += -DPCBGRUVIN9X -DEEPROM_VARIANT=0 -DAUDIO -DHAPTIC
  EXTRAINCDIRS += gruvin9x stock FatFs FatFs/option
  BOARDSRC += gruvin9x/board_gruvin9x.cpp
  EXTRABOARDSRC += stock/lcd_driver.cpp
  EEPROMSRC = eeprom_avr.cpp
  PULSESSRC = pulses_avr.cpp  
  CPPSRC += audio_avr.cpp haptic.cpp

  ifeq ($(SDCARD), YES)
    CPPDEFS += -DRTCLOCK
    CPPSRC += rtc.cpp gruvin9x/rtc_driver.cpp 
    EXTRABOARDSRC += FatFs/ff.c FatFs/fattime.c FatFs/option/ccsbcs.c gruvin9x/diskio.cpp
  endif
    
  ifeq ($(VOICE), YES)
    CPPSRC += gruvin9x/somo14d.cpp
    CPPDEFS += -DVOICE
  endif
  
  # Enable extra rotary encoders (Gruvin9x board only)
  # Values = YES, NO
  ROTARY_ENCODERS = 2
  CPPDEFS += -DROTARY_ENCODERS=${ROTARY_ENCODERS}
endif

ifeq ($(PCB), $(filter $(PCB), STD128 9X128 9XR128))
  # 9x radio with Atmega 64 replace by Atmega 128
  OPT = 2
  CPPDEFS += -DPCBSTD -DCPUM128 -DEEPROM_VARIANT=0x8000
  EXTRAINCDIRS += stock
  BOARDSRC = stock/board_stock.cpp
  EXTRABOARDSRC += stock/lcd_driver.cpp
  EEPROMSRC = eeprom_avr.cpp
  PULSESSRC = pulses_avr.cpp

  ifeq ($(PCB), 9XR128)
    CPPDEFS += -DPCB9XR128
  else
    CPPDEFS += -DPCB9X128
  endif
    
  ifeq ($(AUDIO), YES)
    CPPDEFS += -DAUDIO
    CPPSRC += audio_avr.cpp
  else
    CPPSRC += beeper.cpp 
  endif
  
  ifeq ($(VOICE), YES)
    CPPSRC += stock/voice.cpp
    CPPDEFS += -DVOICE
  endif
  
  ifeq ($(PWM_BACKLIGHT), YES)
    ifeq ($(VOICE), YES)
      $(error Sorry, PWM_BACKLIGHT doesnt work on stock board with VOICE mod)
    else
      CPPDEFS += -DPWM_BACKLIGHT
    endif
  endif

  ifeq ($(HAPTIC), YES)
   CPPDEFS += -DHAPTIC
   CPPSRC += haptic.cpp
  endif
  
  ifeq ($(SP22), YES)
   CPPDEFS += -DSP22
  endif
endif

ifeq ($(PCB), $(filter $(PCB), STD 9X 9XR))
  # 9x radio
  OPT = s
  EXTRAINCDIRS += stock
  CPPDEFS += -DPCBSTD -DCPUM64 -DEEPROM_VARIANT=$(shell echo ${EEPROM_VARIANT} | bc)
  BOARDSRC = stock/board_stock.cpp
  EXTRABOARDSRC += stock/lcd_driver.cpp
  EEPROMSRC = eeprom_avr.cpp
  PULSESSRC = pulses_avr.cpp

  ifeq ($(PCB), 9XR)
    CPPDEFS += -DPCB9XR
  else
    CPPDEFS += -DPCB9X
  endif
    
  ifeq ($(AUDIO), YES)
    CPPDEFS += -DAUDIO
    CPPSRC += audio_avr.cpp
  else
    CPPSRC += beeper.cpp 
  endif
     
  ifeq ($(VOICE), YES)
    CPPSRC += stock/voice.cpp
    CPPDEFS += -DVOICE
  endif
   
  ifeq ($(PWM_BACKLIGHT), YES)
    ifeq ($(VOICE), YES)
      $(error Sorry, PWM_BACKLIGHT doesnt work on stock board with VOICE mod)
    else
      CPPDEFS += -DPWM_BACKLIGHT
    endif
  endif

  ifeq ($(HAPTIC), YES)
   CPPDEFS += -DHAPTIC
   CPPSRC += haptic.cpp
  endif
  
  ifeq ($(SP22), YES)
   CPPDEFS += -DSP22
  endif
endif

### Global Build-Option Directives ###

ifeq ($(PPM_CENTER_ADJUSTABLE), YES)
  CPPDEFS += -DPPM_CENTER_ADJUSTABLE
endif

ifeq ($(PPM_LIMITS_SYMETRICAL), YES)
  CPPDEFS += -DPPM_LIMITS_SYMETRICAL
endif

ifeq ($(PPM_UNIT), US)
  CPPDEFS += -DPPM_UNIT_US
endif

ifeq ($(PPM_UNIT), PERCENT_PREC1)
  CPPDEFS += -DPPM_UNIT_PERCENT_PREC1
endif

ifeq ($(PPM_UNIT), PERCENT_PREC0)
  CPPDEFS += -DPPM_UNIT_PERCENT_PREC0
endif

ifeq ($(HELI), YES)
  CPPDEFS += -DHELI
endif

ifeq ($(TEMPLATES), YES)
  CPPDEFS += -DTEMPLATES
  CPPSRC += templates.cpp
endif

ifeq ($(PXX), YES)
  CPPDEFS += -DPXX
endif

ifeq ($(DSM2), SERIAL)
  CPPDEFS += -DDSM2 -DDSM2_SERIAL
endif

ifeq ($(DSM2), PPM)
  CPPDEFS += -DDSM2 -DDSM2_PPM
endif

ifeq ($(TX_CADDY), YES)
  CPPDEFS += -DTX_CADDY
endif

ifeq ($(IRPROTOS), YES)
  CPPDEFS += -DIRPROTOS
endif

ifeq ($(3POS), POT1)
  CPPDEFS += -DEXTRA_3POS=1
  # EEPROM_VARIANT += ${3POS_VARIANT}
endif
ifeq ($(3POS), POT2)
  CPPDEFS += -DEXTRA_3POS=2
  # EEPROM_VARIANT += ${3POS_VARIANT}
endif
ifeq ($(3POS), POT3)
  CPPDEFS += -DEXTRA_3POS=3
  # EEPROM_VARIANT += ${3POS_VARIANT}
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
CPPFLAGS += -Wall
CPPFLAGS += -fno-exceptions
CPPFLAGS += -Wno-strict-aliasing
CPPFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

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
LDFLAGS += $(MATH_LIB)
#LDFLAGS += -T linker_script.x

# Define Messages
# English
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_COMPILING = Compiling C++:
MSG_CLEANING = Cleaning project:

# Combine all necessary flags and optional flags.
# Add target processor to flags.

ARMCPPFLAGS = -c -mcpu=$(MCU) -mthumb -fomit-frame-pointer -fverbose-asm -Wa,-ahlms=open9x.lst -DRUN_FROM_FLASH=1 -O$(OPT) $(CPPFLAGS) $(GENDEPFLAGS)
AVRCPPFLAGS = -mmcu=$(MCU) -I. -x c++ -O$(OPT) $(CPPFLAGS) $(GENDEPFLAGS) -fwhole-program

ifeq ($(ARCH), AVR)
  GCCVERSIONGTE462 := $(shell expr 4.6.2 \<= `$(CC) -dumpversion`)
  ifeq ($(GCCVERSIONGTE462),1)
    CPPFLAGS += -flto
  endif

  AVRCPPFLAGS += -fno-inline-small-functions

  GCCVERSIONGTE470 := $(shell expr 4.7.0 \<= `$(CC) -dumpversion`)
  ifeq ($(GCCVERSIONGTE470),1)
    AVRCPPFLAGS += -mstrict-X
  endif
endif

MAJ_VER = ${shell sh -c "grep \"MAJ_VERS\" open9x.h | cut -d\  -f3 | egrep -o \"[[:digit:]]\""}
MIN_VER = ${shell sh -c "grep \"MIN_VERS\" open9x.h | cut -d\  -f3"}

# Default target.
all: begin gccversion sizebefore build sizeafter end

# Change the build target to build a HEX file or a library.
ifeq ($(ARCH), AVR)
  build: stamp_header lbm allsrc.cpp elf remallsrc hex eep lss sym
else
  build: stamp_header lbm allsrc.cpp elf remallsrc bin lss
endif

elf: $(TARGET).elf
bin: $(TARGET).bin
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym

lbm: font.lbm font_tiny.lbm font_small.lbm font_midsize.lbm font_dblsize.lbm sticks.lbm splash_9x.lbm splash_X9D.lbm x9d_logo.lbm icons.lbm asterisk.lbm

# Build stamp-file
stamp_header:
	@echo
	@echo "Generate Version-stamp:"
	@echo "//Automatically generated file (Makefile) - do not edit" > stamp-open9x.h
	@echo "#define DATE_STR \"`date +%Y-%m-%d`\"" >> stamp-open9x.h
	@echo "#define TIME_STR \"`date +%H:%M:%S`\"" >> stamp-open9x.h
	@echo "#define SVN_STR  \"open9x-r$(SVNREV)\"" >> stamp-open9x.h
	@cat stamp-open9x.h

stock-stamp:
	@echo "#define DATE_STR \"`date +%Y-%m-%d`\"" > stamp-open9x-stock.txt
	@echo "#define TIME_STR \"`date +%H:%M:%S`\"" >> stamp-open9x-stock.txt
	@echo "#define SVN_VERS \"open9x-r$(SVNREV)\"" >> stamp-open9x-stock.txt
	@cat stamp-open9x-stock.txt

stock128-stamp:
	@echo "#define DATE_STR \"`date +%Y-%m-%d`\"" > stamp-open9x-stock128.txt
	@echo "#define TIME_STR \"`date +%H:%M:%S`\"" >> stamp-open9x-stock128.txt
	@echo "#define SVN_VERS \"open9x-r$(SVNREV)\"" >> stamp-open9x-stock128.txt
	@cat stamp-open9x-stock128.txt

v4-stamp:
	@echo "#define DATE_STR \"`date +%Y-%m-%d`\"" > stamp-open9x-v4.txt
	@echo "#define TIME_STR \"`date +%H:%M:%S`\"" >> stamp-open9x-v4.txt
	@echo "#define SVN_VERS \"open9x-r$(SVNREV)\"" >> stamp-open9x-v4.txt
	@cat stamp-open9x-v4.txt

arm-stamp:
	@echo "#define DATE_STR \"`date +%Y-%m-%d`\"" > stamp-open9x-arm.txt
	@echo "#define TIME_STR \"`date +%H:%M:%S`\"" >> stamp-open9x-arm.txt
	@echo "#define SVN_VERS \"open9x-r$(SVNREV)\"" >> stamp-open9x-arm.txt
	@cat stamp-open9x-arm.txt

font.lbm: font.xbm font_extra.xbm translations/font_se.xbm translations/font_de.xbm translations/font_it.xbm translations/font_cz.xbm translations/font_fr.xbm translations/font_es.xbm
	@echo
	@echo "Convert font from xbm to lbm:"
	$(IMG2LBM) font.xbm font.lbm 5x7
	$(IMG2LBM) font_extra.xbm font_extra.lbm 5x7
	$(IMG2LBM) translations/font_se.xbm translations/font_se.lbm 5x7
	$(IMG2LBM) translations/font_de.xbm translations/font_de.lbm 5x7
	$(IMG2LBM) translations/font_it.xbm translations/font_it.lbm 5x7
	$(IMG2LBM) translations/font_cz.xbm translations/font_cz.lbm 5x7
	$(IMG2LBM) translations/font_fr.xbm translations/font_fr.lbm 5x7
	$(IMG2LBM) translations/font_es.xbm translations/font_es.lbm 5x7

font_tiny.lbm: font_tiny.xbm
	@echo
	@echo "Convert font from xbm to lbm:"
	$(IMG2LBM) $< $@ 3x5

font_small.lbm: font_small.xbm
	@echo
	@echo "Convert font from xbm to lbm:"
	$(IMG2LBM) $< $@ 4x6

font_midsize.lbm: font_midsize.xbm
	@echo
	@echo "Convert font from xbm to lbm:"
	$(IMG2LBM) $< $@ 8x10
	
font_dblsize.lbm: font_dblsize.xbm font_dblsize_extra.xbm translations/font_dblsize_se.xbm translations/font_dblsize_de.xbm translations/font_dblsize_it.xbm translations/font_dblsize_fr.xbm translations/font_dblsize_es.xbm
	@echo
	@echo "Convert font from xbm to lbm:"
	$(IMG2LBM) font_dblsize.xbm font_dblsize.lbm 10x14
	$(IMG2LBM) font_dblsize_extra.xbm font_dblsize_extra.lbm 10x14
	$(IMG2LBM) translations/font_dblsize_se.xbm translations/font_dblsize_se.lbm 10x14
	$(IMG2LBM) translations/font_dblsize_de.xbm translations/font_dblsize_de.lbm 10x14
	$(IMG2LBM) translations/font_dblsize_it.xbm translations/font_dblsize_it.lbm 10x14
	$(IMG2LBM) translations/font_dblsize_fr.xbm translations/font_dblsize_fr.lbm 10x14
	$(IMG2LBM) translations/font_dblsize_es.xbm translations/font_dblsize_es.lbm 10x14

sticks.lbm: sticks_4x1.xbm
	@echo
	@echo "Convert sticks from xbm to lbm:"
	$(IMG2LBM) $< $@ img 4

splash_9x.lbm: splash_9x.xbm
	@echo
	@echo "Convert 9x splash from xbm to lbm:"
	$(IMG2LBM) $< $@ img
	
splash_X9D.lbm: splash_X9D.bmp
	@echo
	@echo "Convert X9D splash from bmp to lbm:"
	$(IMG2LBM) $< $@ bmp
	
x9d_logo.lbm: x9d_logo.bmp
	@echo
	@echo "Convert X9D logo from bmp to lbm:"
	$(IMG2LBM) $< $@ bmp
	
icons.lbm: icons.png	
	@echo
	@echo "Convert icons from png to lbm:"
	$(IMG2LBM) $< $@ bmp

asterisk.lbm: asterisk.xbm
	@echo
	@echo "Convert asterisk from xbm to lbm:"
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
ifeq ($(ARCH), AVR)
  MCU_FLAG = $(shell $(SIZE) --help | grep -- --mcu > /dev/null && echo --mcu=$(MCU) )
  FORMAT_FLAG = $(shell $(SIZE) --help | grep -- --format=.*avr > /dev/null && echo --format=avr )
  ELFSIZE = $(SIZE) $(MCU_FLAG) $(FORMAT_FLAG) $(TARGET).elf
else
  ELFSIZE = $(SIZE) $(TARGET).elf
endif

sizebefore:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); fi

sizeafter:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); fi

# Display compiler version information.
gccversion :
	@$(CC) --version

# gruvin: added extra include and lib paths to get simu working on my Mac
FOXINC=-I/usr/local/include/fox-1.6 -I/usr/include/fox-1.6 \
       -I$(FOXPATH)/include \
       -I/opt/local/include/fox-1.6
FOXLIB=-L/usr/local/lib \
       -L$(FOXPATH)/src/.libs \
       -L/opt/local/lib \
       -lFOX-1.6 \
       -Wl,-rpath,$(FOXPATH)/src/.libs

simu: stamp_header $(BOARDSRC) $(CPPSRC) Makefile simu.cpp simpgmspace.cpp *.h *.lbm eeprom.bin
	g++ $(CPPFLAGS) simu.cpp $(BOARDSRC) $(CPPSRC) simpgmspace.cpp -MD -DSIMU -O0 -o simu $(FOXINC) $(FOXLIB) -pthread

eeprom.bin:
	dd if=/dev/zero of=$@ bs=1 count=2048


# Create final output files (.hex, .eep) from ELF output file.
ifeq ($(ARCH), AVR)
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@
else
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	 $(OBJCOPY) -O ihex open9x.elf open9x.hex
endif

%.bin: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O binary  $< $@

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

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S $< > $@

# Concatenate all sources files in one big file to optimize size
allsrc.cpp: Makefile $(BOARDSRC) $(CPPSRC) $(EXTRABOARDSRC)
	@rm -f allsrc.cpp 
	for f in $(BOARDSRC) $(CPPSRC) $(EXTRABOARDSRC) ; do echo "# 1 \"$$f\"" >> allsrc.cpp; cat "$$f" >> allsrc.cpp; done

remallsrc:
	$(REMOVE) allsrc.cpp

# Link: create ELF output file from object files.
ifeq ($(ARCH), AVR)
%.elf: allsrc.cpp
	@echo
	@echo $(MSG_COMPILING) $@
	$(CC) $(AVRCPPFLAGS) $< --output $@ $(LDFLAGS)
else
%.o: %.c
	$(CC) $(ARMCPPFLAGS) $< -o $@
%.o: %.s
	$(AS) $< -o $@
TMP = $(SRC:.c=.o)
OBJS =  $(TMP:.s=.o)
%.elf: allsrc.cpp $(OBJS)
	@echo
	@echo $(MSG_COMPILING) $@
	$(CC) $(ARMCPPFLAGS) $< -o allsrc.o
	$(CC) $(OBJS) allsrc.o -mcpu=cortex-m3 -mthumb -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(TARGET).map,--cref,--no-warn-mismatch -o $@
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
	$(REMOVE) $(TARGET).bin
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).eep
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(TARGET).lst
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) *.o
	$(REMOVE) */*.o
	$(REMOVE) */*/*.o
	$(REMOVE) */*/*/*.o
	$(REMOVE) */*/*/*/*.o
	$(REMOVE) */*/*/*/*/*.o
	$(REMOVE) *.d
	$(REMOVE) *.lst
	$(REMOVE) allsrc.cpp
	$(REMOVE) stamp-open9x.h

#### Install

DATE = ${shell sh -c "date +%F:%T" }
AVRCOMMAND = $(AVRDUDE) $(AVRDEVICE)

WRITE_COMMAND = $(AVRCOMMAND) -U flash:w:open9x.hex:i
ifeq ($(RESTORE_EEPROM), YES)
   BACKUP_EEPROM = YES
   ifeq ($(DOUBLE_WRITE_OK), YES)
      WRITE_COMMAND += -U eeprom:w:$(DATA_DIR)/eeprom-$(DATE).hex:i
   else
     SECOND_WRITE_COMMAND = -U eeprom:w:$(DATA_DIR)/eeprom-$(DATE).hex:i
   endif
endif

ifeq ($(BACKUP_FLASH), YES)
  BACKUP_ARGS += -U eeprom:r:$(DATA_DIR)/eeprom-$(DATE).hex:i $(BACKUPFLASH)
endif
ifeq ($(BACKUP_EEPROM), YES)
  BACKUP_ARGS += -U flash:r:$(DATA_DIR)/open9x-$(DATE).hex:i
endif

install: all
ifdef BACKUP_ARGS
	$(AVRCOMMAND) $(BACKUP_ARGS)
	$(SLEEP) $(DELAY)
endif
	$(WRITE_COMMAND)
ifdef	SECOND_WRITE_COMMAND
	$(SLEEP) $(DELAY)
	$(SECOND_WRITE_COMMAND)
endif

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

gtests: $(BOARDSRC) $(CPPSRC) gtests.cpp simpgmspace.cpp *.h gtest_main.a
	g++ -std=gnu++0x $(CPPFLAGS) $(BOARDSRC) $(CPPSRC) gtests.cpp simpgmspace.cpp -I$(GTEST_DIR) -o gtests -lpthread -MD -DSIMU gtest_main.a 
