#!/usr/bin/env python

from __future__ import division, print_function

import sys
import os.path
from PIL import Image

class RLE_encoder:
    RLE_BYTE = 0
    RLE_SEQ  = 1

    def __init__(self, out):
        self.state = self.RLE_BYTE
        self.count = 0
        self.prev_byte = None
        self.out = out

    def eq_prev_byte(self, in_byte):
        if self.prev_byte is None:
            return False
        return in_byte == self.prev_byte
        
    def encode_byte(self, in_byte):

        if self.state == self.RLE_BYTE:
            self.out(in_byte)
            if self.eq_prev_byte(in_byte):
                self.state = self.RLE_SEQ
                self.count = 0
            else:
                self.prev_byte = in_byte

        elif self.state == self.RLE_SEQ:
            if self.eq_prev_byte(in_byte):
                self.count += 1
                if self.count == 255:
                    self.out(self.count)
                    self.prev_byte = None
                    self.state = self.RLE_BYTE
            else:
                self.out(self.count)
                self.out(in_byte)
                self.prev_byte = in_byte
                self.state = self.RLE_BYTE

    def encode_end(self):
        if self.state == self.RLE_SEQ:
            self.out(self.count)

class dummy_encoder:
    def __init__(self, out):
        self.out = out

    def encode_byte(self, in_byte):
        self.out(in_byte)

    def encode_end(self):
        pass

