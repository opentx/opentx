#!/usr/bin/python3

import argparse


def line_index(lines, start):
    for i, line in enumerate(lines):
        if line.startswith(start):
            return i


def extract_vars(lines):
    result = []
    lines = lines[line_index(lines, ".data"):line_index(lines, ".memory")]
    i = 0
    while i < len(lines):
        line = lines[i]
        i += 1
        if line.startswith("*"):
            continue
        if line.startswith(" .data.") or line.startswith(" .bss."):
            fields = (line + lines[i]).split()
            # print(fields)
            i += 1
            var = fields[0].split(".")[-1]
            offset = int(fields[1], 16)
            size = int(fields[2], 16)
            result.append((var, offset, size))
    return result


def main():
    parser = argparse.ArgumentParser(description="Extract firmware.map")
    parser.add_argument("file", type=argparse.FileType("r"))

    args = parser.parse_args()

    f = args.file
    lines = f.readlines()

    vars = extract_vars(lines)
    vars.sort(key=lambda var: "%08d %s" % (var[2], var[0]))
    for var, offset, size in vars:
        print("%s\t %d" % (var, size))


if __name__ == "__main__":
    main()
