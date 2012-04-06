#!/usr/bin/env python

import os, sys, shutil, platform
import subprocess

options_stock = [[("", "EXT=STD"), ("frsky", "EXT=FRSKY"), ("jeti", "EXT=JETI"), ("ardupilot", "EXT=ARDUPILOT"), ("nmea", "EXT=NMEA")],
                 [("", "HELI=NO"), ("heli", "HELI=YES")],
                 [("", "TEMPLATES=NO"), ("templates", "TEMPLATES=YES")],
                 [("", "AUDIO=NO"), ("audio", "AUDIO=YES")],
                 [("", "HAPTIC=NO"), ("haptic", "HAPTIC=YES")],
                 [("", "DSM2=NO"), ("DSM2", "DSM2=PPM")],
                 [("", "NAVIGATION=NO"), ("potscroll", "NAVIGATION=POTS")],
#                 ("PXX", "PXX=NO", "PXX=YES"),
                ]

options_v4 = [[("", "EXT=FRSKY")],
              [("", "HELI=NO"), ("heli", "HELI=YES")],
              [("", "TEMPLATES=NO"), ("templates", "TEMPLATES=YES")],
              [("", "LOGS=NO"), ("logs", "LOGS=YES")],
              [("", "SOMO=NO"), ("SOMO", "SOMO=YES")],
             ]

options_arm = [[("", "EXT=FRSKY")],
               [("", "HELI=NO"), ("heli", "HELI=YES")],
               [("", "TEMPLATES=NO"), ("templates", "TEMPLATES=YES")],
              ]

languages = ["en", "fr", "se"]

def generate(hex, arg, extension, options, maxsize):
    result = []
    states = [0] * len(options)
    
    count = len(languages)
    for option in options:
      count *= len(option)
    current = 0
    
    while 1:
        # print index, states
    
        for language in languages:
            current += 1
            hex_file = hex
            make_args = ["make", arg]
            for i, option in enumerate(options):
                state = states[len(options) - 1 - i]
                if option[state][0]:
                    hex_file += "-" + option[state][0]
                make_args.append(option[state][1])
            hex_file += "-" + language
            make_args.append("TRANSLATIONS=" + language.upper())
            print "[%d/%d]" % (current, count), hex_file
            subprocess.check_output(["make", "clean", arg])
            p = subprocess.Popen(make_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            p.wait()
            stderr = p.stderr.read()
            if "error" in stderr:
                print stderr
                exit()
            for line in stderr.split("\n"):
                if "warning" in line:
                    print "  ", line
            size = 0
            for line in p.stdout.readlines():
                if line.startswith("Program:"):
                    parts = line.split(" ")
                    while "" in parts:
                        parts.remove("")
                    size = int(parts[1])
                    if size > maxsize:
                        print "  ", line[:-1], "[NOT RELEASED]"
                    else:
                        print "  ", line,
                if line.startswith("Data:"):
                    print "  ", line,
            
            if size <= maxsize:
                shutil.copyfile("open9x." + extension, "../binaries/" + hex_file + "." + extension)
                result.append(hex_file)
        
        for index, state in enumerate(states):
            if state < len(options[len(options) - 1 - index]) - 1:
                for i in range(index):
                    states[i] = 0
                states[index] += 1
                break
        else:
            break
        
    return result
        
def generate_c9x_list(filename, hexes, extension, size):
    f = file(filename, "w")
    for hex in hexes:
        f.write('open9x->add_option(new Open9xFirmware("%s", new Open9xInterface(%s), OPEN9X_BIN_URL "%s.%s"));\n' % (hex, size, hex, extension))

if platform.system() == "Windows":
    # arm board
    hexes = generate("open9x-arm", "PCB=ARM", "bin", options_arm, 262000)
    generate_c9x_list("../../companion9x/src/open9x-arm-binaries.cpp", hexes, "bin", "BOARD_ERSKY9X")
else:
    # stock board
    hexes = generate("open9x-stock", "PCB=STD", "hex", options_stock, 65530)
    generate_c9x_list("../../companion9x/src/open9x-stock-binaries.cpp", hexes, "hex", "BOARD_STOCK")

    # v4 board
    hexes = generate("open9x-v4", "PCB=V4", "hex", options_v4, 262000)
    generate_c9x_list("../../companion9x/src/open9x-v4-binaries.cpp", hexes, "hex", "BOARD_GRUVIN9X")

    # stamp
    subprocess.check_output(["make", "stamp"])
