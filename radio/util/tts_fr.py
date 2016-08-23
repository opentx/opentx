# -*- coding: utf-8 -*-

# French language sounds configuration

from tts_common import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE

systemSounds = []
sounds = []

for i in range(100):
    systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE + i)))
for i in range(10):
    systemSounds.append((str(100 * (i + 1)), filename(PROMPT_SYSTEM_BASE + 100 + i)))
for i, s in enumerate(["une", "onze", "vingt et une", "trente et une", "quarante et une", "cinquante et une", "soixante et une", "soixante et onze", "quatre vingt une"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 110 + i)))
for i, s in enumerate(["virgule", "et", "moins", "minuit", "midi"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 119 + i)))
    
# The list bellow MUST be kept in sync with /radio/src/translate/tts_fr.cpp
for s, f, a in enumerate[(u"volts","volt", NO_ALTERNATE),  
             (u"ampères", "amp", NO_ALTERNATE),
             (u"milli ampères", "mamp", NO_ALTERNATE),
             (u"knots", "knot", NO_ALTERNATE),
             (u"mètres seconde", "mps", NO_ALTERNATE),
             (u"pieds par seconde", "fps", NO_ALTERNATE),
             (u"kilomètre heure", "kph", NO_ALTERNATE),
             (u"miles par heure", "mph", NO_ALTERNATE),
             (u"mètres", "meter", NO_ALTERNATE),
             (u"pieds", "foot", NO_ALTERNATE),
             (u"degrés", "celsius", NO_ALTERNATE),
             (u"degrés fahr", "degf", NO_ALTERNATE),
             (u"pourcents", "percent", NO_ALTERNATE),
             (u"milli ampères heure", "mamph", NO_ALTERNATE),
             (u"watt", "watt", NO_ALTERNATE),
             (u"milli watt", "mwatt", NO_ALTERNATE),
             (u"db", "db", NO_ALTERNATE),
             (u"tours minute", "rpm", NO_ALTERNATE),
             (u"g", "g", NO_ALTERNATE),
             (u"degrés", "degree", NO_ALTERNATE),
             (u"radians", "rad", NO_ALTERNATE),
             (u"millilitres", "ml", NO_ALTERNATE),
             (u"onces", "founce", NO_ALTERNATE),
             (u"heure", "hour", NO_ALTERNATE),
             (u"minute", "minute", NO_ALTERNATE),
             (u"seconde", "second", NO_ALTERNATE),
             ]:
    systemSounds.append((s, filename(f, a)))
for i, s in enumerate(["virgule 0", "virgule 1", "virgule 2", "virgule 3", "virgule 4", "virgule 5", "virgule 6", "virgule 7", "virgule 8", "virgule 9"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 180 + i)))
for s, f, a in [(u"Trim centré", "midtrim", 495),
                (u"Trim maximum atteint", "endtrim", NO_ALTERNATE),
                (u"Chrono 1 à 0", "timovr1", NO_ALTERNATE),
                (u"Chrono 2 à 0", "timovr2", NO_ALTERNATE),
                (u"Chrono 3 à 0", "timovr3", NO_ALTERNATE),
                (u"Batterie radio faible !", "lowbatt", 485),
                (u"Radio inactive !", "inactiv", 486),
                (u"Alerte manche des gaz", "thralert", 481),
                (u"Alerte inters", "swalert", 482),
                (u"éprome corrompue", "eebad", 483),
                (u"Bienvenue sur Open Thé Hix!", "hello", 480),
                (u"Signal RF, faible", "rssi_org", NO_ALTERNATE),
                (u"Signal RF, critique", "rssi_red", NO_ALTERNATE),
                (u"Antenne défectueuse", "swr_red", NO_ALTERNATE),
                (u"Plus de télémétrie", "telemko", NO_ALTERNATE),
                (u"Télémétrie retrouvée", "telemok", NO_ALTERNATE),
                (u"Signal écolage perdu", "trainko", NO_ALTERNATE),
                (u"Signal écolage retrouvé", "trainok", NO_ALTERNATE),
                (u"Sonde de télémétrie perdue", "sensorko", NO_ALTERNATE),
                (u"Servo en surcharge", "servoko", NO_ALTERNATE),
                (u"Surcharge réception", "rxko", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, (s, f) in enumerate([(u"altitude", "altitude"),
                            (u"température moteur", "moteur"),
                            (u"température contrôleur", "cntrleur"),
                            (u"train rentré", "gearup"),
                            (u"train sorti", "geardn"),
                            (u"volets rentrés", "flapup"),
                            (u"volets sortis", "flapdn"),
                            (u"atterrissage", "attero"),
                            (u"écolage", "trnon"),
                            (u"fin écolage", "trnoff"),
                            (u"moteur coupé", "engoff"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
