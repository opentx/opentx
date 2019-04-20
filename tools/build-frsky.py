#!/usr/bin/python3

import argparse
import datetime
import os
from builtins import NotADirectoryError
import shutil


options = {
    "XLITES": {
        "PXX1": "NO",
        "LUA": "NO_MODEL_SCRIPTS",
        "GVARS": "NO"
    },
    "X3": {
        "PXX1": "NO",
        "LUA": "NO_MODEL_SCRIPTS",
        "GVARS": "NO"
    }
}

def timestamp():
    return datetime.datetime.now().strftime("%y%m%d")


def build(board, srcdir):
    cmake_options = " ".join(["-D%s=%s" % (key, value) for key, value in options[board].items()])
    shutil.rmtree("build", ignore_errors=True)
    os.mkdir("build")
    if not os.path.exists("output"):
        os.mkdir("output")
    os.chdir("build")
    os.system("cmake -DPCB=%s %s %s" % (board, cmake_options, srcdir))
    os.system("make firmware -j6")
    os.chdir("..")
    index = 0
    while 1:
        suffix = "" if index == 0 else "_%d" % index
        filename = "output/firmware_%s_%s%s.bin" % (board.lower(), timestamp(), suffix)
        if not os.path.exists(filename):
            os.rename("build/firmware.bin", filename)
            break
        index += 1
    shutil.rmtree("build")


def dir_path(string):
    if os.path.isdir(string):
        return string
    else:
        raise NotADirectoryError(string)


def main():
    parser = argparse.ArgumentParser(description="Build FrSky firmware")
    parser.add_argument("-b", "--boards", action="append", help="Destination boards", required=True)
    parser.add_argument("srcdir", type=dir_path)

    args = parser.parse_args()
    for board in args.boards:
        build(board, args.srcdir)


if __name__ == "__main__":
    main()
