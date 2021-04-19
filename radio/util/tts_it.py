# -*- coding: utf-8 -*-

# Italian language sounds configuration

from tts_common import filename


systemSounds = []
sounds = []

for i in range(101):
    systemSounds.append((str(i), filename(i)))
systemSounds.append(("mila", filename(101)))
systemSounds.append(("mille", filename(102)))
for i, s in enumerate(["virgola", "un", "e", "meno", "ora", "ore", "minuto", "minuti", "secondo", "secondi"]):
    systemSounds.append((s, filename(103 + i)))
for i, (s, f) in enumerate([("volt", "volt0"),("volt", "volt1"),
                            ("ampere", "amp0"),("ampere", "amp1"),
                            ("milliampere", "mamp0"),("milliampere", "mamp1"),
                            ("nodo", "knot0"), ("nodi", "knot1"),
                            ("metri al secondo", "mps0"), ("metri al secondo", "msp1"),
                            ("piedi al secondo", "fps0"), ("piedi al secondo", "fps1"),
                            ("chilometri orari", "kph0"), ("chilometri orari", "kph1"),
                            ("miglia orarie", "mph0"), ("miglia orarie", "mph1"),
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
                            ("radiante", "rad0"), ("radianti", "rad1"),
                            ("millilitro", "m10"), ("millilitri", "m11"),
                            ("oncia fluida", "founce0"), ("once fluide", "founce1"),
                            ("millilitro per minuto", "mlpm0"), ("millilitri per minuto", "mlpm1"),
                            ("ora", "hour0"), ("ore", "hour1"),
                            ("minuto", "minute0"), ("minuti", "minute1"),
                            ("secondo", "second0"), ("secondi", "second1"),
                            ]):
    systemSounds.append((s, filename(f)))
for s, f in [("radio inattiva controllare", "inactiv"),
             ("batteria della radio scarica", "lowbatt"),
             ("controllo motore non in posizione, verificare", "thralert"),
             ("interruttori non in posizione, verificare", "swalert"),
             ("eeprom corrotta", "eebad"),
             ("errore", "error"),
             ("trim centrato", "midtrim"),
             ("massimo trim raggiunto", "maxtrim"),
             ("minimo trim raggiunto", "mintrim"),
             ("assorbimento elevato", "highmah"),
             ("temperatura elevata", "hightemp"),
             ("Segnale radio basso", "rssi_org"),
             ("Segnale radio critico", "rssi_red"),
             ("Problema all'antenna della radio", "swr_red"),
             ("Telemetria assente", "telemko"),
             ("Telemetria disponibile", "telemok"),
             ("Segnale trainer perso", "trainko"),
             ("Segnale trainer disponibile", "trainok"),
             ("Sensore perso", "sensorko"),
             ("sovraccarico servo", "servoko"),
             ("sovraccarico di alimentazione", "rxko"),
             ("ricevente ancora connessa", "modelpwr"),
             ]:
    systemSounds.append((s, filename(f)))
for i, s in enumerate(["timer", "", "tensione", "tensione", "trasmissione", "ricezione", "altitudine", "motore",
                       "carburante", "temperatura", "temperatura", "velocità", "distanza", "altitudine", "cella lipo",
                       "totale lipo", "tensione", "corrente", "consumo", "potenza", "accelerazione X", "accelerazione Y", "accelerazione Z",
                       "direzione", "variometro", "minimo", "massimo"]):
    systemSounds.append((s, filename(155 + i)))
for i, (s, f) in enumerate([("carrello chiuso", "gearup"),
                            ("carrello aperto", "geardn"),
                            ("flap rientrati", "flapup"),
                            ("flap estesi", "flapdn"),
                            ("spoiler su", "splrup"),
                            ("spoiler giù", "splrdn"),
                            ("atterraggio", "attero"),
                            ("modalità maestro attiva", "trnon"),
                            ("modalità maestro disattiva", "trnoff"),
                            ("motore spento", "engoff"),
                            ("troppo alto", "tohigh"),
                            ("troppo basso", "tolow"),
                            ("batteria scarica", "lowbat"),
                            ("crow attivato", "crowon"),
                            ("crow disattivato", "crowof"),
                            ("segnale radio basso", "siglow"),
                            ("segnale radio critico", "sigcrt"),
                            ("modo velocità", "spdmod"),
                            ("modo termica", "thmmod"),
                            ("modo volo normale", "nrmmod"),
                            ("modo atterraggio", "lnding"),
                            ("modo acro", "acro"),
                            ("fase di volo 1", "fltmd1"),
                            ("fase di volo 2", "fltmd2"),
                            ("fase di volo 3", "fltmd3"),
                            ("fase di volo 4", "fltmd4"),
                            ("fase di volo 5", "fltmd5"),
                            ("fase di volo 6", "fltmd6"),
                            ("fase di volo 7", "fltmd7"),
                            ("variometro attivato", "vrion"),
                            ("variometro disabilitato", "vrioff"),
                            ("fase di volo power", "fm-pwr"),
                            ("fase di volo land", "fm-lnd"),
                            ("fase di volo planata", "fm-flt"),
                            ("fase di volo velocità", "fm-spd"),
                            ("fase di volo veloce", "fm-fst"),
                            ("fase di volo normale", "fm-nrm"),
                            ("fase di volo cruise", "fm-crs"),
                            ("fase di volo acro", "fm-acr"),
                            ("fase di volo race", "fm-rce"),
                            ("fase di volo lancio", "fm-lch"),
                            ("fase di volo ping", "fm-png"),
                            ("fase di volo termica", "fm-thm"),
                            ("fase di volo termica sinistra", "fm-thl"),
                            ("fase di volo termica destra", "fm-thr"),
                            ]):
    sounds.append((s, filename(f)))
