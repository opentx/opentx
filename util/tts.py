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
#  ffmpeg, sox, adconvertor.
# have fun!



import os, sys, shutil, platform, subprocess, wave, zipfile

def generate(str, idx):
    result = None
    if "speak" in sys.argv:
        if "sapi" in sys.argv:
            tts.Speak(str)
        else:
            subprocess.Popen(["espeak", "-v", espeakVoice, "-s", espeakspeed, "-z", str.encode("latin-1")], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
    else:
        if isinstance(idx, int):
            result = "%04d.wav" % idx
        else:
            result = idx + ".wav"
        temp = "_" + result
            
        print result, str
    
        if "sapi" in sys.argv:
            tts.SpeakToWave(temp, str)
            # we remove empty frames at start and end of the file                
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
            subprocess.Popen(["espeak", "-v", espeakVoice, "-s", espeakspeed, "-z", "-w", result, str.encode("latin-1")], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
    
        if 'ad4' in sys.argv:
            subprocess.Popen(["AD4CONVERTER", "-E4", result], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
            result = result.replace(".wav", ".ad4")
        elif 'sox' in sys.argv:
            maxvolume = subprocess.Popen(["sox",result,"-n","stat","-v"],stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[1]
            if "not sound" in maxvolume:
                os.rename(result, temp)  
                subprocess.Popen(["sox", "--show-progress",result,temp],stdout=subprocess.PIPE).communicate()[0];          			
            else:    	
                subprocess.Popen(["sox", "--show-progress","-v",maxvolume,result,temp],stdout=subprocess.PIPE).communicate()[0];		
            subprocess.Popen(["sox", "-twav", temp, "-b1600","-c1","-e","a-law",result], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
            os.remove(temp)
        else:
            os.rename(result, temp) 
            subprocess.Popen(["ffmpeg", "-y", "-i", temp, "-acodec", "pcm_alaw", "-ar", "16000", result], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
            os.remove(temp)
	
    if result:
        return [(result, str)]
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
        else:
            espeakVoice = "mb-us1"
            espeakspeed = "150"
            voice = "english"      
            
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
        for s, f in [(u"trim center", "midtrim"),
                     (u"maximum trim reached", "endtrim"),
                     (u"transmitter battery low", "lowbatt"),
                     ]:
            systemSounds.extend(generate(s, f))           
        for s, f in [(u"gear up", "gearup"),
                     (u"gear down", "geardn"),
                     (u"flaps up", "flapup"),
                     (u"flaps down", "flapdn"),
                     (u"landing", "attero"),
                     (u"trainer on", "trnon"),
                     (u"trainer off", "trnoff"),
                     (u"engine off", "engoff"),
                     (u"too high", "tohigh"),
                     (u"too low", "tolow"),
                     (u"low battery", "lowbat"),
                     (u"crow on", "crowon"),
                     (u"crow off", "crowof"),
                     (u"rf signal low!", "siglow"),
                     (u"rf signal critical", "sigcrt"),
                     (u"high speed mode active", "spdmod"),
                     (u"thermal mode active", "thmmod"),
                     (u"normal mode active", "nrmmod"),
                     (u"flight mode one", "fltmd1"),
                     (u"flight mode two", "fltmd2"),
                     (u"flight mode three", "fltmd3"),
                     (u"flight mode four", "fltmd4"),
                     (u"flight mode five", "fltmd5"),
                     (u"flight mode six", "fltmd6"),
                     (u"flight mode seven", "fltmd7"),
                     (u"flight mode eight", "fltmd8"),
                     (u"flight mode nine", "fltmd9"),
                     ]:
            sounds.extend(generate(s, f))

    
    elif "fr" in sys.argv:
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
            voice = "french"
        else:
            espeakVoice = "mb-fr4+f4"
            espeakspeed = "140"
            voice = "french"      
            
        for i in range(101):
            systemSounds.extend(generate(str(i), i))
        systemSounds.extend(generate("1000", 101))
        for i, s in enumerate(["une", "onze", "vingt et une", "trente et une", "quarante et une", "cinquante et une", "soixante et une", "soixante et onze", "quatre vingt une"]): 
            systemSounds.extend(generate(s, 102+i))
        for i, s in enumerate(["et", "moins"]): 
            systemSounds.extend(generate(s, 117+i))
        for i, s in enumerate(["timer", "", "tension", "tension", u"émission", u"réception", "altitude", "moteur",
                               "essence", u"température", u"température", "vitesse", "distance", "altitude", u"élément lipo",
                               "total lipo", "tension", "courant", "consommation", "puissance", u"accelération X", u"accelération Y", u"accelération Z",
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
    elif "it" in sys.argv:
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
            voice = "italian"
        else:
            espeakVoice = "mb-it4"
            espeakspeed = "160"
            voice = "italian"

        for i in range(101):
            systemSounds.extend(generate(str(i), i))
        systemSounds.extend(generate("mila", 101))
        systemSounds.extend(generate("mille", 102))
        for i, s in enumerate(["ora", "ore", "minuto", "minuti", "secondo", "secondi", "", "e", "meno"]):
            systemSounds.extend(generate(s, 103+i))
        for i, s in enumerate(["volt", "amper", "meetri per secondo", "", "chilomeetri ora", "meetri", "gradi", "percento", "milliamper", "milliamper ora", "watt", "", "piedi", "nodi"]):
            systemSounds.extend(generate(s, 113+i))
        for s, f in [(u"trim centrato", "midtrim"),
                     (u"massimo trim raggiunto", "endtrim"),
                     (u"batteria della radio scarica", "lowbatt"),
                     ]:
            systemSounds.extend(generate(s, f))           
        for s, f in [(u"carrello chiuso", "gearup"),
                     (u"carrello aperto", "geardn"),
                     (u"flap rientrati", "flapup"),
                     (u"flap estesi", "flapdn"),
                     (u"atterraggioo", "attero"),
                     (u"modalità maestro attiva", "trnon"),
                     (u"modalità maestro disattiva", "trnoff"),
                     (u"motore spento", "engoff"),
                     (u"troppo alto", "tohigh"),
                     (u"troppo basso", "tolow"),
                     (u"batteria scarica", "lowbat"),
                     (u"crow on", "crowon"),
                     (u"crow off", "crowof"),
                     (u"segnale radio basso!", "siglow"),
                     (u"segnale radio critico", "sigcrt"),
                     (u"modo velocità", "spdmod"),
                     (u"modo termica", "thmmod"),
                     (u"modo volo normale", "nrmmod"),
                     (u"fase di volo 1", "fltmd1"),
                     (u"fase di volo 2", "fltmd2"),
                     (u"fase di volo 3", "fltmd3"),
                     (u"fase di volo 4", "fltmd4"),
                     (u"fase di volo 5", "fltmd5"),
                     (u"fase di volo 6", "fltmd6"),
                     (u"fase di volo 7", "fltmd7"),
                     (u"fase di volo 8", "fltmd8"),
                     (u"fase di volo 9", "fltmd9"),
                     ]:
            sounds.extend(generate(s, f))

    if "csv" in sys.argv:
        csvFile = file(voice + ".csv", "w")
        for f, s in systemSounds:
            s = u"9XSOUNDS/SYSTEM;" + f + u";" + s + u"\n"
            csvFile.write(s.encode("latin-1"))
        for f, s in sounds:
            s = u"9XSOUNDS;" + f + u";" + s + u"\n"
            csvFile.write(s.encode("latin-1"))
        csvFile.close()
            
    if "zip" in sys.argv:
        zip_name = voice + ".zip"
        zip = zipfile.ZipFile(zip_name, "w", zipfile.ZIP_DEFLATED)
        for f, s in systemSounds:
            zip.write(f, "9XSOUNDS/SYSTEM/" + f)
            os.remove(f)
        for f, s in sounds:
            zip.write(f, "9XSOUNDS/" + f)
            os.remove(f)
        zip.close()
    