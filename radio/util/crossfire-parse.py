#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This program parses sport.log files

from __future__ import division, print_function

import sys

lineNumber = 0
crossfireDataBuff = []

crossfire_types = [
    "UINT8",
    "INT8",
    "UINT16",
    "INT16",
    "UINT32",
    "INT32",
    "UINT64",
    "INT64",
    "FLOAT",
    "TEXT_SELECTION",
    "STRING",
    "FOLDER",
    "INFO",
    "COMMAND",
    "VTX",
]

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
    return '[Device] 0x%02x "%s" %d parameters' % (payload[1], "".join([chr(c) for c in payload[2:-14]]), payload[-1])

def ParseFieldsRequest(payload):
    return '[Fields request]'

def ParseFieldRequest(payload):
    return '[Field request] device=0x%02x field=%d' % (payload[1], payload[2])

def ParseField(payload):
    name = ""
    i = 6
    try:
        while payload[i] != 0:
            name += chr(payload[i])
            i += 1
        i += 1
        return '[Field] %s device=0x%02x field=%d parent=%d type=%s' % (name, payload[1], payload[2], payload[4], crossfire_types[payload[5] & 0x7f])
    except:
        return '[Exception]'

parsers = (
    (0x02, ParseGPS),
    (0x08, ParseBattery),
    (0x14, ParseLinkStatistics),
    (0x1E, ParseAttitude),
    (0x21, ParseFlightMode),
    (0x28, ParsePingDevices),
    (0x29, ParseDevice),
    (0x2a, ParseFieldsRequest),
    (0x2b, ParseField),
    (0x2c, ParseFieldRequest),
)

def ParsePacket(packet):
    len = packet[1]
    command = packet[2]
    payload = packet[3:-1]
    crc = packet[-1]
    for id, parser in parsers:
        if id == command:
            print("[%s]" % timeData, dump(packet), parser(payload))
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
        if length < 2 or length > 0x40:
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
    timeData = parts[0].strip()
    crossfireData = parts[1].strip()
    # print "sd: %s" % sportData
    ParseData(crossfireData)
