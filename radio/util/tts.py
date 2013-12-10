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

import os, sys, shutil, platform, subprocess, wave, zipfile, httplib, urllib

NO_ALTERNATE = 1024

def filename(idx, alternate=0):
    if "gruvin9x" in sys.argv:
        ext = ".ad4"
    else:
        ext = ".wav"
    if isinstance(idx, int):
        result = "%04d%s" % (idx, ext)
    elif board in ('sky9x', 'taranis'):
        result = idx + ext
    else:
        if alternate >= NO_ALTERNATE:
            return None
        result = "%04d%s" % (alternate, ext)
    return result

def wavstrip(filename):
	output = "_" + filename
	subprocess.Popen(["sox", filename, output, "silence", "1", "0.1", "0.1%", "reverse", "silence", "1", "0.1", "0.1%", "reverse"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
	os.remove(filename)
	os.rename(output, filename)
	
    
def generate(str, filename):
    print filename, str

    if not str:
		str = " !"	#this is so blank wav files never exist!
		
    if "speak" in sys.argv:
        if "sapi" in sys.argv:
            tts.Speak(str)
        elif "espeak" in sys.argv:
            subprocess.Popen(["espeak", "-v", espeakVoice, "-s", espeakspeed, "-z", str.encode("utf-8")], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        elif "google" in sys.argv:
            "speak not implemented with google tts engine"
            exit()
        else:
            print "which speach engine?"
            exit()
    else:
        if "sapi" in sys.argv:
            ttsfilename = "ttsfile.wav"
            tts.SpeakToWave(ttsfilename, str)
        elif "sapi2" in sys.argv: 
            ttsfilename = "ttsfile.wav"		
            subprocess.Popen(["ttscmd", "/ttw", str.encode("utf-8"), ttsfilename, "-v", voiceID, "-b", "32","-s", "\"-3\"","-w","32","-f","47"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()     			
        elif "espeak" in sys.argv:
            ttsfilename = "ttsfile.wav"
            subprocess.Popen(["espeak", "-v", espeakVoice, "-s", espeakspeed, "-z", "-w", ttsfilename, str.encode("utf-8")], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        elif "google" in sys.argv:
            ttsmp3 = "ttsfile.mp3"
            ttsfilename = "ttsfile.wav"
            conn = httplib.HTTPConnection("translate.google.com")
            params = urllib.urlencode({'ie': "UTF-8", 'tl': directory, 'q': str.encode("utf-8")})
            headers = {"User-Agent": "Mozilla"}
            conn.request("GET", u"/translate_tts?%s" % params, headers=headers)
            # conn.request("GET", "/translate_tts?ie=UTF-8&tl=%s&q=%s" % (directory, urllib.urlencode(str)), headers={"User-Agent": "Mozilla"})
            resp = conn.getresponse()
            file(ttsmp3, "wb").write(resp.read())
            subprocess.Popen(["ffmpeg", "-y", "-i", ttsmp3, "-acodec", "pcm_s16le", ttsfilename], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
            conn.close()
            
        else:
            print "which speach engine?"
            exit()                     

        wavstrip(ttsfilename)
            
        if board in ('sky9x', 'taranis'):
            if 'sox' in sys.argv:
                maxvolume = subprocess.Popen(["sox", ttsfilename, "-n", "stat", "-v"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()[1]
                if "not sound" in maxvolume: 
                    subprocess.Popen(["sox", "--show-progress", filename, ttsfilename], stdout=subprocess.PIPE).communicate()[0];          			
                else:    	
                    subprocess.Popen(["sox", "--show-progress","-v", maxvolume, filename, ttsfilename], stdout=subprocess.PIPE).communicate()[0];		
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

def ttsEn():
    systemSounds = []
    sounds = []
    
    for i in range(100):
        systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE+i)))
    for i in range(9):
        systemSounds.append((str(100*(i+1)), filename(PROMPT_SYSTEM_BASE+100+i)))
    for i, s in enumerate(["thousand", "and", "minus", "point"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+109+i)))
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
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+115+i)))
    for i, s in enumerate(["point zero", "point one", "point two", "point three",
                           "point four", "point five", "point six",
                           "point seven", "point eight", "point nine"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+160+i)))
    for s, f, a in [(u"trim center", "midtrim", 495),
                    (u"maximum trim reached", "endtrim", NO_ALTERNATE),
                    (u"transmitter battery low", "lowbatt", 485),
                    (u"inactivity alarm", "inactiv", 486),
                    (u"throttle warning", "thralert", 481),
                    (u"switch warning", "swalert", 482),
                    (u"bad eeprom", "eebad", 483),
                    (u"Welcome to open tea ex!", "tada", 480),
                    (u"twenty. seconds", "timer20", 500),
                    (u"thirty. seconds", "timer30", 501),
                    (u"10. seconds", "timer10", NO_ALTERNATE),
                    (u"time up!", "timerlt3", NO_ALTERNATE),
                    (u"A1,low", "a1_org", NO_ALTERNATE),
                    (u"A1,critical", "a1_red", NO_ALTERNATE),
                    (u"A2,low", "a2_org", NO_ALTERNATE),
                    (u"A2,critical", "a2_red", NO_ALTERNATE),
                    (u"RF signal, low", "rssi_org", NO_ALTERNATE),
                    (u"RF signal, critical", "rssi_red", NO_ALTERNATE),
                    (u"radio antenna defective", "swr_red", NO_ALTERNATE),
                    (u"telemetry lost", "telemko", NO_ALTERNATE),
                    (u"telemetry recovered", "telemok", NO_ALTERNATE),
					]:
        systemSounds.append((s, filename(f, a)))
    for i, (s, f) in enumerate([
                 (u"gear!, up!", "gearup"),
                 (u"gear!, down!", "geardn"),
                 (u"flaps!, up!", "flapup"),
                 (u"flaps!, down!", "flapdn"),
                 (u"spoiler!, up!", "splrup"),
                 (u"spoiler!, down!", "splrdn"),
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
                 (u"flight mode!, acro", "fm-acr"),                 
                 (u"flight mode!, race", "fm-rce"),				 
                 (u"flight mode!, launch", "fm-lch"),
                 (u"flight mode!, ping", "fm-png"),				 
                 (u"flight mode!, thermal", "fm-thm"),	
                 (u"flight mode!, thermal left", "fm-thml"),	
                 (u"flight mode!, thermal right", "fm-thmr"),					 
                 ]):
        sounds.append((s, filename(f, PROMPT_CUSTOM_BASE+i)))
    return systemSounds, sounds

################################################################

def ttsFr():
    systemSounds = []
    sounds = []
    
    for i in range(100):
        systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE+i)))
    for i in range(10):
        systemSounds.append((str(100*(i+1)), filename(PROMPT_SYSTEM_BASE+100+i)))
    for i, s in enumerate(["une", "onze", "vingt et une", "trente et une", "quarante et une", "cinquante et une", "soixante et une", "soixante et onze", "quatre vingt une"]): 
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+110+i)))
    for i, s in enumerate(["virgule", "et", "moins"]): 
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+119+i)))
    for i, s in enumerate(["volts", u"ampères", u"mètres seconde", "", "km heure", u"mètres", u"degrés", "pourcents", u"milli ampères", u"milli ampères / heure", "watt", "db", "pieds", "knotts", "heure", "minute", "seconde", "tours par minute", "g"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+125+i)))
    for i, s in enumerate(["timer", "", "tension", "tension", u"émission", u"réception", "altitude", "moteur",
                           "essence", u"température", u"température", "vitesse", "distance", "altitude", u"élément lipo",
                           "total lipo", "tension", "courant", "consommation", "puissance", u"accelération X", u"accelération Y", u"accelération Z",
                           "orientation", "vario"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+146+i)))            
    for i, s in enumerate(["virgule 0", "virgule 1", "virgule 2", "virgule 3", "virgule 4", "virgule 5", "virgule 6", "virgule 7", "virgule 8", "virgule 9"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+180+i)))
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
        sounds.append((s, filename(f, PROMPT_CUSTOM_BASE+i)))
    return systemSounds, sounds

