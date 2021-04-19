# -*- coding: utf-8 -*-

# French language sounds configuration

from tts_common import filename

systemSounds = []
sounds = []

for i in range(100):
    systemSounds.append((str(i), filename(i)))
for i in range(10):
    systemSounds.append((str(100 * (i + 1)), filename(100 + i)))
for i, s in enumerate(["une", "onze", "vingt et une", "trente et une", "quarante et une", "cinquante et une", "soixante et une", "soixante et onze", "quatre vingt une"]):
    systemSounds.append((s, filename(110 + i)))
for i, s in enumerate(["virgule", "et", "moins", "minuit", "midi"]):
    systemSounds.append((s, filename(119 + i)))
for i, (s, f) in enumerate([("volts", "volt0"),
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
                            ("millilitre minute", "mlpm0"),
                            ("microsecond", "us0"),
                            ("milisecond", "ms0"),
                            ("hertz", "hertz0"),
                            ("kilometre", "km0"),
                            ("heure", "hour0"),
                            ("minute", "minute0"),
                            ("seconde", "second0"),
                            ]):
    systemSounds.append((s, filename(f)))
for i, s in enumerate(["virgule 0", "virgule 1", "virgule 2", "virgule 3", "virgule 4", "virgule 5", "virgule 6", "virgule 7", "virgule 8", "virgule 9"]):
    systemSounds.append((s, filename(180 + i)))
for s, f in [("Trim centré", "midtrim"),
             ("Trim maximum atteint", "maxtrim"),
             ("Trim minimum atteint", "mintrim"),
             ("Chrono 1 à 0", "timovr1"),
             ("Chrono 2 à 0", "timovr2"),
             ("Chrono 3 à 0", "timovr3"),
             ("Batterie radio faible !", "lowbatt"),
             ("Radio inactive !", "inactiv"),
             ("Alerte manche des gaz", "thralert"),
             ("Alerte inters", "swalert"),
             ("éprome corrompue", "eebad"),
             ("Bienvenue sur o pun t x", "hello"),
             ("Signal RF, faible", "rssi_org"),
             ("Signal RF, critique", "rssi_red"),
             ("Antenne défectueuse", "swr_red"),
             ("Télémétrie perdue", "telemko"),
             ("Télémétrie retrouvée", "telemok"),
             ("Signal écolage perdu", "trainko"),
             ("Signal écolage retrouvé", "trainok"),
             ("Sonde de télémétrie perdue", "sensorko"),
             ("Servo en surcharge", "servoko"),
             ("Surcharge réception", "rxko"),
             ("Récepteur encore sous tension", "modelpwr"),
             ]:
    systemSounds.append((s, filename(f)))
for i, (s, f) in enumerate([("armé", "armed"),
                            ("désarmé", "disarm"),
                            ("altitude", "alt"),
                            ("température moteur", "moteur"),
                            ("température contrôleur", "tctrl"),
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
    sounds.append((s, filename(f)))
