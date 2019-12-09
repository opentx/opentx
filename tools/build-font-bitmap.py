#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import os
import sys
from PIL import Image, ImageDraw, ImageFont
from charset import get_chars, special_chars, extra_chars


class FontBitmap:
    def __init__(self, chars, font_size, font_name, cjk_font_name, foreground, background):
        self.chars = chars
        self.font_size = font_size
        self.foreground = foreground
        self.background = background
        self.font = self.load_font(font_name)
        self.cjk_font = self.load_font(cjk_font_name)
        self.extra_bitmap = self.load_extra_bitmap()

    def load_extra_bitmap(self):
        try:
            tools_path = os.path.dirname(os.path.realpath(__file__))
            path = os.path.join(tools_path, "../radio/src/fonts", "extra_%dpx.png" % self.font_size)
            extra_image = Image.open(path)
            return extra_image.convert('RGB')
        except IOError:
            return None

    def load_font(self, font_name):
        # print(font_name)
        for ext in (".ttf", ".otf"):
            tools_path = os.path.dirname(os.path.realpath(__file__))
            path = os.path.join(tools_path, "../radio/src/fonts", font_name + ext)
            if os.path.exists(path):
                return ImageFont.truetype(path, self.font_size)
        print("Font file %s not found" % font_name)

    @staticmethod
    def is_row_needed(px, y, width):
        for x in range(width):
            if px[x, y] != (255, 255, 255):
                return True
        return False

    @staticmethod
    def is_column_needed(px, x, height):
        for y in range(height):
            if sum(px[x, y]) <= 3 * 0xC6:
                return True
        return False

    def get_real_size(self, image):
        px = image.load()
        left = 0
        while left < image.width:
            if self.is_column_needed(px, left, image.height):
                break
            left += 1
        right = image.width - 1
        while right > left:
            if self.is_column_needed(px, right, image.height):
                break
            right -= 1
        top = 0
        while top < image.height:
            if self.is_row_needed(px, top, image.width):
                break
            top += 1
        bottom = image.height - 1
        while bottom > top:
            if self.is_row_needed(px, bottom, image.width):
                break
            bottom -= 1
        return left, top, right, bottom

    def draw_char(self, image, x, c, font):
        size = font.font.getsize(c)
        width = size[0][0]
        offset = size[1][0]
        char_image = Image.new("RGB", (width, image.height), self.background)
        draw = ImageDraw.Draw(char_image)
        draw.text((-offset, 0), c, fill=self.foreground, font=font)
        left, _, right, _ = self.get_real_size(char_image)
        if image:
            image.paste(char_image.crop((left, 0, right + 1, image.height)), (x, 0))
        return right - left + 1

    def generate(self, filename, generate_coords_file=True):
        coords = []
        image = Image.new("RGB", (len(self.chars) * self.font_size + 200, self.font_size + 20), self.background)

        width = 0
        for c in self.chars:
            coords.append(width)

            if c == " ":
                width += 4
            elif c in special_chars["cn"]:
                width += self.draw_char(image, width, c, self.cjk_font)
            elif c not in extra_chars:
                width += self.draw_char(image, width, c, self.font)
        coords.append(width)

        _, top, _, bottom = self.get_real_size(image)

        if self.extra_bitmap:
            image.paste(self.extra_bitmap.copy(), (width, top))
            width += self.extra_bitmap.width

        image = image.crop((0, top, width, bottom))
        coords.insert(0, bottom - top)

        image.save(filename + ".png")
        if generate_coords_file:
            with open(filename + ".specs", "w") as f:
                f.write(",".join(str(tmp) for tmp in coords))


def main():
    if sys.version_info < (3, 0, 0):
        print("%s requires Python 3. Terminating." % __file__)
        sys.exit(1)

    parser = argparse.ArgumentParser(description="Builder for OpenTX font files")
    parser.add_argument('--output', help="Output file name")
    parser.add_argument('--subset', help="Subset", default="all")
    parser.add_argument('--size', type=int, help="Font size")
    parser.add_argument('--font', help="Font name")
    parser.add_argument('--cjk-font', help="CJK font name")
    args = parser.parse_args()

    font = FontBitmap(get_chars(args.subset), args.size, args.font, args.cjk_font, (0, 0, 0), (255, 255, 255))
    font.generate(args.output)


if __name__ == "__main__":
    main()
