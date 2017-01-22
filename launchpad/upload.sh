#!/bin/sh
set -e
set -x

# $1 is distribution "trusty" OR "yakkety"

cd `dirname ${0}`


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

echo "Uploading ${distro}..."

# prepare debian directory
rm -f ../debian/*
mkdir -p ../debian/
cp common/* ../debian/
cp ${distro}/* ../debian/

echo "Editing rules to set version suffix to \"${suffix}\""
sed -i "s/__VERSION_SUFFIX__/${suffix}/g" ../debian/rules

# remove old package and prepare new one
cd ..
rm -f  ../opentx-companion22_*
debuild -S -sa

# upload to launchpad
cd ..
# ls -l
dput ppa:opentx-test/ppa opentx-companion22_*.changes
