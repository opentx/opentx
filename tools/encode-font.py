#!/usr/bin/env python

import sys
import argparse
from PIL import Image


class FontEncoder:
    def __init__(self, filename):
        self.f = open(filename, "w")

    def write(self, value):
        self.f.write("0x%02x," % value)
    #
    # def write_size(self, width, height):
    #     self.f.write("%d,%d,\n" % (width, height))

    def encode(self, image, step):
        image = image.convert(mode='L')
        width, height = image.size
        for y in range(0, height, step):
            for x in range(width):
                for l in range(0, step, 8):
                    value = 0
                    for z in range(min(8, step)):
                        # print(y + l + z)
                        if image.getpixel((x, y + l + z)) == 0:
                            value += 1 << z
                    if step < 8 and value == 0x7f:
                        value = 0xff
                    self.write(value)
            # self.f.write("\n")

    def encode_special(self, image, step):
        image = image.convert(mode='L')
        width, height = image.size
        for y in range(0, height, step):
            for x in range(width):
                skip = True
                for l in range(0, step, 8):
                    value = 0
                    for z in range(min(8, step)):
                        if l + z < 12:
                            if image.getpixel((x, y + l + z)) == 0:
                                value += 1 << z
                            else:
                                skip = False
                    if skip and l == 8:
                        value = 0xff
                    self.write(value)


def main():
    parser = argparse.ArgumentParser(description='Fonts encoder')
    parser.add_argument('input', action="store", help="Input file name")
    parser.add_argument('output', action="store", help="Output file name")
    args = parser.parse_args()

    image = Image.open(args.input)
    output = args.output

    for s in ("03x05", "04x06", "05x07", "08x10", "10x14", "22x38"):
        if s in args.input:
            size = s
            break

    encoder = FontEncoder(output)

    if size == "03x05":
        encoder.encode(image, 5)
    elif size == "04x06":
        encoder.encode(image, 7)
    elif size == "05x07":
        encoder.encode(image, 8)
    elif size == "08x10":
        encoder.encode_special(image, 12)
    elif size == "10x14":
        encoder.encode(image, 16)
    elif size == "22x38":
        encoder.encode(image, 40)
    else:
        print("Unknown size", sys.argv[4])


if __name__ == "__main__":
    main()
