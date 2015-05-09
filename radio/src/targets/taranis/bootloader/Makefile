#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!    
#
##############################################################################################
# 
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# To rebuild project do "make clean" and "make all".
#

##############################################################################################
# Start of default section
#

TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
CLSS = $(TRGT)objdump
AS   = $(TRGT)as
#gcc -x assembler-with-cpp
BIN  = $(CP) -O ihex 
BINX = $(CP) -O binary 

OBJDIR = .

THIRDPARTY = ../../../thirdparty
FATFSDIR = $(THIRDPARTY)/FatFs

ifeq ($(PCBREV), REV9E)
  MCU = cortex-m4
  STM32LIBPATH = $(THIRDPARTY)/STM32F4xx_DSP_StdPeriph_Lib_V1.4.0/Libraries
else
  MCU = cortex-m3
  STM32LIBPATH = $(THIRDPARTY)/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries
endif

# List all default C defines here, like -D_DEBUG=1
DDEFS =

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = 

# List all default directories to look for include files here
DINCDIR = 

# List the default directory to look for the libraries here
DLIBDIR =

# List all default libraries here
DLIBS = 

MEMORIES = sram

EXT = STD

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

# 
# Define project name and Ram/Flash mode here
PROJECT = bootloader

ifeq ($(PCB), TARANIS)
RUN_FROM_FLASH = 0
 else
 ifeq ($(PCB), X9D)
  RUN_FROM_FLASH = 0
 else
  RUN_FROM_FLASH = 1
 endif
endif

# PCB revision
PCBREV =

EXTRAINCDIRS = $(THIRDPARTY) $(FATFSDIR) $(FATFSDIR)/option

STM32USBPATH = ../../../thirdparty/STM32_USB-Host-Device_Lib_V2.1.0/Libraries

#
# Define linker script file here
#
ifeq ($(PCB), TARANIS)
  ARCH = ARM
  LDSCRIPT = ../stm32_ramboot.ld
  TRGT = arm-none-eabi-
  CPPDEFS += -DHSE_VALUE=12000000
  CPPDEFS += -DPCBTARANIS -DEEPROM_RLC
  FULL_PRJ = $(PROJECT)_ramBoot
  ifeq ($(PCBREV), REV9E)
    CPPDEFS += -DSTM32F40_41xxx
    EXTRAINCDIRS += $(STM32LIBPATH)/STM32F4xx_StdPeriph_Driver/inc
    EXTRAINCDIRS += $(STM32LIBPATH)/CMSIS/Device/ST/STM32F4xx/Include
    EXTRAINCDIRS += $(STM32LIBPATH)/CMSIS/Include
  else
    EXTRAINCDIRS += $(STM32LIBPATH)/STM32F2xx_StdPeriph_Driver/inc
    EXTRAINCDIRS += $(STM32LIBPATH)/CMSIS/Device/ST/STM32F2xx/Include
    EXTRAINCDIRS += $(STM32LIBPATH)/CMSIS/include
  endif
  EXTRAINCDIRS += $(STM32USBPATH)/STM32_USB_OTG_Driver/inc
  EXTRAINCDIRS += $(STM32USBPATH)/STM32_USB_Device_Library/Core/inc
  EXTRAINCDIRS += $(STM32USBPATH)/STM32_USB_Device_Library/Class/msc/inc
  EXTRAINCDIRS += .. ../../.. ../../../fonts/std
  ifeq ($(PCBREV), REV4a)
    CPPDEFS += -DREV4a
  else ifeq ($(PCBREV), REVPLUS)
    CPPDEFS += -DREVPLUS
    HAPTIC = YES
  else ifeq ($(PCBREV), REV9E)
    CPPDEFS += -DREVPLUS -DREV9E
    HAPTIC = YES
  else
    CPPDEFS += -DREV4
  endif
endif

# List all user C define here, like -D_DEBUG=1

# Define ASM defines here
UADEFS = 

SRC =

