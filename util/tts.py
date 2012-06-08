#!/usr/bin/env python

import os, sys, shutil, platform, subprocess

def generate(str, idx):
    if platform.system() == "Windows":
        subprocess.Popen(["C:\Program Files\eSpeak\command_line\espeak.exe", "-w", "%04d.wav" % idx, str], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()
        subprocess.Popen(["D:\Perso\workspace\companion9x\AD4CONVERTER.EXE", "-E4", "%04d.WAV" % idx], stdout=subprocess.PIPE, stderr=subprocess.PIPE).wait()       

if __name__ == "__main__":
    
    if "en" in sys.argv:
        for i in range(20):
            generate(str(i), i)
        for i in range(20, 100+1, 10):
            generate(str(i), 20+(i-20)/10)
        generate("1000", 29)
        for i, s in enumerate(["hour", "hours", "minute", "minutes", "second", "seconds", "", "and", "minus"]):
            generate(s, 40+i)
        for i, s in enumerate(["volts", "amps", "meters per second", "", "km per hour", "meters", "degrees", "percent", "milliamps"]):
            generate(s, 50+i)
    