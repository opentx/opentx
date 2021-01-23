#!/usr/bin/env python
'''
mavgenerate_dialect.py
calls mavlink generator for C header files
(c) olliw, olliw42
'''

#options to set

#mavlinkdialect = "storm32"
mavlinkdialect = "opentx"

mavlinkoutputdirectory = 'out'

mavlinkpathtorepository = r'mavlink'


mavlinklight = False
#mavlinklight = True


'''
Imports
'''
import os
import shutil
import re
import sys

#we may have not installed it or have different pymavlink, so set things straight
sys.path.insert(0,mavlinkpathtorepository)

if mavlinklight:
    from pymavlink_light.generator import mavgen
    from pymavlink_light.generator import mavparse
else:
    from pymavlink.generator import mavgen
    from pymavlink.generator import mavparse

'''
Generates the header files and place them in the output directory.
'''

outdir = mavlinkoutputdirectory
wire_protocol = mavparse.PROTOCOL_2_0
language = 'C'
validate = mavgen.DEFAULT_VALIDATE
error_limit = 5
strict_units = mavgen.DEFAULT_STRICT_UNITS
        
xmfile = mavlinkdialect+'.xml'        

#recreate out directory
print('----------')
print('kill out dir')
try:
    shutil.rmtree(outdir)
except:
    pass    
os.mkdir(outdir)
print('----------')

if True:
        opts = mavgen.Opts(outdir, wire_protocol=wire_protocol, language=language, validate=validate, error_limit=error_limit, strict_units=strict_units)
        args = [xmfile]
        try:
            mavgen.mavgen(opts,args)
            print('Successfully Generated Headers', 'Headers generated successfully.')

        except Exception as ex:
            exStr = str(ex)
            print('Error Generating Headers','{0!s}'.format(exStr))
            exit()

