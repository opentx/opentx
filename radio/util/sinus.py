#!/bin/env python

import math

samples = 1024
amplitudes = (63999, 1024, 512)

for i in range(samples):
  sample = 0.0
  for harmonic, amplitude in enumerate(amplitudes):    
      sample += math.sin(math.pi*2*i*(harmonic+1)/samples) * amplitude / 2
  print "%d," % int(sample),
  if i % 10 == 9:
    print
