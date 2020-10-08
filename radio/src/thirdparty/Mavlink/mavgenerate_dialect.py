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

#you need to do that manually if your dialect has more non-standard dialects 
dialectstocorrect = [mavlinkdialect,'storm32_4otx']

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
args = [xmfile]

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

'''
Corrects the #include in a generated dialect C header files to the required "../dialect/dialect.h"
assumes that it inlcudes only one #include
this is not needed for mavlink-light, or a standard dialect
PR raised but ignored so far: https://github.com/ArduPilot/pymavlink/pull/458
'''
print('----------')
if not mavlinklight:
    for f in dialectstocorrect:
        dialectfile = outdir + "\\" + f + "\\" + f + ".h"
        print('correct', dialectfile)
        F = open(dialectfile, 'r')
        fstr = F.read()
        F.close()
        #get #include we need to correct, it is of the form
        '''
        // base include
        #include "../minimal/minimal.h"
        '''
        m = re.search(r'base include\s+#include[ ]+"(.+)"', fstr)
        d = os.path.basename(m.group(1))[:-2]
        fstr_corrected = re.sub(r'base include\s+#include[ ]+"(.+)"', 'base include\n#include "../'+d+'/'+d+'.h"', fstr)
        F = open(dialectfile+'.orig', 'w')
        F.write(fstr)
        F.close()
        F = open(dialectfile, 'w')
        F.write(fstr_corrected)
        F.close()