################################################################

def ttsIt():
    systemSounds = []
    sounds = []
    
    for i in range(101):
        systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE+i)))
    systemSounds.append(("mila", filename(PROMPT_SYSTEM_BASE+101)))
    systemSounds.append(("mille", filename(PROMPT_SYSTEM_BASE+102)))
    for i, s in enumerate(["virgola", "un", "e", "meno", "ora", "ore", "minuto", "minuti", "secondo", "secondi"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+103+i)))
    for i, s in enumerate(["volt", "ampere", "metri per secondo", "", "chilometri/ora", "metri", "gradi", "percento", "milliampere", "milliampere/ora", "watt", "db", "piedi", "nodi", "ore", "minuti", "secondi", "R P M", "g"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+113+i)))            
    for s, f, a in [(u"radio inattiva controllare", "inactiv", 486),
                    (u"batteria della radio scarica", "lowbatt", 485),
                    (u"controllo motore non in posizione, verificare", "thralert", 481),
                    (u"interruttori non in posizione, verificare", "swalert", 482),
                    (u"eeprom corrotta", "eebad", NO_ALTERNATE),
                    (u"formattazone eeprom in corso", "eeformat", NO_ALTERNATE),
                    (u"errore", "error", NO_ALTERNATE),
                    (u"trim centrato", "midtrim", 495),
                    (u"potenziometro centrato", "midpot", 496),
                    (u"massimo trim raggiunto", "endtrim", NO_ALTERNATE),
                    (u"venti secondi", "timer20", 500),
                    (u"trenta secondi", "timer30", 501),
                    (u"assorbimento elevato", "highmah", NO_ALTERNATE),
                    (u"temperatura elevata", "hightemp", NO_ALTERNATE),
                    (u"A1 in allarme", "a1_org", NO_ALTERNATE),
                    (u"A1 critico", "a1_red", NO_ALTERNATE),
                    (u"A2 in allarme", "a2_org", NO_ALTERNATE),
                    (u"A2 critico", "a2_red", NO_ALTERNATE),
                    (u"Segnale radio basso", "rssi_org", NO_ALTERNATE),
                    (u"Segnale radio critico", "rssi_red", NO_ALTERNATE),
                    (u"Problema all'antenna della radio", "swr_red", NO_ALTERNATE),
                    (u"Telemetria assente", "telemko", NO_ALTERNATE),
                    (u"Telemetria disponibile", "telemok", NO_ALTERNATE),
                 ]:
        systemSounds.append((s, filename(f, a)))
    for i, s in enumerate([u"timer", u"timer",  u"trasmissione", u"ricezione", u"A1", u"A2", u"altitudine", u"motore",
                           u"carburante", u"temperatura", u"temperatura", u"velocità", "distanza", u"altitudine", u"cella lipo",
                           u"totale lipo", u"tensione", u"corrente", u"consumo", u"potenza", u"accellerazione X", u"accellerazione Y", u"accellerazione Z",
                           u"direzione", u"variometro",u"minimo",u"massimo"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+132+i)))
    for i, (s, f) in enumerate([
                 (u"carrello chiuso", "gearup"),
                 (u"carrello aperto", "geardn"),
                 (u"flap rientrati", "flapup"),
                 (u"flap estesi", "flapdn"),
                 (u"atterraggiio", "attero"),
                 (u"modalità maestro attiva", "trnon"),
                 (u"modalità maestro disattiva", "trnoff"),
                 (u"motore spento", "engoff"),
                 (u"troppo alto", "tohigh"),
                 (u"troppo basso", "tolow"),
                 (u"batteria scarica", "lowbat"),
                 (u"crow on", "crowon"),
                 (u"crow off", "crowof"),
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
        sounds.append((s, filename(f, PROMPT_CUSTOM_BASE+i)))
    return systemSounds, sounds

################################################################

def ttsPt():
    systemSounds = []
    sounds = []

    for i in range(101):
        systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE+i)))
    systemSounds.append(("cento", filename(PROMPT_SYSTEM_BASE+101)))
    systemSounds.append(("duzentos", filename(PROMPT_SYSTEM_BASE+102)))
    systemSounds.append(("trezentos", filename(PROMPT_SYSTEM_BASE+103)))
    systemSounds.append(("quatrocentos", filename(PROMPT_SYSTEM_BASE+104)))
    systemSounds.append(("quinhentos", filename(PROMPT_SYSTEM_BASE+105)))
    systemSounds.append(("seiscentos", filename(PROMPT_SYSTEM_BASE+106)))
    systemSounds.append(("setecentos", filename(PROMPT_SYSTEM_BASE+107)))
    systemSounds.append(("oitocentos", filename(PROMPT_SYSTEM_BASE+108)))
    systemSounds.append(("novecentos", filename(PROMPT_SYSTEM_BASE+109)))
    systemSounds.append(("mil", filename(PROMPT_SYSTEM_BASE+110)))
    for i, s in enumerate(["virgula", "uma", "duas", "e", "menos", "hora", "horas", "minuto", "minutos", "segundo", "segundos"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+111+i)))
    for i, s in enumerate([u"Volt", u"ampére", u"metros por segundo", u"", u"quilômetros por hora", u"metros", u"graus", u"cento", u"miliamperes", u"miliamperes por hora", u"watt", u"db", u"pés", u"nós", u"horas", u"minutos", u"segundos", u"RPM", u"g"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+122+i)))            
    for s, f, a in [(u"atenção, o rádio foi esquecido ligado, por favor desligue-o", "inactiv", 486),
                    (u"bateria do rádio fraca", "lowbatt", 485),
                    (u"atenção,acelerador não está no mínimo", "thralert", 481),
                    (u"atenção, certifique-se que os interruptores estão na posição certa", "swalert", 482),
                    (u"eeprom corrompida", "eebad", NO_ALTERNATE),
                    (u"formatação da eeprom em curso", "eeformat", NO_ALTERNATE),
                    (u"erro", "error", NO_ALTERNATE),
                    (u"trim centrado", "midtrim", 495),
                    (u"potenciómetro centrado", "midpot", 496),
                    (u"trim no máximo", "endtrim", NO_ALTERNATE),
                    (u"20 segundos", "timer20", 500),
                    (u"30 segundos", "timer30", 501),
                    (u"assorbimento elevato", "highmah", NO_ALTERNATE),
                    (u"temperatura elevata", "hightemp", NO_ALTERNATE),
                    (u"Alarme A1", "a1_org", NO_ALTERNATE),
                    (u"A1 critica", "a1_red", NO_ALTERNATE),
                    (u"Alarme A2", "a2_org", NO_ALTERNATE),
                    (u"A2 critica", "a2_red", NO_ALTERNATE),
                    (u"recepção de sinal muito baixa", "rssi_org", NO_ALTERNATE),
                    (u"recepção de sinal crítica", "rssi_red", NO_ALTERNATE),
                    (u"Problema com a antena do transmissor", "swr_red", NO_ALTERNATE),
                 ]:
        systemSounds.append((s, filename(f, a)))
    for i, s in enumerate([u"cronómetro", u"cronómetro", u"transmissão", u"recepção", u"A1", u"A2", u"altitude", u"motor",
                            u"combustível", u"temperatura", u"temperatura", u"velocidade", u"distância", u"altitude", u"célula lipo",
                            u"Total lipo", u"tensão", u"corrente", u"consumo", u"potência", u"aceleração X", u"aceleração Y", u"aceleração Z",
                            u"Direcção", u"variómetro", u"mínimo", u"máximo"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+141+i)))
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
        sounds.append((s, filename(f, PROMPT_CUSTOM_BASE+i)))
    return systemSounds, sounds