ifeq ($(PCBREV), REV9E)
  SRC += ../system_stm32f4xx.c
  SRC += $(STM32LIBPATH)/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
  SRC += $(STM32LIBPATH)/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
  SRC += $(STM32LIBPATH)/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
  SRC += $(STM32LIBPATH)/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
  SRC += $(STM32LIBPATH)/STM32F4xx_StdPeriph_Driver/src/misc.c
else
  SRC += ../system_stm32f2xx.c
  SRC += $(STM32LIBPATH)/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rcc.c
  SRC += $(STM32LIBPATH)/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_gpio.c
  SRC += $(STM32LIBPATH)/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_spi.c
  SRC += $(STM32LIBPATH)/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_i2c.c
  SRC += $(STM32LIBPATH)/STM32F2xx_StdPeriph_Driver/src/misc.c
endif  

SRC += $(STM32USBPATH)/STM32_USB_OTG_Driver/src/usb_core.c
SRC += $(STM32USBPATH)/STM32_USB_OTG_Driver/src/usb_dcd.c
SRC += $(STM32USBPATH)/STM32_USB_OTG_Driver/src/usb_dcd_int.c
SRC += $(STM32USBPATH)/STM32_USB_Device_Library/Core/src/usbd_core.c
SRC += $(STM32USBPATH)/STM32_USB_Device_Library/Core/src/usbd_ioreq.c
SRC += $(STM32USBPATH)/STM32_USB_Device_Library/Core/src/usbd_req.c
SRC += $(STM32USBPATH)/STM32_USB_Device_Library/Class/msc/src/usbd_msc_data.c
SRC += $(STM32USBPATH)/STM32_USB_Device_Library/Class/msc/src/usbd_msc_scsi.c
SRC += $(STM32USBPATH)/STM32_USB_Device_Library/Class/msc/src/usbd_msc_bot.c
SRC += $(STM32USBPATH)/STM32_USB_Device_Library/Class/msc/src/usbd_msc_core.c
SRC += ../delays.c
SRC += $(THIRDPARTY)/FatFs/ff.c $(THIRDPARTY)/FatFs/option/ccsbcs.c
SRC += ../usbd_desc.c
SRC += ../aspi.c
SRC += ../usb_bsp.c
SRC += ../usb_driver.c
SRC += ../pwr_driver.c
SRC += init.c

CPPSRC = ../lcd_driver.cpp \
	../configure_pins.cpp \
	../keys_driver.cpp \
	../i2c_driver.cpp \
	../flash_driver.cpp \
	../diskio.cpp \
	../usbd_usr.cpp \
	../usbd_storage_msd.cpp \
	../../../gui/Taranis/lcd.cpp \
	../../../keys.cpp \
	../../../fonts.cpp \
	../../../strhelpers.cpp \
	boot.cpp

ifeq ($(PCBREV), REV9E)
  CPPSRC += ../rotenc_driver.cpp
endif

# List ASM source files here

ifeq ($(PCBREV), REV9E)
  ASRC = $(STM32LIBPATH)/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc_ride7/startup_stm32f40_41xxx.s
else
  ASRC = $(STM32LIBPATH)/CMSIS/Device/ST/STM32F2xx/Source/Templates/gcc_ride7/startup_stm32f2xx.s
endif

# List all user directories here
UINCDIR = ./inc
# \
#          ./cmsis/core \
#          ./cmsis/device

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS = 

# Define optimisation level here
OPT = -Os

#
# End of user defines
##############################################################################################


VPATH = $(DIRS)

INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))

ifeq ($(RUN_FROM_FLASH), 0)
DEFS    = $(DDEFS) $(UDEFS) -DRUN_FROM_FLASH=0
else
DEFS    = $(DDEFS) $(UDEFS) -DRUN_FROM_FLASH=1
endif

ADEFS   = $(DADEFS) $(UADEFS)

AXOBJS    = $(ASRC:%.s=%.o)
CXOBJS    = $(SRC:%.c=%.o)
CPPXOBJS  = $(CPPSRC:%.cpp=%.o)

LIBS    = $(DLIBS) $(ULIBS)
MCFLAGS = -mcpu=$(MCU)

