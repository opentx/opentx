# -*- coding: utf-8 -*-

# German language sounds configuration

from tts_common import filename


systemSounds = []
sounds = []

for i in range(100):
    systemSounds.append((str(i), filename(i)))
systemSounds.append(("ein", filename(100)))
systemSounds.append(("eine", filename(101)))
systemSounds.append(("hundert", filename(102)))
systemSounds.append(("tausend", filename(103)))
for i, s in enumerate(["komma", "und", "minus", "uhr", "minute", "minuten", "sekunde", "sekunden", "stunde", "stunden"]):
    systemSounds.append((s, filename(104 + i)))
for i, (s, f) in enumerate([("Wolt", "volt0"),
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
                            ("Milliampär stunde", "mamph0"),
                            ("Watt", "watt0"),
                            ("Milli Watt", "mwatt0"),
                            ("D B", "db0"),
                            ("Drehzahl", "rpm0"),
                            ("g", "g0"),
                            ("Grad", "degree0"),
                            ("radians", "rad0"),
                            ("Milliliter", "ml0"),
                            ("Unze", "founce0"),
                            ("Milliliter pro minute", "mlpm0"), ("Milliliter pro minuten", "mlpm1"),
                            ("Stunde", "hour0"), ("Stunden", "hour1"),
                            ("Minute", "minute0"), ("Minuten", "minute1"),
                            ("Sekunde", "second0"), ("Sekunden", "second1"),
                            ]):
    systemSounds.append((s, filename(f)))
for s, f in [("Inaktivitätsalarm", "inactiv"),
             ("Senderspannung schwach", "lowbatt"),
             ("Gaskanal nicht Null, bitte prüfen", "thralert"),
             ("Schalter fehlpositioniert, bitte prüfen", "swalert"),
             ("EEPROM fehlerhaft", "eebad"),
             ("EEPROM formatiert", "eeformat"),
             ("Fehler", "error"),
             ("Trim zentriert", "midtrim"),
             ("Poti zentriert", "midpot"),
             ("Obere Trimmungsgrenze erreicht", "maxtrim"),
             ("Untere Trimmungsgrenze erreicht", "mintrim"),
             ("20 Sekunden", "timer20"),
             ("30 Sekunden", "timer30"),
             ("Senderstrom zu hoch!", "highmah"),
             ("Sendertemperatur zu hoch!", "hightemp"),
             ("A1 schwach!", "a1_org"),
             ("A1 kritisch!", "a1_red"),
             ("A2 schwach!", "a2_org"),
             ("A2 kritisch!", "a2_red"),
             ("A3 schwach!", "a3_org"),
             ("A3 kritisch!", "a3_red"),
             ("A4 schwach!", "a4_org"),
             ("A4 kritisch!", "a4_red"),
             ("Funksignal schwach!", "rssi_org"),
             ("Funksignal kritisch!", "rssi_red"),
             ("Problem mit der Sender Antenne", "swr_red"),
             ("Telemetrie verloren", "telemko"),
             ("Telemetrie wiederhergestellt", "telemok"),
             ("Schülersignal verloren", "trainko"),
             ("Schülersignal wiederhergestellt", "trainok"),
             ("servo overload", "servoko"),
             ("power overload", "rxko"),
             ("Empfänger noch verbunden", "modelpwr"),
             ("timer eins abgelaufen", "timovr1"),
             ("timer zwei abgelaufen", "timovr2"),
             ("timer drei abgelaufen", "timovr3"),
             ("Sensor verloren", "sensorko"),
             ("Willkommen bei open t x", "hello"),
             ]:
    systemSounds.append((s, filename(f)))
for i, s in enumerate(["Uhr", "Uhr", "Sender", "Empfang", "A1", "A2", "Hoehe", "Motor",
                       "Treibstoff", "Temperatur", "Temperatur", "Geschwindigkeit", "Entfernung", "Höhe", "Lipo-Zelle",
                       "Zellen gesamt", "Spannung", "Strom", "Verbrauch", "Power", "Beschleunigung X", "Beschleunigung Y", "Beschleunigung Z",
                       "Richtung", "Variometer", "Minimum", "Maximum"]):
    systemSounds.append((s, filename(134 + i)))
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
    sounds.append((s, filename(f)))
