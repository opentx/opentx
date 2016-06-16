#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This program parses sport.log files

from __future__ import division, print_function

import sys

lineNumber = 0
crossfireDataBuff = []

def dump(data, maxLen=None):
    if maxLen and len(data) > maxLen:
        data = data[:maxLen]
    return " ".join("{:02x}".format(c) for c in data)

def CheckCrc(packet):
    return True

def ParseGPS(payload):
    pass

def ParseBattery(payload):
    voltage = float((payload[0] << 8) + payload[1]) / 10
    current = float((payload[2] << 8) + payload[3]) / 10
    consumption = (payload[4] << 16) + (payload[5] << 8) + payload[6]
    return "[Battery] %.1fV %.1fA %dmAh" % (voltage, current, consumption)

def ParseLinkStatistics(payload):
    return "[Link Statistics] "

def ParseAttitude(payload):
    pitch = float((payload[0] << 8) + payload[1]) / 1000
    roll = float((payload[2] << 8) + payload[3]) / 1000
    yaw = float((payload[4] << 8) + payload[5]) / 1000
    return "[Attitude] pitch=%.3f roll=%.3f yaw=%.3f" % (pitch, roll, yaw)

def ParseFlightMode(payload):
    return '[Flight Mode] "%s"' % "".join([chr(c) for c in payload[:-1]])

def ParsePingDevices(_):
    return '[Ping Devices]'

def ParseDevice(payload):
    return '[Device] "%s" %d parameters' % ("".join([chr(c) for c in payload[2:-14]]), payload[-1])

def ParseFieldsRequest(payload):
    return '[Fields request] Device=0x%02x' % payload[0]

parsers = (
    (0x02, ParseGPS),
    (0x08, ParseBattery),
    (0x14, ParseLinkStatistics),
    (0x1E, ParseAttitude),
    (0x21, ParseFlightMode),
    (0x28, ParsePingDevices),
    (0x29, ParseDevice),
    (0x2a, ParseFieldsRequest),
)

def ParsePacket(packet):
    len = packet[1]
    command = packet[2]
    payload = packet[3:-1]
    crc = packet[-1]
    for id, parser in parsers:
        if id == command:
            print("(%d)" % lineNumber, dump(packet), parser(payload))
            return
    print("(%d)" % lineNumber, dump(packet))

def ParseData(data):
    global crossfireDataBuff
    # convert from hex
    parts = data.split(' ')
    binData = [int(hex, 16) for hex in parts]
    # print binData
    crossfireDataBuff += binData
    # process whole packets
    while len(crossfireDataBuff) > 4:
        if crossfireDataBuff[0] != 0x00 and crossfireDataBuff[0] != 0xee and crossfireDataBuff[0] != 0xea:
            print("Skipped 1 byte", dump(crossfireDataBuff[:1]))
            crossfireDataBuff = crossfireDataBuff[1:]
            continue
        length = crossfireDataBuff[1]
        if length < 2 or length > 0x32:
            print("Skipped 2 bytes", dump(crossfireDataBuff[:2]))
            crossfireDataBuff = crossfireDataBuff[2:]
            continue
        if len(crossfireDataBuff) < length+2:
            break
        ParsePacket(crossfireDataBuff[:length+2])
        crossfireDataBuff = crossfireDataBuff[length+2:]

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
    crossfireData = parts[1].strip()
    # print "sd: %s" % sportData
    ParseData(crossfireData)
