# -*- coding: utf-8 -*-

# German language sounds configuration

from tts import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


systemSounds = []
sounds = []

for i in range(101):
    systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE + i)))
systemSounds.append(("tausend", filename(PROMPT_SYSTEM_BASE + 101)))
for i, s in enumerate(["comma", "und", "minus", "uhr", "minute", "minuten", "sekunde", "sekunden"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 102 + i)))
for i, s in enumerate(["", "Volt", "Ampere", "MilliAmpere", "Knoten",
                       "Meter pro sekunde", "Fuesse pro sekunde",
                       "kilometer pro stunde", "Meilen pro Stunde", "Meter",
                       "Fuesse", "Grad", "Grad Fahrenheit", "Prozent",
                       "Milliampere pro stunde", "Watt", "db", "R P M", "g",
                       "Milliliter", "Unze", "Uhr", "Minuten", "Secunden"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 110 + i)))
for s, f, a in [(u"Sender ist inaktiv,bitte Ueberpruefen Sie", "inactiv", 486),
                (u"Senderakku niedrig", "lowbatt", 485),
                (u"Gaskanal nicht Null, bitte pruefen", "thralert", 481),
                (u"Schalten fehlpositioniert, bitte pruefen", "swalert", 482),
                (u"Das EEPROM ist fehlerhaft", "eebad", NO_ALTERNATE),
                (u"Das EEPROM ist immer formatiert", "eeformat", NO_ALTERNATE),
                (u"fehler", "error", NO_ALTERNATE),
                (u"Trim zentriert", "midtrim", 495),
                (u"Poti zentriert", "midpot", 496),
                (u"Maximale trimmung erreicht", "endtrim", NO_ALTERNATE),
                (u"20 sekunden", "timer20", 500),
                (u"30 sekunden", "timer30", 501),
                (u"Senderstrom ist zu hoch!", "highmah", NO_ALTERNATE),
                (u"Sendertemperatur ist zu hoch!", "hightemp", NO_ALTERNATE),
                (u"A1 schlecht!", "a1_org", NO_ALTERNATE),
                (u"A1 kritisch!", "a1_red", NO_ALTERNATE),
                (u"A2 schlecht!", "a2_org", NO_ALTERNATE),
                (u"A2 kritisch!", "a2_red", NO_ALTERNATE),
                (u"A3 schlecht!", "a3_org", NO_ALTERNATE),
                (u"A3 kritisch!", "a3_red", NO_ALTERNATE),
                (u"A4 schlecht!", "a4_org", NO_ALTERNATE),
                (u"A4 kritisch!", "a4_red", NO_ALTERNATE),
                (u"Funksignal schlecht!", "rssi_org", NO_ALTERNATE),
                (u"Funksignal kritisch!", "rssi_red", NO_ALTERNATE),
                (u"Problem mit der sender Antenna", "swr_red", NO_ALTERNATE),
                (u"Telemetrie verloren", "telemko", NO_ALTERNATE),
                (u"Telemetrie wiederhergestellt", "telemok", NO_ALTERNATE),
                (u"Schülersignal verloren", "trainko", NO_ALTERNATE),
                (u"Schülersignal wiederhergestellt", "trainok", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, s in enumerate(["Timer", "Timer", "Sendung", "Empfang", "A1", "A2", "Hoehe", "Motor",
                       "Treibstoff", "Temperatur", "Temperatur", "Geschwindigkeit", "Ferne", "Hoehe", "Lipo-Zelle",
                       "Zellen gesamt", "Spannung", "Strom", "Verbrauch", "Power", "Beschleunigung X", "Beschleunigung Y", "Beschleunigung Z",
                       "Richtung", "Variometer", "Minimum", "Maximum"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 134 + i)))
for i, (s, f) in enumerate([(u"Fahrwerk eingezogen", "gearup"),
                            (u"Fahrwerk ausgefahren", "geardn"),
                            (u"Klappen eingefahren", "flapup"),
                            (u"Klappen ausgefahren", "flapdn"),
                            (u"Landung", "attero"),
                            (u"Trainer-Modus ein", "trnon"),
                            (u"Trainer-Modus aus", "trnoff"),
                            (u"Motor aus", "engoff"),
                            (u"zu hoch", "tohigh"),
                            (u"zu niedrig", "tolow"),
                            (u"Batterie schwach", "lowbat"),
                            (u"crow ein", "crowon"),
                            (u"crow aus", "crowof"),
                            (u"Schnell-Modus ist aktiviert", "spdmod"),
                            (u"Thermik-Modus ist aktiviert", "thmmod"),
                            (u"Normal-Modus ist aktiviert", "nrmmod"),
                            (u"Regime 1", "fltmd1"),
                            (u"Regime 2", "fltmd2"),
                            (u"Regime 3", "fltmd3"),
                            (u"Regime 4", "fltmd4"),
                            (u"Regime 5", "fltmd5"),
                            (u"Regime 6", "fltmd6"),
                            (u"Regime 7", "fltmd7"),
                            (u"Regime 8", "fltmd8"),
                            (u"Regime 9", "fltmd9"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
