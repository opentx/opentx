#!/usr/bin/env python

import os
import sys
from fwoptions import *

# Board types
BOARD_9X = 0
BOARD_GRUVIN9X = 1
BOARD_SKY9X = 2
BOARD_TARANIS = 3
BOARD_HORUS = 4

# Board families
BOARD_FAMILY_AVR = 0
BOARD_FAMILY_ARM = 1

srcdir = os.path.dirname(os.path.realpath(__file__)) + "/../.."
filename = sys.argv[1]
root, ext = os.path.splitext(filename)
options = root.split("-")

if options[0] != "opentx":
    sys.exit("Invalid firmware");

filename = "opentx-"
optcount = 1

command_options = {}

if options[optcount] == "9x":
    command_options["PCB"] = "9X"
    firmware_options = options_9x
    maxsize = 65536
    board = BOARD_9X
    board_family = BOARD_FAMILY_AVR
elif options[optcount] == "9xr":
    command_options["PCB"] = "9XR"
    firmware_options = options_9x
    maxsize = 65536
    board = BOARD_9X
    board_family = BOARD_FAMILY_AVR
elif options[optcount] == "9x128":
    command_options["PCB"] = "9X128"
    command_options["FRSKY_STICKS"] = "YES"
    firmware_options = options_9x128
    maxsize = 65536 * 2
    board = BOARD_9X
    board_family = BOARD_FAMILY_AVR
elif options[optcount] == "9xr128":
    command_options["PCB"] = "9XR128"
    command_options["FRSKY_STICKS"] = "YES"
    firmware_options = options_9x128
    maxsize = 65536 * 2
    board = BOARD_9X
    board_family = BOARD_FAMILY_AVR
elif options[optcount] == "gruvin9x":
    command_options["PCB"] = "GRUVIN9X"
    command_options["EXT"] = "FRSKY"
    firmware_options = options_gruvin9x
    maxsize = 65536 * 4
    board = BOARD_GRUVIN9X
    board_family = BOARD_FAMILY_AVR
elif options[optcount] == "mega2560":
    command_options["PCB"] = "MEGA2560"
    command_options["EXT"] = "FRSKY"
    firmware_options = options_mega2560
    maxsize = 65536 * 4
    board = BOARD_GRUVIN9X
    board_family = BOARD_FAMILY_AVR
elif options[optcount] == "sky9x":
    command_options["PCB"] = "SKY9X"
    firmware_options = options_sky9x
    maxsize = 65536 * 4
    board = BOARD_SKY9X
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "9xrpro":
    command_options["PCB"] = "9XRPRO"
    command_options["SDCARD"] = "YES"
    firmware_options = options_sky9x
    maxsize = 65536 * 4
    board = BOARD_SKY9X
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "ar9x":
    command_options["PCB"] = "AR9X"
    command_options["SDCARD"] = "YES"
    command_options["HARDWARE_VOLUME"] = "NO"
    firmware_options = options_ar9x
    maxsize = 65536 * 4
    board = BOARD_SKY9X
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "taranis":
    command_options["PCB"] = "TARANIS"
    firmware_options = options_taranis
    maxsize = 65536 * 8
    board = BOARD_TARANIS
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "taranisplus":
    command_options["PCB"] = "TARANIS"
    command_options["PCBREV"] = "REVPLUS"
    firmware_options = options_taranisplus
    maxsize = 65536 * 8
    board = BOARD_TARANIS
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "taranisx9e":
    command_options["PCB"] = "TARANIS"
    command_options["PCBREV"] = "REV9E"
    firmware_options = options_taranisx9e
    maxsize = 65536 * 8
    board = BOARD_TARANIS
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "horus":
    command_options["PCB"] = "HORUS"
    firmware_options = options_horus
    maxsize = 2 * 1024 * 1024
    board = BOARD_HORUS
    board_family = BOARD_FAMILY_ARM
else:
    sys.exit("Unknown board")

filename += options[optcount]
optcount += 1

# The firmware options
for opt, value in firmware_options.items():
    found = False
    for i in range(optcount, len(options)):
        if options[i] == opt:
            found = True
            break

    if found:
        optvalue = value[1];
        filename += "-" + opt;
    else:
        optvalue = value[2]

    if optvalue != None:
        command_options[value[0]] = optvalue

# The firmware display language
language = ""
for key in languages:
    if key == options[-1]:
        language = key
if not language:
    sys.exit("Invalid language")
command_options["TRANSLATIONS"] = language.upper()
filename += "-" + language

if board_family == BOARD_FAMILY_ARM:
    ext = ".bin"
else:
    ext = ".hex"
filename += ext
firmware = "firmware" + ext

# Launch CMake
cmd = "cmake"
for opt, value in command_options.items():
    cmd += " -D%s=%s" % (opt, value)
cmd += " " + srcdir
if "OPENTX_VERSION_SUFFIX" in os.environ:
    cmd += ' -DVERSION_SUFFIX="%s"' % os.environ["OPENTX_VERSION_SUFFIX"]
print cmd
os.system(cmd)

# Launch make firmware
cmd = "make firmware"
os.system(cmd)

# Check binary size
if board_family == BOARD_FAMILY_ARM:
    size = os.stat(firmware).st_size
else:
    os.system("avr-size -A %s | grep Total | cut -f2- -d \" \" > ./size" % firmware)
    size = file("./size").read().strip()
if int(size) > maxsize:
    exit("Invalid size")
