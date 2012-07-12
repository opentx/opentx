#!/usr/bin/env python
# -*- coding: utf-8 -*-


# To use this script you need the following installed
#  Python 2.5  http://www.python.org/download/releases/2.5.4/
#  Python 2.7   http://www.python.org/download/releases/2.7.3/
#  PyTTS  http://pypi.python.org/pypi/pyTTS
#
#  Note
#  At the moment, pyTTS is only available for Python 2.3, 2.4 and 2.5. To use it for later versions without having to 
#  recompile it, a quick and dirty solution is to:
#  copy the entire pyTTS directory from Python25\Lib\site-packages to Python26 or Python27
#  replace TTSFast.py with an empty file. This way the version-dependent pyd file isn't loaded.

import os, sys, shutil, platform, subprocess, wave, zipfile

def generate(str, idx):
    result = None
    if str and platform.system() == "Windows":
        if "sapi" in sys.argv:
            if "speak" in sys.argv:
                tts.Speak(str)
            else:
                if isinstance(idx, int):
                    result = "%04d.wav" % idx
                else:
                    result = idx + ".wav"
                print result, str
                
                temp = "_" + result
                tts.SpeakToWave(temp, str)
                
                i = wave.open(temp, "r")
                n = i.getnframes()
                f = i.readframes(n)
                i.close()
                o = wave.open(result, "w")
                o.setnchannels(i.getnchannels())
                o.setsampwidth(i.getsampwidth())
                o.setframerate(i.getframerate())
                o.writeframes(f[6400:-6400])
                o.close()
                
                os.remove(temp)
                            
        else:
            subprocess.Popen(["C:\Program Files\eSpeak\command_line\espeak.exe", "-z", "-w", "%04d.wav" % idx, str], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        if not "speak" in sys.argv:
            if 'ad4' in sys.argv:
                subprocess.Popen(["D:\Perso\workspace\companion9x\AD4CONVERTER.EXE", "-E4", result], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
                result = result.replace(".wav", ".ad4")
            else:
                os.rename(result, temp)
                subprocess.Popen(["C:/Programs/ffmpeg/bin/ffmpeg.exe", "-y", "-i", temp, "-acodec", "pcm_alaw", "-ar", "16000", result], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
                os.remove(temp)
                
    if result:
        return [result]
    else:
        return []       

if __name__ == "__main__":
    
    sounds = []
    systemSounds = []
    
    if "sapi" in sys.argv:
        import pyTTS
        tts = pyTTS.Create()
        # tts.SetOutputFormat(16, 16, 1)
        # tts.Volume = 40
        # tts.SetRate(1)
        if "list" in sys.argv:
            print tts.GetVoiceNames()

    if "en" in sys.argv:
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftFiona_Full_22kHz")
            voice = "english-scottish"
        for i in range(20):
            systemSounds.extend(generate(str(i), i))
        for i in range(20, 100, 10):
            systemSounds.extend(generate(str(i), 20+(i-20)/10))
        systemSounds.extend(generate("hundred", 28))
        systemSounds.extend(generate("thousand", 29))
        for i, s in enumerate(["hour", "hours", "minute", "minutes", "second", "seconds", "", "and", "minus"]):
            systemSounds.extend(generate(s, 40+i))
        for i, s in enumerate(["volts", "amps", "meters per second", "", "km per hour", "meters", "degrees", "percent", "milliamps", "milliamps per hour", "watts", "", "feet", "knots"]):
            systemSounds.extend(generate(s, 50+i))
    
    elif "fr" in sys.argv:
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
            voice = "french"
        for i in range(101):
            systemSounds.extend(generate(str(i), i))
        systemSounds.extend(generate("1000", 101))
        for i, s in enumerate(["une", "onze", "vingt et une", "trente et une", "quarante et une", "cinquante et une", "soixante et une", "soixante et onze", "quatre vingt une"]): 
            systemSounds.extend(generate(s, 102+i))
        for i, s in enumerate(["et", "moins"]): 
            systemSounds.extend(generate(s, 117+i))
        for i, s in enumerate(["timer", "", "tension", "tension", "émission", u"réception", "altitude", "moteur",
                               "essence", u"température", "température", "vitesse", "distance", "altitude", "élément lipo",
                               "total lipo", "tension", "courant", "consommation", "puissance", "accelération X", "accelération Y", "accelération Z",
                               "orientation", "vario"]):
            systemSounds.extend(generate(s, 141+i))            
        for i, s in enumerate(["volts", u"ampères", u"mètres seconde", "", "km heure", u"mètres", u"degrés", "pourcents", u"milli ampères", u"milli ampères / heure", "watt", "", "pieds", "knotts", "heure", "minute", "seconde"]):
            systemSounds.extend(generate(s, 120+i))
        for s, f in [(u"train rentré", "gearup"),
                     (u"train sorti", "geardn"),
                     (u"volets rentrés", "flapup"),
                     (u"volets sortis", "flapdn"),
                     (u"atterrissage", "attero"),
                     (u"écolage", "trnon"),
                     (u"fin écolage", "trnoff"),
                     (u"moteur coupé", "engoff"),
                     ]:
            sounds.extend(generate(s, f))
            
    if "zip" in sys.argv:
        zip_name = voice + ".zip"
        zip = zipfile.ZipFile(zip_name, "w", zipfile.ZIP_DEFLATED)
        for f in systemSounds:
            zip.write(f, "9XSOUNDS/SYSTEM/" + f)
            os.remove(f)
        for f in sounds:
            zip.write(f, "9XSOUNDS/" + f)
            os.remove(f)
        zip.close()
    