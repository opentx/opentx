#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys, os
import glob

from PIL import Image, ImageDraw, ImageFont

chars_en = u""" !"#$%&'()*+,-./0123456789:;<=>?°ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz~|≥→←↑↓↗↘↙↖△"""
chars_fr = u"""éèàîç"""
chars_de = u"""ÄäÖöÜüß"""
chars_cz = u"""ěščřžýáíéňóůúďťĚŠČŘŽÝÁÍÉŇÓÚŮĎŤ"""
chars_es = u"""Ññ"""
chars_fi = u"""åäöÅÄÖ"""
chars_it = u"""àù"""
chars_pl = u"""ąćęłńóśżźĄĆĘŁŃÓŚŻŹ"""
chars_pt = u"""ÁáÂâÃãÀàÇçÉéÊêÍíÓóÔôÕõÚú"""
chars_se = u"""åäöÅÄÖ"""

COUNT_EXTRA_CHARS = 12

chars_extra = u"".join([chr(1+i) for i in range(COUNT_EXTRA_CHARS)])
chars = chars_en + chars_extra + chars_fr + chars_de + chars_cz + chars_es + chars_fi + chars_it + chars_pl + chars_se


def createFontBitmap(filename, fontname, fontsize, fontbold, foreground, background, coordsfile=True):

    font_filename = 'fonts/DejaVuSansCondensed'
    if fontbold:
        font_filename = font_filename + '-Bold'
    font_filename = font_filename + '.ttf'

    coords = []
    print 'Font filename: %s' % font_filename
    font = ImageFont.truetype(font_filename, fontsize)

    extraImage = None
    extraWidth, extraHeight = 0, 0
    extraFilename = 'fonts/extra_%dpx.png' % fontsize

    try:
        extraImage = Image.open(extraFilename)
        extraImage = extraImage.convert('RGB');
        extraWidth, extraHeight = extraImage.size

        extraWidth = extraWidth / COUNT_EXTRA_CHARS

    except IOError:
        print 'Missing extra symbol filename %s' % extraFilename

    def getCharWidth(c):
        if c in chars_extra:
            return extraWidth
        elif c == " ":
            return 4
        else:
            w, h = font.getsize(c)
            #if fontsize >= 24:
            #    return w + 3
            #else:
            return w + 1

    def getFontWidth():
        width = 0
        for c in chars:
            width += getCharWidth(c)
        return width

    # def getFontTopBottom():
    #     top, bottom = 0, 0
    #     for i, c in enumerate(chars):
    #         w, h = font.getsize(chars[i])
    #         top = min(top, r.top())
    #         bottom = max(bottom, r.bottom())
    #     return top, bottom

    width = getFontWidth()
    # top, bottom = getFontTopBottom()
    image = Image.new('RGB', (width, fontsize + 4), background)
    draw  = ImageDraw.Draw(image)

    width = 0
    for c in chars:
        coords.append(width)

        if c in chars_extra:
            if extraImage and c == chars_extra[0]:
                for x in range(extraWidth * COUNT_EXTRA_CHARS):
                    for y in range(extraHeight):
                        rgb = extraImage.getpixel((x, y))
                        draw.point([x+width, y], fill=rgb)
        elif c == " ":
            pass
        else:
            #(0,0), chars, fill=(0,0,0), font=font
            w, h = font.getsize(c)
            draw.text( (width + 1, 1) , c, fill=foreground, font=font)

        width += getCharWidth(c)

    coords.append(width)

    image.save(filename + ".png")
    if coordsfile:
        with open(filename + ".specs", "w") as f:
            f.write(",".join(str(tmp) for tmp in coords))

    return coords

if __name__ == "__main__":
    createFontBitmap(sys.argv[4], sys.argv[1], int(sys.argv[2]), sys.argv[3] == "True", (0,0,0), (255,255,255))
