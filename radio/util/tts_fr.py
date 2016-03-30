# -*- coding: utf-8 -*-

# French language sounds configuration

from tts import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


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
for i, s in enumerate(["volts", u"ampères", u"milli ampères", u"knots", u"mètres seconde", u"pieds par seconde", u"kilomètre heure", u"miles par heure", u"mètres", "pieds", u"degrés", u"degrés fahrenheit", "pourcents", u"milli ampères / heure", "watt", "db", "tours minute", "g", u"degrés", "millilitres", "onces", "heure", "minute", "seconde"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 125 + i)))
# for i, s in enumerate(["timer", "", "tension", "tension", u"émission", u"réception", "altitude", "moteur",
#                        "essence", u"température", u"température", "vitesse", "distance", "altitude", u"élément lipo",
#                        "total lipo", "tension", "courant", "consommation", "puissance", u"accelération X", u"accelération Y", u"accelération Z",
#                        "orientation", "vario"]):
#     systemSounds.append((s, filename(PROMPT_SYSTEM_BASE+146+i)))
for i, s in enumerate(["virgule 0", "virgule 1", "virgule 2", "virgule 3", "virgule 4", "virgule 5", "virgule 6", "virgule 7", "virgule 8", "virgule 9"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 180 + i)))
for s, f, a in [(u"Trim centré", "midtrim", 495),
                (u"Trim maximum atteint", "endtrim", NO_ALTERNATE),
                (u"Batterie radio faible !", "lowbatt", 485),
                (u"Radio inactive !", "inactiv", 486),
                (u"Alerte manche des gaz", "thralert", 481),
                (u"Alerte inters", "swalert", 482),
                (u"éprome corrompue", "eebad", 483),
                (u"Bienvenue sur Open TI X!", "tada", 480),
                (u"vingt secondes", "timer20", 500),
                (u"trente secondes", "timer30", 501),
                (u"A1,faible", "a1_org", NO_ALTERNATE),
                (u"A1,critique", "a1_red", NO_ALTERNATE),
                (u"A2,faible", "a2_org", NO_ALTERNATE),
                (u"A2,critique", "a2_red", NO_ALTERNATE),
                (u"A3,faible", "a3_org", NO_ALTERNATE),
                (u"A3,critique", "a3_red", NO_ALTERNATE),
                (u"A4,faible", "a4_org", NO_ALTERNATE),
                (u"A4,critique", "a4_red", NO_ALTERNATE),
                (u"Signal RF, faible", "rssi_org", NO_ALTERNATE),
                (u"Signal RF, critique", "rssi_red", NO_ALTERNATE),
                (u"Antenne défectueuse", "swr_red", NO_ALTERNATE),
                (u"Plus de télémétrie", "telemko", NO_ALTERNATE),
                (u"Télémétrie retrouvée", "telemok", NO_ALTERNATE),
                (u"Signal écolage perdu", "trainko", NO_ALTERNATE),
                (u"Signal écolage retrouvé", "trainok", NO_ALTERNATE),
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
