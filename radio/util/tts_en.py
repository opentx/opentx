# -*- coding: utf-8 -*-

# English language sounds configuration

from tts import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


systemSounds = []
sounds = []

for i in range(100):
    systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE + i)))
for i in range(9):
    systemSounds.append((str(100 * (i + 1)), filename(PROMPT_SYSTEM_BASE + 100 + i)))
for i, s in enumerate(["thousand", "and", "minus", "point"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 109 + i)))
for i, s in enumerate(["volt", "volts",
                       "amp", "amps",
                       "milliamp", "milliamps",
                       "knot", "knots",
                       "meter per second", "meters per second",
                       "foot per second", "feet per second",
                       "kilometer per hour", "kilometers per hour",
                       "mile per hour", "miles per hour",
                       "meter", "meters",
                       "foot", "feet",
                       "degree celsius", "degrees celsius",
                       "degree fahrenheit", "degrees fahrenheit",
                       "percent", "percent",
                       "milliamp-hour", "milliamp-hours",
                       "watt", "watts",
                       "db", "db",
                       "r p m", "r p m",
                       "g", "g",
                       "degree", "degrees",
                       "milliliter", "milliliters",
                       "fluid ounce", "fluid ounces",
                       "hour", "hours",
                       "minute", "minutes",
                       "second", "seconds",
                       ]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 115 + i)))
for i, s in enumerate(["point zero", "point one", "point two", "point three",
                       "point four", "point five", "point six",
                       "point seven", "point eight", "point nine"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 165 + i)))
for s, f, a in [(u"trim center", "midtrim", 495),
                (u"maximum trim reached", "endtrim", NO_ALTERNATE),
                (u"transmitter battery low", "lowbatt", 485),
                (u"inactivity alarm", "inactiv", 486),
                (u"throttle warning", "thralert", 481),
                (u"switch warning", "swalert", 482),
                (u"bad eeprom", "eebad", 483),
                (u"Welcome to open tea ex!", "tada", 480),
                (u"twenty. seconds", "timer20", 500),
                (u"thirty. seconds", "timer30", 501),
                (u"A1,low", "a1_org", NO_ALTERNATE),
                (u"A1,critical", "a1_red", NO_ALTERNATE),
                (u"A2,low", "a2_org", NO_ALTERNATE),
                (u"A2,critical", "a2_red", NO_ALTERNATE),
                (u"A3,low", "a3_org", NO_ALTERNATE),
                (u"A3,critical", "a3_red", NO_ALTERNATE),
                (u"A4,low", "a4_org", NO_ALTERNATE),
                (u"A4,critical", "a4_red", NO_ALTERNATE),
                (u"RF signal, low", "rssi_org", NO_ALTERNATE),
                (u"RF signal, critical", "rssi_red", NO_ALTERNATE),
                (u"radio antenna defective", "swr_red", NO_ALTERNATE),
                (u"telemetry lost", "telemko", NO_ALTERNATE),
                (u"telemetry recovered", "telemok", NO_ALTERNATE),
                (u"trainer signal lost", "trainko", NO_ALTERNATE),
                (u"trainer signal recovered", "trainok", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, (s, f) in enumerate([(u"gear!, up!", "gearup"),
                            (u"gear!, down!", "geardn"),
                            (u"flaps!, up!", "flapup"),
                            (u"flaps!, down!", "flapdn"),
                            (u"spoiler!, up!", "splrup"),
                            (u"spoiler!, down!", "splrdn"),
                            (u"trainer!, on!", "trnon"),
                            (u"trainer!, off!", "trnoff"),
                            (u"engine!, off!", "engoff"),
                            (u"too. high!", "tohigh"),
                            (u"too. low!", "tolow"),
                            (u"low. battery!", "lowbat"),
                            (u"crow!, on!", "crowon"),
                            (u"crow!, off!", "crowof"),
                            (u"rf. signal!, low!", "siglow"),
                            (u"rf. signal!, critical!", "sigcrt"),
                            (u"high. speed. mode!, active", "spdmod"),
                            (u"thermal. mode!, on", "thmmod"),
                            (u"normal. mode!, on", "nrmmod"),
                            (u"landing. mode!, on", "lnding"),
                            (u"acro. mode!, on", "acro"),
                            (u"flight. mode!, one", "fm-1"),
                            (u"flight. mode!, two", "fm-2"),
                            (u"flight. mode!, three", "fm-3"),
                            (u"flight. mode!, four", "fm-4"),
                            (u"flight. mode!, five", "fm-5"),
                            (u"flight. mode!, six", "fm-6"),
                            (u"flight. mode!, seven", "fm-7"),
                            (u"flight. mode!, eight", "fm-8"),
                            (u"vario!, on", "vrion"),
                            (u"vario!, off", "vrioff"),
                            (u"flight mode!, power", "fm-pwr"),
                            (u"flight mode!, land", "fm-lnd"),
                            (u"flight mode!, float", "fm-flt"),
                            (u"flight mode!, speed", "fm-spd"),
                            (u"flight mode!, fast", "fm-fst"),
                            (u"flight mode!, normal", "fm-nrm"),
                            (u"flight mode!, cruise", "fm-crs"),
                            (u"flight mode!, acro", "fm-acr"),
                            (u"flight mode!, race", "fm-rce"),
                            (u"flight mode!, launch", "fm-lch"),
                            (u"flight mode!, ping", "fm-png"),
                            (u"flight mode!, thermal", "fm-thm"),
                            (u"flight mode!, thermal left", "fm-thml"),
                            (u"flight mode!, thermal right", "fm-thmr"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
