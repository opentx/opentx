#!/bin/bash

# Stops on first error, echo on
set -e
set -x

# Allow variable core usage, default uses two cores, to set 8 cores for example : commit-tests.sh -j8
: ${CORES:=2}
# Default build treats warnings as errors, set -Wno-error to override, e.g.: commit-tests.sh -Wno-error
: ${WERROR:=1}
# A board name to build for, or ALL 
: ${FLAVOR:=ALL}

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
    -Wno-error)
      WERROR=0
      shift
      ;;
    -b*)
      FLAVOR="${i#*b}"
      shift
      ;;
esac
done

if [ "$(uname)" = "Darwin" ]; then
    SCRIPT=$(python -c 'import os,sys;print(os.path.realpath(sys.argv[1]))' "$0")
else
    SCRIPT=$(readlink -f "$0")
fi
#export CMAKE_PREFIX_PATH=/opt/qt${QT_BASE}
if [[ ! -z ${GCC_ARM} ]] ; then
  export PATH=${GCC_ARM}:$PATH
fi

: ${SRCDIR:=$(dirname "$SCRIPT")/..}

: ${COMMON_OPTIONS:="-DCMAKE_BUILD_TYPE=Debug -DTRACE_SIMPGMSPACE=NO -DVERBOSE_CMAKELISTS=YES -DCMAKE_RULE_MESSAGES=OFF -Wno-dev"}
if (( $WERROR )); then COMMON_OPTIONS+=" -DWARNINGS_AS_ERRORS=YES"; fi

: ${TEST_OPTIONS:="--gtest_shuffle --gtest_repeat=5 --gtest_break_on_failure"}

: ${FIRMARE_TARGET:="firmware-size"}

mkdir build || true
cd build

if [[ " 9X AVR9X ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on 9X stock with FrSky telemetry
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=9X -DHELI=YES -DTEMPLATES=YES -DTELEMETRY=FRSKY ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}

  # OpenTX on 9X stock with Ardupilot telemetry
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=9X -DHELI=YES -DTEMPLATES=YES -DTELEMETRY=ARDUPILOT ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}

  # OpenTX on 9X stock with JETI telemetry
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=9X -DHELI=YES -DTEMPLATES=YES -DTELEMETRY=JETI ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
fi

if [[ " MEGA2560 AVR9X ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on Mega2560
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=MEGA2560 -DTEMPLATES=YES -DHELI=YES ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}

  # OpenTX on Mega2560 with Mavlink telemetry
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=MEGA2560 -DTELEMETRY=MAVLINK -DHELI=YES -DTEMPLATES=YES -DAUDIO=YES -DVOICE=YES ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}
fi

if [[ " GRUVIN9X AVR9X ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on gruvin9x board
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=GRUVIN9X -DHELI=YES -DTEMPLATES=YES -DAUDIO=YES -DVOICE=YES ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}
fi

if [[ " SKY9X ARM9X ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on Sky9x
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=SKY9X -DHELI=YES ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}
fi

if [[ " AR9X ARM9X ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on AR9X
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=AR9X -DHELI=YES ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}
fi

if [[ " 9XRPRO ARM9X ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on Sky9x
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=9XRPRO -DHELI=YES ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}
fi

if [[ " X7 ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on X7
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=X7 -DHELI=YES ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}
fi

if [[ " X9D X9 ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on X9D
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=X9D -DHELI=YES -DLUA=YES -DGVARS=YES ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}
fi

if [[ " X9D+ X9 ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on X9D+
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=X9D+ -DHELI=YES -DLUA=YES -DGVARS=YES ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}
fi

if [[ " X9E X9 ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on Taranis X9E
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=X9E -DHELI=YES -DLUA=YES -DGVARS=YES -DPPM_UNIT=PERCENT_PREC1 ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}
fi

if [[ " X12Sr10 X12 ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on Horus beta boards
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=HORUS -DPCBREV=10 -DHELI=YES -DLUA=YES -DGVARS=YES ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}
fi

if [[ " X12S X12 ALL " =~ " ${FLAVOR} " ]] ; then
  # OpenTX on Horus
  rm -rf *
  cmake ${COMMON_OPTIONS} -DPCB=HORUS -DHELI=YES -DLUA=YES -DGVARS=YES ${SRCDIR}
  make -j${CORES} ${FIRMARE_TARGET}
  make -j${CORES} simu
  make -j${CORES} gtests ; ./gtests ${TEST_OPTIONS}
fi

if [[ " DEFAULT ALL " =~ " ${FLAVOR} " ]] ; then
  # Companion
  rm -rf *
  cmake ${COMMON_OPTIONS} ${SRCDIR}
  make -j${CORES}
fi
