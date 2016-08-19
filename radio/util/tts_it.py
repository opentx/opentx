# -*- coding: utf-8 -*-

# Italian language sounds configuration

from tts_common import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


systemSounds = []
sounds = []

for i in range(101):
    systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE + i)))
systemSounds.append(("mila", filename(PROMPT_SYSTEM_BASE + 101)))
systemSounds.append(("mille", filename(PROMPT_SYSTEM_BASE + 102)))
for i, s in enumerate(["virgola", "un", "e", "meno", "ora", "ore", "minuto", "minuti", "secondo", "secondi"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 103 + i)))
    
# The list bellow MUST be kept in sync with /radio/src/translate/tts_it.cpp
for s, f, a in [(u"volt", "volt", NO_ALTERNATE),
                (u"ampere", "amp", NO_ALTERNATE),
                (u"milliampers", "mamp", NO_ALTERNATE),
                (u"nodo", "knot", NO_ALTERNATE), ("nodi", "knots", NO_ALTERNATE),
                (u"metro al secondo", "mps", NO_ALTERNATE), ("metri al secondo"", "msps", NO_ALTERNATE),
                (u"piede al secondo", "footps", NO_ALTERNATE), ("piedi al secondo", "feetps", NO_ALTERNATE),
                (u"chilometro ora", "kph", NO_ALTERNATE), ("kilometers per hour", "ksph", NO_ALTERNATE),
                (u"miglio per ora", "mph", NO_ALTERNATE), ("miglia per ora", "msph", NO_ALTERNATE),
                (u"metro", "meter", NO_ALTERNATE), ("metri", "meters", NO_ALTERNATE),
                (u"piede", "foot", NO_ALTERNATE), ("piedi", "feet", NO_ALTERNATE),
                (u"grado celsius", "degc", NO_ALTERNATE), ("gradi celsius", "degsc", NO_ALTERNATE),
                (u"grado fahrenheit", "degf", NO_ALTERNATE), ("gradi fahrenheit", "degsf", NO_ALTERNATE),
                (u"percento", "percent", NO_ALTERNATE),
                (u"milliampere ora", "mamph", NO_ALTERNATE),
                (u"watt", "watt", NO_ALTERNATE),
                (u"milli-watt", "mwatt", NO_ALTERNATE), ("milli-watts", "mwatts", NO_ALTERNATE),
                (u"db", "db", NO_ALTERNATE),
                (u"r p m", "rpm", NO_ALTERNATE),
                (u"g", "g", NO_ALTERNATE),
                (u"grado", "degree", NO_ALTERNATE), ("gradi", "degrees", NO_ALTERNATE),
                (u"radian", "rad", NO_ALTERNATE), ("radians", "rads", NO_ALTERNATE),
                (u"millilitro", "mm", NO_ALTERNATE), ("millilitri", "mms", NO_ALTERNATE),
                (u"oncia", "founce", NO_ALTERNATE), ("once", "founces", NO_ALTERNATE),
                (u"hour", "hour", NO_ALTERNATE), ("hours", "hours", NO_ALTERNATE),
                (u"minute", "minute", NO_ALTERNATE), ("minutes", "minutes", NO_ALTERNATE),
                (u"second", "second", NO_ALTERNATE), ("seconds", "seconds", NO_ALTERNATE),
                       ]:
    systemSounds.append((s, filename(f, a)))
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
                (u"A3 in allarme", "a3_org", NO_ALTERNATE),
                (u"A3 critico", "a3_red", NO_ALTERNATE),
                (u"A4 in allarme", "a4_org", NO_ALTERNATE),
                (u"A4 critico", "a4_red", NO_ALTERNATE),
                (u"Segnale radio basso", "rssi_org", NO_ALTERNATE),
                (u"Segnale radio critico", "rssi_red", NO_ALTERNATE),
                (u"Problema all'antenna della radio", "swr_red", NO_ALTERNATE),
                (u"Telemetria assente", "telemko", NO_ALTERNATE),
                (u"Telemetria disponibile", "telemok", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, s in enumerate([u"timer", u"", u"tensione", u"tensione", u"trasmissione", u"ricezione", u"altitudine", u"motore",
                       u"carburante", u"temperatura", u"temperatura", u"velocità", "distanza", u"altitudine", u"cella lipo",
                       u"totale lipo", u"tensione", u"corrente", u"consumo", u"potenza", u"accellerazione X", u"accellerazione Y", u"accellerazione Z",
                       u"direzione", u"variometro", u"minimo", u"massimo"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 135 + i)))
for i, (s, f) in enumerate([(u"carrello chiuso", "gearup"),
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
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
