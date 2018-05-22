#!/bin/bash

# Stops on first error, echo on
set -e
set -x

JOBS=2

while [ $# -gt 0 ]
do
  case "$1" in
    --jobs=*)
      JOBS="${1#*=}";;
    -j*)
      JOBS="${1#*j}";;
    -*)
      echo >&2 "usage: $0 [-j<jobs>|--jobs=<jobs>] SRCDIR OUTDIR VERSION_SUFFIX"
      exit 1;;
    *)
      break;;   # terminate while loop
  esac
  shift
done

SRCDIR=$1
OUTDIR=$2

COMMON_OPTIONS="-DGVARS=YES -DHELI=YES -DLUA=YES -DMULTIMODULE=YES -DTELEMETRY=FRSKY -DPPM_LIMITS_SYMETRICAL=YES -DVARIO=YES -DGAUGES=YES -DAUTOSWITCH=YES -DAUTOSOURCE=YES -DAUDIO=YES -DGPS=YES -DPPM_CENTER_ADJUSTABLE=YES -DFLIGHT_MODES=YES -DOVERRIDE_CHANNEL_FUNCTION=YES -DFRSKY_STICKS=YES"
if [ "$(uname)" = "Darwin" ]; then
    COMMON_OPTIONS="${COMMON_OPTIONS} -DCMAKE_PREFIX_PATH=~/Qt/5.7/clang_64/ -DCMAKE_OSX_DEPLOYMENT_TARGET='10.9'"
fi

if [ "$3" != "" ]; then
  COMMON_OPTIONS="${COMMON_OPTIONS} -DVERSION_SUFFIX=$3"
fi

rm -rf build
mkdir build
cd build

cmake ${COMMON_OPTIONS} -DPCB=AR9X ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=9X ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=9X128 ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=9XR128 ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=9XR ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=GRUVIN9X ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=MEGA2560 ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=SKY9X ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=9XRPRO ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=X7 ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=X9D ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=X9D+ ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=XLITE ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=X9E ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=X10 ${SRCDIR}
make -j${JOBS} libsimulator

cmake ${COMMON_OPTIONS} -DPCB=X12S ${SRCDIR}
make -j${JOBS} libsimulator

make -j${JOBS} package

if [ "$(uname)" = "Darwin" ]; then
    cp *.dmg ${OUTDIR}
else
    cp *.deb ${OUTDIR}
fi
