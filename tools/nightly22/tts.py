#!/usr/bin/env python
# -*- coding: utf-8 -*-


# This script is a modified version to support Linux TTS fiel genration using PicoTTS

# Sound pack maintainers (incomplete list) by language alphabetical order
# Czech      : Martin Hotar
# French     : Bertrand Songis & André Bernet
# English    : Rob Thompson & Martin Hotar
# German     : Romolo Manfredini (Some corrections by Peer)
# Italian    : Romolo Manfredini
# Portuguese : Romolo Manfredini
# Spanish    : Romolo Manfredini (With the help of Jose Moreno)

# from __future__ import print_function

import os
import sys
import subprocess
import zipfile
from tts_common import *

try:
    # Python 3
    from http.client import HTTPConnection
    from urllib.parse import urlencode
except ImportError:
    # Python 2
    from httplib import HTTPConnection
    from urllib import urlencode


def generate(str, filename):

    command = "pico2wave -l=" + voice + " -w=" + filename + " \"" + str + "\""
    os.system(command.encode('utf-8'))


################################################################

if __name__ == "__main__":


    if "en" in sys.argv:
        from tts_en import systemSounds, sounds

        directory = "en"
        voice = "en-US"

    elif "fr" in sys.argv:
        from tts_fr import systemSounds, sounds

        directory = "fr"
        voice = "fr-FR"

    elif "it" in sys.argv:
        from tts_it import systemSounds, sounds

        directory = "it"
        voice = "it-IT"

    elif "de" in sys.argv:
        from tts_de import systemSounds, sounds

        directory = "de"
        voice = "de-DE"

    elif "es" in sys.argv:
        from tts_es import systemSounds, sounds

        directory = "es"
        voice = "es-ES"
 
    else:
        print("which language?")
        exit()

    if "csv" in sys.argv:
        with open("%s-%s.csv" % (voice, board), "w") as csvFile:
            for s, f in systemSounds:
                if s and f:
                    l = u""
                    if board in ("sky9x", "taranis"):
                        l += u"SOUNDS/%s/SYSTEM;" % directory
                    l += f + u";" + s + u"\n"
                    csvFile.write(l.encode("utf-8"))
            for s, f in sounds:
                if s and f:
                    l = u""
                    if board in ("sky9x", "taranis"):
                        l += u"SOUNDS/%s;" % directory
                    l += f + u";" + s + u"\n"
                    csvFile.write(l.encode("utf-8"))

    if "zip" in sys.argv:
        zip_name = "%s-%s.zip" % (voice, board)
        with zipfile.ZipFile(zip_name, "w", zipfile.ZIP_DEFLATED) as zip:
            for s, f in systemSounds:
                if s and f:
                    generate(s, f)
                    if board in ("sky9x", "taranis"):
                        zip.write(f, "SOUNDS/%s/SYSTEM/" % directory + f)
                    else:
                        zip.write(f, f)
                    os.remove(f)
            for s, f in sounds:
                if s and f:
                    generate(s, f)
                    if board in ("sky9x", "taranis"):
                        zip.write(f, "SOUNDS/%s/" % directory + f)
                    else:
                        zip.write(f, f)
                    os.remove(f)
            
    if "files" in sys.argv:
              path = "/tmp/SOUNDS/" + directory + "/SYSTEM/"
              if not os.path.exists(path):
                os.makedirs(path)
              os.chdir(path)
              for s, f in systemSounds:
                if s and f:
                    generate(s, f)
              os.chdir("..")
              for s, f in sounds:
                if s and f:
                    generate(s, f)                  
