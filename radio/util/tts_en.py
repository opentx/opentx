# -*- coding: utf-8 -*-

# English language sounds configuration

from tts_common import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


systemSounds = []
sounds = []

for i in range(100):
    systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE + i)))
for i in range(9):
    systemSounds.append((str(100 * (i + 1)), filename(PROMPT_SYSTEM_BASE + 100 + i)))
for i, s in enumerate(["thousand", "and", "minus", "point"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 109 + i)))
for i, (s, f) in enumerate([("volt", "volt0"), ("volts", "volt1"),
                            ("amp", "amp0"), ("amps", "amp1"),
                            ("milliamp", "mamp0"), ("milliamps", "mamp1"),
                            ("knot", "knot0"), ("knots", "knot1"),
                            ("meter per second", "mps0"), ("meters per second", "mps1"),
                            ("foot per second", "fps0"), ("feet per second", "fps1"),
                            ("kilometer per hour", "kph0"), ("kilometers per hour", "kph1"),
                            ("mile per hour", "mph0"), ("miles per hour", "mph1"),
                            ("meter", "meter0"), ("meters", "meter1"),
                            ("foot", "foot0"), ("feet", "foot1"),
                            ("degree celsius", "celsius0"), ("degrees celsius", "celsius1"),
                            ("degree fahrenheit", "fahr0"), ("degrees fahrenheit", "fahr1"),
                            ("percent", "percent0"), ("percent", "percent1"),
                            ("milliamp-hour", "mamph0"), ("milliamp-hours", "mamph1"),
                            ("watt", "watt0"), ("watts", "watt1"),
                            ("milli-watt", "mwatt0"), ("milli-watts", "mwatt1"),
                            ("D B", "db0"),("D B", "db1"),
                            ("r p m", "rpm0"),("r p m", "rpm1"),
                            ("g", "g0"),("g", "g1"),
                            ("degree", "degree0"), ("degrees", "degree1"),
                            ("radian", "rad0"), ("radians", "rad1"),
                            ("milliliter", "ml0"), ("milliliters", "ml1"),
                            ("fluid ounce", "founce0"), ("fluid ounces", "founce1"),
                            ("hour", "hour0"), ("hours", "hour1"),
                            ("minute", "minute0"), ("minutes", "minute1"),
                            ("second", "second0"), ("seconds", "second1"),
                            ]):
    systemSounds.append((s, filename(f, PROMPT_SYSTEM_BASE + 115 + i)))
for i, s in enumerate(["point zero", "point one", "point two", "point three",
                       "point four", "point five", "point six",
                       "point seven", "point eight", "point nine"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 167 + i)))
for s, f, a in [("trim center", "midtrim", 495),
                ("maximum trim reached", "maxtrim", NO_ALTERNATE),
                ("minimum trim reached", "mintrim", NO_ALTERNATE),
                ("timer 1 elapsed", "timovr1", NO_ALTERNATE),
                ("timer 2 elapsed", "timovr2", NO_ALTERNATE),
                ("timer 3 elapsed", "timovr3", NO_ALTERNATE),
                ("transmitter battery low", "lowbatt", 485),
                ("inactivity alarm", "inactiv", 486),
                ("throttle warning", "thralert", 481),
                ("switch warning", "swalert", 482),
                ("bad eeprom", "eebad", 483),
                ("Welcome to open tea ex!", "hello", 480),
                ("RF signal, low", "rssi_org", NO_ALTERNATE),
                ("RF signal, critical", "rssi_red", NO_ALTERNATE),
                ("radio antenna defective", "swr_red", NO_ALTERNATE),
                ("telemetry lost", "telemko", NO_ALTERNATE),
                ("telemetry recovered", "telemok", NO_ALTERNATE),
                ("trainer signal lost", "trainko", NO_ALTERNATE),
                ("trainer signal recovered", "trainok", NO_ALTERNATE),
                ("sensor lost", "sensorko", NO_ALTERNATE),
                ("servo overload", "servoko", NO_ALTERNATE),
                ("power overload", "rxko", NO_ALTERNATE),
                ("receiver still connected", "modelpwr", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, (s, f) in enumerate([("gear!, up!", "gearup"),
                            ("gear!, down!", "geardn"),
                            ("flaps!, up!", "flapup"),
                            ("flaps!, down!", "flapdn"),
                            ("spoiler!, up!", "splrup"),
                            ("spoiler!, down!", "splrdn"),
                            ("trainer!, on!", "trnon"),
                            ("trainer!, off!", "trnoff"),
                            ("engine!, off!", "engoff"),
                            ("too. high!", "tohigh"),
                            ("too. low!", "tolow"),
                            ("low. battery!", "lowbat"),
                            ("crow!, on!", "crowon"),
                            ("crow!, off!", "crowof"),
                            ("rf. signal!, low!", "siglow"),
                            ("rf. signal!, critical!", "sigcrt"),
                            ("high. speed. mode!, active", "spdmod"),
                            ("thermal. mode!, on", "thmmod"),
                            ("normal. mode!, on", "nrmmod"),
                            ("landing. mode!, on", "lnding"),
                            ("acro. mode!, on", "acro"),
                            ("flight mode one", "fm-1"),
                            ("flight mode two", "fm-2"),
                            ("flight mode three", "fm-3"),
                            ("flight mode four", "fm-4"),
                            ("flight mode five", "fm-5"),
                            ("flight mode six", "fm-6"),
                            ("flight mode seven", "fm-7"),
                            ("flight mode eight", "fm-8"),
                            ("vario!, on", "vrion"),
                            ("vario!, off", "vrioff"),
                            ("flight mode power", "fm-pwr"),
                            ("flight mode land", "fm-lnd"),
                            ("flight mode float", "fm-flt"),
                            ("flight mode speed", "fm-spd"),
                            ("flight mode fast", "fm-fst"),
                            ("flight mode normal", "fm-nrm"),
                            ("flight mode cruise", "fm-crs"),
                            ("flight mode acro", "fm-acr"),
                            ("flight mode race", "fm-rce"),
                            ("flight mode launch", "fm-lch"),
                            ("flight mode ping", "fm-png"),
                            ("flight mode thermal", "fm-thm"),
                            ("flight mode thermal left", "fm-thl"),
                            ("flight mode thermal right", "fm-thr"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
