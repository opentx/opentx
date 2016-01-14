#!/bin/bash

# Stops on first error, echo on
set -e
set -x

mkdir build
cd build 

# OpenTX on Taranis and Companion
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=TARANIS -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES .. 
make -j2
make -j2 firmware.bin
make -j2 simu
make -j2 gtests
./gtests

# OpenTX on Taranis X9E
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=TARANIS -DPCBREV=REV9E -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES .. 
make -j2 firmware.bin
make -j2 simu
make -j2 gtests
./gtests

# OpenTX on Taranis Plus
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=TARANIS -DPCBREV=REVPLUS -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES .. 
make -j2 firmware.bin
make -j2 simu
make -j2 gtests
./gtests

# OpenTX on Horus
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=HORUS -DHELI=NO -DUSB=SERIAL -DCLI=YES -DDEBUG=YES .. 
make -j2 firmware.bin
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=HORUS -DHELI=NO -DUSB=SERIAL -DCLI=NO -DDEBUG=YES .. 
make -j2 simu
#make gtests
#./gtests


# Old, not yet converted


cd ../radio/src/


# OpenTX on 9X stock
make clean
make        PCB=9X EXT=FRSKY HELI=YES
make simu   PCB=9X EXT=FRSKY HELI=YES 
make gtests EXT=FRSKY HELI=YES
./gtests


# OpenTX on 9X stock with MAVLINK
make clean
make PCB=9X EXT=MAVLINK HELI=YES


# OpenTX on Sky9x
make clean
make        PCB=SKY9X HELI=YES WARNINGS_AS_ERRORS=YES
make simu   PCB=SKY9X HELI=YES
make gtests PCB=SKY9X HELI=YES
./gtests

# OpenTX on Gruvin9x
make clean
make      PCB=GRUVIN9X EXT=FRSKY SDCARD=YES
make simu PCB=GRUVIN9X EXT=FRSKY SDCARD=YES

# OpenTX on MEGA2560
make clean
make PCB=MEGA2560

# OpenTX on MEGA2560 with MAVLINK
make clean
make PCB=MEGA2560 EXT=MAVLINK

