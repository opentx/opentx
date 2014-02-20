#!/bin/env python

import math

samples = 1024
amplitudes = (28000, 0, 28000, 0, 28000)

for i in range(samples):
  sample = 0.0
  for harmonic, amplitude in enumerate(amplitudes):    
      sample += math.sin(math.pi*2*i*(harmonic+1)/samples) * amplitude / 2
      if sample > 32767 or sample < -32768:
        print "erreur"
  print "%d," % int(sample),
  if i % 10 == 9:
    print
