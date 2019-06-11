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
    "multimodule": ("MULTIMODULE", "YES", "NO"),
    "flexr9m": ("R9M_PROTO_FLEX", "YES", None)
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
    "multimodule": ("MULTIMODULE", "YES", "NO"),
    "flexr9m": ("R9M_PROTO_FLEX", "YES", None)
}

options_taranis_x9d = {
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
    "internalppm": ("INTERNAL_MODULE_PPM", "YES", "NO"),
    "shutdownconfirm": ("SHUTDOWN_CONFIRMATION", "YES", "NO"),
    "eu": ("SUPPORT_D16_EU_ONLY", "YES", "NO"),
    "multimodule": ("MULTIMODULE", "YES", "NO"),
    "crossfire": ("CROSSFIRE", "YES", "NO")
}

options_taranis_x9dp = {
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
    "internalppm": ("INTERNAL_MODULE_PPM", "YES", "NO"),
    "shutdownconfirm": ("SHUTDOWN_CONFIRMATION", "YES", "NO"),
    "eu": ("SUPPORT_D16_EU_ONLY", "YES", "NO"),
    "multimodule": ("MULTIMODULE", "YES", "NO"),
    "crossfire": ("CROSSFIRE", "YES", "NO")
}

options_taranis_x9lite = {
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
    "shutdownconfirm": ("SHUTDOWN_CONFIRMATION", "YES", "NO"),
    "eu": ("SUPPORT_D16_EU_ONLY", "YES", "NO"),
    "multimodule": ("MULTIMODULE", "YES", "NO"),
    "internalpxx1": ("INTERNAL_MODULE_PXX1", "YES", "NO"),
    "crossfire": ("CROSSFIRE", "YES", "NO")
}

options_taranis_xlite = {
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
    "crossfire": ("CROSSFIRE", "YES", "NO")
}

options_taranis_xlites = {
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
    "internalpxx1": ("INTERNAL_MODULE_PXX1", "YES", "NO"),
    "crossfire": ("CROSSFIRE", "YES", "NO")
}

options_taranis_x9e = {
    "noheli": ("HELI", "NO", "YES"),
    "ppmus": ("PPM_UNIT", "US", "PERCENT_PREC1"),
    "lua": ("LUA", "YES", "NO_MODEL_SCRIPTS"),
    "luac": ("LUA_COMPILER", "YES", "NO"),
    "nogvars": ("GVARS", "NO", "YES"),
    "sqt5font": ("FONT", "SQT5", None),
    "faimode": ("FAI", "YES", None),
    "faichoice": ("FAI", "CHOICE", None),
    "nooverridech": ("OVERRIDE_CHANNEL_FUNCTION", "NO", "YES"),
    "internalppm": ("INTERNAL_MODULE_PPM", "YES", "NO"),
    "shutdownconfirm": ("SHUTDOWN_CONFIRMATION", "YES", "NO"),
    "eu": ("SUPPORT_D16_EU_ONLY", "YES", "NO"),
    "horussticks": ("STICKS", "HORUS", "STANDARD"),
    "multimodule": ("MULTIMODULE", "YES", "NO"),
    "crossfire": ("CROSSFIRE", "YES", "NO")
}

options_horus_x12s = {
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
    "pcbdev": ("PCBREV", "10", None),
    "crossfire": ("CROSSFIRE", "YES", "NO")
}

options_horus_x10 = {
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
    "crossfire": ("CROSSFIRE", "YES", "NO")
}
