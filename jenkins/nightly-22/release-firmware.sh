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

# make lua fields for all radio variants
cd opentx/radio/src
make lua/lua_exports_taranis.inc lua/lua_exports_taranis_x9e.inc lua/lua_exports_horus.inc lua/lua_exports_flamenco.inc

# copy the stamp and the release-notes to the http server
cd $DIR
cp opentx/radio/src/stamp.h /var/www/html/downloads-$version/nightly/firmware/stamp-opentx.txt
cp opentx/radio/releasenotes.txt /var/www/html/downloads-$version/nightly/firmware/
cp opentx/radio/src/lua/lua_fields_*.txt /var/www/html/downloads-$version/nightly/firmware/
echo ${OPENTX_VERSION_SUFFIX} > /var/www/html/downloads-$version/nightly/firmware/suffix.txt

# erase all previous builds
rm -f /var/www/html/downloads-$version/nightly/firmware/binaries/opentx*.hex
rm -f /var/www/html/downloads-$version/nightly/firmware/binaries/opentx*.bin

# clean the sources
cd opentx/radio/src
make clean

