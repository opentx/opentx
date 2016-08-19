# -*- coding: utf-8 -*-

# Czech language sounds configuration

from tts_common import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


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

for s, f, a in [(u"volt", u"volt", NO_ALTERNATE), (u"volty", u"volty", NO_ALTERNATE), (u"voltů", u"", NO_ALTERNATE), (u"voltů", u"", NO_ALTERNATE),
                (u"ampér", "", NO_ALTERNATE), (u"ampéry", u"", NO_ALTERNATE), (u"ampérů", u"", NO_ALTERNATE), (u"ampéru", u"", NO_ALTERNATE),
                (u"miliampér", "", NO_ALTERNATE), (u"miliampéry", u"", NO_ALTERNATE), (u"miliampérů", u"", NO_ALTERNATE), (u"miliampéru", u"", NO_ALTERNATE),
                (u"uzel", "", NO_ALTERNATE), (u"uzly", u"", NO_ALTERNATE), (u"uzlů", u"", NO_ALTERNATE), (u"uzlu", u"", NO_ALTERNATE),
                (u"metr za sekundu", "", NO_ALTERNATE), (u"metry za sekundu", u"", NO_ALTERNATE), (u"metrů za sekundu", u"", NO_ALTERNATE), (u"metru za sekundu", u"", NO_ALTERNATE),
                (u"stopa za sekundu", "", NO_ALTERNATE), (u"stopy za sekundu", u"", NO_ALTERNATE), (u"stop za sekundu", u"", NO_ALTERNATE), (u"stopy za sekundu", u"", NO_ALTERNATE),
                (u"kilometr za hodinu", "", NO_ALTERNATE), (u"kilometry za hodinu", u"", NO_ALTERNATE), (u"kilometrů za hodinu", u"", NO_ALTERNATE), (u"kilometru za hodinu", u"", NO_ALTERNATE),
                (u"míle za hodinu", "", NO_ALTERNATE), (u"míle za hodinu", u"", NO_ALTERNATE), (u"mil za hodinu", u"", NO_ALTERNATE), (u"míle za hodinu", u"", NO_ALTERNATE),
                (u"metr", "", NO_ALTERNATE), (u"metry", u"", NO_ALTERNATE), (u"metrů", u"", NO_ALTERNATE), (u"metru", u"", NO_ALTERNATE),
                (u"stopa", "", NO_ALTERNATE), (u"stopy", u"", NO_ALTERNATE), (u"stop", u"", NO_ALTERNATE), (u"stopy", u"", NO_ALTERNATE),
                (u"stupeň celsia", "", NO_ALTERNATE), (u"stupně celsia", u"", NO_ALTERNATE), (u"stupňů celsia", u"", NO_ALTERNATE), (u"stupně celsia", u"", NO_ALTERNATE),
                (u"stupeň fahrenheita", "", NO_ALTERNATE), (u"stupně fahrenheita", u"", NO_ALTERNATE), (u"stupňů fahrenheita", u"", NO_ALTERNATE), (u"stupně fahrenheita", u"", NO_ALTERNATE),
                (u"procento", "", NO_ALTERNATE), (u"procenta", u"", NO_ALTERNATE), (u"procent", u"", NO_ALTERNATE), (u"procenta", u"", NO_ALTERNATE),
                (u"miliampérhodina", "", NO_ALTERNATE), (u"miliampérhodiny", u"", NO_ALTERNATE), (u"miliampérhodin", u"", NO_ALTERNATE), (u"miliampérhodiny", u"", NO_ALTERNATE),
                (u"vat", "", NO_ALTERNATE), (u"vaty", u"", NO_ALTERNATE), (u"vatů", u"", NO_ALTERNATE), (u"vatu", u"", NO_ALTERNATE),
                (u"milivat", "", NO_ALTERNATE), (u"milivaty", u"", NO_ALTERNATE), (u"milivatů", u"", NO_ALTERNATE), (u"milivatu", u"", NO_ALTERNATE),
                (u"decibel", "", NO_ALTERNATE), (u"decibely", u"", NO_ALTERNATE), (u"decibelů", u"", NO_ALTERNATE), (u"decibelu", u"", NO_ALTERNATE),
                (u"otáčka za minutu", "", NO_ALTERNATE), (u"otáčky za minutu", u"", NO_ALTERNATE), (u"otáček za minutu", u"", NO_ALTERNATE), (u"otáčky za minutu", u"", NO_ALTERNATE),
                (u"gé", "g", NO_ALTERNATE),
                (u"stupeň", "", NO_ALTERNATE), (u"stupně", u"", NO_ALTERNATE), (u"stupňů", u"", NO_ALTERNATE), (u"stupně", u"", NO_ALTERNATE),
                (u"", "empty", NO_ALTERNATE),
                (u"mililitr", "", NO_ALTERNATE), (u"mililitry", u"", NO_ALTERNATE), (u"mililitrů", u"", NO_ALTERNATE), (u"mililitru", u"", NO_ALTERNATE),
                (u"dutá unce", "", NO_ALTERNATE), (u"duté unce", u"", NO_ALTERNATE), (u"duté unce", u"", NO_ALTERNATE), (u"duté unce", u"", NO_ALTERNATE),
                (u"hodina", "", NO_ALTERNATE), (u"hodiny", u"", NO_ALTERNATE), (u"hodin", u"", NO_ALTERNATE),
                (u"minuta", "", NO_ALTERNATE), (u"minuty", u"", NO_ALTERNATE), (u"minut", u"", NO_ALTERNATE), 
                (u"sekunda", "", NO_ALTERNATE), (u"sekundy", u"", NO_ALTERNATE), (u"sekund", u"", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))

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