def writeSize(f, width, height):
    if lcdwidth > 255:
        f.write("%d,%d,%d,%d,\n" % (width % 256, width // 256, height % 256, height // 256))
    else:
        f.write("%d,%d,\n" % (width, height))

def F_writeValue(f):
    def writeValue(value):
        f.write("0x%02x," % value)
    return writeValue

def F_getPixelCoord():
    def getCoord(x,y):
        return (x,y)
    return getCoord

def F_getPixelCoord_R(w,h):
    def getCoord(x,y):
        return (w-x-1, h-y-1)
    return getCoord

## MAIN ##

image = Image.open(sys.argv[1])
width, height = image.size

image_fmt = image.format
output_filename = sys.argv[2]

with open(output_filename, "w") as f:
    lcdwidth = int(sys.argv[3])

    if len(sys.argv) > 4:
        s = sys.argv[4].split('-')
        what = s[0]
        pixel_coord = F_getPixelCoord()
        if (len(s) > 1) and (s[1] == 'R'):
            pixel_coord = F_getPixelCoord_R(width,height)
    else:
        for s in ("03x05", "04x06", "05x07", "08x10", "10x14", "22x38"):
            if s in sys.argv[2]:
                what = s
                break

    extension = os.path.splitext(output_filename)[1]
    out = F_writeValue(f)

    if extension == ".rle" or ((len(sys.argv) > 5) and (sys.argv[5] == "rle")):
        encoder = RLE_encoder(out)
    else:
        encoder = dummy_encoder(out)

    if what == "1bit":
        rows = 1
        if len(sys.argv) > 5:
            rows = int(sys.argv[5])
        image = image.convert(mode='1')
        writeSize(f, width, height // rows)
        for y in range(0, height, 8):
            for x in range(width):
                value = 0
                for z in range(8):
                    if y + z < height:
                        if image_fmt == 'XBM':
                            if image.getpixel((x, y + z)) > 0:
                                value += 1 << z
                        else:
                            if image.getpixel((x, y + z)) == 0:
                                value += 1 << z
                encoder.encode_byte(value)
            f.write("\n")
        encoder.encode_end()
    elif what == "4bits":
        colors = []
        writeSize(f, width, height)
        image = image.convert(mode='L')
        for y in range(0, height, 2):
            for x in range(width):
                value = 0xFF
                gray1 = image.getpixel((x, y))
                if y + 1 < height:
                    gray2 = image.getpixel((x, y + 1))
                else:
                    gray2 = 255
                for i in range(4):
                    if (gray1 & (1 << (4 + i))):
                        value -= 1 << i
                    if (gray2 & (1 << (4 + i))):
                        value -= 1 << (4 + i)
                encoder.encode_byte(value)
            f.write("\n")
        encoder.encode_end()
    elif what == "8bits":
        colors = []
        writeSize(f, width, height)
        image = image.convert(mode='L')
        for y in range(height):
            for x in range(width):
                value = image.getpixel((x, y))
                value = 0x0f - (value >> 4)
                encoder.encode_byte(value)
            f.write("\n")
        encoder.encode_end()
    elif what == "4/4/4/4":
        constant = sys.argv[2].upper()[:-4]
        values = []
        writeSize(f, width, height)
        for y in range(height):
            for x in range(width):
                pixel = image.getpixel(pixel_coord(x, y))
                val = ((pixel[3] // 16) << 12) + ((pixel[0] // 16) << 8) + ((pixel[1] // 16) << 4) + ((pixel[2] // 16) << 0)
                encoder.encode_byte(val & 255)
                encoder.encode_byte(val >> 8)
        encoder.encode_end()
        #f.write("\n};\n")
        #if extension != ".rle":
        #    f.write("const Bitmap %s(BMP_ARGB4444, %d, %d, (uint16_t*)__%s);\n" % (constant, width, height, constant))
    elif what == "5/6/5":
        constant = sys.argv[2].upper()[:-4]
        values = []
        writeSize(f, width, height)
        for y in range(height):
            for x in range(width):
                pixel = image.getpixel(pixel_coord(x, y))
                val = ((pixel[0] >> 3) << 11) + ((pixel[1] >> 2) << 5) + ((pixel[2] >> 3) << 0)
                encoder.encode_byte(val & 255)
                encoder.encode_byte(val >> 8)
        encoder.encode_end()
        #f.write("\n};\n")
        #if extension != ".rle":
        #    f.write("const Bitmap %s(BMP_RGB565, %d, %d, (uint16_t*)__%s);\n" % (constant, width, height, constant))
    elif what == "03x05":
        image = image.convert(mode='L')
        for y in range(0, height, 5):
            for x in range(width):
                value = 0
                for z in range(5):
                    if image.getpixel((x, y + z)) == 0:
                        value += 1 << z
                f.write("0x%02x," % value)
            f.write("\n")
    elif what == "04x06":
        image = image.convert(mode='L')
        for y in range(0, height, 7):
            for x in range(width):
                value = 0
                for z in range(7):
                    if image.getpixel((x, y + z)) == 0:
                        value += 1 << z
                if value == 0x7f:
                    value = 0xff
                f.write("0x%02x," % value)
            f.write("\n")
    elif what == "05x07":
        image = image.convert(mode='L')
        for y in range(0, height, 8):
            for x in range(width):
                value = 0
                for z in range(8):
                    if image.getpixel((x, y + z)) == 0:
                        value += 1 << z
                f.write("0x%02x," % value)
            f.write("\n")
    elif what == "08x10":
        image = image.convert(mode='L')
        for y in range(0, height, 12):
            for x in range(width):
                skip = True
                for l in range(0, 12, 8):
                    value = 0
                    for z in range(8):
                        if l + z < 12:
                            if image.getpixel((x, y + l + z)) == 0:
                                value += 1 << z
                            else:
                                skip = False
                    if skip and l == 8:
                        value = 0xff
                    f.write("0x%02x," % value)
            f.write("\n")
    elif what == "10x14":
        image = image.convert(mode='L')
        for y in range(0, height, 16):
            for x in range(width):
                for l in range(0, 16, 8):
                    value = 0
                    for z in range(8):
                        if image.getpixel((x, y + l + z)) == 0:
                            value += 1 << z
                    f.write("0x%02x," % value)
            f.write("\n")
    elif what == "22x38":
        image = image.convert(mode='L')
        for y in range(0, height, 40):
            for x in range(width):
                for l in range(0, 40, 8):
                    value = 0
                    for z in range(8):
                        if image.getpixel((x, y + l + z)) == 0:
                            value += 1 << z
                    f.write("0x%02x," % value)
            f.write("\n")
    else:
        print("wrong argument", sys.argv[4])
