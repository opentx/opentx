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

def generate(str, idx, alternate=0):
    result = None
    if "speak" in sys.argv:
        if "sapi" in sys.argv:
            tts.Speak(str)
        elif "espeak" in sys.argv:
            subprocess.Popen(["espeak", "-v", espeakVoice, "-s", espeakspeed, "-z", str.encode("utf-8")], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        else:
            print "which speach engine?"
    else:
        if isinstance(idx, int):
            result = "%04d.wav" % idx
        elif board == 'arm':
            result = idx + ".wav"
        else:
            if alternate == 0:
                return []
            result = "%04d.wav" % alternate
            
        temp = "_" + result
    
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
        elif "espeak" in sys.argv:
            subprocess.Popen(["espeak", "-v", espeakVoice, "-s", espeakspeed, "-z", "-w", result, str.encode("utf-8")], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        else:
            print "which speach engine?"
            return []
    
        if board == 'arm':
            if 'sox' in sys.argv:
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
        elif board == 'v4':
            subprocess.Popen(["AD4CONVERTER", "-E4", result], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
            os.remove(result)
            result = result.replace(".wav", ".ad4")
        else:
            os.rename(result, temp) 
            subprocess.Popen(["ffmpeg", "-y", "-i", temp, "-acodec", "pcm_u8", "-ar", "16000", result], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
            os.remove(temp)
	
        print result, str
        
    if result:
        return [(result, str)]
    else:
        return []       

if __name__ == "__main__":
    
    sounds = []
    systemSounds = []
    
    if "arm" in sys.argv:
        board = "arm"
    elif "v4" in sys.argv:
        board = "v4"
    else:
        board = "stock"

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
        for i, s in enumerate(["volts", "amps", "meters per second", "", "km per hour", "meters", "degrees", "percent", "milliamps", "milliamps per hour", "watts", "db", "feet", "knots", "hour", "minute", "second", "r p m", "g"]):
            systemSounds.extend(generate(s, 50+i))
        for s, f, a in [(u"trim center", "midtrim", 244),
                        (u"maximum trim reached", "endtrim", 0),
                        (u"transmitter battery low", "lowbatt", 231),
                        (u"inactivity alarm", "inactiv", 230),
                        (u"throttle warning", "thralert", 232),
                        (u"switch warning", "swalert", 233),
                        (u"tada", "tada", 245),
                        (u"tick tock", "timerlt3", 250),
                        (u"ten seconds", "timer10", 251),
                        (u"twenty seconds", "timer20", 252),
                        (u"thirty seconds", "timer30", 253),
                       ]:
            systemSounds.extend(generate(s, f, a))
        for i, (s, f) in enumerate([
                     (u"gear up", "gearup"),
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
                     ]):
            sounds.extend(generate(s, f, 256+i))

    
    elif "fr" in sys.argv:
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
            voice = "french"
        else:
            espeakVoice = "mb-fr4+f4"
            espeakspeed = "140"
            voice = "french"      
            
        for i in range(100):
            systemSounds.extend(generate(str(i), i))
        for i in range(10):
            systemSounds.extend(generate(str(100*(i+1)), 100+i))    
        for i, s in enumerate(["une", "onze", "vingt et une", "trente et une", "quarante et une", "cinquante et une", "soixante et une", "soixante et onze", "quatre vingt une"]): 
            systemSounds.extend(generate(s, 110+i))
        for i, s in enumerate(["virgule", "et", "moins"]): 
            systemSounds.extend(generate(s, 119+i))
        for i, s in enumerate(["timer", "", "tension", "tension", u"émission", u"réception", "altitude", "moteur",
                               "essence", u"température", u"température", "vitesse", "distance", "altitude", u"élément lipo",
                               "total lipo", "tension", "courant", "consommation", "puissance", u"accelération X", u"accelération Y", u"accelération Z",
                               "orientation", "vario"]):
            systemSounds.extend(generate(s, 146+i))            
        for i, s in enumerate(["volts", u"ampères", u"mètres seconde", "", "km heure", u"mètres", u"degrés", "pourcents", u"milli ampères", u"milli ampères / heure", "watt", "db", "pieds", "knotts", "heure", "minute", "seconde", "tours par minute", "g"]):
            systemSounds.extend(generate(s, 125+i))
        for i, (s, f) in enumerate([
                     (u"train rentré", "gearup"),
                     (u"train sorti", "geardn"),
                     (u"volets rentrés", "flapup"),
                     (u"volets sortis", "flapdn"),
                     (u"atterrissage", "attero"),
                     (u"écolage", "trnon"),
                     (u"fin écolage", "trnoff"),
                     (u"moteur coupé", "engoff"),
                     ]):
            sounds.extend(generate(s, f, 256+i))
            
            
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
        for i, s in enumerate(["virgola", "un", "e", "meno", "ora", "ore", "minuto", "minuti", "secondo", "secondi"]):
            systemSounds.extend(generate(s, 103+i))
        for i, s in enumerate(["volt", "amper", "meetri per secondo", "", "chilomeetri ora", "meetri", "gradi", "percento", "milliamper", "milliamper ora", "watt", "db", "piedi", "nodi", "ore", "minuti", "secondi", "R P M", "g"]):
            systemSounds.extend(generate(s, 113+i))            
        for s, f, a in [(u"radio inattiva controllare", "inactiv", 230),
                        (u"batteria della radio scarica", "lowbatt", 231),
                        (u"controllo motore non in posizione, verificare", "thralert", 232),
                        (u"interruttori non in posizione, verificare", "swalert", 233),
                        (u"eeprom corrotta", "eebad", 0),
                        (u"formattazone eeprom in corso", "eeformat", 0),
                        (u"errore", "error", 0),
                        (u"trim centrato", "midtrim", 244),
                        (u"tada", "tada", 245),
                        (u"potenziometro centrato", "midpot", 246),
                        (u"massimo trim raggiunto", "endtrim", 0),
                        (u"3 secondi al termine", "timerlt3", 250),
                        (u"dieci secondi", "timer10", 251),
                        (u"venti secondi", "timer20", 252),
                        (u"trenta secondi", "timer30", 253),
                     ]:
            systemSounds.extend(generate(s, f, a))
        for i, s in enumerate(["timer", "", "tensione", "tensione", "trasmissione", "ricezione", "altitudine", "motore",
                               "carburante", "temperatura", "temperatura", "velocita'", "distanza", "altitudine", "cella lipo",
                               "totale lipo", "tensione", "corrente", "consumo", "potenza", "accelerazione X", "accellerazione Y", "accelerazione Z",
                               "direzione", "variometro"]):
            systemSounds.extend(generate(s, 132+i))                
        for i, (s, f) in enumerate([
                     (u"carrello chiuso", "gearup"),
                     (u"carrello aperto", "geardn"),
                     (u"flap rientrati", "flapup"),
                     (u"flap estesi", "flapdn"),
                     (u"atterragggiio", "attero"),
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
                     ]):
            sounds.extend(generate(s, f, 256+i))
            
    elif "cz" in sys.argv:
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftZuzana_Full_22kHz")
            voice = "czech"           	
        else:
            espeakVoice = "mb-cz2"
            espeakspeed = "150"
            voice = "czech"     
             
        for i, s in enumerate(["nula", "jedna", "dva"]):
            systemSounds.extend(generate(s, i))    
        for i in range(97):
            systemSounds.extend(generate(str(3+i), 3+i))
        for i, s in enumerate(["sto", u"dvěsta", u"třista", u"čtyřista", u"pětset", u"šestset", "sedmset", "osmset", 			
        						u"devětset", u"tisíc"]):
            systemSounds.extend(generate(s, 100+i))
        for i, s in enumerate([u"tisíce", "jeden", "jedno", u"dvě", u"celá", u"celé", u"celých", u"mínus"]):
            systemSounds.extend(generate(s, 110+i))
            
        for i, s in enumerate(["volt", "volty", u"voltů", "voltu",
        					   u"ampér", u"ampéry", u"ampérů", u"ampéru",
        					   "metr za sekundu", "metry za sekundu", u"metrů za sekundu", "metru za sekundu",
        					   "jednotka", "jednotky", "jednotek", "jednotky",
        					   "kilometr za hodinu", "kilometry za hodinu", u"kilometrů za hodinu", "kilometru za hodinu",
        					   "metr", "metry", u"metrů", "metru",
        					   u"stupeň", u"stupně", u"stupňů", u"stupně",
        					   "procento", "procenta", "procento", "procenta",
        					   u"miliampér", u"miliampéry", u"miliampérů", u"miliampéru",
        					   u"miliampérhodina", u"miliampérhodiny", u"miliampérhodin", u"miliampérhodiny",
        					   "vat", "vaty", u"vatů", "vatu",
        					   "decibel", "decibely", u"decibelů", "decibelu",
        					   "stopa", "stopy", "stop", "stopy",
        					   "uzel", "uzly", u"uzlů", "uzlu",
        					   "hodina", "hodiny", "hodin", "hodiny",
        					   "minuta", "minuty", "minut", "minuty",
        					   "sekunda", "sekundy", "sekund", "sekundy",
        					   u"otáčka za minutu", u"otáčky za minutu", u"otáček za minutu", u"otáčky za minutu",
        					   u"gé", u"gé", u"gé", u"gé"]):
            systemSounds.extend(generate(s, 118+i))
            
        for s, f, a in [(u"střed trimu", "midtrim", 244),
                        (u"maximum trimu", "endtrim", 0),
                        (u"baterie rádia je vybitá", "lowbatt", 231),
                        (u"zapoměl jsi na mě!", "inactiv", 230),
                        (u"plyn není na nule", "thralert", 232),
                        (u"přepínače nejsou ve výchozí poloze", "swalert", 233),
                        (u"tadá", "tada", 245),
                        (u"ťik ťak", "timerlt3", 250),
                        (u"deset sekund", "timer10", 251),
                        (u"dvacet sekund", "timer20", 252),
                        (u"třicet sekund", "timer30", 253),
                       ]:
            systemSounds.extend(generate(s, f, a))
        for i, (s, f) in enumerate([
                     (u"podvozek je zasunut", "gearup"),
                     (u"podvozek je vysunut", "geardn"),
                     (u"klapky jsou zapnuty", "flapup"),
                     (u"klapky jsou vypnuty", "flapdn"),
                     (u"přistání", "attero"),
                     (u"trenér je zapnutý", "trnon"),
                     (u"trenér je vypnutý", "trnoff"),
                     (u"motor je vypnutý", "engoff"),
                     (u"jsi příliš vysoko", "tohigh"),
                     (u"jsi příliš nízko", "tolow"),
                     (u"vybitá baterie", "lowbat"),
                     (u"krou zapnutý", "crowon"),
                     (u"krou vypnutý", "crowof"),
                     (u"nízký signál!", "siglow"),
                     (u"kritický signál!", "sigcrt"),
                     (u"rychlý mód aktivován", "spdmod"),
                     (u"termický mód aktivován", "thmmod"),
                     (u"normální mód aktivován", "nrmmod"),
                     (u"režim jedna", "fltmd1"),
                     (u"režim dva", "fltmd2"),
                     (u"režim tři", "fltmd3"),
                     (u"režim čtyři", "fltmd4"),
                     (u"režim pět", "fltmd5"),
                     (u"režim šest", "fltmd6"),
                     (u"režim sedum", "fltmd7"),
                     (u"režim osm", "fltmd8"),
                     (u"režim devět", "fltmd9"),
                     ]):
            sounds.extend(generate(s, f, 256+i))
            

    voice += "-" + board
        
    if "csv" in sys.argv:
        csvFile = open(voice + ".csv", "w")
        for f, s in systemSounds:
            l = u""
            if board == "arm":
                l += u"9XSOUNDS/SYSTEM;"
            l += f + u";" + s + u"\n"
            csvFile.write(l.encode("latin-1"))
        for f, s in sounds:
            l = u""
            if board == "arm":
                l += u"9XSOUNDS;"
            l += f + u";" + s + u"\n"
            csvFile.write(l.encode("latin-1"))
        csvFile.close()
            
    if "zip" in sys.argv:
        zip_name = voice + ".zip"
        zip = zipfile.ZipFile(zip_name, "w", zipfile.ZIP_DEFLATED)
        for f, s in systemSounds:
            if board == "arm":
                zip.write(f, "9XSOUNDS/SYSTEM/" + f)
            else:
                zip.write(f, f)
            os.remove(f)
        for f, s in sounds:
            if board == "arm":
                zip.write(f, "9XSOUNDS/" + f)
            else:
                zip.write(f, f)
            os.remove(f)
        zip.close()
    
