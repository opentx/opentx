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
# Sound pack maintainers (incomplete list)
# French  : Bertrand Songis & André Bernet
# English : Rob Thompson & Martin Hotar 
# German  : Romolo Manfredini (Some corrections by Peer)
# Italian : Romolo Manfredini
# Czeck   : Martin Hotar

import os, sys, shutil, platform, subprocess, wave, zipfile

NO_ALTERNATE = 1024

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
        elif board == 'sky9x':
            result = idx + ".wav"
        else:
            if alternate >= NO_ALTERNATE:
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
            start = 0
            end = 0
            for i in range(n/2):
                sample = ord(f[2*i+1])
                # print sample,
                if sample != 0xFF and sample != 0x00:
                    start = i
                    break
            for i in range(n/2):
                sample = ord(f[-2*i-1])
                # print sample,
                if sample != 0xFF and sample != 0x00:
                    end = i
                    break
            # print 2*start, 2*end,  
            o.writeframes(f[2*start:-2*end])
            o.close()                
            os.remove(temp)           
        elif "espeak" in sys.argv:
            subprocess.Popen(["espeak", "-v", espeakVoice, "-s", espeakspeed, "-z", "-w", result, str.encode("utf-8")], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        else:
            print "which speach engine?"
            return []
    
        if board == 'sky9x':
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
                subprocess.Popen(["ffmpeg", "-y", "-i", temp, "-acodec", defaultcodec, "-ar", "16000", result], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
                os.remove(temp)
        elif board == 'gruvin9x':
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
    
    if "sky9x" in sys.argv:
        board = "sky9x"
        PROMPT_CUSTOM_BASE = 256
        PROMPT_SYSTEM_BASE = 0
    elif "gruvin9x" in sys.argv:
        board = "gruvin9x"
        PROMPT_CUSTOM_BASE = 0
        PROMPT_SYSTEM_BASE = 256
    else:
        board = "stock"
        PROMPT_CUSTOM_BASE = 0
        PROMPT_SYSTEM_BASE = 256

    if "sapi" in sys.argv:
        import pyTTS
        tts = pyTTS.Create()
        # tts.SetOutputFormat(16, 16, 1)
        # tts.Volume = 40
        # tts.SetRate(1)
        if "list" in sys.argv:
            print tts.GetVoiceNames()
    
    if "mulaw" in sys.argv:
        defaultcodec = "pcm_mulaw"
    else:
        defaultcodec = "pcm_alaw"
    
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
            
        for i in range(100):
            systemSounds.extend(generate(str(i), PROMPT_SYSTEM_BASE+i))
        for i in range(9):
            systemSounds.extend(generate(str(100*(i+1)), PROMPT_SYSTEM_BASE+100+i))
        for i, s in enumerate(["thousand", "and", "minus", "point"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+109+i))
        for i, s in enumerate(["volt", "volts",
        					   "amp", "amps",
        					   "meter per second", "meters per second",
        					   "", "",
        					   "kilometer per hour","kilometers per hour",
        					   "meter", "meters",
        					   "degree", "degrees",
        					   "percent", "percent",
        					   "milliamp", "milliamps",
        					   "milliamp-hour", "milliamp-hours",
        					   "watt", "watts",
        					   "db", "db",
        					   "foot", "feet",
        					   "knot", "knots",
        					   "hour", "hours",
        					   "minute", "minutes",
        					   "second", "seconds",
        					   "r p m", "r p m",
        					   "g", "g"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+115+i))
        for i, s in enumerate(["point zero", "point one", "point two", "point three",
                               "point four", "point five", "point six",
                               "point seven", "point eight", "point nine"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+160+i))
        for s, f, a in [(u"trim center", "midtrim", 494),
                        (u"maximum trim reached", "endtrim", NO_ALTERNATE),
                        (u"transmitter battery low", "lowbatt", 481),
                        (u"inactivity alarm", "inactiv", 480),
                        (u"throttle warning", "thralert", 482),
                        (u"switch warning", "swalert", 483),
                        (u"Welcome to open nine ex!", "tada", 495),
                        (u"tick. tock", "timerlt3", 500),
                        (u"ten. seconds", "timer10", 501),
                        (u"twenty. seconds", "timer20", 502),
                        (u"thirty. seconds", "timer30", 503),
                       ]:
            systemSounds.extend(generate(s, f, a))
        for i, (s, f) in enumerate([
                     (u"gear!, up!", "gearup"),
                     (u"gear!, down!", "geardn"),
                     (u"flaps!, up!", "flapup"),
                     (u"flaps!, down!", "flapdn"),
                     (u"trainer!, on!", "trnon"),
                     (u"trainer!, off!", "trnoff"),
                     (u"engine!, off!", "engoff"),
                     (u"too. high!", "tohigh"),
                     (u"too. low!", "tolow"),
                     (u"low. battery!", "lowbat"),
                     (u"crow!, on!", "crowon"),
                     (u"crow!, off!", "crowof"),
                     (u"rf. signal!, low!", "siglow"),
                     (u"rf. signal!, critical!", "sigcrt"),
                     (u"high. speed. mode!, active", "spdmod"),
                     (u"thermal. mode!, on", "thmmod"),
                     (u"normal. mode!, on", "nrmmod"),
                     (u"landing. mode!, on", "lnding"),
                     (u"acro. mode!, on", "acro"),
                     (u"flight. mode!, one", "fm-1"),
                     (u"flight. mode!, two", "fm-2"),
                     (u"flight. mode!, three", "fm-3"),
                     (u"flight. mode!, four", "fm-4"),
                     (u"flight. mode!, five", "fm-5"),
                     (u"flight. mode!, six", "fm-6"),
                     (u"flight. mode!, seven", "fm-7"),
                     (u"flight. mode!, eight", "fm-8"),
                     (u"vario!, on", "vrion"),
                     (u"vario!, off", "vrioff"),
                     (u"flight mode!, power", "fm-pwr"),
                     (u"flight mode!, land", "fm-lnd"),
                     (u"flight mode!, float", "fm-flt"),
                     (u"flight mode!, speed", "fm-spd"),
                     (u"flight mode!, fast", "fm-fst"), 
                     (u"flight mode!, normal", "fm-nrm"),
                     (u"flight mode!, cruise", "fm-crs"),                 
                     ]):
            sounds.extend(generate(s, f, PROMPT_CUSTOM_BASE+i))

    
    elif "fr" in sys.argv:
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
            voice = "french"
        else:
            espeakVoice = "mb-fr4+f4"
            espeakspeed = "140"
            voice = "french"      
            
        for i in range(100):
            systemSounds.extend(generate(str(i), PROMPT_SYSTEM_BASE+i))
        for i in range(10):
            systemSounds.extend(generate(str(100*(i+1)), PROMPT_SYSTEM_BASE+100+i))    
        for i, s in enumerate(["une", "onze", "vingt et une", "trente et une", "quarante et une", "cinquante et une", "soixante et une", "soixante et onze", "quatre vingt une"]): 
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+110+i))
        for i, s in enumerate(["virgule", "et", "moins"]): 
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+119+i))
        for i, s in enumerate(["volts", u"ampères", u"mètres seconde", "", "km heure", u"mètres", u"degrés", "pourcents", u"milli ampères", u"milli ampères / heure", "watt", "db", "pieds", "knotts", "heure", "minute", "seconde", "tours par minute", "g"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+125+i))
        for i, s in enumerate(["timer", "", "tension", "tension", u"émission", u"réception", "altitude", "moteur",
                               "essence", u"température", u"température", "vitesse", "distance", "altitude", u"élément lipo",
                               "total lipo", "tension", "courant", "consommation", "puissance", u"accelération X", u"accelération Y", u"accelération Z",
                               "orientation", "vario"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+146+i))            
        for i, s in enumerate(["virgule 0", "virgule 1", "virgule 2", "virgule 3", "virgule 4", "virgule 5", "virgule 6", "virgule 7", "virgule 8", "virgule 9"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+180+i))
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
            sounds.extend(generate(s, f, PROMPT_CUSTOM_BASE+i))
            
            
    elif "it" in sys.argv:
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
            voice = "italian"
        else:
            espeakVoice = "mb-it4"
            espeakspeed = "160"
            voice = "italian"

        for i in range(101):
            systemSounds.extend(generate(str(i), PROMPT_SYSTEM_BASE+i))
        systemSounds.extend(generate("mila", PROMPT_SYSTEM_BASE+101))
        systemSounds.extend(generate("mille", PROMPT_SYSTEM_BASE+102))
        for i, s in enumerate(["virgola", "un", "e", "meno", "ora", "ore", "minuto", "minuti", "secondo", "secondi"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+103+i))
        for i, s in enumerate(["volt", "amper", "meetri per secondo", "", "chilomeetri ora", "meetri", "gradi", "percento", "milliamper", "milliamper ora", "watt", "db", "piedi", "nodi", "ore", "minuti", "secondi", "R P M", "g"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+113+i))            
        for s, f, a in [(u"radio inattiva controllare", "inactiv", 480),
                        (u"batteria della radio scarica", "lowbatt", 481),
                        (u"controllo motore non in posizione, verificare", "thralert", 482),
                        (u"interruttori non in posizione, verificare", "swalert", 483),
                        (u"eeprom corrotta", "eebad", NO_ALTERNATE),
                        (u"formattazone eeprom in corso", "eeformat", NO_ALTERNATE),
                        (u"errore", "error", NO_ALTERNATE),
                        (u"trim centrato", "midtrim", 494),
                        (u"tada", "tada", 495),
                        (u"potenziometro centrato", "midpot", 496),
                        (u"massimo trim raggiunto", "endtrim", NO_ALTERNATE),
                        (u"3 secondi al termine", "timerlt3", 500),
                        (u"dieci secondi", "timer10", 501),
                        (u"venti secondi", "timer20", 502),
                        (u"trenta secondi", "timer30", 503),
                     ]:
            systemSounds.extend(generate(s, f, a))
        for i, s in enumerate(["timer", "timer",  "trasmissione", "ricezione", "A1", "A2", "altitudine", "motore",
                               "carburante", "temperatura", "temperatura", "velocita'", "distanza", "altitudine", "cella lipo",
                               "totale lipo", "tensione", "corrente", "consumo", "potenza", "accelerazione X", "accellerazione Y", "accelerazione Z",
                               "direzione", "variometro","minimo","massimo"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+132+i))
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
            sounds.extend(generate(s, f, PROMPT_CUSTOM_BASE+i))

    elif "de" in sys.argv:
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
            voice = "german"
        else:
            espeakVoice = "mb-de4"
            espeakspeed = "160"
            voice = "german"

        for i in range(101):
            systemSounds.extend(generate(str(i), PROMPT_SYSTEM_BASE+i))
        systemSounds.extend(generate("tausend", PROMPT_SYSTEM_BASE+101))
        for i, s in enumerate(["comma", "und", "minus", "uhr", "minute", "minuten", "sekunde", "sekunden"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+102+i))
        for i, s in enumerate(["Volt", "Ampere", "Meter pro sekunde", "", "kilometer pro stunde", "Meter", "Grad", "Prozent", "Milliampere", "Milliampere pro stunde", "Watt", "db", "Fuesse", "Knoten", "Uhr", "Minuten", "Secunden", "R P M", "g"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+110+i))            
        for s, f, a in [(u"Sender ist inaktiv,bitte Ueberpruefen Sie", "inactiv", 480),
                        (u"Senderakku niedrig", "lowbatt", 481),
                        (u"Gaskanal nicht Null, bitte pruefen", "thralert", 482),
                        (u"Schalten fehlpositioniert, bitte pruefen", "swalert", 483),
                        (u"Das EEPROM ist fehlerhaft", "eebad", NO_ALTERNATE),
                        (u"Das EEPROM ist immer formatiert", "eeformat", NO_ALTERNATE),
                        (u"fehler", "error", NO_ALTERNATE),
                        (u"Trim zentriert", "midtrim", 494),
                        (u"tada", "tada", 495),
                        (u"Poti zentriert", "midpot", 496),
                        (u"Maximale trimmung erreicht", "endtrim", NO_ALTERNATE),
                        (u"Noch drei sekunden", "timerlt3", 500),
                        (u"10 sekunden", "timer10", 501),
                        (u"20 sekunden", "timer20", 502),
                        (u"30 sekunden", "timer30", 503),
                     ]:
            systemSounds.extend(generate(s, f, a))
        for i, s in enumerate(["Timer", "Timer",  "Sendung", "Empfang", "A1", "A2", "Hoehe", "Motor",
                               "Treibstoff", "Temperatur", "Temperatur", "Geschwindigkeit", "Ferne", "Hoehe", "Lipo-Zelle",
                               "Zellen gesamt", "Spannung", "Strom", "Verbrauch", "Power", "Beschleunigung X", "Beschleunigung Y", "Beschleunigung Z",
                               "Richtung", "Variometer","Minimum","Maximum"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+129+i))
        for i, (s, f) in enumerate([
                     (u"Fahrwerk eingezogen", "gearup"),
                     (u"Fahrwerk ausgefahren", "geardn"),
                     (u"Klappen eingefahren", "flapup"),
                     (u"Klappen ausgefahren", "flapdn"),
                     (u"Landung", "attero"),
                     (u"Trainer-Modus ein", "trnon"),
                     (u"Trainer-Modus aus", "trnoff"),
                     (u"Motor aus", "engoff"),
                     (u"zu hoch", "tohigh"),
                     (u"zu niedrig", "tolow"),
                     (u"Batterie schwach", "lowbat"),
                     (u"crow ein", "crowon"),
                     (u"crow aus", "crowof"),
                     (u"Funksignal schlecht!", "siglow"),
                     (u"Funksignal kritisch!", "sigcrt"),
                     (u"Schnell-Modus ist aktiviert", "spdmod"),
                     (u"Thermik-Modus ist aktiviert", "thmmod"),
                     (u"Normal-Modus ist aktiviert", "nrmmod"),
                     (u"Regime 1", "fltmd1"),
                     (u"Regime 2", "fltmd2"),
                     (u"Regime 3", "fltmd3"),
                     (u"Regime 4", "fltmd4"),
                     (u"Regime 5", "fltmd5"),
                     (u"Regime 6", "fltmd6"),
                     (u"Regime 7", "fltmd7"),
                     (u"Regime 8", "fltmd8"),
                     (u"Regime 9", "fltmd9"),
                     ]):
            sounds.extend(generate(s, f, PROMPT_CUSTOM_BASE+i))

    elif "pt" in sys.argv:
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
            voice = "portuguese"
        else:
            espeakVoice = "mb-pt1+f1"
            espeakspeed = "160"
            voice = "portuguese"

        for i in range(101):
            systemSounds.extend(generate(str(i), PROMPT_SYSTEM_BASE+i))
        systemSounds.extend(generate("cento", PROMPT_SYSTEM_BASE+101))
        systemSounds.extend(generate("duzentos", PROMPT_SYSTEM_BASE+102))
        systemSounds.extend(generate("trezentos", PROMPT_SYSTEM_BASE+103))
        systemSounds.extend(generate("quatrocentos", PROMPT_SYSTEM_BASE+104))
        systemSounds.extend(generate("quinhentos", PROMPT_SYSTEM_BASE+105))
        systemSounds.extend(generate("seiscentos", PROMPT_SYSTEM_BASE+106))
        systemSounds.extend(generate("setecentos", PROMPT_SYSTEM_BASE+107))
        systemSounds.extend(generate("oitocentos", PROMPT_SYSTEM_BASE+108))
        systemSounds.extend(generate("novecentos", PROMPT_SYSTEM_BASE+109))
        systemSounds.extend(generate("mil", PROMPT_SYSTEM_BASE+110))
        for i, s in enumerate(["virgula", "uma", "duas", "e", "menos", "hora", "horas", "minuto", "minutos", "segundo", "segundos"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+111+i))
        for i, s in enumerate([u"Volt", u"ampére", u"metros por segundo", u"", u"quilômetros por hora", u"metros", u"graus", u"cento", u"miliamperes", u"miliamperes por hora", u"watt", u"db", u"pés", u"nós", u"horas", u"minutos", u"segundos", u"RPM", u"g"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+122+i))            
        for s, f, a in [(u"atenção, o rádio foi esquecido ligado, por favor desligue-o", "inactiv", 480),
                        (u"bateria do rádio fraca", "lowbatt", 481),
                        (u"atenção,acelerador não está no mínimo", "thralert", 482),
                        (u"atenção, certifique-se que os interruptores estão na posição certa", "swalert", 483),
                        (u"eeprom corrompida", "eebad", NO_ALTERNATE),
                        (u"formatação da eeprom em curso", "eeformat", NO_ALTERNATE),
                        (u"erro", "error", NO_ALTERNATE),
                        (u"trim centrado", "midtrim", 494),
                        (u"tada", "tada", 495),
                        (u"potenciómetro centrado", "midpot", 496),
                        (u"trim no máximo", "endtrim", NO_ALTERNATE),
                        (u"3 segundos", "timerlt3", 500),
                        (u"10 segundos", "timer10", 501),
                        (u"20 segundos", "timer20", 502),
                        (u"30 segundos", "timer30", 503),
                     ]:
            systemSounds.extend(generate(s, f, a))
        for i, s in enumerate([u"cronómetro", u"cronómetro", u"transmissão", u"recepção", u"A1", u"A2", u"altitude", u"motor",
                                u"combustível", u"temperatura", u"temperatura", u"velocidade", u"distância", u"altitude", u"célula lipo"
                                u"Total lipo", u"tensão", u"corrente", u"consumo", u"potência", u"aceleração X", u"aceleração Y", u"aceleração Z"
                                u"Direcção", u"variómetro", u"mínimo", u"máximo"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+141+i))
        for i, (s, f) in enumerate([
                      (u"trem, em cima.", "gearup"),
                      (u"trem, em baixo.", "geardn"),
                      (u"flaps recolhidos", "flapup"),
                      (u"flaps estendidos", "flapdn"),
                      (u"aterragem", "attero"),
                      (u"modo de treino ligado", "trnon"),
                      (u"modo de treino desligado", "trnoff"),
                      (u"motor desligado", "engoff"),
                      (u"muito alto", "tohigh"),
                      (u"muito baixo", "tolow"),
                      (u"bateria fraca", "lowbat"),
                      (u"crou, activo", "crowon"),
                      (u"crou, desligado", "crowof"),
                      (u"recepção de sinal muito baixa", "siglow"),
                      (u"recepção de sinal crítica", "sigcrt"),
                      (u"modo de velocidade", "spdmod"),
                      (u"modo de térmica", "thmmod"),
                      (u"modo de voo normal", "nrmmod"),
                      (u"fase de voo 1", "fltmd1"),
                      (u"fase de voo 2", "fltmd2"),
                      (u"fase de voo 3", "fltmd3"),
                      (u"fase de voo 4", "fltmd4"),
                      (u"fase de voo 5", "fltmd5"),
                      (u"fase de vôo 6", "fltmd6"),
                      (u"fase de voo 7", "fltmd7"),
                      (u"fase de voo 8", "fltmd8"),
                      (u"fase de voo 9", "fltmd9"),
                     ]):
            sounds.extend(generate(s, f, PROMPT_CUSTOM_BASE+i))
            
    elif "cz" in sys.argv:
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftZuzana_Full_22kHz")
            voice = "czech"           	
        else:
            espeakVoice = "mb-cz2"
            espeakspeed = "150"
            voice = "czech"     
             
        for i, s in enumerate(["nula", "jedna", "dva"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+i))    
        for i in range(97):
            systemSounds.extend(generate(str(3+i), PROMPT_SYSTEM_BASE+3+i))
        for i, s in enumerate(["sto", u"dvěsta", u"třista", u"čtyřista", u"pětset", u"šestset", "sedmset", "osmset", 			
        						u"devětset", u"tisíc"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+100+i))
        for i, s in enumerate([u"tisíce", "jeden", "jedno", u"dvě", u"celá", u"celé", u"celých", u"mínus"]):
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+110+i))
            
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
            systemSounds.extend(generate(s, PROMPT_SYSTEM_BASE+118+i))
            
        for s, f, a in [(u"střed trimu", "midtrim", 494),
                        (u"maximum trimu", "endtrim", NO_ALTERNATE),
                        (u"baterie rádia je vybitá", "lowbatt", 481),
                        (u"zapoměl jsi na mě!", "inactiv", 480),
                        (u"plyn není na nule", "thralert", 482),
                        (u"přepínače nejsou ve výchozí poloze", "swalert", 483),
                        (u"tadá", "tada", 495),
                        (u"ťik ťak", "timerlt3", 500),
                        (u"deset sekund", "timer10", 501),
                        (u"dvacet sekund", "timer20", 502),
                        (u"třicet sekund", "timer30", 503),
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
            sounds.extend(generate(s, f, PROMPT_CUSTOM_BASE+i))
            

    voice += "-" + board
        
    if "csv" in sys.argv:
        csvFile = open(voice + ".csv", "w")
        for f, s in systemSounds:
            l = u""
            if board == "sky9x":
                l += u"SOUNDS/SYSTEM;"
            l += f + u";" + s + u"\n"
            csvFile.write(l.encode("latin-1"))
        for f, s in sounds:
            l = u""
            if board == "sky9x":
                l += u"SOUNDS;"
            l += f + u";" + s + u"\n"
            csvFile.write(l.encode("latin-1"))
        csvFile.close()
            
    if "zip" in sys.argv:
        zip_name = voice + ".zip"
        zip = zipfile.ZipFile(zip_name, "w", zipfile.ZIP_DEFLATED)
        for f, s in systemSounds:
            if board == "sky9x":
                zip.write(f, "SOUNDS/SYSTEM/" + f)
            else:
                zip.write(f, f)
            os.remove(f)
        for f, s in sounds:
            if board == "sky9x":
                zip.write(f, "SOUNDS/" + f)
            else:
                zip.write(f, f)
            os.remove(f)
        zip.close()
    
