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
# Values: 9X, 9X128, 9X2561, 9XR, 9XR128, 9XR2561, GRUVIN9X, MEGA2560, SKY9X, 9XRPRO, TARANIS 
# Old values STD and STD128 are mapped to 9X and 9X128 for compatibility
PCB = 9X

# PCB revision
PCBREV =

# Enable JETI-Telemetry or FrSky Telemetry reception on UART0
# For this option you need to modify your hardware!
# More information at [insertURLhere]
# Values = STD, FRSKY, FRSKY_SPORT, JETI, NMEA, ARDUPILOT, MAVLINK, TELEMETREZ
EXT = STD

# FAI mode
# Values = YES, NO, CHOICE
FAI = NO

# the latests deliveries for Turnigy 9x transmitters (since 2 years now, 2014) do not stop sending if no pulses are generated.
# This fact totally breaks the nice throttle and switch warning concept, because even we prevent sending pulses the
# transmitter module ignores this, and continues to send the last value or 0 at beginning. And 0 means in the middle of throttle -> 50%.
# To cope with this situation, we need a new concept how to handle throttle and switch warnings.
# if you have a 0 value (50% ) for throttle, while throttle warning is active, you should activate this switch
# Values = YES, NO
TURNIGY_TRANSMITTER_FIX = NO

# Enable heli menu
# Values = YES, NO
HELI = NO

# Enable templates menu
# Values = YES, NO
TEMPLATES = YES

# Enable navigation with Pots / Sticks / Rotary encoders
# Values = NO
#          POT1 (only POT1 available for fields modification),
#          POT2 (menus scroll),
#          POT3 (cursor down/up),
#          POTS (POT1, POT2, POT3),
#          STICKS
#          ROTENC (Rotary encoder)
NAVIGATION = NO

# Switches automatic selection
# Values = YES, NO 
AUTOSWITCH = NO

# Stock sticks replaced by FrSky ones
# Values = YES, NO 
FRSKY_STICKS = NO

# Sources automatic selection
# Values = YES, NO 
AUTOSOURCE = NO

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

# increases resolution for throttle statistics and throttle counting, in very rare cases.
# this will be only true if an output channel is used for throttle calculation, and this channel has changed limits, e.g. -90% to +80%
# if you use standard limits for throttle or directly the throttle stick you do not need to bother about this option
# not valid for ARM based transmitters like Taranis, They do not have this rounding issue at all
# Values = YES, NO
ACCURAT_THROTTLE_STATS = NO

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

# FONT TYPE
# Values = STD, SQT5
FONT = STD

# PPM center adjustable
# Values = YES, NO
PPM_CENTER_ADJUSTABLE = NO

# PPM symmetrical limits
# Values = YES, NO
PPM_LIMITS_SYMETRICAL = NO

# PPM display unit
# Values = PERCENT_PREC1, PERCENT_PREC0, US
PPM_UNIT = PERCENT_PREC1

# OverrideCHxx functions enable
# Values = YES, NO
OVERRIDE_CHANNEL_FUNCTION = YES

# FrSky Hub 
# Values = YES, NO
FRSKY_HUB = YES

# FrSky FAS current sensor offset compensation
# Values = YES, NO
FAS_OFFSET = NO

# WS HowHigh 
# Values = YES, NO
WS_HOW_HIGH = YES

# Gauges in Telemetry screen
# Values = YES, NO
GAUGES = YES

# GPS 
# Values = YES, NO
GPS = YES

# Vario 
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

# HUBSAN protocol
# Values = NO, YES
HUBSAN = NO

# DSM/PPM TX caddy PCB (Gruvin)
# For auto-switching switching between DSM2/X and Fr-Sky transmitter modules
# Values = YES, NO
TX_CADDY = NO

#IR protocols (SILV / TRAC09 / PICZ / SWIFT)
IRPROTOS = NO

# TRANSLATIONS
# Values = cz, de, en, fi, fr, it, se, es, pl, pt
TRANSLATIONS = EN

# TTS
# Values = en, fr, hu
TTS = $(TRANSLATIONS)

# UNITS
# Values = imperial, metric 
UNITS = METRIC

# Default Tx Mode
# Values = NO / 1 / 2 / 3 / 4
DEFAULT_MODE = NO

# EEPROM_PROGRESS_BAR
# Values = YES, NO
EEPROM_PROGRESS_BAR = NO

# Activates the DEBUG Mode (all boards)
# On G9X V4 board, when VOICE=NO:
#  PORTH:7 --> Transition LOW to HIGH = start of setupPulses(). Transition HIGH to LOW = end of setupPulses().
#  PORTH:6 --> LOW->HIGH = start of mixer calcs. HIGH->LOW, end of mixer calcs.
# Values = YES, NO
DEBUG = NO

# Activate writing of SPORT telemetry received data to sport.log file
# Values = YES, NO
SPORT_FILE_LOG = NO

# Flight Modes
# Values = YES, NO
FLIGHT_MODES = YES

# Curves
# Values = YES, NO
CURVES = YES

# Global Vars
# Values = YES, NO
GVARS = NO

# Enable another 3 positions switch to replace a pot
# Values = NO, POT1, POT2, POT3,
3POS = NO

# Lua interpreter embedded
# Values = NO, NO_MODEL_SCRIPTS, YES 
LUA = NO

# Menu Lock modes
# Values = NO, READONLY, NOMENUS 
MENUS_LOCK = NO

# USB options in the main image
# Values = JOYSTICK, MASSSTORAGE
# JOYSTICK: The TX will appear as a joystick
# MASSSTORAGE: The TX will appear as a drive
USB = JOYSTICK

# Channel monitor display behavior
# Values = NO, YES
# YES - hide the fact that channel is inverted in limits tab 
#       and show inverted channel value with opposite sign
# NO - normal behavior, show real channel values
CHANNELS_MONITOR_INV_HIDE = NO

# Use newlib-nano 
# Values = NO, YES
NANO = NO

#------- END BUILD OPTIONS ---------------------------

# Define programs and commands.
SHELL = sh
IMG2LBM = -python ../util/img2lbm.py
BIN2LBM = -python ../util/bin2lbm.py
TRANSLATE = -python ../util/translate.py
VERSION = 2.1.0

