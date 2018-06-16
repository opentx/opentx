# -*- coding: utf-8 -*-

# Russian language sounds configuration

from tts_common import filename, NO_ALTERNATE, PROMPT_SYSTEM_BASE, PROMPT_CUSTOM_BASE


systemSounds = []
sounds = []

# 0 - 100
for i in range(100):
    systemSounds.append((str(i), filename(PROMPT_SYSTEM_BASE + i)))
# 100, 200, ... 900
for i in range(9):
    systemSounds.append((str(100 * (i + 1)), filename(PROMPT_SYSTEM_BASE + 100 + i)))
for i, s in enumerate([u"тысяча", u"и", u"минус"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 109 + i)))

for i, s in enumerate([u"тысяча", u"тысячи", u"тысяч"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 200 + i)))

# decimal point
for i, s in enumerate([u"ровно", u"и одна десятая", u"и две десятых", u"и три десятых", u"и четыре десятых",
                       u"и пять десятых", u"и шесть десятых", u"и семь десятых", u"и восемь десятых", u"и девять десятых" ]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 165 + i)))

# last digit = 1, female
for i, s in enumerate([u"одна", u"одиннадцать", u"двадцать одна", u"тридцать одна", u"сорок одна",
                       u"пятьдесят одна", u"шестьдесят одна", u"семьдесят одна", u"восемьдесят одна", u"девяносто одна"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 180 + i)))

# last digit = 2, female
for i, s in enumerate([u"две", u"двенадцать", u"двадцать две", u"тридцать две", u"сорок две",
                       u"пятьдесят две", u"шестьдесят две", u"семьдесят две", u"восемьдесят две", u"девяносто две"]):
    systemSounds.append((s, filename(PROMPT_SYSTEM_BASE + 190 + i)))

for i, (s, f) in enumerate([(u"вольт", "volt0"), (u"вольт", "volt1"),
                            (u"вольта", "volt2"), (u"вольт", "volt5"),
                            (u"ампер", "amp0"), (u"ампер", "amp1"),
                            (u"ампера", "amp2"), (u"ампер", "amp5"),
                            (u"миллиампер", "mamp0"), (u"миллиампер", "mamp1"),
                            (u"миллиампера", "mamp2"), (u"миллиампер", "mamp5"),
                            (u"узлов", "knot0"), (u"узел", "knot1"),
                            (u"узла", "knot2"), (u"узлов", "knot5"),
                            (u"метров в секунду", "mps0"), (u"метр в секунду", "mps1"),
                            (u"метра в секунду", "mps2"), (u"метров в секунду", "mps5"),
                            (u"футов в секунду", "fps0"), (u"фут в секунду", "fps1"),
                            (u"фута в секунду", "fps2"), (u"футов в секунду", "fps5"),
                            (u"километров в час", "kph0"), (u"километр в час", "kph1"),
                            (u"километра в час", "kph2"), (u"километров в час", "kph5"),
                            (u"миль в час", "mph0"), (u"миля в час", "mph1"),
                            (u"мили в час", "mph2"), (u"миль в час", "mph5"),
                            (u"метров", "meter0"), (u"метр", "meter1"),
                            (u"метра", "meter2"), (u"метров", "meter5"),
                            (u"футов", "foot0"), (u"фут", "foot1"),
                            (u"фута", "foot2"), (u"футов", "foot5"),
                            (u"градусов", "celsius0"), (u"градус", "celsius1"),
                            (u"градуса", "celsius2"), (u"градусов", "celsius5"),
                            (u"градусов по фаренгейту", "fahr0"), (u"градус по фаренгейту", "fahr1"),
                            (u"градуса по фаренгейту", "fahr2"), (u"градусов по фаренгейту", "fahr5"),
                            (u"процентов", "percent0"), (u"процент", "percent1"),
                            (u"процента", "percent2"), (u"процентов", "percent5"),
                            (u"миллиампер в час", "mamph0"), (u"миллиампер в час", "mamph1"),
                            (u"миллиампера в час", "mamph2"), (u"миллиампер в час", "mamph5"),
                            (u"ватт", "watt0"), (u"ватт", "watt1"),
                            (u"ватта", "watt2"), (u"ватт", "watt5"),
                            (u"милливатт", "mwatt0"), (u"милливатт", "mwatt1"),
                            (u"милливатта", "mwatt2"), (u"милливатт", "mwatt5"),
                            (u"децибел", "db0"),(u"децибел", "db1"),
                            (u"децибела", "db2"),(u"децибел", "db5"),
                            (u"оборотов в минуту", "rpm0"),(u"оборот в минуту", "rpm1"),
                            (u"оборота в минуту", "rpm2"),(u"оборотов в минуту", "rpm5"),
                            (u"джи", "g0"),(u"джи", "g1"),
                            (u"джи", "g2"),(u"джи", "g5"),
                            (u"градусов", "degree0"), (u"градус", "degree1"),
                            (u"градуса", "degree2"), (u"градусов", "degree5"),
                            (u"радиан", "rad0"), (u"радиан", "rad1"),
                            (u"радиана", "rad2"), (u"радиан", "rad5"),
                            (u"миллилитров", "ml0"), (u"миллилитр", "ml1"),
                            (u"миллилитра", "ml2"), (u"миллилитров", "ml5"),
                            (u"унций", "founce0"), (u"унция", "founce1"),
                            (u"унции", "founce2"), (u"унций", "founce5"),
                            (u"часов", "hour0"), (u"час", "hour1"),
                            (u"часа", "hour2"), (u"часов", "hour5"),
                            (u"минут", "minute0"), (u"минута", "minute1"),
                            (u"минуты", "minute2"), (u"минут", "minute5"),
                            (u"секунд", "second0"), (u"секунда", "second1"),
                            (u"секунды", "second2"), (u"секунд", "second5"),
                            ]):
    systemSounds.append((s, filename(f, PROMPT_SYSTEM_BASE + 115 + i)))

