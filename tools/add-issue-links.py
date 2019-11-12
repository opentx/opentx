#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function

import sys
import re


if len(sys.argv) > 1:
    inputFile = sys.argv[1]
    inp = open(inputFile, "r")
else:
    inp = sys.stdin

pattern = re.compile("#\d+")
while True:
    skip = False
    line = inp.readline()
    if len(line) == 0:
        break
    line = line.strip('\r\n')

    if len(line) == 0:
        skip = True
    if line.startswith("<"):
        skip = True
    if line.startswith("["):
        skip = True

    if not skip:
        # line = line.strip()
        # print("line: %s" % line)
        found = re.findall(pattern, line)
        if len(found) > 0:
            for issue in found:
                line = line.replace(issue, '')
            # add issues
            issue_links = ["<a href=https://github.com/opentx/opentx/issues/%d>#%d</a>" % (int(issue[1:]), int(issue[1:])) for issue in found]
            links = "(" + ", ".join(issue_links) + ")"
            line = "<li>" + line + " " + links + "</li>"

        print(line)


inp.close()
sys.exit(0)