SRC =
CPPSRC =
INCDIRS = . translations
EXTRAINCDIRS = 
LUADEP =

# MCU name
ifneq ($(PCB), $(filter $(PCB), STD 9X 9XR STD128 9X128 9XR128 9X2561 9XR2561 GRUVIN9X MEGA2560 SKY9X 9XRPRO TARANIS))
  $(error Sorry, the PCB $(PCB) is not supported yet in OpenTX)
endif

GVARS_VARIANT   = +1 
FRSKY_VARIANT   = +2
3POS_VARIANT    = +4
MAVLINK_VARIANT = +8
M128_VARIANT    = +32768
M2561_VARIANT   = +16384
EEPROM_VARIANT  = 0

ifeq ($(PCB), $(filter $(PCB), STD 9X 9XR))
  # 9x/9xr radio
  ARCH = AVR
  TRGT = avr-
  MCU = atmega64
  OPT = s
  SDCARD = NO
  THR_TRACE = NO
  INCDIRS += targets/stock
  CPPDEFS = -DF_CPU=$(F_CPU)UL -DPCBSTD -DCPUM64 -DEEPROM_VARIANT=$(shell echo ${EEPROM_VARIANT} | bc)
  BOARDSRC = targets/stock/board_stock.cpp
  EXTRABOARDSRC = targets/stock/lcd_driver.cpp targets/common_avr/telemetry_driver.cpp
  EEPROMSRC = eeprom_common.cpp eeprom_rlc.cpp
  LCDSRC = lcd_common.cpp lcd_default.cpp
  PULSESSRC = pulses/pulses_avr.cpp

  ifeq ($(PCB), 9XR)
    CPPDEFS += -DPCB9XR
  else
    CPPDEFS += -DPCB9X
  endif
  
  ifeq ($(TELEMETRY_MOD), 14051)
    CPPDEFS += -DTELEMETRY_MOD_14051
  endif
  
  ifeq ($(AUDIO), YES)
    CPPDEFS += -DAUDIO
    CPPSRC += audio_avr.cpp
  else
    CPPDEFS += -DBUZZER
    CPPSRC += buzzer.cpp 
  endif
  
  ifeq ($(VOICE), YES)
    CPPDEFS += -DVOICE
    CPPSRC += targets/stock/voice.cpp
  endif
   
  ifeq ($(PWM_BACKLIGHT), YES)
    ifeq ($(VOICE), YES)
      $(error Sorry, PWM_BACKLIGHT doesnt work on stock board with VOICE mod)
    endif
    ifeq ($(AUDIO), YES)
      $(error Sorry, PWM_BACKLIGHT doesnt work on stock board with AUDIO mod)
    endif
    CPPDEFS += -DPWM_BACKLIGHT
  endif

  ifeq ($(HAPTIC), YES)
   CPPDEFS += -DHAPTIC
   CPPSRC += haptic.cpp
  endif
  
  ifeq ($(SP22), YES)
   CPPDEFS += -DSP22
  endif
  
  ifeq ($(FAS_OFFSET), YES)
   CPPDEFS += -DFAS_OFFSET
  endif
endif

ifeq ($(PCB), $(filter $(PCB), STD128 9X128 9XR128))
  # 9X/9XR radio with ATmega64 replaced by ATmega128
  ARCH = AVR
  TRGT = avr-
  MCU = atmega128
  OPT = 2
  SDCARD = NO
  THR_TRACE = NO
  EEPROM_VARIANT += ${M128_VARIANT}
  CPPDEFS = -DF_CPU=$(F_CPU)UL -DPCBSTD -DCPUM128 -DEEPROM_VARIANT=$(shell echo ${EEPROM_VARIANT} | bc)
  INCDIRS += targets/stock
  BOARDSRC = targets/stock/board_stock.cpp
  EXTRABOARDSRC = targets/stock/lcd_driver.cpp targets/common_avr/telemetry_driver.cpp
  EEPROMSRC = eeprom_common.cpp eeprom_rlc.cpp
  LCDSRC = lcd_common.cpp lcd_default.cpp
  PULSESSRC = pulses/pulses_avr.cpp
  
  ifeq ($(PCB), 9XR128)
    CPPDEFS += -DPCB9XR128
  else
    CPPDEFS += -DPCB9X128
  endif
  
  ifeq ($(TELEMETRY_MOD), 14051)
    CPPDEFS += -DTELEMETRY_MOD_14051
  endif  
    
  ifeq ($(AUDIO), YES)
    CPPDEFS += -DAUDIO
    CPPSRC += audio_avr.cpp
  else
    CPPDEFS += -DBUZZER
    CPPSRC += buzzer.cpp 
  endif
  
  ifeq ($(VOICE), YES)
    CPPDEFS += -DVOICE
    CPPSRC += targets/stock/voice.cpp
  endif
  
  ifeq ($(PWM_BACKLIGHT), YES)
    ifeq ($(VOICE), YES)
      $(error Sorry, PWM_BACKLIGHT doesnt work on stock board with VOICE mod)
    endif
    ifeq ($(AUDIO), YES)
      $(error Sorry, PWM_BACKLIGHT doesnt work on stock board with AUDIO mod)
    endif
    CPPDEFS += -DPWM_BACKLIGHT
  endif

  ifeq ($(HAPTIC), YES)
   CPPDEFS += -DHAPTIC
   CPPSRC += haptic.cpp
  endif
  
  ifeq ($(SP22), YES)
   CPPDEFS += -DSP22
  endif
endif

