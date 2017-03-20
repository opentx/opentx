NO_ALTERNATE = 1024
PROMPT_CUSTOM_BASE = 256
PROMPT_SYSTEM_BASE = 0
board = "taranis"

import sys

def filename(idx, alternate=0):
  ext = ".wav"
  if isinstance(idx, int):
    result = "%04d%s" % (idx, ext)
  elif board in ('sky9x', 'taranis'):
    result = idx + ext
  return result


    
    

