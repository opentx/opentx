# -*- coding: utf-8 -*-

# Spanish language sounds configuration

from tts import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


systemSounds = []
sounds = []

for i in range(101):
    systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE + i)))
systemSounds.append(("ciento", filename(PROMPT_SYSTEM_BASE + 101)))
systemSounds.append(("doscientos", filename(PROMPT_SYSTEM_BASE + 102)))
systemSounds.append(("trescientos", filename(PROMPT_SYSTEM_BASE + 103)))
systemSounds.append(("cuatrocientos", filename(PROMPT_SYSTEM_BASE + 104)))
systemSounds.append(("quinientos", filename(PROMPT_SYSTEM_BASE + 105)))
systemSounds.append(("sescientos", filename(PROMPT_SYSTEM_BASE + 106)))
systemSounds.append(("setecientos", filename(PROMPT_SYSTEM_BASE + 107)))
systemSounds.append(("ochocientos", filename(PROMPT_SYSTEM_BASE + 108)))
systemSounds.append(("nuevecientos", filename(PROMPT_SYSTEM_BASE + 109)))
systemSounds.append(("mil", filename(PROMPT_SYSTEM_BASE + 110)))
for i, s in enumerate(["coma", "un", "una", "y", "meno", "hora", "horas", "minuto", "minutos", "segundo", "segundos"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 111 + i)))
for i, s in enumerate([u"Voltio", u"ampério", u"metros por segundo", u"", u"kilómetro por hora", u"metros", u"grados", u"por ciento", u"miliamperios", u"miliamperios por hora", u"vatio", u"db", u"pés", u"nós", u"horas", u"minutos", u"segundos", u"RPM", u"g", u"mililitro", u"onzas"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 122 + i)))
for s, f, a in [(u"me tienes abandonada", "inactiv", 486),
                (u"batería del transmisor baja", "lowbatt", 485),
                (u"El acelerador está activado, por favor, corrijalo", "thralert", 481),
                (u"Los interruptores no están en la posición correcta, por favor corrijalo", "swalert", 482),
                (u"eeprom corrompida", "eebad", NO_ALTERNATE),
                (u"Formateo de eeprom en curso", "eeformat", NO_ALTERNATE),
                (u"error", "error", NO_ALTERNATE),
                (u"trim centrado", "midtrim", 495),
                (u"potenciómetro centrado", "midpot", 496),
                (u"trim al máximo", "endtrim", NO_ALTERNATE),
                (u"20 segundos", "timer20", 500),
                (u"30 segundos", "timer30", 501),
                (u"assorbimiento elevado", "highmah", NO_ALTERNATE),
                (u"temperatura elevada", "hightemp", NO_ALTERNATE),
                (u"Alarma A1", "a1_org", NO_ALTERNATE),
                (u"A1 crítica", "a1_red", NO_ALTERNATE),
                (u"Alarma A2", "a2_org", NO_ALTERNATE),
                (u"A2 crítica", "a2_red", NO_ALTERNATE),
                (u"recepcion de señal baja", "rssi_org", NO_ALTERNATE),
                (u"recepción de señal crítica", "rssi_red", NO_ALTERNATE),
                (u"Problemas con la antena del transmisor", "swr_red", NO_ALTERNATE),
                (u"Sin telemetría", "telemko", NO_ALTERNATE),
                (u"Telemetría disponible", "telemok", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, s in enumerate([u"cronómetro", u"cronómetro", u"transmisión", u"recepción", u"A1", u"A2", u"altitud", u"motor",
                       u"combustible", u"temperatura", u"temperatura", u"velocidad", u"distancia", u"altitude", u"célula lipo",
                       u"Total lipo", u"voltaje", u"corriente", u"consumo", u"potencia", u"aceleración X", u"aceleración Y", u"aceleración Z",
                       u"dirección", u"variómetro", u"minimo", u"máximo"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 141 + i)))
for i, (s, f) in enumerate([(u"tren arriba.", "gearup"),
                            (u"tren abajo.", "geardn"),
                            (u"flaps arriba", "flapup"),
                            (u"flaps abajo", "flapdn"),
                            (u"aterrizaje", "attero"),
                            (u"modo de entrenamiento activado", "trnon"),
                            (u"modo de entrenamiento deshabilitado", "trnoff"),
                            (u"motor apagado", "engoff"),
                            (u"muy alto", "tohigh"),
                            (u"muy bajo", "tolow"),
                            (u"bateria baja", "lowbat"),
                            (u"crou, activo", "crowon"),
                            (u"crou, desligado", "crowof"),
                            (u"modo de velocidad", "spdmod"),
                            (u"modo de térmica", "thmmod"),
                            (u"modo de vuelo normal", "nrmmod"),
                            (u"fase de vuelo 1", "fltmd1"),
                            (u"fase de vuelo 2", "fltmd2"),
                            (u"fase de vuelo 3", "fltmd3"),
                            (u"fase de vuelo 4", "fltmd4"),
                            (u"fase de vuelo 5", "fltmd5"),
                            (u"fase de vuelo 6", "fltmd6"),
                            (u"fase de vuelo 7", "fltmd7"),
                            (u"fase de vuelo 8", "fltmd8"),
                            (u"fase de vuelo 9", "fltmd9"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