ifeq ($(PCB), $(filter $(PCB), 9X2561))
  # 9X/9XR radio with ATmega64 replaced by ATmega2561
  ARCH = AVR
  TRGT = avr-
  MCU = atmega2561
  OPT = 2
  SDCARD = NO
  THR_TRACE = YES
  EEPROM_VARIANT += ${M2561_VARIANT}
  CPPDEFS = -DF_CPU=$(F_CPU)UL -DPCBSTD -DCPUM2561 -DEEPROM_VARIANT=$(shell echo ${EEPROM_VARIANT} | bc)
  INCDIRS += targets/stock
  BOARDSRC = targets/stock/board_stock.cpp
  EXTRABOARDSRC = targets/stock/lcd_driver.cpp targets/common_avr/telemetry_driver.cpp
  EEPROMSRC = eeprom_common.cpp eeprom_rlc.cpp
  LCDSRC = lcd_common.cpp lcd_default.cpp
  PULSESSRC = pulses/pulses_avr.cpp

  ifeq ($(PCB), 9XR2561)
    CPPDEFS += -DPCB9XR2561
  else
    CPPDEFS += -DPCB9X2561
  endif
    
  ifeq ($(AUDIO), YES)
    CPPDEFS += -DAUDIO
    CPPSRC += audio_avr.cpp
  else
    CPPDEFS += -DBUZZER
    CPPSRC += buzzer.cpp 
  endif
  
  ifeq ($(VOICE), YES)
    CPPDEFS += -DVOICE
    CPPSRC += targets/stock/voice.cpp
  endif
  
  ifeq ($(PWM_BACKLIGHT), YES)
    ifeq ($(VOICE), YES)
      $(error Sorry, PWM_BACKLIGHT doesnt work on stock board with VOICE mod)
    endif
    ifeq ($(AUDIO), YES)
      $(error Sorry, PWM_BACKLIGHT doesnt work on stock board with AUDIO mod)
    endif
    CPPDEFS += -DPWM_BACKLIGHT
  endif

  ifeq ($(HAPTIC), YES)
   CPPDEFS += -DHAPTIC
   CPPSRC += haptic.cpp
  endif
  
  ifeq ($(SP22), YES)
   CPPDEFS += -DSP22
  endif
endif

ifeq ($(PCB), GRUVIN9X)
  # 9x radio with a gruvin9x replacement board
  ARCH = AVR
  ifeq ($(PCBREV), REV0)
    CPPDEFS = -DREV0
  else
    CPPDEFS = -DREV1
  endif
  TRGT = avr-
  MCU = atmega2560
  OPT = 2
  CPPDEFS += -DF_CPU=$(F_CPU)UL -DPCBGRUVIN9X -DCPUM2560 -DEEPROM_VARIANT=0 -DAUDIO -DHAPTIC
  SDCARD = NO
  BUZZER = NO
  THR_TRACE = YES
  INCDIRS += targets/gruvin9x targets/stock FatFs FatFs/option
  BOARDSRC = targets/gruvin9x/board_gruvin9x.cpp
  EXTRABOARDSRC = targets/stock/lcd_driver.cpp targets/common_avr/telemetry_driver.cpp
  EEPROMSRC = eeprom_common.cpp eeprom_rlc.cpp
  LCDSRC = lcd_common.cpp lcd_default.cpp
  PULSESSRC = pulses/pulses_avr.cpp  
  CPPSRC += audio_avr.cpp haptic.cpp
  
  ifeq ($(SDCARD), YES)
    CPPDEFS += -DRTCLOCK
    CPPSRC += rtc.cpp targets/gruvin9x/rtc_driver.cpp 
    EXTRABOARDSRC += FatFs/ff.c FatFs/fattime.c FatFs/option/ccsbcs.c targets/gruvin9x/diskio.cpp
  endif
    
  ifeq ($(BUZZER), YES)
    CPPDEFS += -DBUZZER
    CPPSRC += buzzer.cpp
  endif

  ifeq ($(VOICE), YES)
    CPPDEFS += -DVOICE
    CPPSRC += targets/gruvin9x/somo14d.cpp
  endif
  
  # Enable extra rotary encoders (Gruvin9x board only)
  # Values = YES, NO
  ROTARY_ENCODERS = 2
  CPPDEFS += -DROTARY_ENCODERS=${ROTARY_ENCODERS}
endif

ifeq ($(PCB), MEGA2560)
  # ARDUINO2560 DIY RADIO
  ARCH = AVR
  TRGT = avr-
  MCU = atmega2560
  OPT = 2
  CPPDEFS = -DF_CPU=$(F_CPU)UL -DPCBMEGA2560 -DCPUM2560 -DEEPROM_VARIANT=0 -DAUDIO -DRTCLOCK
  SDCARD = YES
  THR_TRACE = YES
  INCDIRS += targets/mega2560 targets/stock FatFs FatFs/option
  BOARDSRC = targets/mega2560/board_mega2560.cpp
  EXTRABOARDSRC = targets/mega2560/lcd_driver_ST7565R.cpp targets/common_avr/telemetry_driver.cpp
  EEPROMSRC = eeprom_common.cpp eeprom_rlc.cpp
  LCDSRC = lcd_common.cpp lcd_default.cpp
  PULSESSRC = pulses/pulses_avr.cpp  
  CPPSRC += audio_avr.cpp

  ifeq ($(SDCARD), YES)
    EXTRABOARDSRC += FatFs/ff.c FatFs/fattime.c FatFs/option/ccsbcs.c targets/gruvin9x/diskio.cpp 
    CPPDEFS += -DRTCLOCK
    CPPSRC += rtc.cpp targets/mega2560/rtc_driver.cpp 
  endif

  # Enable extra rotary encoders (Gruvin9x board only)
  # Values = YES, NO
  ROTARY_ENCODERS = 2
  CPPDEFS += -DROTARY_ENCODERS=${ROTARY_ENCODERS}
endif

