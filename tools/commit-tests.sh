#!/bin/bash

# Stops on first error, echo on
set -e
set -x

# Allow variable core usage, default uses two cores, to set 8 cores for example : commit-tests.sh -j8
CORES=2
for i in "$@"
do
case $i in
    --jobs=*)
      CORES="${i#*=}"
      shift
      ;;
    -j*)
      CORES="${i#*j}"
      shift
      ;;
esac
done

if [ "$(uname)" = "Darwin" ]; then
    SCRIPT=$(python -c 'import os,sys;print(os.path.realpath(sys.argv[1]))' "$0")
else
    SCRIPT=$(readlink -f "$0")
fi

SRCDIR=$(dirname "$SCRIPT")/..
COMMON_OPTIONS="-DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/opt/qt55"

mkdir build || true
cd build

# OpenTX on 9X stock with FrSky telemetry
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=9X -DHELI=YES -DTEMPLATES=YES -DTELEMETRY=FRSKY ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# OpenTX on 9X stock with Ardupilot telemetry
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=9X -DHELI=YES -DTEMPLATES=YES -DTELEMETRY=ARDUPILOT ${SRCDIR}
make -j${CORES} firmware

# OpenTX on 9X stock with JETI telemetry
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=9X -DHELI=YES -DTEMPLATES=YES -DTELEMETRY=JETI ${SRCDIR}
make -j${CORES} firmware

# OpenTX on Mega2560
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=MEGA2560 -DTEMPLATES=YES -DHELI=YES ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# OpenTX on Mega2560 with Mavlink telemetry
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=MEGA2560 -DTELEMETRY=MAVLINK -DHELI=YES -DTEMPLATES=YES -DAUDIO=YES -DVOICE=YES ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# OpenTX on gruvin9x board
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=GRUVIN9X -DHELI=YES -DTEMPLATES=YES -DAUDIO=YES -DVOICE=YES ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# OpenTX on Sky9x
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=SKY9X -DHELI=YES ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# OpenTX on AR9X
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=AR9X -DHELI=YES ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# OpenTX on X7
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=X7 -DHELI=YES -DWARNINGS_AS_ERRORS=YES ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# OpenTX on X9D
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=X9D -DHELI=YES -DLUA=YES -DGVARS=YES -DWARNINGS_AS_ERRORS=YES ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# OpenTX on X9D+
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=X9D -DHELI=YES -DLUA=YES -DGVARS=YES -DWARNINGS_AS_ERRORS=YES ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# OpenTX on Taranis X9E
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=X9E -DHELI=YES -DLUA=YES -DGVARS=YES -DWARNINGS_AS_ERRORS=YES -DPPM_UNIT=PERCENT_PREC1 ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# OpenTX on Horus beta boards
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=HORUS -DPCBREV=10 -DHELI=YES -DLUA=YES -DGVARS=YES -DWARNINGS_AS_ERRORS=YES ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# OpenTX on Horus
rm -rf *
cmake ${COMMON_OPTIONS} -DPCB=HORUS -DHELI=YES -DLUA=YES -DGVARS=YES -DWARNINGS_AS_ERRORS=YES ${SRCDIR}
make -j${CORES} firmware
make -j${CORES} simu
make -j${CORES} gtests ; ./gtests --gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure

# Companion
rm -rf *
cmake ${COMMON_OPTIONS} ${SRCDIR}
make -j${CORES}
