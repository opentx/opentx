#!/bin/env python

import sys, glob

string = sys.argv[1]
translation = sys.argv[2]
after = sys.argv[3]

def addLine(filename, newline):
  print filename, newline
  lines = file(filename, 'r').readlines()
  for i, line in enumerate(lines):
    if after in line:
      lines.insert(i+1, newline + '\n')
      break
  file(filename, 'w').writelines(lines)

def modifyTranslations():
  for filename in ['translations/en.h'] + glob.glob('translations/*.h.txt'):
    newline = "#define " + string + " "*(23-len(string)) + '"' + translation + '"'
    addLine(filename, newline) 

def modifyDeclaration():
  newline = "extern const pm_char S" + string + "[];"
  filename = "translations.h"
  addLine(filename, newline)

def modifyDefinition():
  newline = "const pm_char S" + string + "[] PROGMEM = " + string + ";"
  filename = "translations.cpp"
  addLine(filename, newline)

modifyTranslations()
modifyDeclaration()
modifyDefinition()