ifeq ($(PCB), $(filter $(PCB), SKY9X 9XRPRO))
  # 9x/9XR radio with a sky9x replacement board
  ARCH = ARM
  CPPDEFS = -Dat91sam3s4
  ifeq ($(PCB), 9XRPRO)
    CPPDEFS += -DREVX
    LDSCRIPT = targets/sky9x/sam3s8c_flash.ld
  else ifeq ($(PCBREV), REVA)
    CPPDEFS += -DREVA
    LDSCRIPT = targets/sky9x/sam3s2c_flash.ld
    CPPSRC += targets/sky9x/coproc_driver.cpp 
  else ifeq ($(PCBREV), REVB)
    CPPDEFS += -DREVB -DCOPROCESSOR
    LDSCRIPT = targets/sky9x/sam3s4c_flash.ld
    CPPSRC += targets/sky9x/coproc_driver.cpp
  else
    CPPDEFS += -DREVC -DCOPROCESSOR
    LDSCRIPT = targets/sky9x/sam3s4c_flash.ld
    CPPSRC += targets/sky9x/coproc_driver.cpp
  endif
  TRGT = arm-none-eabi-
  MCU  = cortex-m3
  OPT = 2
  SDCARD = YES
  BUZZER = NO
  THR_TRACE = YES
  CPPDEFS += -DPCBSKY9X -DCPUARM -DEEPROM_VARIANT=0 -DAUDIO -DHAPTIC -DDSM2 -DDSM2_PPM
  ifeq ($(PCB), SKY9X)
    CPPDEFS += -DROTARY_ENCODERS=1
    CPPSRC += targets/sky9x/rotenc_driver.cpp 
  endif
  ifeq ($(SPORT_FILE_LOG), YES)
    CPPDEFS += -DSPORT_FILE_LOG
  endif
  INCDIRS += targets/sky9x CoOS/kernel CoOS/portable
  BOARDSRC = targets/sky9x/board_sky9x.cpp
  EXTRABOARDSRC = targets/sky9x/lcd_driver.cpp
  SRC += targets/sky9x/core_cm3.c targets/sky9x/board_lowlevel.c targets/sky9x/crt.c targets/sky9x/vectors_sam3s.c
  SRC += CoOS/kernel/core.c CoOS/kernel/hook.c CoOS/kernel/task.c CoOS/kernel/event.c CoOS/kernel/time.c CoOS/kernel/timer.c CoOS/kernel/flag.c CoOS/kernel/mutex.c CoOS/kernel/serviceReq.c CoOS/portable/GCC/port.c CoOS/portable/arch.c
  SRC += targets/sky9x/usb/device/core/USBD_UDP.c targets/sky9x/usb/device/core/USBDDriver.c
  SRC += targets/sky9x/usb/device/massstorage/MSDDriver.c targets/sky9x/usb/device/massstorage/MSDDStateMachine.c targets/sky9x/usb/device/massstorage/MSDLun.c targets/sky9x/usb/device/massstorage/MSDDriverDescriptors.c targets/sky9x/usb/device/massstorage/SBCMethods.c  
  SRC += targets/sky9x/usb/common/core/USBEndpointDescriptor.c targets/sky9x/usb/common/core/USBGenericRequest.c targets/sky9x/usb/common/core/USBFeatureRequest.c targets/sky9x/usb/common/core/USBInterfaceRequest.c targets/sky9x/usb/common/core/USBGetDescriptorRequest.c targets/sky9x/usb/common/core/USBSetAddressRequest.c targets/sky9x/usb/common/core/USBSetConfigurationRequest.c targets/sky9x/usb/common/core/USBConfigurationDescriptor.c targets/sky9x/usb/common/core/USBGenericDescriptor.c 
  SRC += targets/sky9x/MEDSdcard.c
  EEPROMSRC = eeprom_common.cpp eeprom_raw.cpp eeprom_conversions.cpp
  LCDSRC = lcd_common.cpp lcd_default.cpp
  PULSESSRC = pulses/pulses_arm.cpp pulses/ppm_arm.cpp pulses/pxx_arm.cpp pulses/dsm2_arm.cpp
  CPPSRC += tasks_arm.cpp audio_arm.cpp haptic.cpp gui/view_about.cpp gui/view_text.cpp telemetry/telemetry.cpp
  CPPSRC += targets/sky9x/telemetry_driver.cpp targets/sky9x/second_serial_driver.cpp targets/sky9x/pwr_driver.cpp targets/sky9x/adc_driver.cpp targets/sky9x/eeprom_driver.cpp targets/sky9x/pulses_driver.cpp targets/sky9x/keys_driver.cpp targets/sky9x/audio_driver.cpp targets/sky9x/buzzer_driver.cpp targets/sky9x/haptic_driver.cpp targets/sky9x/sdcard_driver.cpp targets/sky9x/massstorage.cpp
  CPPSRC += loadboot.cpp
    
  ifeq ($(SDCARD), YES)
    CPPDEFS += -DVOICE
    INCDIRS += FatFs FatFs/option
    EXTRABOARDSRC += FatFs/ff.c FatFs/fattime.c FatFs/option/ccsbcs.c targets/sky9x/diskio.cpp
  endif
  
  ifeq ($(BUZZER), YES)
    CPPDEFS += -DBUZZER
    CPPSRC += buzzer.cpp
  endif

  ifeq ($(RTCLOCK), YES)
    CPPDEFS += -DRTCLOCK
    CPPSRC += rtc.cpp targets/sky9x/rtc_driver.cpp
  endif
  
  ifeq ($(BLUETOOTH), YES)
  	CPPDEFS += -DBLUETOOTH
  	CPPSRC += targets/sky9x/bluetooth.cpp 
  endif
  
  ifeq ($(DEBUG), YES)
    CPPSRC += debug.cpp
  	SRC += targets/sky9x/syscalls.c
  endif
endif

