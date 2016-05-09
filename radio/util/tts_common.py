NO_ALTERNATE = 1024

import sys

def filename(idx, alternate=0):
    if "gruvin9x" in sys.argv:
        ext = ".ad4"
    else:
        ext = ".wav"
    if isinstance(idx, int):
        result = "%04d%s" % (idx, ext)
    elif board in ('sky9x', 'taranis'):
        result = idx + ext
    else:
        if alternate >= NO_ALTERNATE:
            return None
        result = "%04d%s" % (alternate, ext)
    return result

if "sky9x" in sys.argv:
    board = "sky9x"
    PROMPT_CUSTOM_BASE = 256
    PROMPT_SYSTEM_BASE = 0
elif "taranis" in sys.argv or "horus" in sys.argv:
    board = "taranis"
    PROMPT_CUSTOM_BASE = 256
    PROMPT_SYSTEM_BASE = 0
elif "gruvin9x" in sys.argv:
    board = "gruvin9x"
    PROMPT_CUSTOM_BASE = 0
    PROMPT_SYSTEM_BASE = 256
else:
    board = "stock"
    PROMPT_CUSTOM_BASE = 0
    PROMPT_SYSTEM_BASE = 256