################################################################

def ttsEs():
    systemSounds = []
    sounds = []

    for i in range(101):
        systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE+i)))
    systemSounds.append(("ciento", filename(PROMPT_SYSTEM_BASE+101)))
    systemSounds.append(("doscientos", filename(PROMPT_SYSTEM_BASE+102)))
    systemSounds.append(("trescientos", filename(PROMPT_SYSTEM_BASE+103)))
    systemSounds.append(("cuatrocientos", filename(PROMPT_SYSTEM_BASE+104)))
    systemSounds.append(("quinientos", filename(PROMPT_SYSTEM_BASE+105)))
    systemSounds.append(("sescientos", filename(PROMPT_SYSTEM_BASE+106)))
    systemSounds.append(("setecientos", filename(PROMPT_SYSTEM_BASE+107)))
    systemSounds.append(("ochocientos", filename(PROMPT_SYSTEM_BASE+108)))
    systemSounds.append(("nuevecientos", filename(PROMPT_SYSTEM_BASE+109)))
    systemSounds.append(("mil", filename(PROMPT_SYSTEM_BASE+110)))
    for i, s in enumerate(["coma", "un", "una", "y", "meno", "hora", "horas", "minuto", "minutos", "segundo", "segundos"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+111+i)))
    for i, s in enumerate([u"Voltio", u"ampério", u"metros por segundo", u"", u"kilómetro por hora", u"metros", u"grados", u"por ciento", u"miliamperios", u"miliamperios por hora", u"vatio", u"db", u"pés", u"nós", u"horas", u"minutos", u"segundos", u"RPM", u"g"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+122+i)))            
    for s, f, a in [(u"me tienes abandonada", "inactiv", 486),
                    (u"batería del transmisor baja", "lowbatt", 485),
                    (u"El acelerador está activado, por favor, corrijalo", "thralert", 481),
                    (u"Los interruptores no están en la posición correcta, por favor corrijalo", "swalert", 482),
                    (u"eeprom corrompida", "eebad", NO_ALTERNATE),
                    (u"Formateo de eeprom en curso", "eeformat", NO_ALTERNATE),
                    (u"error", "error", NO_ALTERNATE),
                    (u"trim centrado", "midtrim", 495),
                    (u"potenciómetro centrado", "midpot", 496),
                    (u"trim al máximo", "endtrim", NO_ALTERNATE),
                    (u"20 segundos", "timer20", 500),
                    (u"30 segundos", "timer30", 501),
                    (u"assorbimiento elevado", "highmah", NO_ALTERNATE),
                    (u"temperatura elevada", "hightemp", NO_ALTERNATE),
                    (u"Alarma A1", "a1_org", NO_ALTERNATE),
                    (u"A1 crítica", "a1_red", NO_ALTERNATE),
                    (u"Alarma A2", "a2_org", NO_ALTERNATE),
                    (u"A2 crítica", "a2_red", NO_ALTERNATE),
                    (u"recepcion de señal baja", "rssi_org", NO_ALTERNATE),
                    (u"recepción de señal crítica", "rssi_red", NO_ALTERNATE),
                    (u"Problemas con la antena del transmisor", "swr_red", NO_ALTERNATE),
                    (u"Sin telemetría", "telemko", NO_ALTERNATE),
                    (u"Telemetría disponible", "telemok", NO_ALTERNATE),
                 ]:
        systemSounds.append((s, filename(f, a)))
    for i, s in enumerate([u"cronómetro", u"cronómetro", u"transmisión", u"recepción", u"A1", u"A2", u"altitud", u"motor",
                            u"combustible", u"temperatura", u"temperatura", u"velocidad", u"distancia", u"altitude", u"célula lipo",
                            u"Total lipo", u"voltaje", u"corriente", u"consumo", u"potencia", u"aceleración X", u"aceleración Y", u"aceleración Z",
                            u"dirección", u"variómetro", u"minimo", u"máximo"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+141+i)))
    for i, (s, f) in enumerate([
                  (u"tren arriba.", "gearup"),
                  (u"tren abajo.", "geardn"),
                  (u"flaps arriba", "flapup"),
                  (u"flaps abajo", "flapdn"),
                  (u"aterrizaje", "attero"),
                  (u"modo de entrenamiento activado", "trnon"),
                  (u"modo de entrenamiento deshabilitado", "trnoff"),
                  (u"motor apagado", "engoff"),
                  (u"muy alto", "tohigh"),
                  (u"muy bajo", "tolow"),
                  (u"bateria baja", "lowbat"),
                  (u"crou, activo", "crowon"),
                  (u"crou, desligado", "crowof"),
                  (u"modo de velocidad", "spdmod"),
                  (u"modo de térmica", "thmmod"),
                  (u"modo de vuelo normal", "nrmmod"),
                  (u"fase de vuelo 1", "fltmd1"),
                  (u"fase de vuelo 2", "fltmd2"),
                  (u"fase de vuelo 3", "fltmd3"),
                  (u"fase de vuelo 4", "fltmd4"),
                  (u"fase de vuelo 5", "fltmd5"),
                  (u"fase de vuelo 6", "fltmd6"),
                  (u"fase de vuelo 7", "fltmd7"),
                  (u"fase de vuelo 8", "fltmd8"),
                  (u"fase de vuelo 9", "fltmd9"),
                 ]):
        sounds.append((s, filename(f, PROMPT_CUSTOM_BASE+i)))
    return systemSounds, sounds

