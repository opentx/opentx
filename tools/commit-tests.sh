#!/bin/bash

# Stops on first error, echo on
set -e
set -x

# Allow variable core usage, default uses all cpu cores, to set 8 cores for example : commit-tests.sh -j8
num_cpus=$(nproc)
: "${CORES:=$num_cpus}"
# Default build treats warnings as errors, set -Wno-error to override, e.g.: commit-tests.sh -Wno-error
: "${WERROR:=1}"
# A board name to build for, or ALL
: "${FLAVOR:=ALL}"

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

if [[ -n ${GCC_ARM} ]] ; then
  export PATH=${GCC_ARM}:$PATH
fi

: ${SRCDIR:=$(dirname "$SCRIPT")/..}

: ${COMMON_OPTIONS:="-DCMAKE_BUILD_TYPE=Debug -DTRACE_SIMPGMSPACE=NO -DVERBOSE_CMAKELISTS=YES -DCMAKE_RULE_MESSAGES=OFF -Wno-dev"}
if (( $WERROR )); then COMMON_OPTIONS+=" -DWARNINGS_AS_ERRORS=YES -DMULTIMODULE=YES"; fi

: ${EXTRA_OPTIONS:="$EXTRA_OPTIONS"}

COMMON_OPTIONS+=${EXTRA_OPTIONS}

: ${FIRMARE_TARGET:="firmware-size"}

mkdir build || true
cd build

if [[ " SKY9X ARM9X ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on Sky9x
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=SKY9X -DHELI=YES DLUA=YES -DTELEMETRY=FRSKY -DPPM_LIMITS_SYMETRICAL=YES -DVARIO=YES -DAUTOSWITCH=YES -DAUTOSOURCE=YES -DAUDIO=YES -DGPS=YES -DPPM_CENTER_ADJUSTABLE=YES -DFLIGHT_MODES=YES -DOVERRIDE_CHANNEL_FUNCTION=YES -DFRSKY_STICKS=YES -DGVARS=YES "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " AR9X ARM9X ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on AR9X
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=AR9X -DHELI=YES -DGVARS=YES "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " 9XRPRO ARM9X ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on Sky9x
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=9XRPRO -DHELI=YES -DGVARS=YES "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " X9LITE ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on X9LITE
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X9LITE -DHELI=YES -DGVARS=YES -DTRANSLATIONS=CZ "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " X9LITES X9LITE ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on X9LITES
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X9LITES -DHELI=YES -DGVARS=YES -DMULTIMODULE=NO -DTRANSLATIONS=DE "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " X7 ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on X7
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X7 -DHELI=YES -DGVARS=YES -DTRANSLATIONS=ES "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " X7ACCESS X7 ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on X7 ACCESS
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X7 -DPCBREV=ACCESS -DHELI=YES -DGVARS=YES -DTRANSLATIONS=FR "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " T12 X7 ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on T12
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X7 -DPCBREV=T12 -DHELI=YES -DGVARS=YES -DTRANSLATIONS=IT "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " TX12 X7 ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on TX12
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X7 -DPCBREV=TX12 -DHELI=YES -DGVARS=YES -DTRANSLATIONS=PT "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " T8 X7 ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on T8
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X7 -DPCBREV=T8 -DHELI=YES -DGVARS=YES "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " TLITE X7 ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on TLITE
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X7 -DPCBREV=TLITE -DHELI=YES -DGVARS=YES "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " TX12 X7 ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on TX12
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X7 -DPCBREV=TX12 -DHELI=YES -DGVARS=YES "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " XLITE ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on X-Lite
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=XLITE -DHELI=YES -DGVARS=YES -DTRANSLATIONS=SK "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " XLITES XLITE ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on X-Lites
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=XLITES -DHELI=YES -DGVARS=YES -DTRANSLATIONS=SE "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " X9D X9 ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on X9D
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X9D -DHELI=YES -DLUA=YES -DGVARS=YES -DTRANSLATIONS=PL "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " X9D+ X9 ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on X9D+
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X9D+ -DHELI=YES -DLUA=YES -DGVARS=YES -DTRANSLATIONS=HU "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " X9D+2019 X9 ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on X9D+ 2019
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X9D+ -DPCBREV=2019 -DHELI=YES -DLUA=YES -DGVARS=YES -DTRANSLATIONS=NL "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " X9E X9 ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on Taranis X9E
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X9E -DHELI=YES -DLUA=YES -DGVARS=YES -DPPM_UNIT=PERCENT_PREC1 "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " X10 COLORLCD ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on X10 boards
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X10 -DHELI=YES -DLUA=YES -DGVARS=YES "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " T16 COLORLCD ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on T16 boards
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X10 -DPCBREV=T16 -DHELI=YES -DLUA=YES -DGVARS=YES -DAFHDS3=NO "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " TX16S COLORLCD ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on TX16S boards
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X10 -DPCBREV=TX16S -DHELI=YES -DLUA=YES -DGVARS=YES "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " X12S COLORLCD ALL " =~ \ ${FLAVOR}\  ]] ; then
  # OpenTX on X12S
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" -DPCB=X12S -DHELI=YES -DLUA=YES -DGVARS=YES -DMULTIMODULE=NO "${SRCDIR}"
  make -j"${CORES}" ${FIRMARE_TARGET}
  make -j"${CORES}" libsimulator
  make -j"${CORES}" tests-radio
fi

if [[ " COMPANION ALL " =~ \ ${FLAVOR}\  ]] ; then
  # Companion
  rm -rf ./*
  cmake "${COMMON_OPTIONS}" "${SRCDIR}"
  make -j"${CORES}" companion23 simulator23
  make -j"${CORES}" tests-companion
fi
