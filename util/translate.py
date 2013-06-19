#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Imports
import argparse
import os
import os.path
import codecs

translations = {'cz': [('\\200', u'á'),
                       ('\\201', u'č'),
                       ('\\201', u'Č'),
                       ('\\202', u'é'),
                       ('\\203', u'ě'),
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

                'de': [('\\200', u'Ä'),
                       ('\\201', u'ä'),
                       ('\\202', u'Ö'),
                       ('\\203', u'ö'),
                       ('\\204', u'Ü'),
                       ('\\205', u'ü'),
                       ('\\206', u'ß')],

                'es': [('\\200', u'Ñ'),
                       ('\\201', u'ñ')],

                'fr': [('\\200', u'é'),
                       ('\\201', u'è'),
                       ('\\202', u'à'),
                       ('\\203', u'î'),
                       ('\\204', u'ç')],

                'it': [('\\200', u'à'),
                       ('\\201', u'ù')],

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
                       ('\\225', u'ú'),],

                'se': [('\\200', u'å'),
                       ('\\201', u'ä'),
                       ('\\202', u'ö'),
                       ('\\203', u'Å'),
                       ('\\204', u'Ä'),
                       ('\\205', u'Ö')],
               }

# Take care of command line options
parser = argparse.ArgumentParser(description='Encoder for open9x translations')
parser.add_argument('input', action="store", help="Input file name")
parser.add_argument('output', action="store", help="Output file name")
parser.add_argument('language', action="store", help="Two letter language identifier")
parser.add_argument("--reverse", help="Reversed char conversion (from number to char)", action="store_true")
args =  parser.parse_args()

if args.language not in translations:
    parser.error(args.language  + ' is not a supported language. Try one of the supported ones: ' + str(translations.keys()))
    system.exit()
    
# Read the input file into a buffer
in_file = codecs.open( args.input, "r", "utf-8" )
text = in_file.read()
in_file.close()

# Do the replacements
for before, after in translations[args.language]:
    if args.reverse:
        text = text.replace(before, after)
    else:
        text = text.replace(after, before)
		
# Write the result to a temporary file
out_file = codecs.open(args.output, 'w', 'utf-8')
out_file.write( text )
out_file.close()


