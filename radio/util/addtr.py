#!/usr/bin/env python

import sys
import glob


def add_line(filename, newline, after):
    print(filename, newline)
    with open(filename, 'r') as f:
        lines = f.readlines()
    for i, line in enumerate(lines):
        if after in line:
            lines.insert(i + 1, newline + '\n')
            break
    with open(filename, 'w') as f:
        f.writelines(lines)


def modify_translations(constant, translation, after):
    for filename in glob.glob('translations/*.h.txt'):
        newline = "#define " + constant + " " * max(1, 31 - len(constant)) + '"' + translation + '"'
        add_line(filename, newline, after + " ")


def modify_declaration(constant, after):
    newline = "extern const char S" + constant + "[];"
    filename = "translations.h"
    add_line(filename, newline, after + "[];")


def modify_definition(constant, after):
    newline = "const char S" + constant + "[]  = " + constant + ";"
    filename = "translations.cpp"
    add_line(filename, newline, after + "[] ")


def main():
    after = sys.argv[-1]
    for arg in sys.argv[1:-1]:
        constant, translation = arg.split("=")
        modify_translations(constant, translation, after)
        modify_declaration(constant, after)
        modify_definition(constant, after)
        after = constant


if __name__ == "__main__":
    main()
