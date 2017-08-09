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
for i, (s, f) in enumerate([("volt", "volt0"),
                            ("ampere", "amp0"),
                            ("milliampers", "mamp0"),
                            ("nodo", "knot0"), ("nodi", "knot1"),
                            ("metro al secondo", "mps0"), ("metri al secondo", "msp1"),
                            ("piede al secondo", "fps0"), ("piedi al secondo", "fps1"),
                            ("chilometro ora", "kph0"), ("chilometri orari", "kph1"),
                            ("miglio per ora", "mph0"), ("miglia per ora", "mph1"),
                            ("metro", "meter0"), ("metri", "meter1"),
                            ("piede", "foot0"), ("piedi", "foot1"),
                            ("grado celsius", "celsius0"), ("gradi celsius", "celsius1"),
                            ("grado fahrenheit", "fahr0"), ("gradi fahrenheit", "fahr1"),
                            ("percento", "percent0"), ("percento", "percent1"),
                            ("milliampere ora", "mamph0"), ("milliampere ora", "mamph1"),
                            ("watt", "watt0"), ("watt", "watt1"),
                            ("milli-watt", "mwatt0"), ("milli-watt", "mwatt1"),
                            ("d b", "db0"), ("d b", "db1"),
                            ("r p m", "rpm0"), ("r p m", "rpm1"),
                            ("g", "g0"), ("g", "g1"),
                            ("grado", "degree0"), ("gradi", "degree1"),
                            ("radian", "rad0"), ("radians", "rad1"),
                            ("millilitro", "m10"), ("millilitri", "m11"),
                            ("oncia", "founce0"), ("once", "founce1"),
                            ("ora", "hour0"), ("ore", "hour1"),
                            ("minuto", "minute0"), ("minuti", "minute1"),
                            ("secondo", "second0"), ("secondi", "second1"),
                            ]):
    systemSounds.append((s, filename(f, PROMPT_SYSTEM_BASE + 113 + i)))
for s, f, a in [("radio inattiva controllare", "inactiv", 486),
                ("batteria della radio scarica", "lowbatt", 485),
                ("controllo motore non in posizione, verificare", "thralert", 481),
                ("interruttori non in posizione, verificare", "swalert", 482),
                ("eeprom corrotta", "eebad", NO_ALTERNATE),
                ("formattazzione eeprom in corso", "eeformat", NO_ALTERNATE),
                ("errore", "error", NO_ALTERNATE),
                ("trim centrato", "midtrim", 495),
                ("potenziometro centrato", "midpot", 496),
                ("massimo trim raggiunto", "maxtrim", NO_ALTERNATE),
                ("minimo trim raggiunto", "mintrim", NO_ALTERNATE),
                ("venti secondi", "timer20", 500),
                ("trenta secondi", "timer30", 501),
                ("assorbimento elevato", "highmah", NO_ALTERNATE),
                ("temperatura elevata", "hightemp", NO_ALTERNATE),
                ("A1 in allarme", "a1_org", NO_ALTERNATE),
                ("A1 critico", "a1_red", NO_ALTERNATE),
                ("A2 in allarme", "a2_org", NO_ALTERNATE),
                ("A2 critico", "a2_red", NO_ALTERNATE),
                ("A3 in allarme", "a3_org", NO_ALTERNATE),
                ("A3 critico", "a3_red", NO_ALTERNATE),
                ("A4 in allarme", "a4_org", NO_ALTERNATE),
                ("A4 critico", "a4_red", NO_ALTERNATE),
                ("Segnale radio basso", "rssi_org", NO_ALTERNATE),
                ("Segnale radio critico", "rssi_red", NO_ALTERNATE),
                ("Problema all'antenna della radio", "swr_red", NO_ALTERNATE),
                ("Telemetria assente", "telemko", NO_ALTERNATE),
                ("Telemetria disponibile", "telemok", NO_ALTERNATE),
                ("servo overload", "servoko", NO_ALTERNATE),
                ("power overload", "rxko", NO_ALTERNATE),
                ("receiver still connected", "modelpwr", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, s in enumerate(["timer", "", "tensione", "tensione", "trasmissione", "ricezione", "altitudine", "motore",
                       "carburante", "temperatura", "temperatura", "velocità", "distanza", "altitudine", "cella lipo",
                       "totale lipo", "tensione", "corrente", "consumo", "potenza", "accellerazione X", "accellerazione Y", "accellerazione Z",
                       "direzione", "variometro", "minimo", "massimo"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 135 + i)))
for i, (s, f) in enumerate([("carrello chiuso", "gearup"),
                            ("carrello aperto", "geardn"),
                            ("flap rientrati", "flapup"),
                            ("flap estesi", "flapdn"),
                            ("atterraggio", "attero"),
                            ("modalità maestro attiva", "trnon"),
                            ("modalità maestro disattiva", "trnoff"),
                            ("motore spento", "engoff"),
                            ("troppo alto", "tohigh"),
                            ("troppo basso", "tolow"),
                            ("batteria scarica", "lowbat"),
                            ("crow on", "crowon"),
                            ("crow off", "crowof"),
                            ("modo velocità", "spdmod"),
                            ("modo termica", "thmmod"),
                            ("modo volo normale", "nrmmod"),
                            ("fase di volo 1", "fltmd1"),
                            ("fase di volo 2", "fltmd2"),
                            ("fase di volo 3", "fltmd3"),
                            ("fase di volo 4", "fltmd4"),
                            ("fase di volo 5", "fltmd5"),
                            ("fase di volo 6", "fltmd6"),
                            ("fase di volo 7", "fltmd7"),
                            ("fase di volo 8", "fltmd8"),
                            ("fase di volo 9", "fltmd9"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
