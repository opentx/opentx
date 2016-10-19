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
for i, (s, f) in enumerate([(u"Wolt","volt0"),
                            (u"Ampär", "amp0"),
                            (u"MilliAmpär", "mamp0"),
                            (u"Knoten", "knot0"),
                            (u"Meter pro sekunde", "mps0"),
                            (u"Fuss pro sekunde", "fps0"),
                            (u"Kilometer pro stunde", "kph0"),
                            (u"Meilen pro Stunde", "mph0"),
                            (u"Meter", "meter0"),
                            (u"Fuesse", "foot0"),
                            (u"Grad Celsius", "celsius0"),
                            (u"Grad Fahrenheit", "fahr0"),
                            (u"Prozent", "percent0"),
                            (u"Milliampär pro stunde", "mamps0"),
                            (u"Watt", "watt0"),
                            (u"Milli Watt", "mwatt0"),
                            (u"D B", "db0"),
                            (u"Drehzahl", "rpm0"),
                            (u"g", "g0"),
                            (u"Grad", "degree0"),
                            (u"radians", "rad0"),
                            (u"Milliliter", "ml0"),
                            (u"Unze", "founce0"),
                            (u"Stunde", "hour0"), (u"Stunden", "hour1"),
                            (u"Minute", "minute0"), (u"Minuten", "minute1"),
                            (u"Sekunde", "second0"), (u"Sekunden", "second1"),
                            ]):
    systemSounds.append((s, filename(f, PROMPT_SYSTEM_BASE + 114 + i)))
for s, f, a in [(u"Inaktivitätsalarm", "inactiv", 486),
                (u"Senderspannung schwach", "lowbatt", 485),
                (u"Gaskanal nicht Null, bitte prüfen", "thralert", 481),
                (u"Schalter fehlpositioniert, bitte prüfen", "swalert", 482),
                (u"EEPROM fehlerhaft", "eebad", NO_ALTERNATE),
                (u"EEPROM formatiert", "eeformat", NO_ALTERNATE),
                (u"Fehler", "error", NO_ALTERNATE),
                (u"Trim zentriert", "midtrim", 495),
                (u"Poti zentriert", "midpot", 496),
                (u"Obere Trimmungsgrenze erreicht", "maxtrim", NO_ALTERNATE),
                (u"Untere Trimmungsgrenze erreicht", "mintrim", NO_ALTERNATE),
                (u"20 Sekunden", "timer20", 500),
                (u"30 Sekunden", "timer30", 501),
                (u"Senderstrom zu hoch!", "highmah", NO_ALTERNATE),
                (u"Sendertemperatur zu hoch!", "hightemp", NO_ALTERNATE),
                (u"A1 schwach!", "a1_org", NO_ALTERNATE),
                (u"A1 kritisch!", "a1_red", NO_ALTERNATE),
                (u"A2 schwach!", "a2_org", NO_ALTERNATE),
                (u"A2 kritisch!", "a2_red", NO_ALTERNATE),
                (u"A3 schwach!", "a3_org", NO_ALTERNATE),
                (u"A3 kritisch!", "a3_red", NO_ALTERNATE),
                (u"A4 schwach!", "a4_org", NO_ALTERNATE),
                (u"A4 kritisch!", "a4_red", NO_ALTERNATE),
                (u"Funksignal schwach!", "rssi_org", NO_ALTERNATE),
                (u"Funksignal kritisch!", "rssi_red", NO_ALTERNATE),
                (u"Problem mit der Sender Antenne", "swr_red", NO_ALTERNATE),
                (u"Telemetrie verloren", "telemko", NO_ALTERNATE),
                (u"Telemetrie wiederhergestellt", "telemok", NO_ALTERNATE),
                (u"Schülersignal verloren", "trainko", NO_ALTERNATE),
                (u"Schülersignal wiederhergestellt", "trainok", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, s in enumerate(["Uhr", "Uhr", "Sender", "Empfang", "A1", "A2", "Hoehe", "Motor",
                       "Treibstoff", "Temperatur", "Temperatur", "Geschwindigkeit", "Entfernung", "Höhe", "Lipo-Zelle",
                       "Zellen gesamt", "Spannung", "Strom", "Verbrauch", "Power", "Beschleunigung X", "Beschleunigung Y", "Beschleunigung Z",
                       "Richtung", "Variometer", "Minimum", "Maximum"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 134 + i)))
for i, (s, f) in enumerate([(u"Fahrwerk eingezogen", "gearup"),
                            (u"Fahrwerk ausgefahren", "geardn"),
                            (u"Klappen eingefahren", "flapup"),
                            (u"Klappen ausgefahren", "flapdn"),
                            (u"Landung", "lnding"),
                            (u"Trainer-Modus ein", "trnon"),
                            (u"Trainer-Modus aus", "trnoff"),
                            (u"Motor aus", "engoff"),
                            (u"Motor an", "engon"),
                            (u"zu hoch", "tohigh"),
                            (u"zu niedrig", "tolow"),
                            (u"Batterie schwach", "lowbatt"),
                            (u"Butterfly ein", "crowon"),
                            (u"Butterfly aus", "crowof"),
                            (u"Geschwindigkeits-Modus ist aktiviert", "fm-spd"),
                            (u"Thermik-Modus ist aktiviert", "fm-thm"),
                            (u"Normal-Modus ist aktiviert", "fm-nrm"),
                            (u"Flugmodus 1", "fm-1"),
                            (u"Flugmodus 2", "fm-2"),
                            (u"Flugmodus 3", "fm-3"),
                            (u"Flugmodus 4", "fm-4"),
                            (u"Flugmodus 5", "fm-5"),
                            (u"Flugmodus 6", "fm-6"),
                            (u"Flugmodus 7", "fm-7"),
                            (u"Flugmodus 8", "fm-8"),
                            (u"Flugmodus 9", "fm-9"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
