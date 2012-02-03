#!/usr/bin/env python

import os, sys, shutil
from subprocess import call

options_stock = [[("", "EXT=STD"), ("frsky", "EXT=FRSKY"), ("jeti", "EXT=JETI"), ("ardupilot", "EXT=ARDUPILOT"), ("nmea", "EXT=NMEA")],
                 [("", "HELI=NO"), ("heli", "HELI=YES")],
                 [("", "TEMPLATES=NO"), ("templates", "TEMPLATES=YES")],
                 [("", "AUDIO=NO"), ("audio", "AUDIO=YES")],
                 [("", "HAPTIC=NO"), ("haptic", "HAPTIC=YES")],
#                 ("DSM2", "DSM2="NO", "DSM2=PPM"),
#                 ("PXX", "PXX=NO", "PXX=YES"),
                ]

options_v4 = [[("", "HELI=NO"), ("heli", "HELI=YES")],
              [("", "TEMPLATES=NO"), ("templates", "TEMPLATES=YES")],
             ]

languages = ["en", "fr"]

def generate(hex, arg, options):
    result = []
    states = [0] * len(options)
    index = 0
    
    while 1:
        # print index, states
    
        for language in languages:
            hex_file = hex
            make_args = ["make", arg]
            for i, option in enumerate(options):
                state = states[len(options) - 1 - i]
                if option[state][0]:
                    hex_file += "-" + option[state][0]
                make_args.append(option[state][1])
            hex_file += "-" + language
            make_args.append("TRANSLATIONS=" + language.upper())
            print hex_file
            call(["make", "clean"])
            call(make_args)
            shutil.copyfile("open9x.hex", "../binaries/" + hex_file + ".hex")
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
        
def generate_c9x_list(filename, hexes, size):
    f = file(filename, "w")
    for hex in hexes:
        f.write('open9x->add_option(new Open9xFirmware("%s", new Open9xInterface(%s), OPEN9X_BIN_URL "%s.hex"));\n' % (hex, size, hex))

# stock board
hexes = generate("open9x-stock", "PCB=STD", options_stock)
generate_c9x_list("../../companion9x/src/open9x-stock-binaries.cpp", hexes, "EESIZE_STOCK")

# v4 board
hexes = generate("open9x-v4", "PCB=V4", options_v4)
generate_c9x_list("../../companion9x/src/open9x-v4-binaries.cpp", hexes, "EESIZE_V4")

# stamp
call(["make", "stamp"])
