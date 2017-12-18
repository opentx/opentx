#!/usr/bin/env python

from __future__ import division, print_function

curr = 0
idx = 0
byte = 0


def push4bits(val):
    global curr, idx, byte
    val = val & 0x0f
    curr += val << idx
    idx += 4
    if idx == 8:
        print("0x%02X," % curr, end=' ')
        idx = 0
        curr = 0
        byte += 1
        if byte % 16 == 0:
            print()


cluster = 0


def pushCluster(val):
    global cluster
    push4bits(val)
    push4bits(val >> 4)
    push4bits(val >> 8)
    cluster += 1


def pushFile(size):
    sectors = size // 512
    count = sectors // 8
    for i in range(count - 1):
        pushCluster(cluster + 1)
    pushCluster(0xFFF)


def pushDisk(eeprom, flash):
    global curr, idx, byte, cluster
    curr = idx = byte = cluster = 0
    print("Disk with %dk EEPROM and %dk FLASH:" % (eeprom, flash))
    pushCluster(0xFF8)
    pushCluster(0xFFF)
    pushFile(flash * 1024)
    if eeprom > 0:
        pushFile(eeprom * 1024)
    while byte < 512:
        push4bits(0)
    print()


#pushDisk(32, 512)
pushDisk(32, 512)

pushDisk(0, 2048)
