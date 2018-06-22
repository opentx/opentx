#!/usr/bin/env python

languages = (
    "en",
    "fr",
    "se",
    "it",
    "cz",
    "de",
    "pt",
    "es",
    "pl",
    "nl"
)

tts_languages = {
    "en",
    "fr",
    "it",
    "cz",
    "de",
    "pt"
}
tts_avr = {
    "tts%s" % language: ("TTS", language.upper(), None) for language in tts_languages
    }

options_9x_ext = {
    "nmea": ("TELEMETRY", "NMEA", None),
    "frsky": ("TELEMETRY", "FRSKY", None),
    "telemetrez": ("TELEMETRY", "TELEMETREZ", None),
    "jeti": ("TELEMETRY", "JETI", None),
    "ardupilot": ("TELEMETRY", "ARDUPILOT", None),
    "mavlink": ("TELEMETRY", "MAVLINK", None),
}

options_9x = {
    "heli": ("HELI", "YES", "NO"),
    "templates": ("TEMPLATES", "YES", "NO"),
    "nosplash": ("SPLASH", "NO", "YES"),
    "nofp": ("FLIGHT_MODES", "NO", "YES"),
    "nocurves": ("CURVES", "NO", "YES"),
    "audio": ("AUDIO", "YES", "NO"),
    "voice": ("VOICE", "YES", "NO"),
    "haptic": ("HAPTIC", "YES", "NO"),
    "pwmbl": ("PWM_BACKLIGHT", "YES", "NO"),
    "turnigyfix": ("TURNIGY_TRANSMITTER_FIX", "YES", "NO"),
    "acurStats": ("ACCURAT_THROTTLE_STATS", "YES", "NO"),
    "arithOvfl": ("ARITHMETIC_OVERFLOW_CHECK", "YES", "NO"),
    # "PXX": ("PXX", "YES", "NO"),
    "DSM2": ("DSM2", "PPM ", "NO"),
    "potscroll": ("NAVIGATION", "POTS ", "NO"),
    "rotenc": ("NAVIGATION", "ROTENC ", "NO"),
    "sp22": ("SP22", "YES", "NO"),
    "autosource": ("AUTOSOURCE", "YES", "NO"),
    "autoswitch": ("AUTOSWITCH", "YES", "NO"),
    "dblkeys": ("DBLKEYS", "YES", "NO"),
    "ppmca": ("PPM_CENTER_ADJUSTABLE", "YES", "NO"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "gvars": ("GVARS", "YES", "NO"),
    "symlimits": ("PPM_LIMITS_SYMETRICAL", "YES", "NO"),
    "nographics": ("GRAPHICS", "NO", "YES"),
    "battgraph": ("BATTGRAPH", "YES", "NO"),
    "nobold": ("BOLD", "NO", "YES"),
    "sqt5font": ("FONT", "SQT5 ", None),
    "thrtrace": ("THR_TRACE", "YES", "NO"),
    "pgbar": ("EEPROM_PROGRESS_BAR", "YES", "NO"),
    "imperial": ("UNITS", "IMPERIAL", "METRIC"),
    "nogps": ("GPS", "NO", "YES"),
    "nogauges": ("GAUGES", "NO", "YES"),
    "novario": ("VARIO", "NO", "YES"),
    "nowshh": ("WS_HOW_HIGH", "NO", "YES"),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE ", None),
    "fasoffset": ("FAS_OFFSET", "YES", "NO"),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES"),
    "stickrev": ("FRSKY_STICKS", "YES", "NO")
}

options_9x.update(options_9x_ext)
options_9x.update(tts_avr)

options_9x128 = {
    "heli": ("HELI", "YES", "NO"),
    "templates": ("TEMPLATES", "YES", "NO"),
    "nosplash": ("SPLASH", "NO", "YES"),
    "nofp": ("FLIGHT_MODES", "NO", "YES"),
    "nocurves": ("CURVES", "NO", "YES"),
    "audio": ("AUDIO", "YES", "NO"),
    "voice": ("VOICE", "YES", "NO"),
    "haptic": ("HAPTIC", "YES", "NO"),
    "pwmbl": ("PWM_BACKLIGHT", "YES", "NO"),
    "turnigyfix": ("TURNIGY_TRANSMITTER_FIX", "YES", "NO"),
    # "PXX": ("PXX", "YES", "NO"),
    "DSM2": ("DSM2", "PPM", "NO"),
    "potscroll": ("NAVIGATION", "POTS", "NO"),
    "rotenc": ("NAVIGATION", "ROTENC", "NO"),
    "sp22": ("SP22", "YES", "NO"),
    "autosource": ("AUTOSOURCE", "YES", "NO"),
    "autoswitch": ("AUTOSWITCH", "YES", "NO"),
    "dblkeys": ("DBLKEYS", "YES", "NO"),
    "ppmca": ("PPM_CENTER_ADJUSTABLE", "YES", "NO"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "gvars": ("GVARS", "YES", "NO"),
    "symlimits": ("PPM_LIMITS_SYMETRICAL", "YES", "NO"),
    "nographics": ("GRAPHICS", "NO", "YES"),
    "battgraph": ("BATTGRAPH", "YES", "NO"),
    "nobold": ("BOLD", "NO", "YES"),
    "sqt5font": ("FONT", "SQT5", None),
    "thrtrace": ("THR_TRACE", "YES", "NO"),
    "pgbar": ("EEPROM_PROGRESS_BAR", "YES", "NO"),
    "imperial": ("UNITS", "IMPERIAL", "METRIC"),
    "nogps": ("GPS", "NO", "YES"),
    "nogauges": ("GAUGES", "NO", "YES"),
    "novario": ("VARIO", "NO", "YES"),
    "nowshh": ("WS_HOW_HIGH", "NO", "YES"),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES")
}

options_9x128.update(options_9x_ext)
options_9x128.update(tts_avr)

options_gruvin9x = {
    "heli": ("HELI", "YES", "NO"),
    "templates": ("TEMPLATES", "YES", "NO"),
    "nocurves": ("CURVES", "NO", "YES"),
    "nofp": ("FLIGHT_MODES", "NO", "YES"),
    "sdcard": ("SDCARD", "YES", "NO"),
    "voice": ("VOICE", "YES", "NO"),
    "PXX": ("PXX", "YES", "NO"),
    "DSM2": ("DSM2", "SERIAL", "NO"),
    "DSM2PPM": ("DSM2", "PPM", "NO"),
    "potscroll": ("NAVIGATION", "POTS", "NO"),
    "ppmca": ("PPM_CENTER_ADJUSTABLE", "YES", "NO"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "gvars": ("GVARS", "YES", "NO"),
    "symlimits": ("PPM_LIMITS_SYMETRICAL", "YES", "NO"),
    "autosource": ("AUTOSOURCE", "YES", "NO"),
    "autoswitch": ("AUTOSWITCH", "YES", "NO"),
    "dblkeys": ("DBLKEYS", "YES", "NO"),
    "nographics": ("GRAPHICS", "NO", "YES"),
    "battgraph": ("BATTGRAPH", "YES", "NO"),
    "nobold": ("BOLD", "NO", "YES"),
    "sqt5font": ("FONT", "SQT5", None),
    "pgbar": ("EEPROM_PROGRESS_BAR", "YES", "NO"),
    "imperial": ("UNITS", "IMPERIAL", "METRIC"),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES")
}

options_gruvin9x.update(tts_avr)

options_mega2560 = {
    "heli": ("HELI", "YES", "NO"),
    "templates": ("TEMPLATES", "YES", "NO"),
    "nofp": ("FLIGHT_MODES", "NO", "YES"),
    "nocurves": ("CURVES", "NO", "YES"),
    "PWR": ("PWRMANAGE", "YES", "NO"),
    "sdcard": ("SDCARD", "YES", "NO"),
    "voice": ("VOICE", "YES", "NO"),
    "PXX": ("PXX", "YES", "NO"),
    "DSM2": ("DSM2", "SERIAL", "NO"),
    "DSM2PPM": ("DSM2", "PPM", "NO"),
    "ST7565R": ("LCD", "ST7565R", None),
    "ERC12864FSF": ("LCD", "ERC12864FSF", None),
    "ST7920": ("LCD", "ST7920", None),
    "potscroll": ("NAVIGATION", "POTS", "NO"),
    "ppmca": ("PPM_CENTER_ADJUSTABLE", "YES", "NO"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "gvars": ("GVARS", "YES", "NO"),
    "symlimits": ("PPM_LIMITS_SYMETRICAL", "YES", "NO"),
    "autosource": ("AUTOSOURCE", "YES", "NO"),
    "autoswitch": ("AUTOSWITCH", "YES", "NO"),
    "dblkeys": ("DBLKEYS", "YES", "NO"),
    "nographics": ("GRAPHICS", "NO", "YES"),
    "battgraph": ("BATTGRAPH", "YES", "NO"),
    "nobold": ("BOLD", "NO", "YES"),
    "sqt5font": ("FONT", "SQT5", None),
    "pgbar": ("EEPROM_PROGRESS_BAR", "YES", "NO"),
    "imperial": ("UNITS", "IMPERIAL", "METRIC"),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES")
}

options_mega2560.update(tts_avr)

options_sky9x = {
    "heli": ("HELI", "YES", "NO"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "gvars": ("GVARS", "YES", "NO"),
    "potscroll": ("NAVIGATION", "POTS", "NO"),
    "autosource": ("AUTOSOURCE", "YES", "NO"),
    "autoswitch": ("AUTOSWITCH", "YES", "NO"),
    "dblkeys": ("DBLKEYS", "YES", "NO"),
    "nographics": ("GRAPHICS", "NO", "YES"),
    "battgraph": ("BATTGRAPH", "YES", "NO"),
    "nobold": ("BOLD", "NO", "YES"),
    "sqt5font": ("FONT", "SQT5", None),
    "imperial": ("UNITS", "IMPERIAL", "METRIC"),
    "bluetooth": ("BLUETOOTH", "YES", "NO"),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES"),
    "multimodule": ("MULTIMODULE", "YES", "NO")
}

options_ar9x = {
    "heli": ("HELI", "YES", "NO"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "gvars": ("GVARS", "YES", "NO"),
    "potscroll": ("NAVIGATION", "POTS", "NO"),
    "autosource": ("AUTOSOURCE", "YES", "NO"),
    "autoswitch": ("AUTOSWITCH", "YES", "NO"),
    "dblkeys": ("DBLKEYS", "YES", "NO"),
    "nographics": ("GRAPHICS", "NO", "YES"),
    "battgraph": ("BATTGRAPH", "YES", "NO"),
    "nobold": ("BOLD", "NO", "YES"),
    "sqt5font": ("FONT", "SQT5", None),
    "imperial": ("UNITS", "IMPERIAL", "METRIC"),
    "bluetooth": ("BLUETOOTH", "YES", "NO"),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES"),
    # "rtc": ("RTCLOCK", "YES", "NO"),
    "multimodule": ("MULTIMODULE", "YES", "NO")
}

options_taranis = {
    "noheli": ("HELI", "NO", "YES"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "lua": ("LUA", "YES", "NO_MODEL_SCRIPTS"),
    "luac": ("LUA_COMPILER", "YES", "NO"),
    "haptic": ("HAPTIC", "YES", "NO"),
    "nogvars": ("GVARS", "NO", "YES"),
    "sqt5font": ("FONT", "SQT5", None),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES"),
    "internalppm": ("TARANIS_INTERNAL_PPM", "YES", "NO"),
    "shutdownconfirm": ("SHUTDOWN_CONFIRMATION", "YES", "NO"),
    "eu": ("SUPPORT_D16_EU_ONLY", "YES", "NO"),
    "multimodule": ("MULTIMODULE", "YES", "NO")
}

options_taranisplus = {
    "noheli": ("HELI", "NO", "YES"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "lua": ("LUA", "YES", "NO_MODEL_SCRIPTS"),
    "luac": ("LUA_COMPILER", "YES", "NO"),
    "nogvars": ("GVARS", "NO", "YES"),
    "sqt5font": ("FONT", "SQT5", None),
    "noras": ("RAS", "NO", "YES"),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES"),
    "internalppm": ("TARANIS_INTERNAL_PPM", "YES", "NO"),
    "shutdownconfirm": ("SHUTDOWN_CONFIRMATION", "YES", "NO"),
    "eu": ("SUPPORT_D16_EU_ONLY", "YES", "NO"),
    "multimodule": ("MULTIMODULE", "YES", "NO")
}

options_xlite = {
    "noheli": ("HELI", "NO", "YES"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "lua": ("LUA", "YES", "NO_MODEL_SCRIPTS"),
    "luac": ("LUA_COMPILER", "YES", "NO"),
    "nogvars": ("GVARS", "NO", "YES"),
    "sqt5font": ("FONT", "SQT5", None),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES"),
    "shutdownconfirm": ("SHUTDOWN_CONFIRMATION", "YES", "NO"),
    "eu": ("SUPPORT_D16_EU_ONLY", "YES", "NO"),
    "multimodule": ("MULTIMODULE", "YES", "NO"),
    "stdr9m": ("MODULE_R9M_FULLSIZE", "YES", None)
}

options_taranisx9e = {
    "noheli": ("HELI", "NO", "YES"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "lua": ("LUA", "YES", "NO_MODEL_SCRIPTS"),
    "luac": ("LUA_COMPILER", "YES", "NO"),
    "nogvars": ("GVARS", "NO", "YES"),
    "sqt5font": ("FONT", "SQT5", None),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES"),
    "internalppm": ("TARANIS_INTERNAL_PPM", "YES", "NO"),
    "shutdownconfirm": ("SHUTDOWN_CONFIRMATION", "YES", "NO"),
    "eu": ("SUPPORT_D16_EU_ONLY", "YES", "NO"),
    "horussticks": ("STICKS", "HORUS", "STANDARD"),
    "multimodule": ("MULTIMODULE", "YES", "NO")
}

options_x12s = {
    "noheli": ("HELI", "NO", "YES"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "lua": ("LUA", "YES", "NO_MODEL_SCRIPTS"),
    "luac": ("LUA_COMPILER", "YES", "NO"),
    "nogvars": ("GVARS", "NO", "YES"),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES"),
    "eu": ("SUPPORT_D16_EU_ONLY", "YES", "NO"),
    "multimodule": ("MULTIMODULE", "YES", "NO"),
    "pcbdev": ("PCBREV", "10", None)
}

options_x10 = {
    "noheli": ("HELI", "NO", "YES"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "lua": ("LUA", "YES", "NO_MODEL_SCRIPTS"),
    "luac": ("LUA_COMPILER", "YES", "NO"),
    "nogvars": ("GVARS", "NO", "YES"),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES"),
    "eu": ("SUPPORT_D16_EU_ONLY", "YES", "NO"),
    "multimodule": ("MULTIMODULE", "YES", "NO")
}
