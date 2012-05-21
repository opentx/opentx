#!/usr/bin/env python
# -*- coding: cp1252 -*-

# Imports
import argparse
import os
import os.path

translations = {'se': [('\\200', 'å'),
                       ('\\201', 'ä'),
                       ('\\202', 'ö'),
                       ('\\203', 'Å'),
                       ('\\204', 'Ä'),
                       ('\\205', 'Ö')],
                
                'fr': [('\\200', 'à'),
                       ('\\201', 'è'),
                       ('\\202', 'è')],
                
                'de': [('\\200', 'Ä'),
                       ('\\201', 'ä'),
                       ('\\202', 'Ö'),
                       ('\\203', 'ö'),
                       ('\\204', 'Ü'),
                       ('\\205', 'ü'),
                       ('\\206', 'ß'),
                       ('\\207', '°')],
                                              
                'cz': [('\\200', 'á'),
                       ('\\201', 'è'),
                       ('\\202', 'é'),
                       ('\\203', 'ì'),
                       ('\\204', 'í'),
                       ('\\205', 'ó'),
                       ('\\206', 'ø'),
                       ('\\207', '¹'),
                       ('\\210', 'ú'),
                       ('\\211', 'ù'),
                       ('\\212', 'ý'),
                       ('\\213', 'Á'),
                       ('\\214', 'Í'),     
                       ('\\215', 'Ø'),
                       ('\\216', 'Ý'),                   
                       ('\\217', '®')],                       
               }

# Take care of command line options
parser = argparse.ArgumentParser(description='Decoder/Encoder for open9x translations')
parser.add_argument('input', action="store")
parser.add_argument('language', action="store")
args =  parser.parse_args()
if args.language not in translations:
    parser.error(args.language  + ' is not a supported language. Try one of the supported ones: ' + str(translations.keys()))
    system.exit()
    
# Read the input file into a buffer
in_file = open( args.input, 'r')
text = in_file.read()
in_file.close()

# Do the replacements
for before, after in translations[args.language]:
    if args.input.endswith("." + args.language):
        text = text.replace(after, before)
    else:
        text = text.replace(before, after)
      
# Write the result to a temporary file
if args.input.endswith("." + args.language):
    out_file = open(args.input[:-3], 'w')
else:
    out_file = open(args.input + "." + args.language, 'w')
out_file.write( text )
out_file.close()


