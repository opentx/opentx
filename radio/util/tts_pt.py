# -*- coding: utf-8 -*-

# Portuguese language sounds configuration

from tts_common import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


systemSounds = []
sounds = []

for i in range(101):
    systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE + i)))
systemSounds.append(("cento", filename(PROMPT_SYSTEM_BASE + 101)))
systemSounds.append(("duzentos", filename(PROMPT_SYSTEM_BASE + 102)))
systemSounds.append(("trezentos", filename(PROMPT_SYSTEM_BASE + 103)))
systemSounds.append(("quatrocentos", filename(PROMPT_SYSTEM_BASE + 104)))
systemSounds.append(("quinhentos", filename(PROMPT_SYSTEM_BASE + 105)))
systemSounds.append(("seiscentos", filename(PROMPT_SYSTEM_BASE + 106)))
systemSounds.append(("setecentos", filename(PROMPT_SYSTEM_BASE + 107)))
systemSounds.append(("oitocentos", filename(PROMPT_SYSTEM_BASE + 108)))
systemSounds.append(("novecentos", filename(PROMPT_SYSTEM_BASE + 109)))
systemSounds.append(("mil", filename(PROMPT_SYSTEM_BASE + 110)))
for i, s in enumerate(["virgula", "uma", "duas", "e", "menos", "hora", "horas", "minuto", "minutos", "segundo", "segundos"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 111 + i)))
for i, (s, f) in enumerate([("Volt", "volt0"),
                            ("ampére", "amp0"),
                            ("miliamperes", "mamp0"),
                            ("knot", "knot0"),
                            ("metros por segundo", "mps0"),
                            ("foot per second", "fps0"),
                            ("quilômetros por hora", "kph0"),
                            ("mile per hour", "mph0"),
                            ("metros", "meter0"),
                            ("foot", "foot0"),
                            ("degree celsius", "celsius0"),
                            ("degree fahrenheit", "fahr0"),
                            ("percent", "percent0"),
                            ("miliamperes por hora", "mamph0"),
                            ("watt", "watt0"),
                            ("milli-watt", "mwatt0"),
                            ("db", "db0"),
                            ("r p m", "rpm0"),
                            ("g", "g0"),
                            ("degree", "degree0"),
                            ("radian", "rad0"),
                            ("milliliter", "ml0"),
                            ("onças", "founce0"),
                            ("horas", "hour0"),
                            ("minutos", "minute0"),
                            ("segundos", "second0"),
                       ]):
    systemSounds.append((s, filename(f, PROMPT_SYSTEM_BASE + 122 + i)))
for s, f, a in [("atenção, o rádio foi esquecido ligado, por favor desligue-o", "inactiv", 486),
                ("bateria do rádio fraca", "lowbatt", 485),
                ("atenção,acelerador não está no mínimo", "thralert", 481),
                ("atenção, certifique-se que os interruptores estão na posição certa", "swalert", 482),
                ("eeprom corrompida", "eebad", NO_ALTERNATE),
                ("formatação da eeprom em curso", "eeformat", NO_ALTERNATE),
                ("erro", "error", NO_ALTERNATE),
                ("trim centrado", "midtrim", 495),
                ("potenciómetro centrado", "midpot", 496),
                ("trim no máximo", "maxtrim", NO_ALTERNATE),
                ("trim no mínimo", "mintrim", NO_ALTERNATE),
                ("20 segundos", "timer20", 500),
                ("30 segundos", "timer30", 501),
                ("assorbimento elevato", "highmah", NO_ALTERNATE),
                ("temperatura elevata", "hightemp", NO_ALTERNATE),
                ("Alarme A1", "a1_org", NO_ALTERNATE),
                ("A1 critica", "a1_red", NO_ALTERNATE),
                ("Alarme A2", "a2_org", NO_ALTERNATE),
                ("A2 critica", "a2_red", NO_ALTERNATE),
                ("recepção de sinal muito baixa", "rssi_org", NO_ALTERNATE),
                ("recepção de sinal crítica", "rssi_red", NO_ALTERNATE),
                ("Problema com a antena do transmissor", "swr_red", NO_ALTERNATE),
                ("servo overload", "servoko", NO_ALTERNATE),
                ("power overload", "rxko", NO_ALTERNATE),
                ("receiver still connected", "modelpwr", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, s in enumerate(["cronómetro", "cronómetro", "transmissão", "recepção", "A1", "A2", "altitude", "motor",
                       "combustível", "temperatura", "temperatura", "velocidade", "distância", "altitude", "célula lipo",
                       "Total lipo", "tensão", "corrente", "consumo", "potência", "aceleração X", "aceleração Y", "aceleração Z",
                       "Direcção", "variómetro", "mínimo", "máximo"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 141 + i)))
for i, (s, f) in enumerate([("trem, em cima.", "gearup"),
                            ("trem, em baixo.", "geardn"),
                            ("flaps recolhidos", "flapup"),
                            ("flaps estendidos", "flapdn"),
                            ("aterragem", "attero"),
                            ("modo de treino ligado", "trnon"),
                            ("modo de treino desligado", "trnoff"),
                            ("motor desligado", "engoff"),
                            ("muito alto", "tohigh"),
                            ("muito baixo", "tolow"),
                            ("bateria fraca", "lowbat"),
                            ("crou, activo", "crowon"),
                            ("crou, desligado", "crowof"),
                            ("modo de velocidade", "spdmod"),
                            ("modo de térmica", "thmmod"),
                            ("modo de voo normal", "nrmmod"),
                            ("fase de voo 1", "fltmd1"),
                            ("fase de voo 2", "fltmd2"),
                            ("fase de voo 3", "fltmd3"),
                            ("fase de voo 4", "fltmd4"),
                            ("fase de voo 5", "fltmd5"),
                            ("fase de vôo 6", "fltmd6"),
                            ("fase de voo 7", "fltmd7"),
                            ("fase de voo 8", "fltmd8"),
                            ("fase de voo 9", "fltmd9"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
