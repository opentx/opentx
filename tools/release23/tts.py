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
from gtts import gTTS
from tts_common import *
board = "taranis"

reload(sys)
sys.setdefaultencoding('utf8')

SOURCE_DIRECTORY = os.path.dirname(os.path.realpath(__file__))
lib_path = os.path.abspath(os.path.join(SOURCE_DIRECTORY, '..', '..', 'radio', 'util'))
sys.path.append(lib_path)

def generate(str, filename):
    if 0:
        output = "output.wav"
        command = 'pico2wave -l=%s -w=%s "%s"' % (voice, output, str)
        os.system(command.encode('utf-8'))
        command = "sox %s -r 32000 %s reverse silence 1 0.1 0.1%% reverse" % (output, filename)
        os.system(command.encode('utf-8'))
    else:
        output = u"output.mp3"
        tts = gTTS(text=str, lang=voice[:2])
        tts.save(output)
        command = "sox --norm %s -r 32000 %s tempo 1.2" % (output, filename)
        os.system(command.encode('utf-8'))
        command = "rm -f output.mp3"
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

    elif "cz" in sys.argv:
        from tts_cz import systemSounds, sounds

        directory = "cz"
        voice = "cs-CZ"

    elif "ru" in sys.argv:
        from tts_ru import systemSounds, sounds

        directory = "ru"
        voice = "ru-RU"

    elif "pt" in sys.argv:
        from tts_pt import systemSounds, sounds

        directory = "pt"
        voice = "pt-PT"

    else:
        print("which language?")
        exit()

    if "csv" in sys.argv:
        path = "/tmp/SOUNDS/" + directory + "/SYSTEM/"
        if not os.path.exists(path):
            os.makedirs(path)
        os.chdir(path)
        with open("%s-%s.csv" % (voice, board), "wb") as csvFile:
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

    if "psv" in sys.argv:
        path = "/tmp/SOUNDS/" + directory + "/"
        if not os.path.exists(path):
            os.makedirs(path)
        os.chdir(path)
        with open("%s-%s.psv" % (voice, board), "wb") as csvFile:
            for s, f in systemSounds:
                if s and f:
                    l = u"SYSTEM|" + f.replace(".wav", "") + u"|" + s + u"\r\n"
                    csvFile.write(l.encode("windows-1251"))
            for s, f in sounds:
                if s and f:
                    l = u"|" + f.replace(".wav", "") + u"|" + s + u"\r\n"
                    csvFile.write(l.encode("windows-1251"))


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
