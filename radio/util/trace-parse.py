#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This program parses trace output files


import os
import sys


lineNumber = 0
allocations = {}
errors = []
noAlloc = 0
noFree = 0
maxAllocatedRam = 0

def getAddress(hexaddr):
  return int(hexaddr, 16)

def getAddresAndSize(hexaddrandsize):
  parts = hexaddrandsize.split('[')
  address = 0
  try:
    address = getAddress(parts[0])
  except:
    pass
  size = int(parts[1][:-1])
  # print "getAddresAndSize(): %s = %x[%d]" % (hexaddrandsize, address, size)
  return (address, size)

def doFree(address):
  #print "free %x" % address
  global noFree
  noFree = noFree + 1
  if address in allocations:
    # all ok
    del allocations[address]
  else:
    # free of unallocated ram
    errors.append("free 0x%x at line %d" % (address, lineNumber))

def doRealloc(frm, to):
  global noAlloc
  #print "realloc %s -> %s" % ( str(frm), str(to))
  if frm[0] != 0:
    #reallocation
    doFree(frm[0])
  if to[0] > 0 and to[1] > 0:
    allocations[to[0]] = to[1]
    noAlloc = noAlloc + 1
  # if frm[0] in allocations:
  #   # all ok
  #   del allocations[address]
  # else:
  #   # free of unallocated ram
  #   errors.append("free 0x%x at line %d" % (address, lineNumber))


def parseLine(line):
  if line.startswith('free '):
    doFree(getAddress(line.split(' ')[1]))
    return True
  if line.startswith('realloc '):
    doRealloc(getAddresAndSize(line.split(' ')[1]), getAddresAndSize(line.split(' ')[3]))
    return True
  return False

def boundsCheck():
  global maxAllocatedRam
  if len(allocations) == 0: return
  data = allocations.items()
  data.sort(key = lambda x: x[0])
  allocatedRam = 0
  for n in range(len(data)-1):
    endAddr = data[n][0] + data[n][1]
    nextAddr = data[n+1][0]
    allocatedRam = allocatedRam + data[n][1]
    # print "%x %x" % (endAddr, nextAddr)
    if nextAddr < endAddr:
      print "logical error"
      sys.exit(1)

    if endAddr >= nextAddr:
      errors.append("bounds error: %x" % data[n][0])
    else:
      # print "free space %d" % (nextAddr-endAddr)
      pass
  allocatedRam = allocatedRam + data[-1][1]
  if maxAllocatedRam < allocatedRam:
    maxAllocatedRam = allocatedRam

inputFile = None
if len(sys.argv) > 1:
  inputFile = sys.argv[1]

#open input
if inputFile:
  inp = open(inputFile, 'r')
else:
  inp = sys.stdin

while True:
  line = inp.readline()
  lineNumber += 1
  if len(line) == 0: break
  line = line.strip('\r\n')
  if len(line) == 0: continue
  #print line
  if parseLine(line):
    boundsCheck()
  # parts = line.split(': ')
  # if len(parts) < 2:
  #   print "weird data: \"%s\" at line %d" % (line, lineNumber)
  #   continue
  # sportData = parts[1].strip()
  # # print "sd: %s" % sportData
  # ParseSportData(sportData)

#do report
print "Mem usage report:"
print "\tparsed %d lines" % lineNumber
print "\tnumber of allocations: %d" % noAlloc
print "\tnumber of free: %d" % noFree
print "\tallocated RAM peak size: %d" % maxAllocatedRam
print "Remaining allocations: %d" % len(allocations)
remainingRam = 0
for (address, size) in allocations.iteritems():
  print "\t0x%x %d" % (address, size)
  remainingRam = remainingRam + size
if remainingRam > 0:
  print "Remaining allocated ram: %d" % remainingRam
print "Detected problems: %d" % len(errors)
for (err) in errors:
  print err