for s, f, a in [(u"Триммер по центру", "midtrim", 495),
                (u"Триммер максимум", "maxtrim", NO_ALTERNATE),
                (u"Триммер минимум", "mintrim", NO_ALTERNATE),
                (u"Таймер 1. Время вышло", "timovr1", NO_ALTERNATE),
                (u"Таймер 2. Время вышло", "timovr2", NO_ALTERNATE),
                (u"Таймер 3. Время вышло", "timovr3", NO_ALTERNATE),
                (u"Аккумулятор садится!", "lowbatt", 485),
                (u"Внимание, пульт не выключен!", "inactiv", 486),
                (u"Проверь газ!", "thralert", 481),
                (u"Проверь, тумблеры!", "swalert", 482),
                (u"Ошибка памяти", "eebad", 483),
                (u"Опен тэ, икс приветствует вас!", "hello", 480),
                (u"Слабый сигнал!", "rssi_org", NO_ALTERNATE),
                (u"Очень слабый сигнал!", "rssi_red", NO_ALTERNATE),
                (u"Проблема с антэнной!", "swr_red", NO_ALTERNATE),
                (u"Телеметрия потеряна", "telemko", NO_ALTERNATE),
                (u"Телеметрия восстановлена", "telemok", NO_ALTERNATE),
                (u"Тренерский сигнал потерян", "trainko", NO_ALTERNATE),
                (u"Тренерский сигнал восстановлен", "trainok", NO_ALTERNATE),
                (u"Сенсор отключен", "sensorko", NO_ALTERNATE),
                (u"Серва перегружена", "servoko", NO_ALTERNATE),
                (u"Перегрузка", "rxko", NO_ALTERNATE),
                (u"Приемник всё ещё подключен!", "modelpwr", NO_ALTERNATE),
                ]:
    systemSounds.append((s, filename(f, a)))

for i, (s, f) in enumerate([
                            (u"Аккумулятор садится!", "lowbat"),
                            (u"Шасси, убраны'", "gearup"),
                            (u"Шасси, выпущены", "geardn"),
                            (u"Закрылки убраны'", "flapup"),
                            (u"Закрылки выпущены", "flapdn"),
                            (u"Спойлеры выпущены", "splrup"),
                            (u"Спойлеры - убраны'", "splrdn"),
                            (u"Тренер: подключен", "trnon"),
                            (u"Тренер: отключен", "trnoff"),
                            (u"Двигатели запущены!", "armed"),
                            (u"Двигатели выключены", "disarm"),
                            (u"Слишком высоко", "tohigh"),
                            (u"Слишком низко", "tolow"),
                            (u"Низкий уровень сигнала", "siglow"),
                            (u"Критически низкий уровень сигнала", "sigcrt"),
                            (u"Полетный режим один", "fm-1"),
                            (u"Полетный режим два", "fm-2"),
                            (u"Полетный режим три", "fm-3"),
                            (u"Полетный режим четыре", "fm-4"),
                            (u"Полетный режим пять", "fm-5"),
                            (u"Полетный режим шесть", "fm-6"),
                            (u"Полетный режим семь", "fm-7"),
                            (u"Полетный режим восемь", "fm-8"),
                            (u"Режим,акро", "fm-acr"),
                            (u"Режим круиза", "fm-crs"),
                            (u"Режим взлета", "fm-lch"),
                            (u"Режим посадки", "fm-lnd"),
                            (u"Нормальный режим", "fm-nrm"),
                            (u"Ручной режим", "fm-man"),
                            (u"Стабилизация", "fm-stb"),
                            (u"Удержание позиции", "fm-hld"),
                            (u"Возврат в точку старта", "fm-rth"),
                            (u"Горизонт", "fm-hor"),
                            (u"Стабилизация", "fm-ang"),
                            (u"аирмод включен", "airmon"),
                            (u"аирмод выключен", "airmof"),
                            (u"Высота", "alt"),
                            (u"Удержание высоты", "althld"),
                            (u"Критический разряд!", "batcrt"),
                            (u"Время полета", "flttm"),
                            (u"Аварийный режим выключен", "fsoff"),
                            (u"Аварийный режим включен", "fson"),
                            (u"ДжиПи Эс", "gps"),
                            (u"Высокие расходы", "hirate"),
                            (u"Низкие расходы", "lorate"),
                            (u"Спутники найдены!", "satfix"),
                            (u"Спутники потеряны!", "satlst"),
                            (u"Бортовые огни выключены", "ledoff"),
                            (u"Бортовые огни включены", "ledon")
                            ]):
    sounds.append((s, filename(f, PROMPT_CUSTOM_BASE + i)))
