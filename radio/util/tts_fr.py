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
for i, (s, f) in enumerate([("volts","volt0"),
                            ("ampères", "amp0"),
                            ("milli ampères", "mamp0"),
                            ("noeud", "knot0"),
                            ("mètres seconde", "mps0"),
                            ("pieds par seconde", "fps0"),
                            ("kilomètre heure", "kph0"),
                            ("miles par heure", "mph0"),
                            ("mètres", "meter0"),
                            ("pieds", "foot0"),
                            ("degrés", "celsius0"),
                            ("degrés fahrenheit", "degf0"),
                            ("pourcents", "percent0"),
                            ("milli ampères heure", "mamph0"),
                            ("watt", "watt0"),
                            ("milli watt", "mwatt0"),
                            ("d b", "db0"),
                            ("tours minute", "rpm0"),
                            ("g", "g0"),
                            ("degrés", "degree0"),
                            ("radians", "rad0"),
                            ("millilitres", "ml0"),
                            ("onces", "founce0"),
                            ("heure", "hour0"),
                            ("minute", "minute0"),
                            ("seconde", "second0"),
                            ]):
    systemSounds.append((s, filename(f, PROMPT_SYSTEM_BASE + 125 + i)))
for i, s in enumerate(["virgule 0", "virgule 1", "virgule 2", "virgule 3", "virgule 4", "virgule 5", "virgule 6", "virgule 7", "virgule 8", "virgule 9"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 180 + i)))
for s, f, a in [("Trim centré", "midtrim", 495),
                ("Trim maximum atteint", "maxtrim", NO_ALTERNATE),
                ("Trim minimum atteint", "mintrim", NO_ALTERNATE),
                ("Chrono 1 à 0", "timovr1", NO_ALTERNATE),
                ("Chrono 2 à 0", "timovr2", NO_ALTERNATE),
                ("Chrono 3 à 0", "timovr3", NO_ALTERNATE),
                ("Batterie radio faible !", "lowbatt", 485),
                ("Radio inactive !", "inactiv", 486),
                ("Alerte manche des gaz", "thralert", 481),
                ("Alerte inters", "swalert", 482),
                ("éprome corrompue", "eebad", 483),
                ("Bienvenue sur o pun t x", "hello", 480),
                ("Signal RF, faible", "rssi_org", NO_ALTERNATE),
                ("Signal RF, critique", "rssi_red", NO_ALTERNATE),
                ("Antenne défectueuse", "swr_red", NO_ALTERNATE),
                ("Télémétrie perdue", "telemko", NO_ALTERNATE),
                ("Télémétrie retrouvée", "telemok", NO_ALTERNATE),
                ("Signal écolage perdu", "trainko", NO_ALTERNATE),
                ("Signal écolage retrouvé", "trainok", NO_ALTERNATE),
                ("Sonde de télémétrie perdue", "sensorko", NO_ALTERNATE),
                ("Servo en surcharge", "servoko", NO_ALTERNATE),
                ("Surcharge réception", "rxko", NO_ALTERNATE),
                ("Récepteur encore sous tension", "modelpwr", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, (s, f) in enumerate([("altitude", "altitude"),
                            ("température moteur", "moteur"),
                            ("température contrôleur", "cntrleur"),
                            ("train rentré", "gearup"),
                            ("train sorti", "geardn"),
                            ("volets rentrés", "flapup"),
                            ("volets sortis", "flapdn"),
                            ("atterrissage", "attero"),
                            ("écolage", "trnon"),
                            ("fin écolage", "trnoff"),
                            ("moteur coupé", "engoff"),
                            ("high. speed. mode!, active", "spdmod"),
                            ("thermal. mode!, on", "thmmod"),
                            ("normal. mode!, on", "nrmmod"),
                            ("landing. mode!, on", "lnding"),
                            ("mode acro", "acro"),
                            ("mode de vol un", "fm-1"),
                            ("mode de vol deux", "fm-2"),
                            ("mode de vol trois", "fm-3"),
                            ("mode de vol quatre", "fm-4"),
                            ("mode de vol 5", "fm-5"),
                            ("mode de vol 6", "fm-6"),
                            ("mode de vol sept", "fm-7"),
                            ("mode de vol 8", "fm-8"),
                            ("vario activé", "vrion"),
                            ("vario désactivé", "vrioff"),
                            ("mode de vol puissance", "fm-pwr"),
                            ("mode de vol attérissage", "fm-lnd"),
                            ("mode de vol amérissage", "fm-flt"),
                            ("mode de vol vitesse", "fm-spd"),
                            ("mode de vol rapide", "fm-fst"),
                            ("mode de vol normal", "fm-nrm"),
                            ("mode de vol de croisière", "fm-crs"),
                            ("mode de vol acro", "fm-acr"),
                            ("mode de vol course", "fm-rce"),
                            ("mode de vol lancé", "fm-lch"),
                            ("mode de vol ping", "fm-png"),
                            ("mode de vol thermique", "fm-thm"),
                            ("mode de vol thermique gauche", "fm-thl"),
                            ("mode de vol thermique droit", "fm-thr"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
