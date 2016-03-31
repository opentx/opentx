# -*- coding: utf-8 -*-

# Portuguese language sounds configuration

from tts import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


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
for i, s in enumerate([u"Volt", u"ampére", u"metros por segundo", u"", u"quilômetros por hora", u"metros", u"graus", u"cento", u"miliamperes", u"miliamperes por hora", u"watt", u"db", u"pés", u"nós", u"horas", u"minutos", u"segundos", u"RPM", u"g", u"mililitri", u"onças"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 122 + i)))
for s, f, a in [(u"atenção, o rádio foi esquecido ligado, por favor desligue-o", "inactiv", 486),
                (u"bateria do rádio fraca", "lowbatt", 485),
                (u"atenção,acelerador não está no mínimo", "thralert", 481),
                (u"atenção, certifique-se que os interruptores estão na posição certa", "swalert", 482),
                (u"eeprom corrompida", "eebad", NO_ALTERNATE),
                (u"formatação da eeprom em curso", "eeformat", NO_ALTERNATE),
                (u"erro", "error", NO_ALTERNATE),
                (u"trim centrado", "midtrim", 495),
                (u"potenciómetro centrado", "midpot", 496),
                (u"trim no máximo", "endtrim", NO_ALTERNATE),
                (u"20 segundos", "timer20", 500),
                (u"30 segundos", "timer30", 501),
                (u"assorbimento elevato", "highmah", NO_ALTERNATE),
                (u"temperatura elevata", "hightemp", NO_ALTERNATE),
                (u"Alarme A1", "a1_org", NO_ALTERNATE),
                (u"A1 critica", "a1_red", NO_ALTERNATE),
                (u"Alarme A2", "a2_org", NO_ALTERNATE),
                (u"A2 critica", "a2_red", NO_ALTERNATE),
                (u"recepção de sinal muito baixa", "rssi_org", NO_ALTERNATE),
                (u"recepção de sinal crítica", "rssi_red", NO_ALTERNATE),
                (u"Problema com a antena do transmissor", "swr_red", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, s in enumerate([u"cronómetro", u"cronómetro", u"transmissão", u"recepção", u"A1", u"A2", u"altitude", u"motor",
                       u"combustível", u"temperatura", u"temperatura", u"velocidade", u"distância", u"altitude", u"célula lipo",
                       u"Total lipo", u"tensão", u"corrente", u"consumo", u"potência", u"aceleração X", u"aceleração Y", u"aceleração Z",
                       u"Direcção", u"variómetro", u"mínimo", u"máximo"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 141 + i)))
for i, (s, f) in enumerate([(u"trem, em cima.", "gearup"),
                            (u"trem, em baixo.", "geardn"),
                            (u"flaps recolhidos", "flapup"),
                            (u"flaps estendidos", "flapdn"),
                            (u"aterragem", "attero"),
                            (u"modo de treino ligado", "trnon"),
                            (u"modo de treino desligado", "trnoff"),
                            (u"motor desligado", "engoff"),
                            (u"muito alto", "tohigh"),
                            (u"muito baixo", "tolow"),
                            (u"bateria fraca", "lowbat"),
                            (u"crou, activo", "crowon"),
                            (u"crou, desligado", "crowof"),
                            (u"modo de velocidade", "spdmod"),
                            (u"modo de térmica", "thmmod"),
                            (u"modo de voo normal", "nrmmod"),
                            (u"fase de voo 1", "fltmd1"),
                            (u"fase de voo 2", "fltmd2"),
                            (u"fase de voo 3", "fltmd3"),
                            (u"fase de voo 4", "fltmd4"),
                            (u"fase de voo 5", "fltmd5"),
                            (u"fase de vôo 6", "fltmd6"),
                            (u"fase de voo 7", "fltmd7"),
                            (u"fase de voo 8", "fltmd8"),
                            (u"fase de voo 9", "fltmd9"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
