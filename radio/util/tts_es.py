# -*- coding: utf-8 -*-

# Spanish language sounds configuration

from tts_common import filename


systemSounds = []
sounds = []

for i in range(101):
    systemSounds.append((str(i), filename(i)))
systemSounds.append(("ciento", filename(101)))
systemSounds.append(("doscientos", filename(102)))
systemSounds.append(("trescientos", filename(103)))
systemSounds.append(("cuatrocientos", filename(104)))
systemSounds.append(("quinientos", filename(105)))
systemSounds.append(("sescientos", filename(106)))
systemSounds.append(("setecientos", filename(107)))
systemSounds.append(("ochocientos", filename(108)))
systemSounds.append(("nuevecientos", filename(109)))
systemSounds.append(("mil", filename(110)))
for i, s in enumerate(["coma", "un", "una", "i", "meno", "hora", "horas", "minuto", "minutos", "segundo", "segundos"]):
    systemSounds.append((s, filename(111 + i)))
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
                            ("hora", "hour0"), ("horas", "hour1"),
                            ("milliliter per minute", "mlpm0"), ("milliliters per minute", "mlpm1"),
                            ("minuto", "minute0"), ("minutos", "minute1"),
                            ("segundo", "second0"), ("segundos", "second1"),
                            ]):
    systemSounds.append((s, filename(f)))
for s, f in [("me tienes abandonada", "inactiv"),
             ("batería del transmisor baja", "lowbatt"),
             ("El acelerador está activado, por favor, corrijalo", "thralert"),
             ("Los interruptores no están en la posición correcta, por favor corrijalo", "swalert"),
             ("eeprom corrompida", "eebad"),
             ("error", "error"),
             ("trim centrado", "midtrim"),
             ("trim al máximo", "maxtrim"),
             ("trim al mínimo", "mintrim"),
             ("consumo alto", "highmah"),
             ("temperatura alta", "hightemp"),
             ("Bienvenido a open t equis", "hello"),
             ("señal baja", "rssi_org"),
             ("señal crítica", "rssi_red"),
             ("Problemas con la antena del transmisor", "swr_red"),
             ("Sin telemetría", "telemko"),
             ("Telemetría disponible", "telemok"),
             ("sobrecarga de servo", "servoko"),
             ("sobrecarga de potencia", "rxko"),
             ("El receptor aún está encendido", "modelpwr"),
             ("cronómetro uno terminado", "timovr1"),
             ("cronómetro dos terminado", "timovr2"),
             ("cronómetro tres terminado", "timovr3"),
             ]:
    systemSounds.append((s, filename(f)))
for i, s in enumerate(["cronómetro", "cronómetro", "transmisión", "recepción", "A1", "A2", "altitud", "motor",
                       "combustible", "temperatura", "temperatura", "velocidad", "distancia", "altitude", "célula lipo",
                       "Total lipo", "voltaje", "corriente", "consumo", "potencia", "aceleración X", "aceleración Y", "aceleración Z",
                       "dirección", "variómetro", "minimo", "máximo"]):
    systemSounds.append((s, filename(141 + i)))
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
                            ("modo de vuelo 1", "fltmd1"),
                            ("modo de vuelo 2", "fltmd2"),
                            ("modo de vuelo 3", "fltmd3"),
                            ("modo de vuelo 4", "fltmd4"),
                            ("modo de vuelo 5", "fltmd5"),
                            ("modo de vuelo 6", "fltmd6"),
                            ("modo de vuelo 7", "fltmd7"),
                            ("modo de vuelo 8", "fltmd8"),
                            ("modo de vuelo 9", "fltmd9"),
                            ]):
    sounds.append((s, filename(f)))
