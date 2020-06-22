#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from __future__ import print_function    # (at top of module)

import sys
import re
import os
import errno
import glob
import argparse
import urllib2
import datetime

DEBUG = False

MODULES = {}
FUNCTIONS = []

STARTMARKER = "[//]: <> (LUADOC-BEGIN:"
ENDMARKER = "[//]: <> (LUADOC-END:"
SUMMARYFILE = "SUMMARY.md"
READMEFILE = "README.md"
DOCBASE = "https://raw.githubusercontent.com/opentx/lua-reference-guide/master/"

def logDebug(txt):
  if DEBUG:
    print(txt)

def logInfo(txt):
  print("Info: %s" % txt)

def logError(txt):
  print("Error: %s" % txt)
  sys.exit(1)


def parseParameters(lines):
  params = []
  for l in lines:
    #l = l.strip(" \t\n")
    #logDebug("l: %s" % l)
    paramName = l.split(" ")[0].strip(" \t\n")
    paramText = " ".join( l.split(" ")[1:] )
    #logDebug("param: %s, desc: %s" %(paramName, paramText))
    params.append( (paramName, paramText) )
  return params

def extractItems(item, doc):
  # find all items
  pattern = "^@" + item + ".*?\n\s*\n"
  items = re.findall(pattern, doc, flags = re.DOTALL | re.MULTILINE)
  #logDebug("Found %d %s items in %s" % (len(items), item, repr(doc)))

  # remove found items from the text
  leftoverLines = doc
  for i in items :
    # print("f:", repr(i))
    leftoverLines = leftoverLines.replace(i, "", 1)
    pass

  # format found items
  result = []
  for i in items:
    result.append( i[len(item)+2:] )

  return result, leftoverLines

def parseFunction(doc):
  functions, lines = extractItems("function", doc + "\n")
  logDebug("Function: %s" % repr(functions))
  funcDefinition = functions[0]
  logDebug("Function: %s" % funcDefinition)
  # get function name
  funcName = funcDefinition.split("(")[0]
  # parse module name
  moduleName = "general"
  try:
    moduleName, funcName = funcName.split(".")
  except:
    pass
  logDebug("module name: %s" % moduleName)
  logDebug("Function name: %s" % funcName)

  params, lines = extractItems("param", lines)
  logDebug("params: %s" % repr(params))
  params = parseParameters(params)

  retvals, lines = extractItems("retval", lines)
  logDebug("retvals: %s" % repr(retvals))
  retvals = parseParameters(retvals)

  notices, lines = extractItems("notice", lines)
  logDebug("notices: %s" % repr(notices))

  description = lines

  #add to registry
  functionObject = (moduleName, funcName, funcDefinition, description, params, retvals, notices)

  if not moduleName in MODULES:
    MODULES[moduleName] = []
  MODULES[moduleName].append(functionObject)

def parseDoc(doc):
  # remove beginning and end delimiter
  doc = doc[9:-3] + "\n"
  logDebug("\n\nDoc:\n %s" % doc)
  # first line defines contents
  firstLine = doc.split('\n')[0].strip(" \t\n")
  if len(firstLine) < 2:
    logError("definition missing in:\n%s" % doc)
  contentType = firstLine.split(" ")[0]
  logDebug("content type: %s" % contentType)
  if contentType == "@function":
    parseFunction(doc)
  elif contentType == "@foobar":
    pass
  else:
    logInfo("Unknown content type: %s" % contentType)


def parseSource(data):
  docs = re.findall("/\*luadoc.*?\*/", data, flags = re.DOTALL)
  logInfo("Found %d documentation sections" % len(docs))
  for doc in docs:
    parseDoc(doc)


def escape(txt):
  return txt.replace("<", "&lt;").replace(">", "&gt;")

def byExtension_key(example):
  # sorts such that display order is notes(.md), example(.lua), output(.png)
  order = ".0" # assume notes
  if example[1] == "lua":
    order = ".1"
  elif example[1] == "png":
    order = ".2"
  return (example[0] + order)

def addExamples(moduleName, funcName):
  doc = ""
  examplePattern = "%s/%s-example*.*" % (moduleName, funcName)
  logDebug("Looking for examples that fit pattern: %s" % examplePattern)
  examples = glob.glob(examplePattern)
  if len(examples) > 0:
    # sort examples considering their extension (.md -> .lua -> .png within the same base name)
    examples = sorted([x.split(".") for x in examples], key = byExtension_key)
    # header
    doc += "\n\n---\n\n### Examples\n\n"
    for example in examples:
      fileName = example[0]+"."+example[1]
      logInfo("Adding contents of example %s" % fileName)
      with open(fileName, "r") as e:
        if example[1] == "md":
          # md files are included verbatim
          doc += e.read()
          doc += "\n\n"
        if example[1] == "lua":
          # add download link before content is included
          doc += "<a class=\"dlbtn\" href=\"%s%s\">%s</a>\n\n" % (DOCBASE, fileName.replace("\\", "/"), example[0].replace("\\", "/"))
          # lua files are escaped in code block
          doc += "```lua\n"
          doc += e.read()
          if doc[-1] != '\n':
            doc += "\n"
          doc += "```\n\n"
        if example[1] == "png":
          # png files are linked as images
          doc += "![](%s)" % os.path.basename(fileName)
          doc += "\n\n"
  return doc

