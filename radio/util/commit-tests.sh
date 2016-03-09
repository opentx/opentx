#!/bin/bash

# Stops on first error, echo on
set -e
set -x

mkdir build || true
cd build 

# Companion
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=TARANIS $1
make -j2

# OpenTX on 9X stock with FrSky telemetry
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=9X -DHELI=YES -DEXT=FRSKY $1
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on 9X stock with Mavlink telemetry
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=9X -DHELI=YES -DEXT=MAVLINK $1
make -j2 firmware

# OpenTX on Mega2560
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=MEGA2560 -DHELI=YES $1
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Mega2560 with Mavlink telemetry
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=MEGA2560 -DEXT=MAVLINK -DHELI=YES $1
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on gruvin9x board
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=GRUVIN9X -DHELI=YES $1 
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Sky9x
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=SKY9X -DHELI=YES $1 
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Taranis
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=TARANIS -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES $1
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Taranis X9E
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=TARANIS -DPCBREV=REV9E -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES $1 
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Taranis Plus
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=TARANIS -DPCBREV=REVPLUS -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES $1 
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Horus
rm -rf *
cmake -DCMAKE_BUILD_TYPE=Debug -DPCB=HORUS -DHELI=NO -DUSB=SERIAL -DCLI=YES -DDEBUG=YES $1 
make -j2 firmware
make -j2 simu
#make -j2 gtests ; ./gtests

