#!/usr/bin/env python

from __future__ import print_function
import os
import sys
import subprocess
import shutil
import filelock
from fwoptions import *

# Error codes
FIRMWARE_SIZE_TOO_BIG = 1
COMPILATION_ERROR = 4
INVALID_FIRMWARE = 5
INVALID_BOARD = 6
INVALID_LANGUAGE = 7

# Board types
BOARD_9X = 0
BOARD_GRUVIN9X = 1
BOARD_SKY9X = 2
BOARD_TARANIS = 3
BOARD_HORUS = 4

# Board families
BOARD_FAMILY_AVR = 0
BOARD_FAMILY_ARM = 1

if len(sys.argv) != 3:
    exit(INVALID_FIRMWARE)

what = sys.argv[1]
directory, filename = os.path.split(sys.argv[2])
root, ext = os.path.splitext(filename)
options = root.split("-")

if len(options) < 2 or options[0] != "opentx":
    exit(INVALID_FIRMWARE)

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
    command_options["TELEMETRY"] = "FRSKY"
    firmware_options = options_gruvin9x
    maxsize = 65536 * 4
    board = BOARD_GRUVIN9X
    board_family = BOARD_FAMILY_AVR
elif options[optcount] == "mega2560":
    command_options["PCB"] = "MEGA2560"
    command_options["TELEMETRY"] = "FRSKY"
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
    firmware_options = options_ar9x
    maxsize = 65536 * 4
    board = BOARD_SKY9X
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "x7":
    command_options["PCB"] = "X7"
    firmware_options = options_taranisplus
    maxsize = 65536 * 8
    board = BOARD_TARANIS
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "xlite":
    command_options["PCB"] = "XLITE"
    firmware_options = options_xlite
    maxsize = 65536 * 8
    board = BOARD_TARANIS
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "x9d":
    command_options["PCB"] = "X9D"
    firmware_options = options_taranis
    maxsize = 65536 * 8
    board = BOARD_TARANIS
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "x9d+":
    command_options["PCB"] = "X9D+"
    firmware_options = options_taranisplus
    maxsize = 65536 * 8
    board = BOARD_TARANIS
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "x9e":
    command_options["PCB"] = "X9E"
    firmware_options = options_taranisx9e
    maxsize = 65536 * 8
    board = BOARD_TARANIS
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "x10":
    command_options["PCB"] = "X10"
    firmware_options = options_x10
    maxsize = 2 * 1024 * 1024
    board = BOARD_HORUS
    board_family = BOARD_FAMILY_ARM
elif options[optcount] == "x12s":
    command_options["PCB"] = "X12S"
    firmware_options = options_x12s
    maxsize = 2 * 1024 * 1024
    board = BOARD_HORUS
    board_family = BOARD_FAMILY_ARM
else:
    exit(INVALID_BOARD)

if what == "firmware":
    if board_family == BOARD_FAMILY_ARM:
        ext = ".bin"
    else:
        ext = ".hex"
    target = "firmware" + ext
    filename = "opentx"
elif what == "libsimulator":
    ext = ".so"
    target = "libopentx-" + options[optcount] + "-simulator.so"
    filename = "libopentx"
else:
    exit(INVALID_BOARD)

filename += "-" + options[optcount]
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

    if optvalue is not None:
        command_options[value[0]] = optvalue

# The firmware display language
language = ""
for key in languages:
    if key == options[-1]:
        language = key
if not language:
    exit(INVALID_LANGUAGE)
command_options["TRANSLATIONS"] = language.upper()

filename += "-" + language + ext
path = os.path.join(directory, filename)
errpath = path + ".err"

def build_firmware(path):
    srcdir = os.path.dirname(os.path.realpath(__file__)) + "/../.."
    outpath = path + ".out"

    # cmake 3.7.2 cannot work from / so we need to go into a subdir
    os.mkdir("/build")
    os.chdir("/build")

    # Launch CMake
    cmd = ["cmake"]
    for opt, value in command_options.items():
        cmd.append("-D%s=%s" % (opt, value))
    if "OPENTX_VERSION_SUFFIX" in os.environ:
        suffix = os.environ["OPENTX_VERSION_SUFFIX"]
        cmd.append('-DVERSION_SUFFIX="%s"' % suffix)
        if suffix.startswith("N"):
            cmd.append('-DNIGHTLY_BUILD_WARNING=YES')
    cmd.append(srcdir)
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = proc.communicate()
    if proc.returncode == 0:
        file(outpath, "a").write("\n".join(cmd) + output + error)
    else:
        file(errpath, "w").write(output + error)
        print(filename)
        exit(COMPILATION_ERROR)

    # Launch make
    cmd = ["make", "-j2", what]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = proc.communicate()
    if proc.returncode == 0:
        file(outpath, "a").write(output + error)
    else:
        file(errpath, "w").write(output + error)
        print(filename)
        exit(COMPILATION_ERROR)

    if what == "firmware":
        # Check binary size
        if board_family == BOARD_FAMILY_ARM:
            size = os.stat(target).st_size
        else:
            size = subprocess.check_output('avr-size -A %s | grep Total | cut -f2- -d " "' % target, shell=True)
            size = int(size.strip())
        if size > maxsize:
            exit(FIRMWARE_SIZE_TOO_BIG)

    # Copy binary to the binaries directory
    shutil.move(target, path)

if os.path.isfile(errpath):
    print(filename)
    exit(COMPILATION_ERROR)

if os.path.isfile(path):
    print(filename)
    exit(0)

lockpath = path + ".lock"
lock = filelock.FileLock(lockpath)
try:
    with lock.acquire(timeout = 60*60):
        if not os.path.isfile(path):
            build_firmware(path)
except filelock.Timeout:
    print(filename)
    exit(COMPILATION_ERROR)

print(filename)
exit(0)
