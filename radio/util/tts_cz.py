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
for i, (s, f) in enumerate([(u"volt", u"volt0"), (u"volty", u"volt1"), (u"voltů", u"volt2"),(u"voltu", u"volt3"),
                            (u"ampér", "amp0"), (u"ampéry", u"amp1"), (u"ampérů", u"amp2"), (u"ampéru", u"amp3"),
                            (u"miliampér", "mamp0"), (u"miliampéry", u"mamp1"), (u"miliampérů", u"mamp2"), (u"miliampéru", u"mamp3"),
                            (u"uzel", "knot0"), (u"uzly", u"knot1"), (u"uzlů", u"knot2"), (u"uzlu", u"knot3"),
                            (u"metr za sekundu", "mps0"), (u"metry za sekundu", u"mps1"), (u"metrů za sekundu", u"mps2"), (u"metru za sekundu", u"mps3"),
                            (u"stopa za sekundu", "fps0"), (u"stopy za sekundu", u"fps1"), (u"stop za sekundu", u"fps2"), (u"stopy za sekundu", u"fps3"),
                            (u"kilometr za hodinu", "kph0"), (u"kilometry za hodinu", u"kph1"), (u"kilometrů za hodinu", u"kph2"), (u"kilometru za hodinu", u"kph3"),
                            (u"míle za hodinu", "mph0"), (u"míle za hodinu", u"mph1"), (u"mil za hodinu", u"mph2"), (u"míle za hodinu", u"mph3"),
                            (u"metr", "meter0"), (u"metry", u"meter1"), (u"metrů", u"meter2"), (u"metru", u"meter3"),
                            (u"stopa", "foot0"), (u"stopy", u"foot1"), (u"stop", u"foot2"),(u"stopy", u"foot3"),
                            (u"stupeň celsia", "celsius0"), (u"stupně celsia", u"celsius1"), (u"stupňů celsia", u"celsius2"), (u"stupně celsia", u"celsius3"),
                            (u"stupeň fahrenheita", "fahr0"), (u"stupně fahrenheita", u"fahr1"), (u"stupňů fahrenheita", u"fahr2"), (u"stupně fahrenheita", u"fahr3"), 
                            (u"procento", "percent0"), (u"procenta", u"percent1"), (u"procent", u"percent2"), (u"procenta", u"percent3"),
                            (u"miliampérhodina", "mamph0"), (u"miliampérhodiny", u"mamph1"), (u"miliampérhodin", u"mamph2"), (u"miliampérhodiny", u"mamph3"),
                            (u"vat", "watt0"), (u"vaty", u"watt1"), (u"vatů", u"watt2"), (u"vatu", u"watt3"),
                            (u"milivat", "mwatt0"), (u"milivaty", u"mwatt1"), (u"milivatů", u"mwatt2"), (u"milivatu", u"mwatt3"),
                            (u"decibel", "db0"), (u"decibely", u"db1"), (u"decibelů", u"db2"), (u"decibelu", u"db3"),
                            (u"otáčka za minutu", "rpm0"), (u"otáčky za minutu", u"rpm1"), (u"otáček za minutu", u"rpm2"), (u"otáčky za minutu", u"rpm3"),
                            (u"gé", "g0"), (u"gé", "g1"), (u"gé", "g2"), (u"gé", "g3"),
                            (u"stupeň", "degree0"), (u"stupně", u"degree1"), (u"stupňů", u"degree2"), (u"stupně", u"degree3"),
                            (u"radián", "rad0"), (u"radiány", "rad1"), (u"radiánů", "rad2"), (u"radiánu", "rad3"),            
                            (u"mililitr", "ml0"), (u"mililitry", u"ml1"), (u"mililitrů", u"ml2"), (u"mililitru", u"ml3"),
                            (u"dutá unce", "founce0"), (u"duté unce", u"founce1"), (u"dutých uncí", u"founce2"), (u"duté unce", u"founce3"),
                            (u"hodina", "hour0"), (u"hodiny", u"hour1"), (u"hodin", u"hour2"), (u"hodiny", u"hour3"),
                            (u"minuta", "minute0"), (u"minuty", u"minute1"), (u"minut", u"minute2"),  (u"minuty", u"minute3"),
                            (u"sekunda", "second0"), (u"sekundy", u"second1"), (u"sekund", u"second2"), (u"sekundy", u"second3"),
                            ]):
    systemSounds.append((s, filename(f, PROMPT_SYSTEM_BASE + 118 + i)))
for s, f, a in [(u"střed trimu", "midtrim", 495),
                (u"maximum trimu", "maxtrim", NO_ALTERNATE),
                (u"minimum trimu", "mintrim", NO_ALTERNATE),
                (u"stopky 1 doběhly", "timovr1", NO_ALTERNATE),
                (u"stopky 2 doběhly", "timovr2", NO_ALTERNATE),
                (u"stopky 3 doběhly", "timovr3", NO_ALTERNATE),
                (u"baterie rádia je vybitá", "lowbatt", 485),
                (u"zapoměl jsi na mě!", "inactiv", 486),
                (u"plyn není na nule", "thralert", 481),
                (u"přepínače nejsou ve výchozí poloze", "swalert", 482),
                (u"neplatná eeprom", "eebad", 483),
                (u"Oupn najn iks", "hello", 480),
                (u"Tak zase příště!", "bye", 480),
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
                (u"senzor ztracen", "sensorko", NO_ALTERNATE),
                (u"servo přetíženo", "servoko", NO_ALTERNATE),
                (u"r f přebuzeno", "rxko", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, (s, f) in enumerate([(u"podvozek je zasunut", "podvo0"),
                            (u"podvozek je vysunut", "podvo1"),
                            (u"klapky jsou zapnuty", "klapk1"),
                            (u"klapky jsou vypnuty", "klapk0"),
                            (u"trenér je zapnut", "trene1"),
                            (u"trenér je vypnutý", "trene0"),
                            (u"motor je vypnut", "motor0"),
                            (u"motor je aktivován", "motor1"),
                            (u"jsi příliš vysoko", "vysoko"),
                            (u"jsi příliš nízko", "nizko"),
                            (u"baterie modelu je vybitá", "bat0"),
                            (u"dochází palivo", "palivo"),
                            (u"nízký signál!", "nsigna"),
                            (u"kritický signál!", "ksigna"),
                            (u"rychlý režim aktivován", "rrychl"),
                            (u"termický režim aktivován", "rtermi"),
                            (u"normální režim aktivován", "rnorm"),
                            (u"režim start aktivován", "rstart"),
                            (u"režim přistání aktivován", "prista"),
                            (u"vario je zapnuto", "vario0"),
                            (u"vario je vypnuto", "vario1"),
                            (u"start povolen", "start1"),
                            (u"letový režim jedna", "fm-1"),
                            (u"letový režim dva", "fm-2"),
                            (u"letový režim tři", "fm-3"),
                            (u"letový režim čtyři", "fm-4"),
                            (u"letový režim pět", "fm-5"),
                            (u"letový režim šest", "fm-6"),
                            (u"letový režim sedm", "fm-7"),
                            (u"letový režim osm", "fm-8"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