################################################################

def ttsDe():
    systemSounds = []
    sounds = []

    for i in range(101):
        systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE+i)))
    systemSounds.append(("tausend", filename(PROMPT_SYSTEM_BASE+101)))
    for i, s in enumerate(["comma", "und", "minus", "uhr", "minute", "minuten", "sekunde", "sekunden"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+102+i)))
    for i, s in enumerate(["Volt", "Ampere", "Meter pro sekunde", "", "kilometer pro stunde", "Meter", "Grad", "Prozent", "Milliampere", "Milliampere pro stunde", "Watt", "db", "Fuesse", "Knoten", "Uhr", "Minuten", "Secunden", "R P M", "g"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+110+i)))            
    for s, f, a in [(u"Sender ist inaktiv,bitte Ueberpruefen Sie", "inactiv", 486),
                    (u"Senderakku niedrig", "lowbatt", 485),
                    (u"Gaskanal nicht Null, bitte pruefen", "thralert", 481),
                    (u"Schalten fehlpositioniert, bitte pruefen", "swalert", 482),
                    (u"Das EEPROM ist fehlerhaft", "eebad", NO_ALTERNATE),
                    (u"Das EEPROM ist immer formatiert", "eeformat", NO_ALTERNATE),
                    (u"fehler", "error", NO_ALTERNATE),
                    (u"Trim zentriert", "midtrim", 495),
                    (u"Poti zentriert", "midpot", 496),
                    (u"Maximale trimmung erreicht", "endtrim", NO_ALTERNATE),
                    (u"20 sekunden", "timer20", 500),
                    (u"30 sekunden", "timer30", 501),
                    (u"Senderstrom ist zu hoch!", "highmah", NO_ALTERNATE),
                    (u"Sendertemperatur ist zu hoch!", "hightemp", NO_ALTERNATE),
                    (u"A1 schlecht!", "a1_org", NO_ALTERNATE),
                    (u"A1 kritisch!", "a1_red", NO_ALTERNATE),
                    (u"A2 schlecht!", "a2_org", NO_ALTERNATE),
                    (u"A2 kritisch!", "a2_red", NO_ALTERNATE),
                    (u"Funksignal schlecht!", "rssi_org", NO_ALTERNATE),
                    (u"Funksignal kritisch!", "rssi_red", NO_ALTERNATE),
                    (u"Problem mit der sender Antenna", "swr_red", NO_ALTERNATE),
                 ]:
        systemSounds.append((s, filename(f, a)))
    for i, s in enumerate(["Timer", "Timer",  "Sendung", "Empfang", "A1", "A2", "Hoehe", "Motor",
                           "Treibstoff", "Temperatur", "Temperatur", "Geschwindigkeit", "Ferne", "Hoehe", "Lipo-Zelle",
                           "Zellen gesamt", "Spannung", "Strom", "Verbrauch", "Power", "Beschleunigung X", "Beschleunigung Y", "Beschleunigung Z",
                           "Richtung", "Variometer","Minimum","Maximum"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+129+i)))
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
        sounds.append((s, filename(f, PROMPT_CUSTOM_BASE+i)))
    return systemSounds, sounds

