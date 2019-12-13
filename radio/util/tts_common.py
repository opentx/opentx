def filename(idx):
    ext = ".wav"
    if isinstance(idx, int):
        return "%04d%s" % (idx, ext)
    else:
        return idx + ext
