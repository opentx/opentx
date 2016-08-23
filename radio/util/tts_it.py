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
for s, f, a in [(u"volt", "volt0", NO_ALTERNATE),
                (u"ampere", "amp0", NO_ALTERNATE),
                (u"milliampers", "mamp0", NO_ALTERNATE),
                (u"nodo", "knot0", NO_ALTERNATE), ("nodi", "knot1", NO_ALTERNATE),
                (u"metro al secondo", "mps0", NO_ALTERNATE), ("metri al secondo", "msp1", NO_ALTERNATE),
                (u"piede al secondo", "fps0", NO_ALTERNATE), ("piedi al secondo", "fps1", NO_ALTERNATE),
                (u"chilometro ora", "kph0", NO_ALTERNATE), ("kilometers per hour", "kph1", NO_ALTERNATE),
                (u"miglio per ora", "mph0", NO_ALTERNATE), ("miglia per ora", "mph1", NO_ALTERNATE),
                (u"metro", "meter0", NO_ALTERNATE), ("metri", "meter1", NO_ALTERNATE),
                (u"piede", "foot0", NO_ALTERNATE), ("piedi", "foot1", NO_ALTERNATE),
                (u"grado celsius", "celsius0", NO_ALTERNATE), ("gradi celsius", "celsius1", NO_ALTERNATE),
                (u"grado fahrenheit", "fahr0", NO_ALTERNATE), ("gradi fahrenheit", "fahr1", NO_ALTERNATE),
                (u"percento", "percent0", NO_ALTERNATE),
                (u"milliampere ora", "mamph0", NO_ALTERNATE),
                (u"watt", "watt0", NO_ALTERNATE),
                (u"milli-watt", "mwatt0", NO_ALTERNATE), ("milli-watts", "mwatt1", NO_ALTERNATE),
                (u"db", "db0", NO_ALTERNATE),
                (u"r p m", "rpm0", NO_ALTERNATE),
                (u"g", "g0", NO_ALTERNATE),
                (u"grado", "degree0", NO_ALTERNATE), ("gradi", "degree1", NO_ALTERNATE),
                (u"radian", "rad0", NO_ALTERNATE), ("radians", "rad1", NO_ALTERNATE),
                (u"millilitro", "m10", NO_ALTERNATE), ("millilitri", "m11", NO_ALTERNATE),
                (u"oncia", "founce0", NO_ALTERNATE), ("once", "founce1", NO_ALTERNATE),
                (u"hour", "hour0", NO_ALTERNATE), ("hours", "hour1", NO_ALTERNATE),
                (u"minute", "minute0", NO_ALTERNATE), ("minutes", "minute1", NO_ALTERNATE),
                (u"second", "second0", NO_ALTERNATE), ("seconds", "second1", NO_ALTERNATE),
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
