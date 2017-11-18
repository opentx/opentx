#!/usr/bin/env python

# The commands used to compare 2.2 flash usage
# on 2.1 release: make PCB=9X EXT=FRSKY VOICE=YES AUDIO=YES AUTOSOURCE=YES AUTOSWITCH=YES HELI=YES TEMPLATES=YES GAUGES=NO
#   => 64818 (program) + 3236 (data)
# on 2.2 release: cmake -DPCB=9X -DTELEMETRY=FRSKY -DAUDIO=YES -DVOICE=YES -DHELI=YES -DTEMPLATES=YES ~/git/opentx
#   => 64828 (program) + 3236 (data)

from __future__ import print_function
import os
import sys
import subprocess

index = -1
size = None

options = sys.argv[1:]
srcdir = os.path.dirname(os.path.realpath(__file__)) + "/../.."
buildir = os.getcwd()

while 1:
    sys.stdout.flush()
    index += 1

    os.chdir(srcdir)
    cmd = ["git", "reset", "--hard", "FETCH_HEAD~%d" % index]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = proc.communicate()
    if proc.returncode != 0:
        print("HEAD~%d git reset failed" % index)
        continue

    os.chdir(buildir)
    cmd = ["cmake"] + options + [srcdir]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = proc.communicate()
    if proc.returncode != 0:
        print("HEAD~%d cmake failed" % index)
        continue

    cmd = ["make", "-j4", "firmware"]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = proc.communicate()
    if proc.returncode != 0:
        print("HEAD~%d make firmware failed" % index)
        continue

    if os.path.isfile("firmware.bin"):
        oldsize = os.stat("firmware.bin").st_size
    else:
        oldsize = int(subprocess.check_output('avr-size -A firmware.hex | grep Total | cut -f2- -d " "', shell=True))
    if size:
        if size > oldsize:
            print("HEAD~%d %d: increase by %d bytes" % (index-1, size, size-oldsize))
        elif size < oldsize:
            print("HEAD~%d %d: decrease by %d bytes" % (index-1, size, oldsize-size))
        else:
            print("HEAD~%d %d" % (index-1, size))
    size = oldsize


