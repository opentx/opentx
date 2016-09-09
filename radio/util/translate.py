#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Imports
import argparse
import codecs
import sys

translations = {'cz_reduced': [('\\200', u'á'),
                       ('\\201', u'č'),
                       ('\\201', u'Č'),
                       ('\\202', u'é'),
                       ('\\203', u'ě'),
                       ('\\203', u'Ě'),
                       ('\\204', u'í'),
                       ('\\205', u'ó'),
                       ('\\206', u'ř'),
                       ('\\207', u'š'),
                       ('\\207', u'Š'),
                       ('\\210', u'ú'),
                       ('\\211', u'ů'),
                       ('\\212', u'ý'),
                       ('\\213', u'Á'),
                       ('\\214', u'Í'),
                       ('\\215', u'Ř'),
                       ('\\216', u'Ý'),
                       ('\\217', u'ž'),
                       ('\\217', u'Ž'),
                       ('\\220', u'É')],

                'cz': [('\\200', u'ě'),
                       ('\\201', u'š'),
                       ('\\202', u'č'),
                       ('\\203', u'ř'),
                       ('\\204', u'ž'),
                       ('\\205', u'ý'),
                       ('\\206', u'á'),
                       ('\\207', u'í'),
                       ('\\210', u'é'),
                       ('\\211', u'ň'),
                       ('\\212', u'ó'),
                       ('\\213', u'ů'),
                       ('\\214', u'ú'),
                       ('\\215', u'ď'),
                       ('\\216', u'ť'),
                       ('\\217', u'Ě'),
                       ('\\220', u'Š'),
                       ('\\221', u'Č'),
                       ('\\222', u'Ř'),
                       ('\\223', u'Ž'),
                       ('\\224', u'Ý'),
                       ('\\225', u'Á'),
                       ('\\226', u'Í'),
                       ('\\227', u'É'),
                       ('\\230', u'Ň'),
                       ('\\231', u'Ó'),
                       ('\\232', u'Ú'),
                       ('\\233', u'Ů'),
                       ('\\234', u'Ď'),
                       ('\\235', u'Ť')],

                'de': [('\\200', u'Ä'),
                       ('\\201', u'ä'),
                       ('\\202', u'Ö'),
                       ('\\203', u'ö'),
                       ('\\204', u'Ü'),
                       ('\\205', u'ü'),
                       ('\\206', u'ß')],

                'es': [('\\200', u'Ñ'),
                       ('\\201', u'ñ')],

                'fi': [('\\200', u'å'),
                       ('\\201', u'ä'),
                       ('\\202', u'ö'),
                       ('\\203', u'Å'),
                       ('\\204', u'Ä'),
                       ('\\205', u'Ö')],

                'fr': [('\\200', u'é'),
                       ('\\201', u'è'),
                       ('\\202', u'à'),
                       ('\\203', u'î'),
                       ('\\204', u'ç')],

                'it': [('\\200', u'à'),
                       ('\\201', u'ù')],

                'pl': [('\\200', u'ą'),
                       ('\\201', u'ć'),
                       ('\\202', u'ę'),
                       ('\\203', u'ł'),
                       ('\\204', u'ń'),
                       ('\\205', u'ó'),
                       ('\\206', u'ś'),
                       ('\\207', u'ż'),
                       ('\\210', u'ź'),
                       ('\\211', u'Ą'),
                       ('\\212', u'Ć'),
                       ('\\213', u'Ę'),
                       ('\\214', u'Ł'),
                       ('\\215', u'Ń'),
                       ('\\216', u'Ó'),
                       ('\\217', u'Ś'),
                       ('\\220', u'Ż'),
                       ('\\221', u'Ź')],

                'pt': [('\\200', u'Á'),
                       ('\\201', u'á'),
                       ('\\202', u'Â'),
                       ('\\203', u'â'),
                       ('\\204', u'Ã'),
                       ('\\205', u'ã'),
                       ('\\206', u'À'),
                       ('\\207', u'à'),
                       ('\\210', u'Ç'),
                       ('\\211', u'ç'),
                       ('\\212', u'É'),
                       ('\\213', u'é'),
                       ('\\214', u'Ê'),
                       ('\\215', u'ê'),
                       ('\\216', u'Í'),
                       ('\\217', u'í'),
                       ('\\218', u'Ó'),
                       ('\\219', u'ó'),
                       ('\\220', u'Ô'),
                       ('\\221', u'ô'),
                       ('\\222', u'Õ'),
                       ('\\223', u'õ'),
                       ('\\224', u'Ú'),
                       ('\\225', u'ú'), ],

                'se': [('\\200', u'å'),
                       ('\\201', u'ä'),
                       ('\\202', u'ö'),
                       ('\\203', u'Å'),
                       ('\\204', u'Ä'),
                       ('\\205', u'Ö')],

                'en': [],

                'nl': [],

                'all': [('\\306', u'Δ'),
                        ('\\173', u'~'),
                        ('\\036', u'\\n'),
                        ('\\035', u'\\t')],
                }

# Take care of command line options
parser = argparse.ArgumentParser(description='Encoder for open9x translations')
parser.add_argument('input', action="store", help="Input file name")
parser.add_argument('output', action="store", help="Output file name")
parser.add_argument('language', action="store", help="Two letter language identifier")
parser.add_argument("--reverse", help="Reversed char conversion (from number to char)", action="store_true")
args = parser.parse_args()

if args.language not in translations:
    parser.error(args.language + ' is not a supported language. Try one of the supported ones: ' + str(list(translations.keys())))
    sys.exit()

if args.reverse:
    for translation in translations:
        translations[translation] = [(after, before) for (before, after) in translations[translation]]

# Read the input file into a buffer
in_file = codecs.open(args.input, "r", "utf-8")

# Write the result to a temporary file
out_file = codecs.open(args.output, 'w', 'utf-8')

for line in in_file.readlines():
    # Do the special chars replacements
    for after, before in translations[args.language] + translations["all"]:
        line = line.replace(before, after)
    if line.startswith("#define ZSTR_"):
        before = line[32:-2]
        after = ""
        for c in before:
            if ord(c) >= ord('A') and ord(c) <= ord('Z'):
                c = "\\%03o" % (ord(c) - ord('A') + 1)
            elif ord(c) >= ord('a') and ord(c) <= ord('z'):
                c = "\\%03o" % (-ord(c) + ord('a') + 255)
            elif ord(c) >= ord('0') and ord(c) <= ord('9'):
                c = "\\%03o" % (ord(c) - ord('0') + 27)
            after = after + c
        line = line[:32] + after + line[-2:]
    out_file.write(line)

out_file.close()
in_file.close()