ifeq ($(NANO), YES)
  # use newlib-nano for linking
  # NEWLIB_NANO_FLAGS = --specs=nano.specs -u _printf_float   #use this version if printf for floats is needed!
  NEWLIB_NANO_FLAGS = --specs=nano.specs
endif

#ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -Wa,-amhls=$(OBJDIR)/$(notdir $(<:.s=.lst)) $(ADEFS)
CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm $(DEFS)
LDFLAGS = $(MCFLAGS) -mthumb -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch,--gc-sections $(LIBDIR) $(NEWLIB_NANO_FLAGS)
CPPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb -fomit-frame-pointer -fdata-sections -ffunction-sections -Wall -fverbose-asm $(DEFS)

CPPFLAGS += $(CPPDEFS)
CPFLAGS += $(CPPDEFS)
CPFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

AOBJS    = $(patsubst %,$(OBJDIR)/%,$(notdir $(AXOBJS) ))
COBJS    = $(patsubst %,$(OBJDIR)/%,$(notdir $(CXOBJS) ))
CPPOBJS  = $(patsubst %,$(OBJDIR)/%,$(notdir $(CPPXOBJS) ))
DIRS = $(sort $(dir $(AXOBJS) $(CXOBJS) $(CPPXOBJS) ) )


CPPDEFS += -DCPUARM

ifeq ($(PCB), TARANIS)
  CPPDEFS += -DCPUSTM32
endif

CPPDEFS += -DTIMERS=0

# If serial Debug required
ifeq ($(DEBUG), 1)
 CPPDEFS += -DDEBUG
endif

CPPDEFS += -DBOOT

# Generate dependency information
CPFLAGS += -MD -MP -MF .dep/$(@F).d
CPPFLAGS += -MD -MP -MF .dep/$(@F).d
CPPFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

#
# makefile rules
#

all: size_before $(OBJS) $(COBJS) $(CPPOBJS) $(FULL_PRJ).elf $(FULL_PRJ).hex $(FULL_PRJ).lss $(FULL_PRJ).bin size

#begin:
#	@echo $(AXOBJS) $(CXOBJS) $(CPPXOBJS)
#	@echo
#	@echo $(OBJS) $(COBJS) $(CPPOBJS)
#	@echo
#	@echo $(DIRS)

size_before:
#	arm-none-eabi-size $(FULL_PRJ).elf
	@if test -f $(FULL_PRJ).elf; then arm-none-eabi-size $(FULL_PRJ).elf; fi

size:
#	arm-none-eabi-size $(FULL_PRJ).elf
	@if test -f $(FULL_PRJ).elf; then arm-none-eabi-size $(FULL_PRJ).elf; fi

$(CPPOBJS) : $(OBJDIR)/%.o : %.cpp
	$(CC) -c $(CPPFLAGS) -fno-exceptions -I . $(INCDIR) $< -o $@

$(COBJS) : $(OBJDIR)/%.o : %.c
	$(CC) -c $(CPFLAGS) -I . $(INCDIR) $< -o $@


$(AOBJS) : $(OBJDIR)/%.o : %.s
	$(AS) -c $(ASFLAGS) $< -o $@

%elf: $(AOBJS) $(COBJS) $(CPPOBJS) 
	$(CC) $(AOBJS) $(COBJS) $(CPPOBJS) $(LDFLAGS) $(LIBS) -o $@
  
# Create extended listing file from ELF output file.
%.lss: %.elf
	$(CLSS) -h -S $< > $@

%hex: %elf
	$(BIN) $< $@

%bin: %elf
	$(BINX) $< $@

#  $(PROJECT)_flash8
#  $(PROJECT)_flash4
#  $(PROJECT)_ramBootS
#  $(PROJECT)_ramBootT

clean:
	-rm -f *.o
	-rm -f *.elf
	-rm -f *.map
	-rm -f *.hex
	-rm -f *.bin
	-rm -f *.lss
	-rm -fR .dep


# 
# Include the dependency files, should be the last of the makefile
#
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
