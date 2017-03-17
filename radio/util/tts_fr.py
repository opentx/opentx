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
for i, (s, f) in enumerate([(u"volts","volt0"),  
                            (u"ampères", "amp0"),
                            (u"milli ampères", "mamp0"),
                            (u"noeud", "knot0"),
                            (u"mètres seconde", "mps0"),
                            (u"pieds par seconde", "fps0"),
                            (u"kilomètre heure", "kph0"),
                            (u"miles par heure", "mph0"),
                            (u"mètres", "meter0"),
                            (u"pieds", "foot0"),
                            (u"degrés", "celsius0"),
                            (u"degrés fahrenheit", "degf0"),
                            (u"pourcents", "percent0"),
                            (u"milli ampères heure", "mamph0"),
                            (u"watt", "watt0"),
                            (u"milli watt", "mwatt0"),
                            (u"d b", "db0"),
                            (u"tours minute", "rpm0"),
                            (u"g", "g0"),
                            (u"degrés", "degree0"),
                            (u"radians", "rad0"),
                            (u"millilitres", "ml0"),
                            (u"onces", "founce0"),
                            (u"heure", "hour0"),
                            (u"minute", "minute0"),
                            (u"seconde", "second0"),
                            ]):
    systemSounds.append((s, filename(f, PROMPT_SYSTEM_BASE + 125 + i)))
for i, s in enumerate(["virgule 0", "virgule 1", "virgule 2", "virgule 3", "virgule 4", "virgule 5", "virgule 6", "virgule 7", "virgule 8", "virgule 9"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 180 + i)))
for s, f, a in [(u"Trim centré", "midtrim", 495),
                (u"Trim maximum atteint", "maxtrim", NO_ALTERNATE),
                (u"Trim minimum atteint", "mintrim", NO_ALTERNATE),
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
