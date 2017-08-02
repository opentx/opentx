# -*- coding: utf-8 -*-

# Spanish language sounds configuration

from tts_common import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


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
for i, (s, f) in enumerate([("Voltio","volt0"),
                            ("ampério", "amp0"),
                            ("miliamperios", "mamp0"),
                            ("knots", "knot0"),
                            ("metros por segundo", "mps0"),
                            ("pie por segundo", "fps0"),
                            ("kilómetro por hora", "kph0"),
                            ("millas por hora", "mph0"),
                            ("metros", "meter0"),
                            ("pie", "foot0"),
                            ("grados", "celsius0"),
                            ("fahrenheit", "fahr0"),
                            ("por ciento", "percent0"),
                            ("miliamperios por hora", "mamph0"),
                            ("vatio", "watt0"),
                            ("millivatio", "mwatt0"),
                            ("D B", "db0"),
                            ("R P M", "rpm0"),
                            ("g", "g0"),
                            ("grados", "degree0"),
                            ("radianes ", "rad0"),
                            ("mililitro", "ml0"),
                            ("onzas", "founce0"),
                            ("horas", "hour0"),
                            ("minutos", "minute0"),
                            ("segundos", "second0"),
                            ]):
    systemSounds.append((s, filename(f, PROMPT_SYSTEM_BASE + 122 + i)))
for s, f, a in [("me tienes abandonada", "inactiv", 486),
                ("batería del transmisor baja", "lowbatt", 485),
                ("El acelerador está activado, por favor, corrijalo", "thralert", 481),
                ("Los interruptores no están en la posición correcta, por favor corrijalo", "swalert", 482),
                ("eeprom corrompida", "eebad", NO_ALTERNATE),
                ("Formateo de eeprom en curso", "eeformat", NO_ALTERNATE),
                ("error", "error", NO_ALTERNATE),
                ("trim centrado", "midtrim", 495),
                ("potenciómetro centrado", "midpot", 496),
                ("trim al máximo", "maxtrim", NO_ALTERNATE),
                ("trim al mínima", "mintrim", NO_ALTERNATE),
                ("20 segundos", "timer20", 500),
                ("30 segundos", "timer30", 501),
                ("assorbimiento elevado", "highmah", NO_ALTERNATE),
                ("temperatura elevada", "hightemp", NO_ALTERNATE),
                ("Alarma A1", "a1_org", NO_ALTERNATE),
                ("A1 crítica", "a1_red", NO_ALTERNATE),
                ("Alarma A2", "a2_org", NO_ALTERNATE),
                ("A2 crítica", "a2_red", NO_ALTERNATE),
                ("recepcion de señal baja", "rssi_org", NO_ALTERNATE),
                ("recepción de señal crítica", "rssi_red", NO_ALTERNATE),
                ("Problemas con la antena del transmisor", "swr_red", NO_ALTERNATE),
                ("Sin telemetría", "telemko", NO_ALTERNATE),
                ("Telemetría disponible", "telemok", NO_ALTERNATE),
                ("servo overload", "servoko", NO_ALTERNATE),
                ("power overload", "rxko", NO_ALTERNATE),
                ("receiver still connected", "modelpwr", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))
for i, s in enumerate(["cronómetro", "cronómetro", "transmisión", "recepción", "A1", "A2", "altitud", "motor",
                       "combustible", "temperatura", "temperatura", "velocidad", "distancia", "altitude", "célula lipo",
                       "Total lipo", "voltaje", "corriente", "consumo", "potencia", "aceleración X", "aceleración Y", "aceleración Z",
                       "dirección", "variómetro", "minimo", "máximo"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 141 + i)))
for i, (s, f) in enumerate([("tren arriba.", "gearup"),
                            ("tren abajo.", "geardn"),
                            ("flaps arriba", "flapup"),
                            ("flaps abajo", "flapdn"),
                            ("aterrizaje", "attero"),
                            ("modo de entrenamiento activado", "trnon"),
                            ("modo de entrenamiento deshabilitado", "trnoff"),
                            ("motor apagado", "engoff"),
                            ("muy alto", "tohigh"),
                            ("muy bajo", "tolow"),
                            ("bateria baja", "lowbat"),
                            ("crou, activo", "crowon"),
                            ("crou, desligado", "crowof"),
                            ("modo de velocidad", "spdmod"),
                            ("modo de térmica", "thmmod"),
                            ("modo de vuelo normal", "nrmmod"),
                            ("fase de vuelo 1", "fltmd1"),
                            ("fase de vuelo 2", "fltmd2"),
                            ("fase de vuelo 3", "fltmd3"),
                            ("fase de vuelo 4", "fltmd4"),
                            ("fase de vuelo 5", "fltmd5"),
                            ("fase de vuelo 6", "fltmd6"),
                            ("fase de vuelo 7", "fltmd7"),
                            ("fase de vuelo 8", "fltmd8"),
                            ("fase de vuelo 9", "fltmd9"),
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