################################################################

def ttsCz():
    systemSounds = []
    sounds = []

    for i, s in enumerate(["nula", "jedna", "dva"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+i)))    
    for i in range(97):
        systemSounds.append((str(3+i), filename(PROMPT_SYSTEM_BASE+3+i)))
    for i, s in enumerate(["sto", u"dvěsta", u"třista", u"čtyřista", u"pětset", u"šestset", "sedmset", "osmset",             
                            u"devětset", u"tisíc"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+100+i)))
    for i, s in enumerate([u"tisíce", "jeden", "jedno", u"dvě", u"celá", u"celé", u"celých", u"mínus"]):
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+110+i)))
        
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
        systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+118+i)))

    for s, f, a in [(u"střed trimu", "midtrim", 495),
                    (u"maximum trimu", "endtrim", NO_ALTERNATE),
                    (u"baterie rádia je vybitá", "lowbatt", 485),
                    (u"zapoměl jsi na mě!", "inactiv", 486),
                    (u"plyn není na nule", "thralert", 481),
                    (u"přepínače nejsou ve výchozí poloze", "swalert", 482),
                    (u"neplatná eeprom", "eebad", 483),
                    (u"Oupn najn iks", "tada", 480),
                    (u"dvacet sekund", "timer20", 500),
                    (u"třicet sekund", "timer30", 501),
                    (u"deset sekund", "timer10", NO_ALTERNATE),
                    (u"time up!", "timerlt3", NO_ALTERNATE),
                    (u"A1,nízká", "a1_org", NO_ALTERNATE),
                    (u"A1,kritická", "a1_red", NO_ALTERNATE),
                    (u"A2,nízká", "a2_org", NO_ALTERNATE),
                    (u"A2,kritická", "a2_red", NO_ALTERNATE),
                    (u"nízký RF signál", "rssi_org", NO_ALTERNATE),
                    (u"kritický RF signál", "rssi_red", NO_ALTERNATE),
                    (u"problém s anténou rádia", "swr_red", NO_ALTERNATE),
                    (u"telemetrie ztracena", "telemko", NO_ALTERNATE),
                    (u"telemetrie je funkční", "telemok", NO_ALTERNATE),
                   ]:
        systemSounds.append((s, filename(f, a)))
    for i, (s, f) in enumerate([
                 (u"podvozek je zasunut", "podvozek0"),
                 (u"podvozek je vysunut", "podvozek1"),
                 (u"klapky jsou zapnuty", "klapky1"),
                 (u"klapky jsou vypnuty", "klapky0"),
                 (u"trenér je zapnut", "trener1"),
                 (u"trenér je vypnutý", "trener0"),
                 (u"motor je vypnut", "motor0"),
                 (u"motor je aktivován", "motor1"),
                 (u"jsi příliš vysoko", "vysoko"),
                 (u"jsi příliš nízko", "nizko"),
                 (u"baterie modelu je vybitá", "bytvybita"),
                 (u"dochází palivo", "palivo"),
                 (u"nízký signál!", "nsignal"),
                 (u"kritický signál!", "ksignal"),
                 (u"rychlý režim aktivován", "r_rychly"),
                 (u"termický režim aktivován", "r_termik"),
                 (u"normální režim aktivován", "r_norm"),
                 (u"režim start aktivován", "r_start"),
                 (u"režim přistání aktivován", "r_pristani"),
                 (u"vario je zapnuto", "vario0"),
                 (u"vario je vypnuto", "vario1"),
                 (u"start povolen", "stpovolen"),
                 ]):
        sounds.append((s, filename(f, PROMPT_CUSTOM_BASE+i)))
    return systemSounds, sounds

