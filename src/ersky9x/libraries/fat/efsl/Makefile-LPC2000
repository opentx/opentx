# efsl library makefile for LPC2000 
# by Martin Thomas
# (based on the efsl avr- and the WinARM-template makefiles)

MCU   = arm7tdmi-s
THUMB = -mthumb -mthumb-interwork

COPT= -mcpu=$(MCU) $(THUMB) -g3 -Wall -Os 
INCLUDEDIRS=-Iinc -Iconf
CFLAGS=$(COPT) $(INCLUDEDIRS)
CC=arm-elf-gcc
AR=arm-elf-ar
OBJCOPY=arm-elf-objcopy
OBJ=src/efs.o src/plibc.o src/disc.o src/partition.o src/time.o src/fs.o src/fat.o src/file.o src/dir.o src/mkfs.o src/debug.o src/ioman.o src/ui.o src/extract.o src/interfaces/sd.o
OBJ+=src/ls.o
OBJ+=src/interfaces/lpc2000_spi.o src/interfaces/lpc2000_dbg_printf.o

all: lib

lib: $(OBJ)
	$(AR) rcs libefsl.a $(OBJ)
	
clean :
	rm -f $(OBJ) libefsl.a