def generateFunctionDoc(f):
  # f = (moduleName, funcName, funcDefinition, description, params, retvals, notices)
  doc = "<!-- This file was generated by the script. Do not edit it, any changes will be lost! -->\n\n"
  
  # name
  doc += "## %s\n\n" % escape(f[2])

  # description
  doc += "%s" % f[3]
  doc += "\n"

  # params
  doc += "#### Parameters\n\n"
  if len(f[4]) == 0:
    doc += "none"
  else:
    for p in f[4]:
      doc += "* `%s` %s" % p
  doc += "\n\n"

  # return values
  doc += "#### Return value\n\n"
  if len(f[5]) == 0:
    doc += "none"
  else:
    for p in f[5]:
      doc += "* `%s` %s" % p
  doc += "\n\n"

  # notices
  if len(f[6]) > 0:
    doc += "##### Notice\n"
    for p in f[6]:
      doc += "%s\n" % p

  # look for other pre-created examples and include them
  doc += addExamples(f[0], f[1])

  return doc

def mkdir_p(path):
  try:
    os.makedirs(path)
  except OSError as exc: # Python >2.5
    if exc.errno == errno.EEXIST and os.path.isdir(path):
      pass
    else: raise

def insertSection(newContents, sectionName):
  logDebug("Inserting section: %s" % sectionName)

  if sectionName == "timestamp":
    newContents.append("%s%s)\n" % (STARTMARKER, sectionName))
    newContents.append("<div class=\"footer\">last updated on %s</div>\n" % datetime.datetime.utcnow().strftime('%Y/%m/%d %H:%M:%S UTC'))
    newContents.append("%s%s)\n" % (ENDMARKER, sectionName))
    return

  newContents.append("   * [%s Functions](%s/%s_functions.md) %s%s)\n" % (sectionName.capitalize(), sectionName, sectionName, STARTMARKER, sectionName))

  # look for an overview for the section and insert it if found
  overviewFileName = "%s/%s_functions-overview.md" % (sectionName, sectionName)
  if os.path.isfile(overviewFileName):
    newContents.append("      * [%s Functions Overview](%s)\n" % (sectionName.capitalize(), overviewFileName))

  for f in sorted(MODULES[sectionName]):
    # f = (moduleName, funcName, funcDefinition, description, params, retvals, notices)
    newContents.append("      * [%s](%s/%s.md)\n" % (f[2].rstrip(), sectionName, f[1]))

  newContents[-1] = "%s %s%s)\n" % (newContents[-1].rstrip(),ENDMARKER, sectionName)

def replaceSections(fileName):
  newContents = []
  ignoreLine = False
  with open(fileName, "r") as data:
    contents = data.readlines()

  for line in contents:
    if line.find(STARTMARKER) >= 0:
      ignoreLine = True
    elif line.find(ENDMARKER) >= 0:
      # parse the section name and call insertSection
      sectionName = line.split(ENDMARKER)[1].split(")")[0]
      insertSection(newContents, sectionName)
      ignoreLine = False
    else:
      if not ignoreLine:
        newContents.append(line)

  with open(fileName, "w+") as out:
    for line in newContents:
      out.write(line)
    logInfo("generated %s" % fileName)

# start of main program

parser = argparse.ArgumentParser(description='Foo bar')
parser.add_argument("-d", "--debug", help="increase output verbosity", action="store_true")
parser.add_argument('files', nargs='*')
args = parser.parse_args()

DEBUG = args.debug

if len(args.files) == 0:
  urlBase = "https://raw.githubusercontent.com/opentx/opentx/master/radio/src/lua/"
  args.files = [ urlBase + f for f in ("api_general.cpp", "api_lcd.cpp", "api_model.cpp")]

for fileName in args.files:
  logInfo("Opening %s" % fileName)
  if fileName.startswith("http"):
    inp = urllib2.urlopen(fileName)
  else:
    inp = open(fileName, "r")
  data = inp.read()
  inp.close()
  parseSource(data)

#show gathered data
for m in MODULES.iterkeys():
  summary = ""
  logDebug("Module: %s" % m)
  for f in sorted(MODULES[m]):
    # f = (moduleName, funcName, funcDefinition, description, params, retvals, notices)
    logDebug("Function: %s" % repr(f))
    doc = generateFunctionDoc(f)
    # print(doc)
    docName = "%s/%s.md" % (f[0], f[1])
    mkdir_p(os.path.dirname(docName))
    with open(docName, "w") as out:
      out.write(doc)
      logInfo("generated %s" % docName)
    if f[0] != "general":
      summary += "       * [%s.%s()](%s)\n" % (f[0], f[1], docName)
    else:
      summary += "       * [%s()](%s)\n" % (f[1], docName)
  print("Summary:")
  print(summary)

#now update SUMMARY.MD replacing the outdated sections
replaceSections(SUMMARYFILE)

#now update the timestamp in README.md
replaceSections(READMEFILE)
