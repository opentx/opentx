#!/bin/bash

# Stops on first error, echo on
set -e
set -x

cd $1
SRCDIR=$(pwd)
cd -

mkdir build || true
cd build 

COMMON_OPTIONS="-DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/opt/qt55"

# Companion
rm -rf *
cmake ${COMMON_OPTIONS} ${SRCDIR}
make -j2

# OpenTX on 9X stock with FrSky telemetry
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=9X -DHELI=YES -DEXT=FRSKY ${SRCDIR}
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on 9X stock with Mavlink telemetry
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=9X -DHELI=YES -DEXT=MAVLINK ${SRCDIR}
make -j2 firmware

# OpenTX on Mega2560
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=MEGA2560 -DHELI=YES ${SRCDIR}
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Mega2560 with Mavlink telemetry
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=MEGA2560 -DEXT=MAVLINK -DHELI=YES -DAUDIO=YES -DVOICE=YES ${SRCDIR}
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on gruvin9x board
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=GRUVIN9X -DHELI=YES -DAUDIO=YES -DVOICE=YES ${SRCDIR}
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Sky9x
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=SKY9X -DHELI=YES ${SRCDIR}
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on AR9X
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=AR9X -DHELI=YES ${SRCDIR}
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Taranis
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=TARANIS -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES ${SRCDIR}
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Taranis X9E
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=TARANIS -DPCBREV=REV9E -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES ${SRCDIR}
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Taranis Plus
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=TARANIS -DPCBREV=REVPLUS -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES ${SRCDIR}
make -j2 firmware
make -j2 simu
make -j2 gtests ; ./gtests

# OpenTX on Horus
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=HORUS -DHELI=NO -DUSB=SERIAL -DCLI=YES -DDEBUG=YES ${SRCDIR}
make -j2 firmware
make -j2 simu
# make -j2 gtests ; ./gtests
