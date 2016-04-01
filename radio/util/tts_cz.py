# -*- coding: utf-8 -*-

# Czech language sounds configuration

from tts import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


systemSounds = []
sounds = []

for i, s in enumerate(["nula", "jedna", "dva"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + i)))
for i in range(97):
    systemSounds.append((str(3 + i), filename(PROMPT_SYSTEM_BASE + 3 + i)))
for i, s in enumerate(["sto", u"dvěsta", u"třista", u"čtyřista", u"pětset", u"šestset", "sedmset", "osmset",
                       u"devětset", u"tisíc"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 100 + i)))
for i, s in enumerate([u"tisíce", "jeden", "jedno", u"dvě", u"celá", u"celé", u"celých", u"mínus"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 110 + i)))

for i, s in enumerate(["volt", "volty", u"voltů", "voltu",
                       u"ampér", u"ampéry", u"ampérů", u"ampéru",
                       u"miliampér", u"miliampéry", u"miliampérů", u"miliampéru",
                       u"uzel", u"uzly", u"uzlů", u"uzlu",
                       "metr za sekundu", "metry za sekundu", u"metrů za sekundu", "metru za sekundu",
                       "stopa za sekundu", "stopy za sekundu", "stop za sekundu", "stopy za sekundu",
                       "kilometr za hodinu", "kilometry za hodinu", u"kilometrů za hodinu", "kilometru za hodinu",
                       u"míle za hodinu", u"míle za hodinu", u"mil za hodinu", u"míle za hodinu",
                       "metr", "metry", u"metrů", "metru",
                       "stopa", "stopy", "stop", "stopy",
                       u"stupeň celsia", u"stupně celsia", u"stupňů celsia", u"stupně celsia",
                       u"stupeň fahrenheita", u"stupně fahrenheita", u"stupňů fahrenheita", u"stupně fahrenheita",
                       "procento", "procenta", "procent", "procenta",
                       u"miliampérhodina", u"miliampérhodiny", u"miliampérhodin", u"miliampérhodiny",
                       "vat", "vaty", u"vatů", "vatu",
                       "decibel", "decibely", u"decibelů", "decibelu",
                       u"otáčka za minutu", u"otáčky za minutu", u"otáček za minutu", u"otáčky za minutu",
                       u"gé", u"gé", u"gé", u"gé"
                       u"stupeň", u"stupně", u"stupňů", u"stupně",
                       u"mililitr", u"mililitry", u"mililitrů", "mililitru",
                       u"dutá unce", u"duté unce", u"dutých uncí", u"duté unce",
                       "hodina", "hodiny", "hodin", "hodiny",
                       "minuta", "minuty", "minut", "minuty",
                       "sekunda", "sekundy", "sekund", "sekundy",
                       ]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 118 + i)))

for s, f, a in [(u"střed trimu", "midtrim", 495),
                (u"maximum trimu", "endtrim", NO_ALTERNATE),
                (u"baterie rádia je vybitá", "lowbatt", 485),
                (u"zapoměl jsi na mě!", "inactiv", 486),
                (u"plyn není na nule", "thralert", 481),
                (u"přepínače nejsou ve výchozí poloze", "swalert", 482),
                (u"neplatná eeprom", "eebad", 483),
                (u"Oupn najn iks", "tada", 480),
                (u"dvacet sekund", "timer20", 500),
                (u"třicet sekund", "timer30", 501),
                (u"A1,nízká", "a1_org", NO_ALTERNATE),
                (u"A1,kritická", "a1_red", NO_ALTERNATE),
                (u"A2,nízká", "a2_org", NO_ALTERNATE),
                (u"A2,kritická", "a2_red", NO_ALTERNATE),
                (u"A3,nízká", "a3_org", NO_ALTERNATE),
                (u"A3,kritická", "a3_red", NO_ALTERNATE),
                (u"A4,nízká", "a4_org", NO_ALTERNATE),
                (u"A4,kritická", "a4_red", NO_ALTERNATE),
                (u"nízký RF signál", "rssi_org", NO_ALTERNATE),
                (u"kritický RF signál", "rssi_red", NO_ALTERNATE),
                (u"problém s anténou rádia", "swr_red", NO_ALTERNATE),
                (u"telemetrie ztracena", "telemko", NO_ALTERNATE),
                (u"telemetrie je funkční", "telemok", NO_ALTERNATE),
                (u"trenér port odpojen", "trainko", NO_ALTERNATE),
                (u"trenér port připojen", "trainok", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, (s, f) in enumerate([(u"podvozek je zasunut", "podvoz0"),
                            (u"podvozek je vysunut", "podvoz1"),
                            (u"klapky jsou zapnuty", "klapky1"),
                            (u"klapky jsou vypnuty", "klapky0"),
                            (u"trenér je zapnut", "trener1"),
                            (u"trenér je vypnutý", "trener0"),
                            (u"motor je vypnut", "motor0"),
                            (u"motor je aktivován", "motor1"),
                            (u"jsi příliš vysoko", "vysoko"),
                            (u"jsi příliš nízko", "nizko"),
                            (u"baterie modelu je vybitá", "batvybit"),
                            (u"dochází palivo", "palivo"),
                            (u"nízký signál!", "nsignal"),
                            (u"kritický signál!", "ksignal"),
                            (u"rychlý režim aktivován", "r_rychly"),
                            (u"termický režim aktivován", "r_termik"),
                            (u"normální režim aktivován", "r_norm"),
                            (u"režim start aktivován", "r_start"),
                            (u"režim přistání aktivován", "r_prista"),
                            (u"vario je zapnuto", "vario0"),
                            (u"vario je vypnuto", "vario1"),
                            (u"start povolen", "startok"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