ifeq ($(PCB), TARANIS)
  # TARANIS radio
  ARCH = ARM
  ifeq ($(PCBREV), REV3)
    CPPDEFS = -DREV3
  else ifeq ($(PCBREV), REV4a)
    CPPDEFS = -DREV4a
  else ifeq ($(PCBREV), REVPLUS)
    CPPDEFS = -DREVPLUS
    HAPTIC = YES
  else
    CPPDEFS = -DREV4
  endif
  ifeq ($(SPORT_FILE_LOG), YES)
    DEBUG = YES
    CPPDEFS += -DSPORT_FILE_LOG
  endif  
  LDSCRIPT = targets/taranis/stm32_flash_bl.ld
  TRGT = arm-none-eabi-
  MCU  = cortex-m3
  OPT = 2
  CPPDEFS += -DHSE_VALUE=12000000
  SDCARD = YES
  THR_TRACE = YES
  EXT = FRSKY_SPORT
  AUTOSWITCH = YES
  AUTOSOURCE = YES
  DBLKEYS = YES
  PPM_LIMITS_SYMETRICAL = YES
  PPM_CENTER_ADJUSTABLE = YES
  DSM2 = PPM
  CPPDEFS += -DPCBTARANIS -DCPUARM -DEEPROM_VARIANT=0 -DAUDIO -DPXX
  INCDIRS += targets/taranis CoOS/kernel CoOS/portable
  EXTRAINCDIRS += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/include
  EXTRAINCDIRS += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/inc targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/inc 
  EXTRAINCDIRS += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/inc targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/hid/inc 
  BOARDSRC = targets/taranis/board_taranis.cpp
  EXTRABOARDSRC += targets/taranis/configure_pins.cpp targets/taranis/lcd_driver.cpp targets/taranis/flash_driver.cpp targets/taranis/aspi.c targets/taranis/i2c.c targets/taranis/delays.c 
  SRC += CoOS/kernel/core.c CoOS/kernel/hook.c CoOS/kernel/task.c CoOS/kernel/event.c CoOS/kernel/time.c CoOS/kernel/timer.c CoOS/kernel/flag.c CoOS/kernel/mutex.c CoOS/kernel/serviceReq.c CoOS/portable/GCC/port.c CoOS/portable/arch.c
  SRC += targets/taranis/pwr_driver.c targets/taranis/usb_driver.c
  EEPROMSRC = eeprom_common.cpp eeprom_rlc.cpp eeprom_conversions.cpp
  LCDSRC = lcd_common.cpp lcd_taranis.cpp
  PULSESSRC = pulses/pulses_arm.cpp pulses/ppm_arm.cpp pulses/pxx_arm.cpp
  CPPSRC += tasks_arm.cpp audio_arm.cpp sbus.cpp telemetry/telemetry.cpp
  CPPSRC += targets/taranis/pulses_driver.cpp targets/taranis/keys_driver.cpp targets/taranis/adc_driver.cpp targets/taranis/trainer_driver.cpp targets/taranis/audio_driver.cpp targets/taranis/uart3_driver.cpp targets/taranis/telemetry_driver.cpp
  CPPSRC += bmp.cpp gui/view_channels.cpp gui/view_about.cpp gui/view_text.cpp loadboot.cpp
  SRC += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Source/Templates/system_stm32f2xx.c
  SRC += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/misc.c
  SRC += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_gpio.c
  SRC += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_dbgmcu.c
  SRC += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rcc.c
  SRC += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_spi.c
  SRC += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_dma.c
  SRC += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rtc.c
  SRC += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_pwr.c
  SRC += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_usart.c
  SRC += targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Source/Templates/gcc_ride7/startup_stm32f2xx.s
  SRC += targets/taranis/usb_bsp.c targets/taranis/usbd_desc.c
  SRC += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/src/usb_core.c
  SRC += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/src/usb_dcd.c
  SRC += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/src/usb_dcd_int.c
  SRC += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_core.c
  SRC += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_ioreq.c
  SRC += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_req.c
  EXTRABOARDSRC += targets/taranis/usbd_usr.cpp
  SRC += targets/sky9x/syscalls.c
  ifeq ($(USB), JOYSTICK)
    CPPDEFS += -DUSB_JOYSTICK
    SRC += targets/taranis/usbd_hid_joystick.c
  else
    CPPDEFS += -DUSB_MASS_STORAGE
    SRC += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_data.c
    SRC += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_scsi.c
    SRC += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_bot.c
    SRC += targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_core.c
    EXTRABOARDSRC += targets/taranis/usbd_storage_msd.cpp
  endif
  
  ifeq ($(NANO), YES)
    # use newlib-nano for linking
    NEWLIB_NANO_FLAGS = --specs=nano.specs -u _printf_float
    CPPDEFS += -DNANO
  endif
  
  ifneq ($(LUA), NO)
    ifeq ($(LUA), YES)
      CPPDEFS += -DLUA_MODEL_SCRIPTS
    endif
    CPPDEFS += -DLUA
    INCDIRS += lua/src
    CPPSRC += lua.cpp
    LUASRC = lua/src/lapi.c lua/src/lcode.c lua/src/lctype.c lua/src/ldebug.c lua/src/ldo.c lua/src/ldump.c lua/src/lfunc.c lua/src/lgc.c lua/src/llex.c lua/src/lmem.c \
           lua/src/lobject.c lua/src/lopcodes.c lua/src/lparser.c lua/src/lstate.c lua/src/lstring.c lua/src/ltable.c lua/src/lrotable.c lua/src/ltm.c lua/src/lundump.c lua/src/lvm.c lua/src/lzio.c \
           lua/src/lbaselib.c lua/src/linit.c lua/src/lmathlib.c lua/src/lbitlib.c lua/src/loadlib.c lua/src/lauxlib.c lua/src/ltablib.c lua/src/lcorolib.c
    SRC += $(LUASRC) 
    LUADEP = lua_exports.cpp
  endif           

  ifeq ($(SDCARD), YES)
    EXTRABOARDSRC += FatFs/ff.c FatFs/fattime.c FatFs/option/ccsbcs.c targets/taranis/diskio.cpp
    CPPDEFS += -DVOICE
    INCDIRS += FatFs FatFs/option
  endif
    
  ifeq ($(RTCLOCK), YES)
    CPPDEFS += -DRTCLOCK
    CPPSRC += rtc.cpp targets/taranis/rtc_driver.cpp
  endif

  ifeq ($(DEBUG), YES)
    CPPSRC += debug.cpp
  endif
  
  ifeq ($(DSM2), PPM)
    CPPSRC += pulses/dsm2_arm.cpp
  endif
  
  ifeq ($(HUBSAN), YES)
    CPPDEFS += -DHUBSAN
    CPPSRC += pulses/hubsan.cpp
    CPPSRC += targets/taranis/a7105_driver.cpp
  endif
  
  ifeq ($(HAPTIC), YES)
    CPPDEFS += -DHAPTIC
    CPPSRC += haptic.cpp
    CPPSRC += targets/taranis/haptic_driver.cpp
  endif

  ifeq ($(CHANNELS_MONITOR_INV_HIDE), YES)
    CPPDEFS += -DCHANNELS_MONITOR_INV_HIDE
  endif

endif

CC      = $(TRGT)gcc
CPP     = $(TRGT)cpp
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
TARGET = opentx

# List C++ source files here. (C dependencies are automatically generated.)
ifeq ($(PCB), $(filter $(PCB), SKY9X 9XRPRO TARANIS))
  TTS_SRC = translations/tts_cz.cpp translations/tts_de.cpp translations/tts_en.cpp translations/tts_es.cpp translations/tts_fr.cpp translations/tts_it.cpp translations/tts_pt.cpp translations/tts_sk.cpp translations/tts_se.cpp translations/tts_pl.cpp translations/tts_hu.cpp
