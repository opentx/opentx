#!/bin/bash

# stops on first error
set -e
set -x

# make sure we are in the good directory
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR
source ./version.sh

# pull the latest changes
./update-repo.sh

# make the stamp
cd opentx/radio/src
make clean
make stamp.h

# create the sources tgz in the release directory
cd ../../..
tar czf ./opentx.tgz opentx/radio/src opentx/radio/util

# copy the stamp and the release-notes to the http server
cd $DIR
cp opentx/radio/src/stamp.h /var/www/html/downloads-$version/nightly/firmware/stamp-opentx.txt
cp opentx/radio/releasenotes.txt /var/www/html/downloads-$version/nightly/firmware/
echo ${OPENTX_VERSION_SUFFIX} > /var/www/html/downloads-$version/nightly/firmware/suffix.txt

# make lua fields for different radios
make -C opentx/radio/src clean
make -C opentx/radio/src lua/lua_exports_taranis.inc PCB=TARANIS PCBREV=REV9E
cp opentx/radio/src/lua_fields*.txt /var/www/html/downloads-$version/nightly/firmware/
make -C opentx/radio/src clean
make -C opentx/radio/src lua/lua_exports_horus.inc PCB=HORUS
cp opentx/radio/src/lua_fields*.txt /var/www/html/downloads-$version/nightly/firmware/
make -C opentx/radio/src clean
make -C opentx/radio/src lua/lua_exports_flamenco.inc PCB=FLAMENCO
cp opentx/radio/src/lua_fields*.txt /var/www/html/downloads-$version/nightly/firmware/


# erase all previous builds
rm -f /var/www/html/downloads-$version/nightly/firmware/binaries/opentx*.hex
rm -f /var/www/html/downloads-$version/nightly/firmware/binaries/opentx*.bin

# clean the sources
cd opentx/radio/src
make clean

