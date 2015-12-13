#!/usr/bin/env python

import os
import re
import sys

def writeheader(filename,header):
    """
    write a header to filename, 
    skip files where first line after optional shebang matches the skip regex
    filename should be the name of the file to write to
    header should be a list of strings
    skip should be a regex
    """
    f = open(filename,"r")
    inpt =f.readlines()
    f.close()
    output = []

    #comment out the next 3 lines if you don't wish to preserve shebangs
    if len(inpt) > 0 and inpt[0].startswith("#!"): 
        output.append(inpt[0])
        inpt = inpt[1:]

    if inpt[0].strip().startswith("/*"):
        for i, line in enumerate(inpt):
            if line.strip().endswith("*/"):
                inpt = inpt[i+1:]
                break

    while not inpt[0].strip():
        inpt = inpt[1:]

    output.extend(header) #add the header
    for line in inpt:
        output.append(line)
    try:
        f = open(filename,'w')
        f.writelines(output)
        f.close()
        print "added header to %s" %filename
    except IOError,err:
        print "something went wrong trying to add header to %s: %s" % (filename,err)

def main(args=sys.argv):
    headerfile = open(args[1])
    header = headerfile.readlines()
    headerfile.close()
    writeheader(args[2], header)

#call the main method
main()
