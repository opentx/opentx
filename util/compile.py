#!/usr/bin/env python

import os, sys, shutil
from subprocess import call

options_stock = [("frsky", "EXT=FRSKY"),
                 ("heli", "HELI=YES"),
                 ("templates", "TEMPLATES=YES"),
                 ("speaker", "BEEPER=SPEAKER"),
#                 ("DSM2", "DSM2=PPM"),
#                 ("PXX", "PXX=YES"),
                 ]

options_v4 = [("heli", "HELI=YES"),
              ("templates", "TEMPLATES=YES"),
             ]

languages = ["en", "fr"]

def generate(hex, arg, options):
    states = [False] * len(options)
    index = 0
    
    while 1:
        # print index, states
    
        for language in languages:
            hex_file = hex
            make_args = ["make", arg]
            for i, option in enumerate(options):
                if states[len(options) - 1 - i]:
                    hex_file += "-" + option[0]
                    make_args.append(option[1])
            hex_file += "-" + language
            make_args.append("TRANSLATIONS=" + language.upper())
            print hex_file
            call(["make", "clean"])
            call(make_args)
            shutil.copyfile("open9x.hex", "../binaries/" + hex_file + ".hex")
        
        try:
            index = states.index(False)
        except:
            break
        for i in range(index):
            states[i] = False
        states[index] = True

# stock board
generate("open9x-stock", "PCB=STD", options_stock)

# v4 board
generate("open9x-v4", "PCB=V4", options_v4)

# stamp
call(["make", "stamp"])