else
  STD_TTS_SRC = translations/tts_$(shell sh -c "echo $(TTS) | tr '[:upper:]' '[:lower:]'").cpp
  TTS_SRC = $(shell sh -c "if test -f $(STD_TTS_SRC); then echo $(STD_TTS_SRC); else echo translations/tts_en.cpp; fi")
endif

CPPSRC += opentx.cpp functions.cpp strhelpers.cpp $(PULSESSRC) switches.cpp curves.cpp mixer.cpp stamp.cpp gui/menus.cpp gui/menu_model.cpp gui/menu_general.cpp gui/view_main.cpp gui/view_statistics.cpp $(EEPROMSRC) $(LCDSRC) keys.cpp maths.cpp translations.cpp fonts.cpp $(TTS_SRC)

# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DBGFMT = dwarf-2

CPPDEFS += -DTRANSLATIONS_$(TRANSLATIONS)

# TTS language
CPPDEFS += -DTTS_$(TTS)

ifeq ($(UNITS), IMPERIAL)
  CPPDEFS += -DIMPERIAL_UNITS
endif

ifneq ($(DEFAULT_MODE), NO)
  CPPDEFS += -DDEFAULT_MODE=$(DEFAULT_MODE)
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
ifeq ($(NAVIGATION), STICKS)
  CPPDEFS += -DNAVIGATION_STICKS
endif
ifeq ($(NAVIGATION), ROTENC)
  CPPDEFS += -DROTARY_ENCODER_NAVIGATION
endif

ifeq ($(TURNIGY_TRANSMITTER_FIX), YES)
  ifeq ($(PCB), TARANIS)
    $(warning TURNIGY_TRANSMITTER_FIX is not available on Taranis)
  else
    CPPDEFS += -DMODULE_ALWAYS_SEND_PULSES
  endif
endif

ifeq ($(AUTOSWITCH), YES)
  CPPDEFS += -DAUTOSWITCH
endif

ifeq ($(AUTOSOURCE), YES)
  CPPDEFS += -DAUTOSOURCE
endif

ifeq ($(FRSKY_STICKS), YES)
  CPPDEFS += -DFRSKY_STICKS
endif

ifeq ($(DBLKEYS), YES)
  ifeq ($(PCB), $(filter $(PCB), 9XR 9XR128 9XR2561))
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

ifneq ($(PCB), TARANIS)
  ifeq ($(BATTGRAPH), YES)
    CPPDEFS += -DBATTGRAPH
  endif
endif

ifneq ($(FONT), STD)
  INCDIRS += fonts/$(shell sh -c "echo $(FONT) | tr '[:upper:]' '[:lower:]'")
endif
INCDIRS += fonts/std
  
ifeq ($(THR_TRACE), YES)
  CPPDEFS += -DTHRTRACE
endif

ifeq ($(FLIGHT_MODES), YES)
  CPPDEFS += -DFLIGHT_MODES
endif

ifeq ($(CURVES), YES)
  CPPDEFS += -DCURVES
endif

ifeq ($(GVARS), YES)
  CPPDEFS += -DGVARS
  EEPROM_VARIANT += ${GVARS_VARIANT}
endif

ifeq ($(MENUS_LOCK), READONLY)
  CPPDEFS += -DMENUS_LOCK=1
endif 

ifeq ($(MENUS_LOCK), NOMENUS)
  CPPDEFS += -DMENUS_LOCK=2
endif 

ifeq ($(CORRECT_NEGATIVE_VALUES), YES)
  CPPDEFS += -DCORRECT_NEGATIVE_SHIFTS
endif

ifeq ($(ARITHMETIC_OVERFLOW_CHECK), YES)
  CPPDEFS += -DPREVENT_ARITHMETIC_OVERFLOW
endif

ifeq ($(ACCURAT_THROTTLE_STATS), YES)
  CPPDEFS += -DACCURAT_THROTTLE_TIMER
endif

# EXTENSIONS (for Telemetry protocols)
ifneq ($(EXT), $(filter $(EXT), STD FRSKY FRSKY_SPORT ARDUPILOT NMEA JETI MAVLINK TELEMETREZ))
  $(error Sorry, the EXT $(EXT) is not supported yet in openTx)
endif

ifeq ($(EXT), STD)
  CPPDEFS += -DEXTSTD
endif

ifeq ($(EXT), ARDUPILOT)
  CPPDEFS += -DARDUPILOT
  CPPSRC += telemetry/ardupilot.cpp
endif

ifeq ($(EXT), NMEA)
  CPPDEFS += -DNMEA
  CPPSRC += telemetry/nmea.cpp
endif

ifeq ($(EXT), JETI)
  CPPDEFS += -DJETI
  CPPSRC += telemetry/jeti.cpp
endif

ifeq ($(EXT), MAVLINK)
 CPPDEFS += -DMAVLINK
 CPPSRC += telemetry/mavlink.cpp gui/view_mavlink.cpp serial.cpp
 EEPROM_VARIANT += ${MAVLINK_VARIANT}
endif

ifeq ($(EXT), TELEMETREZ)
  CPPDEFS += -DTELEMETREZ
endif

ifeq ($(EXT), $(filter $(EXT), FRSKY FRSKY_SPORT TELEMETREZ))
  CPPDEFS += -DFRSKY
  ifeq ($(EXT), FRSKY_SPORT)
	CPPSRC += telemetry/frsky.cpp telemetry/frsky_sport.cpp telemetry/frsky_d.cpp
    CPPDEFS += -DFRSKY_SPORT
  else
    CPPSRC += telemetry/frsky.cpp telemetry/frsky_d.cpp
  endif
  CPPSRC += gui/view_telemetry.cpp
  
  ifeq ($(FRSKY_HUB), YES)
    CPPDEFS += -DFRSKY_HUB
  endif
  
  ifeq ($(WS_HOW_HIGH), YES)
    CPPDEFS += -DWS_HOW_HIGH
  endif
  
  ifeq ($(GAUGES), YES)
    CPPDEFS += -DGAUGES
  endif

  ifeq ($(GPS), YES)
    CPPDEFS += -DGPS
  endif

  ifeq ($(VARIO), YES)
    CPPSRC += vario.cpp
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

