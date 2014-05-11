#!/usr/bin/env python

import sys

filename = sys.argv[1]
fileout = sys.argv[2]

fr = open(filename, "rb")
fw = open(fileout, "w")

st = fr.read(16)

while st:
  for b in st:
    fw.write("0x%02x," % ord(b))
  fw.write("\n")
  st = fr.read(16)

fw.write("\n") 
fw.close()
