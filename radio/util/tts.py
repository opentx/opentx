#!/usr/bin/env python
# -*- coding: utf-8 -*-


# To use this script you need the following installed within python
#  Python 2.5  http://www.python.org/download/releases/2.5.4/
#  Python 2.7   http://www.python.org/download/releases/2.7.3/
#  PyTTS  http://pypi.python.org/pypi/pyTTS
#  PyWin32 http://sourceforge.net/projects/pywin32/files/pywin32/
#
#  Note
#  At the moment, pyTTS is only available for Python 2.3, 2.4 and 2.5. To use it for later versions without having to
#  recompile it, a quick and dirty solution is to:
#  copy the entire pyTTS directory from Python25\Lib\site-packages to Python26 or Python27
#  replace TTSFast.py with an empty file. This way the version-dependent pyd file isn't loaded.

# in addition you will need some tools.
#  ffmpeg, sox, adconvertor ttscmd (2cnd speach centre)
# have fun!
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


def wavstrip(filename):
    output = "_" + filename
    subprocess.Popen(["sox", filename, output, "silence", "1", "0.1", "0.1%", "reverse", "silence", "1", "0.1", "0.1%", "reverse"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
    os.remove(filename)
    os.rename(output, filename)


def generate(str, filename):
    print(filename, str)

    if not str:
        str = " !"  # this is so blank wav files never exist!

    if "speak" in sys.argv:
        if "sapi" in sys.argv:
            tts.Speak(str)
        elif "espeak" in sys.argv:
            subprocess.Popen(["espeak", "-v", espeakVoice, "-s", espeakspeed, "-z", str.encode("utf-8")], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        elif "google" in sys.argv:
            "speak not implemented with google tts engine"
            exit()
        else:
            print("which speach engine?")
            exit()
    else:
        if "sapi" in sys.argv:
            ttsfilename = "ttsfile.wav"
            tts.SpeakToWave(ttsfilename, str)
        elif "sapi2" in sys.argv:
            ttsfilename = "ttsfile.wav"
            subprocess.Popen(["ttscmd", "/ttw", str.encode("utf-8"), ttsfilename, "-v", voiceID, "-b", "32", "-s", "\"-3\"", "-w", "32", "-f", "47"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        elif "espeak" in sys.argv:
            ttsfilename = "ttsfile.wav"
            subprocess.Popen(["espeak", "-v", espeakVoice, "-s", espeakspeed, "-z", "-w", ttsfilename, str.encode("utf-8")], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        elif "google" in sys.argv:
            ttsmp3 = "ttsfile.mp3"
            ttsfilename = "ttsfile.wav"
            conn = HTTPConnection("translate.google.com")
            params = urlencode({'ie': "UTF-8", 'tl': directory, 'q': str.encode("utf-8")})
            headers = {"User-Agent": "Mozilla"}
            conn.request("GET", u"/translate_tts?%s" % params, headers=headers)
            # conn.request("GET", "/translate_tts?ie=UTF-8&tl=%s&q=%s" % (directory, urllib.urlencode(str)), headers={"User-Agent": "Mozilla"})
            resp = conn.getresponse()
            with open(ttsmp3, "wb") as f:
                f.write(resp.read())
            subprocess.Popen(["ffmpeg", "-y", "-i", ttsmp3, "-acodec", "pcm_s16le", ttsfilename], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
            conn.close()

        else:
            print("which speach engine?")
            exit()

        wavstrip(ttsfilename)

        if board in ('sky9x', 'taranis'):
            if 'sox' in sys.argv:
                maxvolume = subprocess.Popen(["sox", ttsfilename, "-n", "stat", "-v"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[1]
                if "not sound" in maxvolume:
                    subprocess.Popen(["sox", "--show-progress", filename, ttsfilename], stdout=subprocess.PIPE).communicate()[0]
                else:
                    subprocess.Popen(["sox", "--show-progress", "-v", maxvolume, filename, ttsfilename], stdout=subprocess.PIPE).communicate()[0]
                    if board == 'sky9x':
                        subprocess.Popen(["sox", "-twav", ttsfilename, "-b1600", "-c1", "-e", "a-law", filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
                    else:
                        subprocess.Popen(["sox", "-twav", ttsfilename, "-b32000", "-c1", "-e", "a-law", filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
            else:
                if board == 'sky9x':
                    subprocess.Popen(["ffmpeg", "-y", "-i", ttsfilename, "-acodec", defaultcodec, "-ar", "16000", filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
                else:
                    subprocess.Popen(["ffmpeg", "-y", "-i", ttsfilename, "-acodec", defaultcodec, "-ar", "32000", filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        elif board == 'gruvin9x':
            subprocess.Popen(["AD4CONVERTER", "-E4", ttsfilename], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
            os.rename(ttsfilename.replace(".wav", ".ad4"), filename)
        else:
            subprocess.Popen(["ffmpeg", "-y", "-i", ttsfilename, "-acodec", "pcm_u8", "-ar", "16000", filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        os.remove(ttsfilename)

################################################################

if __name__ == "__main__":



    if "sapi" in sys.argv:
        import pyTTS
        tts = pyTTS.Create()
        # tts.SetOutputFormat(16, 16, 1)
        # tts.Volume = 40
        # tts.SetRate(1)
        if "list" in sys.argv:
            print(tts.GetVoiceNames())
            exit()

    if "mulaw" in sys.argv:
        defaultcodec = "pcm_mulaw"
    else:
        defaultcodec = "pcm_alaw"

    if "en" in sys.argv:
        from tts_en import systemSounds, sounds

        directory = "en"
        voice = "english"
        if "sapi" in sys.argv:
            if "scottish" in sys.argv:
                tts.SetVoiceByName("ScanSoftFiona_Full_22kHz")
                voice = "english-scottish"
            elif "american" in sys.argv:
                tts.SetVoiceByName("ScanSoftJennifer_Full_22kHz")
                voice = "english-american"
            elif "australian" in sys.argv:
                tts.SetVoiceByName("ScanSoftKaren_Full_22kHz")
                voice = "english-australian"
            elif "irish" in sys.argv:
                tts.SetVoiceByName("ScanSoftMoira_Full_22kHz")
                voice = "english-irish"
            else:
                tts.SetVoiceByName("ScanSoftFiona_Full_22kHz")
                voice = "english-english"
        elif "sapi2" in sys.argv:
            if "scottish" in sys.argv:
                voiceID = "17"
                voice = "english-scottish"
            elif "american" in sys.argv:
                voiceID = "18"
                voice = "english-american"
            elif "australian" in sys.argv:
                voiceID = "20"
                voice = "english-australian"
            elif "irish" in sys.argv:
                voiceID = "21"
                voice = "english-irish"
            elif "french" in sys.argv:
                voiceID = "19"
                voice = "english-french"
            elif "german" in sys.argv:
                voiceID = "22"
                voice = "english-german"
            else:
                voiceID = "17"
                voice = "english-english"

        elif "espeak" in sys.argv:
            espeakVoice = "mb-us1"
            espeakspeed = "150"

    elif "fr" in sys.argv:
        from tts_fr import systemSounds, sounds

        directory = "fr"
        voice = "french"
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-fr4+f4"
            espeakspeed = "140"

    elif "it" in sys.argv:
        from tts_it import systemSounds, sounds

        directory = "it"
        voice = "italian"
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-it4"
            espeakspeed = "160"

    elif "de" in sys.argv:
        from tts_de import systemSounds, sounds

        directory = "de"
        voice = "german"
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-de4"
            espeakspeed = "160"

    elif "pt" in sys.argv:
        from tts_pt import systemSounds, sounds

        directory = "pt"
        voice = "portuguese"
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-pt1+f1"
            espeakspeed = "160"

    elif "es" in sys.argv:
        from tts_es import systemSounds, sounds

        directory = "es"
        voice = "spanish"
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-es1+f1"
            espeakspeed = "160"

    elif "cz" in sys.argv:
        from tts_cz import systemSounds, sounds

        directory = "cz"
        voice = "czech"
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftZuzana_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-cz2"
            espeakspeed = "150"

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