### Global Build-Option Directives ###

ifeq ($(PPM_CENTER_ADJUSTABLE), YES)
  CPPDEFS += -DPPM_CENTER_ADJUSTABLE
endif

ifeq ($(PPM_LIMITS_SYMETRICAL), YES)
  CPPDEFS += -DPPM_LIMITS_SYMETRICAL
endif

ifeq ($(OVERRIDE_CHANNEL_FUNCTION), YES)
  CPPDEFS += -DOVERRIDE_CHANNEL_FUNCTION
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

ifeq ($(FAI), YES)
  CPPDEFS += -DFAI
endif
ifeq ($(FAI), CHOICE)
  CPPDEFS += -DFAI_CHOICE
endif

ifeq ($(HELI), YES)
  CPPDEFS += -DHELI
endif

ifeq ($(TEMPLATES), YES)
  ifneq ($(PCB), TARANIS) 
    CPPDEFS += -DTEMPLATES
    CPPSRC += templates.cpp
  endif
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
INCFLAGS = $(patsubst %,-I%,$(INCDIRS))
EXTRAINCFLAGS = $(patsubst %,-I%,$(EXTRAINCDIRS))

MATH_LIB = -lm

#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(MATH_LIB)

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

ARMCPPFLAGS = -c -mcpu=$(MCU) -mthumb -fomit-frame-pointer -fverbose-asm -Wa,-ahlms=opentx.lst -DRUN_FROM_FLASH=1 -O$(OPT) $(CPPFLAGS) -fdata-sections -ffunction-sections $(INCFLAGS) $(EXTRAINCFLAGS) $(GENDEPFLAGS) 
AVRCPPFLAGS = -mmcu=$(MCU) -I. -x c++ -O$(OPT) $(CPPFLAGS) $(INCFLAGS) $(EXTRAINCFLAGS) $(GENDEPFLAGS) -fwhole-program

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

MAJ_VER = ${shell sh -c "grep \"MAJ_VERS\" opentx.h | cut -d\  -f3 | egrep -o \"[[:digit:]]\""}
MIN_VER = ${shell sh -c "grep \"MIN_VERS\" opentx.h | cut -d\  -f3"}

# Default target.
all: begin gccversion sizebefore build sizeafter end

# Change the build target to build a HEX file or a library.
ifeq ($(ARCH), AVR)
  build: stamp_header tra lbm allsrc.cpp elf remallsrc hex
else ifeq ($(PCB), $(filter $(PCB), SKY9X 9XRPRO TARANIS))
  build: stamp_header tra lbm bootloader.lbm $(LUADEP) allsrc.cpp elf remallsrc bin
else
  build: stamp_header tra lbm allsrc.cpp elf remallsrc bin
endif

# build bootloader
ifeq ($(PCB), TARANIS)
bootloader.lbm: bootloader/bootloader_ramBoot.bin
	$(BIN2LBM) $^ $@

.PHONY bootloader/bootloader_ramBoot.bin:
	@echo "Building bootloader..."
	$(MAKE) -C bootloader PCB=$(PCB) PCBREV=$(PCBREV)
else ifeq ($(PCB), 9XRPRO)
bootloader.lbm:
	@echo "Fetching Mike's bootloader..."
	wget http://ersky9x.googlecode.com/svn/trunk/src/bootloader/bootflash8.lbm
	mv bootflash8.lbm $@
else ifeq ($(PCB), SKY9X)
bootloader.lbm:
	@echo "Fetching Mike's bootloader..."
	wget http://ersky9x.googlecode.com/svn/trunk/src/bootloader/bootflash4.lbm
	mv bootflash4.lbm $@
endif

tra: translations/en.h translations/cz.h translations/de.h translations/es.h translations/fi.h translations/fr.h translations/it.h translations/pl.h translations/pt.h translations/se.h
lbm: fonts bitmaps/sticks.lbm
  
elf: $(TARGET).elf
bin: $(TARGET).bin
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss
sym: $(TARGET).sym

# Build stamp-file
stamp_header:
	@echo "// Automatically generated file (Makefile) - do not edit" > stamp-opentx.h
	@echo "#define DATE_STR \"`date +%Y-%m-%d | tr -d '\r\n'`\"" >> stamp-opentx.h
	@echo "#define TIME_STR \"`date +%H:%M:%S | tr -d '\r\n'`\"" >> stamp-opentx.h
	@echo "#define VERS_STR \"$(VERSION)\"" >> stamp-opentx.h

opentx-stamp:
	@echo "#define DATE_STR \"`date +%Y-%m-%d`\"" > stamp-opentx.txt
	@echo "#define TIME_STR \"`date +%H:%M:%S`\"" >> stamp-opentx.txt
	@echo "#define VERS_STR \"$(VERSION)\"" >> stamp-opentx.txt
	@cat stamp-opentx.txt

translations/en.h: translations/en.h.txt
	@echo
	@echo "Create English language specific header files"
	$(TRANSLATE) translations/en.h.txt translations/en.h en

translations/cz.h: translations/cz.h.txt
	@echo
	@echo "Create Czech language specific header files"
	$(TRANSLATE) translations/cz.h.txt translations/cz.h cz

translations/de.h: translations/de.h.txt  
	@echo
	@echo "Create German language specific header files"
	$(TRANSLATE) translations/de.h.txt translations/de.h de

translations/es.h: translations/es.h.txt 
	@echo
	@echo "Create Spanish language specific header files"
	$(TRANSLATE) translations/es.h.txt translations/es.h es

translations/fi.h: translations/fi.h.txt 
	@echo
	@echo "Create Finnish language specific header files"
	$(TRANSLATE) translations/fi.h.txt translations/fi.h fi

translations/fr.h: translations/fr.h.txt 
	@echo
	@echo "Create French language specific header files"
	$(TRANSLATE) translations/fr.h.txt translations/fr.h fr

translations/it.h: translations/it.h.txt 
	@echo
	@echo "Create Italian language specific header files"
	$(TRANSLATE) translations/it.h.txt translations/it.h it

translations/pl.h: translations/pl.h.txt 
	@echo
	@echo "Create Polish language specific header files"
	$(TRANSLATE) translations/pl.h.txt translations/pl.h pl

