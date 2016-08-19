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

for s, f, a in [(u"volt", u"volt", NO_ALTERNATE), (u"volty", u"volt1", NO_ALTERNATE), (u"voltů", u"volt2", NO_ALTERNATE),(u"voltu", u"volt3", NO_ALTERNATE),
                (u"ampér", "amp", NO_ALTERNATE), (u"ampéry", u"amp1", NO_ALTERNATE), (u"ampérů", u"amp2", NO_ALTERNATE), (u"ampéru", u"amp3", NO_ALTERNATE),
                (u"miliampér", "mamp", NO_ALTERNATE), (u"miliampéry", u"mamp1", NO_ALTERNATE), (u"miliampérů", u"mamp2", NO_ALTERNATE), (u"miliampéru", u"mamp3", NO_ALTERNATE),
                (u"uzel", "knot", NO_ALTERNATE), (u"uzly", u"knot1", NO_ALTERNATE), (u"uzlů", u"knot2", NO_ALTERNATE), (u"uzlu", u"knot3", NO_ALTERNATE),
                (u"metr za sekundu", "mps", NO_ALTERNATE), (u"metry za sekundu", u"mps1", NO_ALTERNATE), (u"metrů za sekundu", u"mps2", NO_ALTERNATE), (u"metru za sekundu", u"mps3", NO_ALTERNATE),
                (u"stopa za sekundu", "fps", NO_ALTERNATE), (u"stopy za sekundu", u"fps1", NO_ALTERNATE), (u"stop za sekundu", u"fps2", NO_ALTERNATE), (u"stopy za sekundu", u"fps3", NO_ALTERNATE),
                (u"kilometr za hodinu", "kph", NO_ALTERNATE), (u"kilometry za hodinu", u"kph1", NO_ALTERNATE), (u"kilometrů za hodinu", u"kph2", NO_ALTERNATE), (u"kilometru za hodinu", u"kph3", NO_ALTERNATE),
                (u"míle za hodinu", "mph", NO_ALTERNATE), (u"míle za hodinu", u"mph1", NO_ALTERNATE), (u"mil za hodinu", u"mph2", NO_ALTERNATE), (u"míle za hodinu", u"mph3", NO_ALTERNATE),
                (u"metr", "meter", NO_ALTERNATE), (u"metry", u"meter1", NO_ALTERNATE), (u"metrů", u"meter2", NO_ALTERNATE), (u"metru", u"meter3", NO_ALTERNATE),
                (u"stopa", "foot", NO_ALTERNATE), (u"stopy", u"foot1", NO_ALTERNATE), (u"stop", u"foot2", NO_ALTERNATE),(u"stopy", u"foot3", NO_ALTERNATE),
                (u"stupeň celsia", "celsius", NO_ALTERNATE), (u"stupně celsia", u"celsius1", NO_ALTERNATE), (u"stupňů celsia", u"celsius2", NO_ALTERNATE), (u"stupně celsia", u"celsius3", NO_ALTERNATE),
                (u"stupeň fahrenheita", "fahr", NO_ALTERNATE), (u"stupně fahrenheita", u"fahr1", NO_ALTERNATE), (u"stupňů fahrenheita", u"fahr2", NO_ALTERNATE), (u"stupně fahrenheita", u"fahr3", NO_ALTERNATE), 
                (u"procento", "percent", NO_ALTERNATE), (u"procenta", u"percent1", NO_ALTERNATE), (u"procent", u"percent2", NO_ALTERNATE), (u"procenta", u"percent3", NO_ALTERNATE),
                (u"miliampérhodina", "mamph", NO_ALTERNATE), (u"miliampérhodiny", u"mamph1", NO_ALTERNATE), (u"miliampérhodin", u"mamph2", NO_ALTERNATE), (u"miliampérhodiny", u"mamph3", NO_ALTERNATE),
                (u"vat", "watt", NO_ALTERNATE), (u"vaty", u"watt1", NO_ALTERNATE), (u"vatů", u"watt2", NO_ALTERNATE), (u"vatu", u"watt3", NO_ALTERNATE),
                (u"milivat", "mwatt", NO_ALTERNATE), (u"milivaty", u"mwatt1", NO_ALTERNATE), (u"milivatů", u"mwatt2", NO_ALTERNATE), (u"milivatu", u"mwatt3", NO_ALTERNATE),
                (u"decibel", "db", NO_ALTERNATE), (u"decibely", u"db1", NO_ALTERNATE), (u"decibelů", u"db2", NO_ALTERNATE), (u"decibelu", u"db3", NO_ALTERNATE),
                (u"otáčka za minutu", "rpm", NO_ALTERNATE), (u"otáčky za minutu", u"rpm1", NO_ALTERNATE), (u"otáček za minutu", u"rpm2", NO_ALTERNATE), (u"otáčky za minutu", u"rpm3", NO_ALTERNATE),
                (u"gé", "g", NO_ALTERNATE),
                (u"stupeň", "degree", NO_ALTERNATE), (u"stupně", u"degree1", NO_ALTERNATE), (u"stupňů", u"degree2", NO_ALTERNATE), (u"stupně", u"degree3", NO_ALTERNATE),
                (u"radian", "rad", NO_ALTERNATE),
                (u"mililitr", "ml", NO_ALTERNATE), (u"mililitry", u"ml1", NO_ALTERNATE), (u"mililitrů", u"ml2", NO_ALTERNATE), (u"mililitru", u"ml3", NO_ALTERNATE),
                (u"dutá unce", "founce", NO_ALTERNATE), (u"duté unce", u"founce1", NO_ALTERNATE),
                (u"hodina", "hour", NO_ALTERNATE), (u"hodiny", u"hour1", NO_ALTERNATE), (u"hodin", u"hour2", NO_ALTERNATE), (u"hodiny", u"hour2", NO_ALTERNATE),
                (u"minuta", "minute", NO_ALTERNATE), (u"minuty", u"minute1", NO_ALTERNATE), (u"minut", u"minute1", NO_ALTERNATE),  (u"minuty", u"minute1", NO_ALTERNATE),
                (u"sekunda", "second", NO_ALTERNATE), (u"sekundy", u"second1", NO_ALTERNATE), (u"sekund", u"second2", NO_ALTERNATE), (u"sekundy", u"second3", NO_ALTERNATE),
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
