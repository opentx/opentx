# -*- coding: utf-8 -*-

# German language sounds configuration

from tts_common import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


systemSounds = []
sounds = []

for i in range(100):
    systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE + i)))
systemSounds.append(("ein", filename(PROMPT_SYSTEM_BASE + 100)))
systemSounds.append(("eine", filename(PROMPT_SYSTEM_BASE + 101)))
systemSounds.append(("hundert", filename(PROMPT_SYSTEM_BASE + 102)))
systemSounds.append(("tausend", filename(PROMPT_SYSTEM_BASE + 103)))
for i, s in enumerate(["komma", "und", "minus", "uhr", "minute", "minuten", "sekunde", "sekunden", "stunde", "stunden"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 104 + i)))
for i, (s, f) in enumerate([("Wolt","volt0"),
                            ("Ampär", "amp0"),
                            ("MilliAmpär", "mamp0"),
                            ("Knoten", "knot0"),
                            ("Meter pro sekunde", "mps0"),
                            ("Fuss pro sekunde", "fps0"),
                            ("Kilometer pro stunde", "kph0"),
                            ("Meilen pro Stunde", "mph0"),
                            ("Meter", "meter0"),
                            ("Fuesse", "foot0"),
                            ("Grad Celsius", "celsius0"),
                            ("Grad Fahrenheit", "fahr0"),
                            ("Prozent", "percent0"),
                            ("Milliampär pro stunde", "mamph0"),
                            ("Watt", "watt0"),
                            ("Milli Watt", "mwatt0"),
                            ("D B", "db0"),
                            ("Drehzahl", "rpm0"),
                            ("g", "g0"),
                            ("Grad", "degree0"),
                            ("radians", "rad0"),
                            ("Milliliter", "ml0"),
                            ("Unze", "founce0"),
                            ("Stunde", "hour0"), ("Stunden", "hour1"),
                            ("Minute", "minute0"), ("Minuten", "minute1"),
                            ("Sekunde", "second0"), ("Sekunden", "second1"),
                            ]):
    systemSounds.append((s, filename(f, PROMPT_SYSTEM_BASE + 114 + i)))
for s, f, a in [("Inaktivitätsalarm", "inactiv", 486),
                ("Senderspannung schwach", "lowbatt", 485),
                ("Gaskanal nicht Null, bitte prüfen", "thralert", 481),
                ("Schalter fehlpositioniert, bitte prüfen", "swalert", 482),
                ("EEPROM fehlerhaft", "eebad", NO_ALTERNATE),
                ("EEPROM formatiert", "eeformat", NO_ALTERNATE),
                ("Fehler", "error", NO_ALTERNATE),
                ("Trim zentriert", "midtrim", 495),
                ("Poti zentriert", "midpot", 496),
                ("Obere Trimmungsgrenze erreicht", "maxtrim", NO_ALTERNATE),
                ("Untere Trimmungsgrenze erreicht", "mintrim", NO_ALTERNATE),
                ("20 Sekunden", "timer20", 500),
                ("30 Sekunden", "timer30", 501),
                ("Senderstrom zu hoch!", "highmah", NO_ALTERNATE),
                ("Sendertemperatur zu hoch!", "hightemp", NO_ALTERNATE),
                ("A1 schwach!", "a1_org", NO_ALTERNATE),
                ("A1 kritisch!", "a1_red", NO_ALTERNATE),
                ("A2 schwach!", "a2_org", NO_ALTERNATE),
                ("A2 kritisch!", "a2_red", NO_ALTERNATE),
                ("A3 schwach!", "a3_org", NO_ALTERNATE),
                ("A3 kritisch!", "a3_red", NO_ALTERNATE),
                ("A4 schwach!", "a4_org", NO_ALTERNATE),
                ("A4 kritisch!", "a4_red", NO_ALTERNATE),
                ("Funksignal schwach!", "rssi_org", NO_ALTERNATE),
                ("Funksignal kritisch!", "rssi_red", NO_ALTERNATE),
                ("Problem mit der Sender Antenne", "swr_red", NO_ALTERNATE),
                ("Telemetrie verloren", "telemko", NO_ALTERNATE),
                ("Telemetrie wiederhergestellt", "telemok", NO_ALTERNATE),
                ("Schülersignal verloren", "trainko", NO_ALTERNATE),
                ("Schülersignal wiederhergestellt", "trainok", NO_ALTERNATE),
                ("servo overload", "servoko", NO_ALTERNATE),
                ("power overload", "rxko", NO_ALTERNATE),
                ("Empfänger noch verbunden", "modelpwr", NO_ALTERNATE),
                ("timer eins abgelaufen", "timovr1", NO_ALTERNATE),
                ("timer zwei abgelaufen", "timovr2", NO_ALTERNATE),
                ("timer drei abgelaufen", "timovr3", NO_ALTERNATE),
                ("Sensor veloren", "sensorko", NO_ALTERNATE),
                ("Willkommen bei open te ix", "hello", 480),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, s in enumerate(["Uhr", "Uhr", "Sender", "Empfang", "A1", "A2", "Hoehe", "Motor",
                       "Treibstoff", "Temperatur", "Temperatur", "Geschwindigkeit", "Entfernung", "Höhe", "Lipo-Zelle",
                       "Zellen gesamt", "Spannung", "Strom", "Verbrauch", "Power", "Beschleunigung X", "Beschleunigung Y", "Beschleunigung Z",
                       "Richtung", "Variometer", "Minimum", "Maximum"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 134 + i)))
for i, (s, f) in enumerate([("Fahrwerk eingezogen", "gearup"),
                            ("Fahrwerk ausgefahren", "geardn"),
                            ("Klappen eingefahren", "flapup"),
                            ("Klappen ausgefahren", "flapdn"),
                            ("Landung", "lnding"),
                            ("Trainer-Modus ein", "trnon"),
                            ("Trainer-Modus aus", "trnoff"),
                            ("Motor aus", "engoff"),
                            ("Motor an", "engon"),
                            ("zu hoch", "tohigh"),
                            ("zu niedrig", "tolow"),
                            ("Batterie schwach", "lowbatt"),
                            ("Butterfly ein", "crowon"),
                            ("Butterfly aus", "crowof"),
                            ("Geschwindigkeits-Modus ist aktiviert", "fm-spd"),
                            ("Thermik-Modus ist aktiviert", "fm-thm"),
                            ("Normal-Modus ist aktiviert", "fm-nrm"),
                            ("Flugmodus 1", "fm-1"),
                            ("Flugmodus 2", "fm-2"),
                            ("Flugmodus 3", "fm-3"),
                            ("Flugmodus 4", "fm-4"),
                            ("Flugmodus 5", "fm-5"),
                            ("Flugmodus 6", "fm-6"),
                            ("Flugmodus 7", "fm-7"),
                            ("Flugmodus 8", "fm-8"),
                            ("Flugmodus 9", "fm-9"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
