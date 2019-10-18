#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import os
from PIL import Image, ImageDraw, ImageFont
from charset import get_chars, special_chars, extra_chars


class FontBitmap:
    def __init__(self, chars, font_size, font_name, cjk_font_name, foreground, background):
        self.chars = chars
        self.font_size = font_size
        self.foreground = foreground
        self.background = background
        self.font, self.font_offset = self.load_font(font_name)
        self.cjk_font, self.cjk_font_offset = self.load_font(cjk_font_name)
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
        font_name, font_offset = font_name.split(":")
        for ext in (".ttf", ".otf"):
            tools_path = os.path.dirname(os.path.realpath(__file__))
            path = os.path.join(tools_path, "../radio/src/fonts", font_name + ext)
            if os.path.exists(path):
                return ImageFont.truetype(path, self.font_size), int(font_offset)
        print("Font file %s not found" % font_name)

    def get_char_width(self, c):
        if c in extra_chars:
            if self.extra_bitmap:
                # Extra characters 16px
                # if fontsize == 16:
                if extra_chars.index(c) < 9:
                    return 13
                else:
                    return 15
            else:
                return 0
        elif c == " ":
            return 4
        elif c in special_chars["cn"]:
            w, h = self.cjk_font.getsize(c)
            return w + 1
        else:
            w, h = self.font.getsize(c)
            return w + 1

    def get_width(self):
        width = 0
        for c in self.chars:
            width += self.get_char_width(c)
        return width

    def generate(self, filename, generate_coords_file=True):
        coords = [self.font_size + 4]
        width = self.get_width()
        image = Image.new('RGB', (width, self.font_size + 4), self.background)
        draw = ImageDraw.Draw(image)

        width = 0
        for c in self.chars:
            coords.append(width)

            if c in extra_chars:
                if self.extra_bitmap and c == extra_chars[0]:
                    if self.font_size == 16:
                        offset = 1
                    else:
                        offset = self.font_size % 2
                    image.paste(self.extra_bitmap.copy(), (width, offset))
            elif c == " ":
                pass
            elif c in special_chars["cn"]:
                offset = self.cjk_font.getoffset(c)[0]
                draw.text((width - offset, self.cjk_font_offset), c, fill=self.foreground, font=self.cjk_font)
            else:
                offset = self.font.getoffset(c)[0]
                draw.text((width - offset, self.font_offset), c, fill=self.foreground, font=self.font)

            width += self.get_char_width(c)

        coords.append(width)

        image.save(filename + ".png")
        if generate_coords_file:
            with open(filename + ".specs", "w") as f:
                f.write(",".join(str(tmp) for tmp in coords))


def main():
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
