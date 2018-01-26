#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This program parses sport.log files

from __future__ import division, print_function

import sys, struct

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


# CRC8 implementation with polynom = x^8+x^7+x^6+x^4+x^2+1 (0xD5)
crc8tab = [
    0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54,
    0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
    0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06,
    0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
    0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0,
    0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
    0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2,
    0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
    0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9,
    0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
    0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B,
    0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
    0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D,
    0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
    0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F,
    0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
    0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB,
    0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
    0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9,
    0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
    0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F,
    0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
    0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D,
    0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
    0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26,
    0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
    0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74,
    0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
    0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82,
    0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
    0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0,
    0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9
]

def crc8(buffer):
    crc = 0
    for c in buffer:
        crc = crc8tab[crc ^ c]
    return crc

def ParseGPS(payload):
    lat, long, speed, head, alt, numsat = struct.unpack('>iiHHHB', bytes(bytearray(payload)))          # bytes(bytearray) casting is required for python 2.7.3 compatibility
    return "[GPS] lat:%f long:%f speed:%d heading:%d alt:%d numsat:%d" % (lat / 1e7, long / 1e7, speed / 100, head / 100, alt - 1000, numsat)

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
    length = packet[1]
    command = packet[2]
    payload = packet[3:-1]
    crc = packet[-1]
    if crc != crc8(packet[2:-1]):
        print("[%s]" % timeData, dump(packet), "[CRC error]")
        return
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
            print("Skipped 1 bytex", dump(crossfireDataBuff[:1]))
            crossfireDataBuff = crossfireDataBuff[1:]
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
