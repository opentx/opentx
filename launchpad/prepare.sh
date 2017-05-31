#!/bin/sh
set -e
set -x

# $1 is distribution "trusty" OR "yakkety"
#
# Option "-n" or "--nightly" can be used to configure build script to produce a Companion with DALLOW_NIGHTLY_BUILDS=YES flag


cd `dirname ${0}`

nightly_build=0

while [ $# -gt 0 ]
do
  case "$1" in
    --nightly*)
      nightly_build=1;;
    -n*)
      nightly_build=1;;
    -*)
      echo >&2 "usage: $0 [-n|--nightly] DISTRO"
      exit 1;;
    *)
      break;;   # terminate while loop
  esac
  shift
done


distro=${1%/}

. common/suffix

if [ -z ${distro} ] ; then
  echo "Missing distribution name"
  exit 1
fi

if [ ! -d ${distro} ] ; then
  echo "Files for ${distro} not found. Most likely unsupported distribution. Aborting..."
  exit 1
fi

echo "Preparing sources for ${distro}..."

# prepare debian directory
rm -f ../debian/*
mkdir -p ../debian/
cp common/* ../debian/
cp ${distro}/* ../debian/


if [ ${nightly_build} -gt 0 ] ; then
  echo "Editing rules to set version suffix to \"${suffix}\""
  sed -i "s/__VERSION_SUFFIX__/${suffix}/g" ../debian/rules
  echo "Setting nightly build flag"
  sed -i "s/DALLOW_NIGHTLY_BUILDS=YES/DALLOW_NIGHTLY_BUILDS=YES/g" ../debian/rules
else
  sed -i "s/__VERSION_SUFFIX__//g" ../debian/rules
  sed -i "s/DALLOW_NIGHTLY_BUILDS=YES/DALLOW_NIGHTLY_BUILDS=NO/g" ../debian/rules
fi

# remove old package and prepare new one
cd ..
rm -f  ../opentx-companion22_*${distro}*
debuild -S -sa -I -Ijenkins -Ilaunchpad -Idoc -Ibuild -I*.luac

echo "Use this command to upload to launchpad (nightly builds):"
echo "  dput ppa:opentx-test/ppa opentx-companion22_<version>.changes"
echo "OR releases:"
echo "  dput ppa:opentx-test/rel opentx-companion22_<version>.changes"