################################################################

if __name__ == "__main__":
    
    if "sky9x" in sys.argv:
        board = "sky9x"
        PROMPT_CUSTOM_BASE = 256
        PROMPT_SYSTEM_BASE = 0
    elif "taranis" in sys.argv:  
        board = "taranis"
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
            exit()
    
    if "mulaw" in sys.argv:
        defaultcodec = "pcm_mulaw"
    else:
        defaultcodec = "pcm_alaw"
    
    if "en" in sys.argv:
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
        systemSounds, sounds = ttsEn()
    
    elif "fr" in sys.argv:
        directory = "fr"
        voice = "french"
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-fr4+f4"
            espeakspeed = "140"
        systemSounds, sounds = ttsFr()
        
    elif "it" in sys.argv:
        directory = "it"
        voice = "italian"
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-it4"
            espeakspeed = "160"
        systemSounds, sounds = ttsIt()
        
    elif "de" in sys.argv:
        directory = "de"
        voice = "german"
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-de4"
            espeakspeed = "160"
        systemSounds, sounds = ttsDe()

    elif "pt" in sys.argv:
        directory = "pt"
        voice = "portuguese"
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-pt1+f1"
            espeakspeed = "160"
        systemSounds, sounds = ttsPt()

    elif "es" in sys.argv:
        directory = "es"
        voice = "spanish"
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftVirginie_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-es1+f1"
            espeakspeed = "160"
        systemSounds, sounds = ttsEs()
        
    elif "cz" in sys.argv:
        directory = "cz"
        voice = "czech"               
        if "sapi" in sys.argv:
            tts.SetVoiceByName("ScanSoftZuzana_Full_22kHz")
        elif "espeak" in sys.argv:
            espeakVoice = "mb-cz2"
            espeakspeed = "150"
        systemSounds, sounds = ttsCz()
        
    else:
        print "which language?"
        exit()                             
        
    if "csv" in sys.argv:
        csvFile = open("%s-%s.csv" % (voice, board), "w")
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
        csvFile.close()
            
    if "zip" in sys.argv:
        zip_name = "%s-%s.zip" % (voice, board)
        zip = zipfile.ZipFile(zip_name, "w", zipfile.ZIP_DEFLATED)
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
        zip.close()
