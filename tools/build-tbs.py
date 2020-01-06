#!/usr/bin/python3

import argparse
import datetime
import os
from builtins import NotADirectoryError
import shutil
import tempfile


boards = {
    "XLITE": {
        "PCB": "XLITE",
    },
    "XLITES": {
        "PCB": "XLITES",
    },
    "X9LITE": {
        "PCB": "X9LITE",
    },
    "X9LITES": {
        "PCB": "X9LITES",
    },
    "X9D": {
        "PCB": "X9D+",
    },
    "X9D+": {
        "PCB": "X9D+",
    },
    "X9D+2019": {
        "PCB": "X9D+",
        "PCBREV": "2019",
    },
    "X9E": {
        "PCB": "X9E",
    },
    "X7": {
        "PCB": "X7",
    },
    "X7ACCESS": {
        "PCB": "X7",
        "PCBREV": "ACCESS",
    },
    "X10": {
        "PCB": "X10",
    },
    "X10EXPRESS": {
        "PCB": "X10",
        "PCBREV": "EXPRESS",
    },
    "X12S": {
        "PCB": "X12S",
    },
    "T16": {
        "PCB": "X10",
        "PCBREV": "T16",
        "INTERNAL_MODULE_MULTI": "YES"
    },
    "T12": {
        "PCB": "X7",
        "PCBREV": "T12",
    },
}

translations = [
    "EN"
]


def timestamp():
    return datetime.datetime.now().strftime("%y%m%d")


def build(board, translation, srcdir):
    cmake_options = " ".join(["-D%s=%s" % (key, value) for key, value in boards[board].items()])
    cwd = os.getcwd()
    if not os.path.exists("output"):
        os.mkdir("output")
    path = tempfile.mkdtemp()
    os.chdir(path)
    command = "cmake %s -DTRANSLATIONS=%s -DTBS_RELEASE=YES -DTEST_BUILD_WARNING=YES %s" % (cmake_options, translation, srcdir)
    print(command)
    os.system(command)
    os.system("make firmware -j6")
    os.chdir(cwd)
    index = 0
    while 1:
        suffix = "" if index == 0 else "_%d" % index
        filename = "output/firmware_%s_%s_%s%s.bin" % (board.lower(), translation.lower(), timestamp(), suffix)
        if not os.path.exists(filename):
            shutil.copy("%s/firmware.bin" % path, filename)
            break
        index += 1
    shutil.rmtree(path)


def dir_path(string):
    if os.path.isdir(string):
        return string
    else:
        raise NotADirectoryError(string)


def main():
    parser = argparse.ArgumentParser(description="Build FrSky firmware")
    parser.add_argument("-b", "--boards", action="append", help="Destination boards", required=True)
    parser.add_argument("-t", "--translations", action="append", help="Translations", required=True)
    parser.add_argument("srcdir", type=dir_path)

    args = parser.parse_args()

    for board in (boards.keys() if "ALL" in args.boards else args.boards):
        for translation in (translations if "ALL" in args.translations else args.translations):
            build(board, translation, args.srcdir)


if __name__ == "__main__":
    main()
