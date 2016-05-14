#!/usr/bin/env python

from __future__ import division
import sys

filename = sys.argv[1]
fileout = sys.argv[2]

# Read entire file
with open(filename, "rb") as fr:
    sts = fr.read()

# Parse into chunks of 16 bytes
sts = [sts[i:i+16] for i in range(0, len(sts), 16)]

with open(fileout, "w") as fw:
    for st in sts:
        for b in st:
            fw.write("0x%02x," % (ord(b) if isinstance(b, str) else b))
        fw.write("\n")
    fw.write("\n")