translations/pt.h: translations/pt.h.txt
	@echo
	@echo "Create Portugese language specific header files"
	$(TRANSLATE) translations/pt.h.txt translations/pt.h pt

translations/se.h: translations/se.h.txt 
	@echo
	@echo "Create Swedish language specific header files"
	$(TRANSLATE) translations/se.h.txt translations/se.h se

.PHONY: fonts
fonts: $(patsubst %.png,%.lbm,$(wildcard fonts/*/*.png)) 

fonts/%.lbm: fonts/%.png
	$(IMG2LBM) $^ $@

bitmaps/sticks.lbm: bitmaps/sticks_4x1.xbm bitmaps/splash_9x.xbm bitmaps/splash_taranis.png bitmaps/logo_taranis.bmp bitmaps/icons.png bitmaps/asterisk.xbm bitmaps/about_9x.png bitmaps/about_taranis.png bitmaps/asterisk_4bits.png
	@echo
	@echo "Convert bitmaps from xbm to lbm:"
	$(IMG2LBM) bitmaps/sticks_4x1.xbm bitmaps/sticks.lbm img 4
	$(IMG2LBM) bitmaps/splash_9x.xbm bitmaps/splash_9x.lbm img
	$(IMG2LBM) bitmaps/splash_taranis.png bitmaps/splash_taranis.lbm bmp
	$(IMG2LBM) bitmaps/logo_taranis.bmp bitmaps/logo_taranis.lbm bmp
	$(IMG2LBM) bitmaps/icons.png bitmaps/icons.lbm bmp
	$(IMG2LBM) bitmaps/asterisk.xbm bitmaps/asterisk.lbm img
	$(IMG2LBM) bitmaps/about_taranis.png bitmaps/about_taranis.lbm bmp
	$(IMG2LBM) bitmaps/about_9x.png bitmaps/about_9x.lbm img
	$(IMG2LBM) bitmaps/asterisk_4bits.png bitmaps/asterisk_4bits.lbm bmp

lua_exports.cpp: myeeprom.h ../util/luaexport.py
	@echo "Generating a list of Lua exported constants in lua_exports.txt" 
	@$(CPP) -DPCBTARANIS -DCPUARM -DLUA -DEXPORT myeeprom.h | grep LEXP > lua_exports.txt
	@echo "Parsing and generating C Lua exports in lua_exports.cpp"
	@python ../util/luaexport.py $(VERSION) lua_exports.txt lua_exports.cpp lua_fields.txt
	@rm lua_exports.txt
	 
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
gccversion:
	@$(CC) --version

FOXINC=-I/usr/local/include/fox-1.6 -I/usr/include/fox-1.6 \
       -I$(FOXPATH)/include \
       -I/opt/local/include/fox-1.6
FOXLIB=-L/usr/local/lib \
       -L$(FOXPATH)/src/.libs \
       -L/opt/local/lib \
       -lFOX-1.6 \
       -Wl,-rpath,$(FOXPATH)/src/.libs

simu: $(LUADEP) stamp_header allsimusrc.cpp Makefile simu.cpp targets/simu/simpgmspace.cpp *.h tra lbm eeprom.bin
	g++ $(CPPFLAGS) $(INCFLAGS) simu.cpp allsimusrc.cpp $(LUASRC) targets/simu/simpgmspace.cpp -MD -DSIMU -DLUA_USE_APICHECK -O0 -o simu $(FOXINC) $(FOXLIB) -pthread -fexceptions

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
	$(OBJCOPY) -O ihex opentx.elf opentx.hex
endif

%.bin: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O binary  $< $@

%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O $(FORMAT) $< $@

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

allsimusrc.cpp: Makefile $(BOARDSRC) $(CPPSRC)
	@rm -f allsimusrc.cpp 
	for f in $(BOARDSRC) $(CPPSRC) ; do echo "# 1 \"$$f\"" >> allsimusrc.cpp; cat "$$f" >> allsimusrc.cpp; done

remallsrc:
	$(REMOVE) allsrc.cpp
	$(REMOVE) allsimusrc.cpp

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
	$(CC) $(OBJS) allsrc.o -mcpu=cortex-m3 -mthumb -nostartfiles -lm -T$(LDSCRIPT) -Wl,-Map=$(TARGET).map,--cref,--no-warn-mismatch,--gc-sections $(NEWLIB_NANO_FLAGS) -o $@
endif

# Target: clean project.
clean: begin clean_list end

CLEAN_SRC=$(filter %.c,$(SRC))
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
	$(REMOVE) $(CLEAN_SRC:.c=.s)
	$(REMOVE) `find . -name "*.o"` 
	$(REMOVE) *.d
	$(REMOVE) *.lst
	$(REMOVE) allsrc.cpp
	$(REMOVE) allsimusrc.cpp
	$(REMOVE) stamp-opentx.h
	$(REMOVE) bitmaps/*.lbm 
	$(REMOVE) fonts/*/*.lbm
	$(REMOVE) lua_exports.txt lua_exports.cpp lua_fields.txt
	$(MAKE) -C bootloader clean

#### Install

DATE = ${shell sh -c "date +%F:%T" }
AVRCOMMAND = $(AVRDUDE) $(AVRDEVICE)

WRITE_COMMAND = $(AVRCOMMAND) -U flash:w:opentx.hex:i
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
  BACKUP_ARGS += -U flash:r:$(DATA_DIR)/opentx-$(DATE).hex:i
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
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) -I$(GTEST_DIR)/include -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) -I$(GTEST_DIR)/include -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.

#use all .cpp files from tests/ dir
GTEST_TESTS_SRCS = $(shell find tests/ -type f -name '*.cpp')

gtests: $(BOARDSRC) $(CPPSRC) $(GTEST_TESTS_SRCS) targets/simu/simpgmspace.cpp *.h gtest_main.a
	g++ -std=gnu++0x $(CPPFLAGS) $(BOARDSRC) $(CPPSRC) $(LUASRC) $(GTEST_TESTS_SRCS) targets/simu/simpgmspace.cpp -I$(GTEST_DIR) ${INCFLAGS} -I$(GTEST_DIR)/include -o gtests -lpthread -MD -DSIMU -lQtCore -lQtGui gtest_main.a -fexceptions 

