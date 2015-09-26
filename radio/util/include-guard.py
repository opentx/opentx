#!/usr/bin/env python

import sys, os

for filename in sys.argv[1:]:
    f = file(filename, "r")
    lines = f.readlines()
    f.close()

    newguard = "_" + os.path.basename(filename).upper().replace(".", "_") + "_"

    for i, line in enumerate(lines):
        line = line.strip()
        if line.startswith("#ifndef "):
            guard = line[8:]
            if lines[i+1].strip() == "#define %s" % guard:
                print filename, ":", guard, "=>", newguard
                lines[i] = "#ifndef %s\n" % newguard
                lines[i+1] = "#define %s\n" % newguard
                end = -1
                while not lines[end].strip().startswith("#endif"):
                    end -= 1
                lines[end] = "#endif // %s\n" % newguard
                f = file(filename, "w")
                f.write("".join(lines))
                f.close()
                break 

