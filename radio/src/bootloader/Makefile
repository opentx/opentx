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

MCU  = cortex-m3

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
PROJECT        = bootloader

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

EXTRAINCDIRS =

#
# Define linker script file here
#
ifeq ($(RUN_FROM_FLASH), 1)
 ifeq ($(REVX), 1)
  LDSCRIPT = sam3s8c_boot.ld
  FULL_PRJ = $(PROJECT)_flash8
  EXTRAINCDIRS += include
#  EXTRAINCDIRS += ../
 else
  LDSCRIPT = sam3s4c_boot.ld
  FULL_PRJ = $(PROJECT)_flash4
  EXTRAINCDIRS += include
#  EXTRAINCDIRS += ../
 endif
 TRGT = arm-none-eabi-
 CPPDEFS += -DPCBSKY 
 UDEFS = -Dat91sam3s8

 # If REVX board
 ifeq ($(REVX), 1)
   CPPDEFS += -DREVX
  endif

  CPPDEFS += -DREVB

else
 ifeq ($(PCB), TARANIS)
  ARCH = ARM
  LDSCRIPT = ../targets/taranis/stm32_ramBoot.ld
  TRGT = arm-none-eabi-
  CPPDEFS += -DHSE_VALUE=12000000
  CPPDEFS += -DPCBTARANIS 
  FULL_PRJ = $(PROJECT)_ramBoot
  EXTRAINCDIRS += ../targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc
  EXTRAINCDIRS += ../targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include
  EXTRAINCDIRS += ../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/inc
  EXTRAINCDIRS += ../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/inc
  EXTRAINCDIRS += ../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/inc
  EXTRAINCDIRS += ../targets/taranis
  EXTRAINCDIRS += ../fonts/std
  EXTRAINCDIRS += ../
  
  ifeq ($(PCBREV), REV3)
    CPPDEFS += -DREV3
  else ifeq ($(PCBREV), REV4a)
    CPPDEFS += -DREV4a
  else ifeq ($(PCBREV), REVPLUS)
    CPPDEFS += -DREVPLUS
  else
    CPPDEFS += -DREV4
  endif
  
 endif
endif


# List all user C define here, like -D_DEBUG=1

# Define ASM defines here
UADEFS = 

ifeq ($(PCB), TARANIS)
SRC  =	../targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Source/Templates/system_stm32f2xx.c \
       	../targets/taranis/i2c.c \
       	../targets/taranis/delays.c \
	../targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_rcc.c \
	../targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_gpio.c \
	../targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/stm32f2xx_spi.c \
	../targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/misc.c \
        ../FatFs/ff.c \
	../FatFs/option/ccsbcs.c \
	../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/src/usb_core.c \
	../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/src/usb_dcd.c \
	../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_OTG_Driver/src/usb_dcd_int.c \
	../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_core.c \
	../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_ioreq.c \
	../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Core/src/usbd_req.c \
	../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_data.c \
	../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_scsi.c \
	../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_bot.c \
	../targets/taranis/STM32_USB-Host-Device_Lib_V2.1.0/Libraries/STM32_USB_Device_Library/Class/msc/src/usbd_msc_core.c \
	../targets/taranis/usbd_desc.c \
	../targets/taranis/aspi.c \
	../targets/taranis/usb_bsp.c \
	../targets/taranis/usb_driver.c \
	../targets/taranis/pwr_driver.c \
	init.c

CPPSRC = ../targets/taranis/lcd_driver.cpp \
	../targets/taranis/configure_pins.cpp \
	../targets/taranis/keys_driver.cpp \
	../targets/taranis/flash_driver.cpp \
	../targets/taranis/diskio.cpp \
	../targets/taranis/usbd_usr.cpp \
	../targets/taranis/usbd_storage_msd.cpp \
	../lcd_common.cpp \
	../lcd_taranis.cpp \
	../keys.cpp \
	../fonts.cpp \
	../strhelpers.cpp \
	boot.cpp

# List ASM source files here
ASRC = ../targets/taranis/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Source/Templates/gcc_ride7/startup_stm32f2xx.s

else

# List C source files here
SRC  = core_cm3.c \
       board_lowlevel.c \
       crt.c \
       usb/device/core/USBD_UDP.c usb/device/core/USBDDriver.c usb/device/core/USBDCallbacks.c \
       usb/device/massstorage/MSDDriver.c usb/device/massstorage/MSDDStateMachine.c \
		 usb/device/massstorage/MSDLun.c usb/device/massstorage/MSDDriverDescriptors.c usb/device/massstorage/SBCMethods.c \
       usb/common/core/USBEndpointDescriptor.c usb/common/core/USBGenericRequest.c \
		 usb/common/core/USBFeatureRequest.c usb/common/core/USBInterfaceRequest.c usb/common/core/USBGetDescriptorRequest.c \
       usb/common/core/USBSetAddressRequest.c usb/common/core/USBSetConfigurationRequest.c \
       usb/common/core/USBConfigurationDescriptor.c usb/common/core/USBGenericDescriptor.c \
       MEDSdcard.c \
       vectors_sam3s.c

CPPSRC = lcdboot.cpp \
         ff.cpp \
			diskio.cpp \
         driversboot.cpp \
         logicioboot.cpp \
         sdcard_driver.cpp \
         massstorage.cpp \
         bootsamEeprom.cpp \
			ff_lfn.cpp \
			power.cpp \
         boot.cpp

# List ASM source files here
ASRC =

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

#ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -Wa,-amhls=$(OBJDIR)/$(notdir $(<:.s=.lst)) $(ADEFS)
CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm $(DEFS)
LDFLAGS = $(MCFLAGS) -mthumb -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR) --specs=nano.specs
CPPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fverbose-asm $(DEFS)

CPPFLAGS += $(CPPDEFS)
CPFLAGS += $(CPPDEFS)
CPFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

AOBJS    = $(patsubst %,$(OBJDIR)/%,$(notdir $(AXOBJS) ))
COBJS    = $(patsubst %,$(OBJDIR)/%,$(notdir $(CXOBJS) ))
CPPOBJS  = $(patsubst %,$(OBJDIR)/%,$(notdir $(CPPXOBJS) ))
DIRS = $(sort $(dir $(AXOBJS) $(CXOBJS) $(CPPXOBJS) ) )


CPPDEFS += -DCPUARM

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
