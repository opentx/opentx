#!/bin/bash

# Stops on first error, echo on
set -e
set -x

SCRIPT=$(readlink -f "$0")
SRCDIR=$(dirname "$SCRIPT")/..
COMMON_OPTIONS="-DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/opt/qt55"

# Allow variable core usage, default uses two cores, to set 8 cores for example : commit-tests.sh -c=8
for i in "$@"
do
case $i in
    -c=*|--cores=*)
    cores="${i#*=}"
    shift
    ;;
    *)
    cores=2
    ;;
esac
done

mkdir build || true
cd build

# OpenTX on 9X stock with FrSky telemetry
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=9X -DHELI=YES -DTEMPLATES=YES -DTELEMETRY=FRSKY ${SRCDIR}
make -j$cores firmware
make -j$cores simu
make -j$cores gtests ; ./gtests

# OpenTX on 9X stock with Ardupilot telemetry
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=9X -DHELI=YES -DTEMPLATES=YES -DTELEMETRY=ARDUPILOT ${SRCDIR}
make -j$cores firmware

# OpenTX on 9X stock with JETI telemetry
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=9X -DHELI=YES -DTEMPLATES=YES -DTELEMETRY=JETI ${SRCDIR}
make -j$cores firmware

# OpenTX on Mega2560
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=MEGA2560 -DTEMPLATES=YES -DHELI=YES ${SRCDIR}
make -j$cores firmware
make -j$cores simu
make -j$cores gtests ; ./gtests

# OpenTX on Mega2560 with Mavlink telemetry
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=MEGA2560 -DTELEMETRY=MAVLINK -DHELI=YES -DTEMPLATES=YES -DAUDIO=YES -DVOICE=YES ${SRCDIR}
make -j$cores firmware
make -j$cores simu
make -j$cores gtests ; ./gtests

# OpenTX on gruvin9x board
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=GRUVIN9X -DHELI=YES -DTEMPLATES=YES -DAUDIO=YES -DVOICE=YES ${SRCDIR}
make -j$cores firmware
make -j$cores simu
make -j$cores gtests ; ./gtests

# OpenTX on Sky9x
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=SKY9X -DHELI=YES ${SRCDIR}
make -j$cores firmware
make -j$cores simu
make -j$cores gtests ; ./gtests

# OpenTX on AR9X
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=AR9X -DHELI=YES ${SRCDIR}
make -j$cores firmware
make -j$cores simu
make -j$cores gtests ; ./gtests

# OpenTX on X7D
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=X7D -DHELI=YES ${SRCDIR}
make -j$cores firmware
make -j$cores simu
make -j$cores gtests ; ./gtests

# OpenTX on X9D
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=X9D -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES ${SRCDIR}
make -j$cores firmware
make -j$cores simu
make -j$cores gtests ; ./gtests

# OpenTX on X9D+
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=X9D -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES ${SRCDIR}
make -j$cores firmware
make -j$cores simu
make -j$cores gtests ; ./gtests

# OpenTX on Taranis X9E
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=X9E -DHELI=YES -DLUA=YES -DWARNINGS_AS_ERRORS=YES -DPPM_UNIT=PERCENT_PREC1 ${SRCDIR}
make -j$cores firmware
make -j$cores simu
make -j$cores gtests ; ./gtests

# OpenTX on Horus beta boards
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=HORUS -DPCBREV=10 -DHELI=NO -DUSB=SERIAL -DCLI=YES -DDEBUG=YES -DGVARS=YES ${SRCDIR}
make -j$cores firmware
make -j$cores simu
# make -j$cores gtests ; ./gtests

# OpenTX on Horus
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=HORUS -DHELI=NO -DUSB=SERIAL -DCLI=YES -DDEBUG=YES -DGVARS=YES ${SRCDIR}
make -j$cores firmware
make -j$cores simu
# make -j$cores gtests ; ./gtests

# Companion
rm -rf *
cmake ${COMMON_OPTIONS} ${SRCDIR}
make -j$cores
