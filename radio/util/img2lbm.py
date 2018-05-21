#!/usr/bin/env python

from __future__ import division, print_function

import sys
from PIL import Image

image = Image.open(sys.argv[1])
width, height = image.size

image_fmt = image.format

def writeSize(f, width, height):
    if lcdwidth > 255:
        f.write("%d,%d,%d,%d,\n" % (width % 256, width // 256, height % 256, height // 256))
    else:
        f.write("%d,%d,\n" % (width, height))


with open(sys.argv[2], "w") as f:
    lcdwidth = int(sys.argv[3])

    if len(sys.argv) > 4:
        what = sys.argv[4]
    else:
        for s in ("03x05", "04x06", "05x07", "08x10", "10x14", "22x38"):
            if s in sys.argv[2]:
                what = s
                break

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
                f.write("0x%02x," % value)
            f.write("\n")
    elif what == "4/4/4/4":
        constant = sys.argv[2].upper()[:-4]
        values = []
        for y in range(height):
            for x in range(width):
                pixel = image.getpixel((x, y))
                val = ((pixel[3] // 16) << 12) + ((pixel[0] // 16) << 8) + ((pixel[1] // 16) << 4) + ((pixel[2] // 16) << 0)
                values.append(str(val))
        f.write("const uint16_t __%s[] __ALIGNED = { %s };\n" % (constant, ",".join(values)))
        f.write("const Bitmap %s(BMP_ARGB4444, %d, %d, __%s);\n" % (constant, width, height, constant))
    elif what == "4/4/4/4-R":
        constant = sys.argv[2].upper()[:-4]
        values = []
        for y in range(height):
            for x in range(width):
                pixel = image.getpixel((width-x-1, height-y-1))
                val = ((pixel[3] // 16) << 12) + ((pixel[0] // 16) << 8) + ((pixel[1] // 16) << 4) + ((pixel[2] // 16) << 0)
                values.append(str(val))
        f.write("const uint16_t __%s[] __ALIGNED = { %s };\n" % (constant, ",".join(values)))
        f.write("const Bitmap %s(BMP_ARGB4444, %d, %d, __%s);\n" % (constant, width, height, constant))
    elif what == "5/6/5":
        constant = sys.argv[2].upper()[:-4]
        values = []
        for y in range(height):
            for x in range(width):
                pixel = image.getpixel((x, y))
                val = ((pixel[0] >> 3) << 11) + ((pixel[1] >> 2) << 5) + ((pixel[2] >> 3) << 0)
                values.append(str(val))
        f.write("const uint16_t __%s[] __ALIGNED = { %s };\n" % (constant, ",".join(values)))
        f.write("const Bitmap %s(BMP_RGB565, %d, %d, __%s);\n" % (constant, width, height, constant))
    elif what == "5/6/5-R":
        constant = sys.argv[2].upper()[:-4]
        values = []
        for y in range(height):
            for x in range(width):
                pixel = image.getpixel((width-x-1, height-y-1))
                val = ((pixel[0] >> 3) << 11) + ((pixel[1] >> 2) << 5) + ((pixel[2] >> 3) << 0)
                values.append(str(val))
        f.write("const uint16_t __%s[] __ALIGNED = { %s };\n" % (constant, ",".join(values)))
        f.write("const Bitmap %s(BMP_RGB565, %d, %d, __%s);\n" % (constant, width, height, constant))
    # elif what == "5/6/5/8":
    #     colors = []
    #     writeSize(f, width, height)
    #     for y in range(height):
    #         for x in range(width):
    #             pixel = image.pixel(x, y)
    #             val = ((Qt.qRed(pixel) >> 4) << 12) + ((Qt.qGreen(pixel) >> 4) << 7) + ((Qt.qBlue(pixel) >> 4) << 1)
    #             f.write("%d,%d,%d," % (val % 256, val // 256, Qt.qAlpha(pixel)))
    #         f.write("\n")
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
                f.write("0x%02x," % value)
            f.write("\n")
    elif what == "8bits":
        colors = []
        writeSize(f, width, height)
        image = image.convert(mode='L')
        for y in range(height):
            for x in range(width):
                value = image.getpixel((x, y))
                value = 0x0f - (value >> 4)
                f.write("0x%02x," % value)
            f.write("\n")
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
                    #print ("0x%02x (%d,%d,%d)" % (value,x,y,l))
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
