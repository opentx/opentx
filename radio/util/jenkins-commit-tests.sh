#!/bin/bash

# Stops on first error, echo on
set -e
set -x

cd radio/src/

# OpenTX on 9X stock
make clean
make PCB=9X EXT=FRSKY HELI=YES
make simu PCB=9X EXT=FRSKY HELI=YES 
make gtests EXT=FRSKY HELI=YES
./gtests

# OpenTX on Taranis X9E
make clean
make PCB=TARANIS PCBREV=REV9E HELI=YES GVARS=YES LUA=YES
make simu PCB=TARANIS PCBREV=REV9E HELI=YES GVARS=YES LUA=YES
make gtests PCB=TARANIS PCBREV=REV9E HELI=YES GVARS=YES LUA=YES
./gtests

# OpenTX on Taranis Plus
make clean
make PCB=TARANIS PCBREV=REVPLUS HELI=YES LUA=YES
make simu PCB=TARANIS PCBREV=REVPLUS HELI=YES LUA=YES
make gtests PCB=TARANIS PCBREV=REVPLUS HELI=YES LUA=YES
./gtests

# OpenTX on Taranis
make clean
make PCB=TARANIS HELI=YES LUA=YES
make simu PCB=TARANIS HELI=YES LUA=YES
make gtests PCB=TARANIS HELI=YES LUA=YES
./gtests

# OpenTX on Sky9x
make clean
make PCB=SKY9X HELI=YES
make simu PCB=SKY9X HELI=YES
make gtests PCB=SKY9X HELI=YES
./gtests

# OpenTX on Gruvin9x
make clean
make PCB=GRUVIN9X EXT=FRSKY SDCARD=YES
make simu PCB=GRUVIN9X EXT=FRSKY SDCARD=YES

# OpenTX on MEGA2560
make clean
make PCB=MEGA2560

# Companion
cd ../../buildir/
cmake ../companion/src
make
