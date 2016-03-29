#!/usr/bin/env python

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
    cmd = ["git", "reset", "--hard", "origin/next~%d" % index]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = proc.communicate()
    if proc.returncode != 0:
        print "HEAD~%d git reset failed" % index
        continue

    os.chdir(buildir)
    cmd = ["cmake"] + options + [srcdir]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = proc.communicate()
    if proc.returncode != 0:
        print "HEAD~%d cmake failed" % index
        continue

    cmd = ["make", "-j4", "firmware"]
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output, error = proc.communicate()
    if proc.returncode != 0:
        print "HEAD~%d make firmware failed" % index
        continue

    oldsize = int(subprocess.check_output('avr-size -A %s | grep Total | cut -f2- -d " "' % "firmware.hex", shell=True))
    if size:
        if size > oldsize:
            print "HEAD~%d %d: increase by %d bytes" % (index-1, size, size-oldsize)
        elif size < oldsize:
            print "HEAD~%d %d: decrease by %d bytes" % (index-1, size, oldsize-size)
        else:
            print "HEAD~%d %d" % (index-1, size)
    size = oldsize


