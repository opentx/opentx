#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys, os
import glob

from PIL import Image, ImageDraw, ImageFont

chars_en = u""" !"#$%&'()*+,-./0123456789:;<=>?°ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz~|≥"""
chars_fr = u"""éèàîç"""
chars_de = u"""ÄäÖöÜüß"""
chars_cz = u"""ěščřžýáíéňóůúďťĚŠČŘŽÝÁÍÉŇÓÚŮĎŤ"""
chars_es = u"""Ññ"""
chars_fi = u"""åäöÅÄÖ"""
chars_it = u"""àù"""
chars_pl = u"""ąćęłńóśżźĄĆĘŁŃÓŚŻŹ"""
chars_pt = u"""ÁáÂâÃãÀàÇçÉéÊêÍíÓóÔôÕõÚú"""
chars_se = u"""åäöÅÄÖ"""

COUNT_EXTRA_CHARS = 21

chars_extra = u"".join([chr(1+i) for i in range(COUNT_EXTRA_CHARS)])
chars = chars_en + chars_extra + chars_fr + chars_de + chars_cz + chars_es + chars_fi + chars_it + chars_pl + chars_se


def createFontBitmap(filename, fontname, fontsize, fontbold, foreground, background, coordsfile=True):

    font_filename = 'fonts/' + fontname + '/' + fontname
    if fontbold:
        font_filename = font_filename + '-Bold'
    else:
        font_filename = font_filename + '-Regular'

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
            if ord(c) <= 9:
                return 13
            else:
                return 15
        elif c == " ":
            return 4
        else:
            w, h = font.getsize(c)
            return w + 1

    def getFontWidth():
        width = 0
        for c in chars:
            width += getCharWidth(c)
        return width

    width = getFontWidth()
    image = Image.new('RGB', (width, fontsize + 4), background)
    draw  = ImageDraw.Draw(image)

    width = 0
    for c in chars:
        coords.append(width)

        if c in chars_extra:
            if extraImage and c == chars_extra[0]:
                image.paste(extraImage.copy(),(width,1))
        elif c == " ":
            pass
        else:
            w, h = font.getsize(c)
            draw.text( (width + 1, 1) , c, fill=foreground, font=font)

        width += getCharWidth(c)

    coords.append(width)

    image.save(filename + ".png")
    if coordsfile:
        with open(filename + ".specs", "w") as f:
            f.write(",".join(str(tmp) for tmp in coords))


# Main
if __name__ == "__main__":
    createFontBitmap(sys.argv[4], sys.argv[1], int(sys.argv[2]), sys.argv[3] == "True", (0,0,0), (255,255,255))
