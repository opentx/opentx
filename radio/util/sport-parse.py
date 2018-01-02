#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This program parses sport.log files

from __future__ import division, print_function

import sys
import struct


START_STOP = '\x7e'
BYTESTUFF = '\x7d'
STUFF_MASK = '\x20'

DATA_FRAME = 0x10


lineNumber = 0
sportDataBuff = ""

quiet = False


def ParseFlVSS(packet, dataId, prim, appId, data, crc):
    # if dataId != 0xa1:
    #     return
    print("packet: %s (%4d)" % (dump(packet), lineNumber), end=' ')
    cells = (data & 0xF0) >> 4
    battnumber = data & 0xF
    voltage1 = ((data & 0x000FFF00) >> 8) // 5
    voltage2 = ((data & 0xFFF00000) >> 20) // 5
    print(" FLVSS: no cells: %d, cell: %d: voltages: %0.2f %0.2f" % (cells, battnumber, voltage1 / 100., voltage2 / 100.))


def ParseASS(packet, dataId, prim, appId, data, crc):
    print("packet: %s (%4d)" % (dump(packet), lineNumber), end=' ')
    print(" ASS: %dkm/h" % (float(data) / 10))


def ParseRSSI(packet, dataId, prim, appId, data, crc):
    print("packet: %s (%4d)" % (dump(packet), lineNumber), end=' ')
    print(" RSSI: %d" % (data & 0xFF))


def ParseAdc(packet, dataId, prim, appId, data, crc):
    print("packet: %s (%4d)" % (dump(packet), lineNumber), end=' ')
    print(" A%d: %d" % (dataId - 0xf102, data & 0xFF))


def ParseBatt(packet, dataId, prim, appId, data, crc):
    print("packet: %s (%4d)" % (dump(packet), lineNumber), end=' ')
    print(" Batt: %d" % (data & 0xFF))


def ParseRAS(packet, dataId, prim, appId, data, crc):
    print("packet: %s (%4d)" % (dump(packet), lineNumber), end=' ')
    print(" RAS: %d" % (data & 0xFF))


def ParseVersion(packet, dataId, prim, appId, data, crc):
    print("packet: %s (%4d)" % (dump(packet), lineNumber), end=' ')
    print(" VERSION: %d" % data)


def ParseAirSpeed(packet, dataId, prim, appId, data, crc):
    print("packet: %s (%4d)" % (dump(packet), lineNumber), end=' ')
    print(" Aspd: %.1f km/h" % (data / 10.0))


appIdParsers = (
    (0x0300, 0x030f, ParseFlVSS),
    (0x0a00, 0x0a0f, ParseASS),
    (0xf101, 0xf101, ParseRSSI),
    (0xf102, 0xf103, ParseAdc),
    (0xf104, 0xf104, ParseBatt),
    (0xf105, 0xf105, ParseRAS),
    (0xf106, 0xf106, ParseVersion),
)


def dump(data, maxLen=None):
    if maxLen and len(data) > maxLen:
        data = data[:maxLen]
    return ":".join("{:02x}".format(ord(c)) for c in data)


def CheckSportCrc(packet):
    return True


def ParseSportPacket(packet):
    global lineNumber
    (dataId, prim, appId, data, crc) = struct.unpack('<BBHIB', packet)
    # print "dataId:%02x, prim:%02x, appId:%04x, data:%08x, crc:%02x)\n" % (dataId, prim, appId, data, crc)
    if prim != DATA_FRAME:
        print("unknown prim: %02x for packet %s in line %s" % (prim, dump(packet), lineNumber))
    # process according to appId
    for firstId, lastId, parser in appIdParsers:
        if firstId <= appId <= lastId:
            parser(packet, dataId, prim, appId, data, crc)
            return
    # no parser found
    if not quiet:
        print("\tdataId:%02x, prim:%02x, appId:%04x, data:%08x, crc:%02x)" % (dataId, prim, appId, data, crc))
        print("\tparser for appId %02x not implemented" % appId)


def ParsePacket(packet):
    global lineNumber
    # unstuff packet
    while True:
        pos = packet.find(BYTESTUFF)
        if pos == -1:
            break
        # print "found stuff at %d in %s" % (pos, dump(packet, 20))
        # if we have nex char, then unstuff it
        if len(packet) > pos + 1:
            unstuffed = ord(packet[pos]) ^ ord(STUFF_MASK)
            # print "unstuffed data: %02x" % unstuffed
            # buffer[pos] = chr(unstuffed)
            packet = packet[:pos] + chr(unstuffed) + packet[pos + 2:]
            # print "buff after unstuff                 %s" % dump(packet, 20)
        else:
            # missing data, wait for more data
            print("unstuff missing data")
            return

    # print "packet: %s" % dump(packet, 10)
    if len(packet) == 9:
        # valid sport packet length
        # print "\npacket: %s @%d" % (dump(packet), lineNumber)
        # check crc
        if not CheckSportCrc(packet):
            print("error: wrong CRC for packet %s at line %d" % (dump(packet), lineNumber))
        ParseSportPacket(packet)
    else:
        if len(packet) > 1:
            print("warning: wrong length %s for packet %s at line %d" % (len(packet), dump(packet, 10), lineNumber))


def ParseSportData(data):
    global sportDataBuff
    # convert from hex
    parts = data.split(' ')
    binData = [chr(int(hex, 16)) for hex in parts]
    # print binData
    data = ''.join(binData)
    sportDataBuff += data
    # process whole packets
    while True:
        # find whole frame
        # remove all data before START_STOP
        # find next START_STOP
        # print "start: %s" % dump(sportDataBuff, 10)
        posStop = sportDataBuff.find(START_STOP)
        # process pacet between two START_STOPs
        if posStop > -1:
            # print "found stop at %d" % posStop
            ParsePacket(sportDataBuff[:posStop])
            # remove processed data
            sportDataBuff = sportDataBuff[posStop + 1:]
            # print "after: %s" % dump(sportDataBuff, 10)
        else:
            break


inputFile = None

if len(sys.argv) > 1:
    inputFile = sys.argv[1]

# open input
if inputFile:
    inp = open(inputFile, 'r')
else:
    inp = sys.stdin

while True:
    line = inp.readline()
    lineNumber += 1
    if len(line) == 0:
        break
    line = line.strip('\r\n')
    if len(line) == 0:
        continue
    # print line
    parts = line.split(': ')
    if len(parts) < 2:
        print("weird data: \"%s\" at line %d" % (line, lineNumber))
        continue
    sportData = parts[1].strip()
    # print "sd: %s" % sportData
    ParseSportData(sportData)
