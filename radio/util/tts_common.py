PROMPT_CUSTOM_BASE = 256
PROMPT_SYSTEM_BASE = 0


def filename(idx):
    ext = ".wav"
    if isinstance(idx, int):
        return "%04d%s" % (idx, ext)
    else:
        return idx + ext
