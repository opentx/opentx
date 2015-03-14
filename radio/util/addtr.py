#!/bin/env python

import sys, glob

def addLine(filename, newline, after):
  print filename, newline
  lines = file(filename, 'r').readlines()
  for i, line in enumerate(lines):
    if after in line:
      lines.insert(i+1, newline + '\n')
      break
  file(filename, 'w').writelines(lines)

def modifyTranslations(constant, translation, after):
  for filename in glob.glob('translations/*.h.txt'):
    newline = "#define " + constant + " "*max(1, 23-len(constant)) + '"' + translation + '"'
    addLine(filename, newline, after+" ") 

def modifyDeclaration(constant, after):
  newline = "extern const pm_char S" + constant + "[];"
  filename = "translations.h"
  addLine(filename, newline, after+"[];")

def modifyDefinition(constant, after):
  newline = "const pm_char S" + constant + "[] PROGMEM = " + constant + ";"
  filename = "translations.cpp"
  addLine(filename, newline, after+"[] ")


after = sys.argv[-1]
for arg in sys.argv[1:-1]:
  constant, translation = arg.split("=")
  modifyTranslations(constant, translation, after)
  modifyDeclaration(constant, after)
  modifyDefinition(constant, after)
  after = constant

