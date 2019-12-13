# -*- coding: utf-8 -*-

# Portuguese language sounds configuration

from tts_common import filename


systemSounds = []
sounds = []

for i in range(101):
    systemSounds.append((str(i), filename(i)))
systemSounds.append(("cento", filename(101)))
systemSounds.append(("duzentos", filename(102)))
systemSounds.append(("trezentos", filename(103)))
systemSounds.append(("quatrocentos", filename(104)))
systemSounds.append(("quinhentos", filename(105)))
systemSounds.append(("seiscentos", filename(106)))
systemSounds.append(("setecentos", filename(107)))
systemSounds.append(("oitocentos", filename(108)))
systemSounds.append(("novecentos", filename(109)))
systemSounds.append(("mil", filename(110)))
for i, s in enumerate(["virgula", "uma", "duas", "e", "menos", "hora", "horas", "minuto", "minutos", "segundo", "segundos"]):
    systemSounds.append((s, filename(111 + i)))
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
                            ("milliliter per minute", "mlpm0"), ("milliliters per minute", "mlpm1"),
                            ("horas", "hour0"),
                            ("minutos", "minute0"),
                            ("segundos", "second0"),
                       ]):
    systemSounds.append((s, filename(f)))
for s, f in [("atenção, o rádio foi esquecido ligado, por favor desligue-o", "inactiv"),
             ("bateria do rádio fraca", "lowbatt"),
             ("atenção,acelerador não está no mínimo", "thralert"),
             ("atenção, certifique-se que os interruptores estão na posição certa", "swalert"),
             ("eeprom corrompida", "eebad"),
             ("formatação da eeprom em curso", "eeformat"),
             ("erro", "error"),
             ("trim centrado", "midtrim"),
             ("potenciómetro centrado", "midpot"),
             ("trim no máximo", "maxtrim"),
             ("trim no mínimo", "mintrim"),
             ("20 segundos", "timer20"),
             ("30 segundos", "timer30"),
             ("assorbimento elevato", "highmah"),
             ("temperatura elevata", "hightemp"),
             ("Alarme A1", "a1_org"),
             ("A1 critica", "a1_red"),
             ("Alarme A2", "a2_org"),
             ("A2 critica", "a2_red"),
             ("recepção de sinal muito baixa", "rssi_org"),
             ("recepção de sinal crítica", "rssi_red"),
             ("Problema com a antena do transmissor", "swr_red"),
             ("servo overload", "servoko"),
             ("power overload", "rxko"),
             ("receiver still connected", "modelpwr"),
             ]:
    systemSounds.append((s, filename(f)))
for i, s in enumerate(["cronómetro", "cronómetro", "transmissão", "recepção", "A1", "A2", "altitude", "motor",
                       "combustível", "temperatura", "temperatura", "velocidade", "distância", "altitude", "célula lipo",
                       "Total lipo", "tensão", "corrente", "consumo", "potência", "aceleração X", "aceleração Y", "aceleração Z",
                       "Direcção", "variómetro", "mínimo", "máximo"]):
    systemSounds.append((s, filename(141 + i)))
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
    sounds.append((s, filename(f)))
