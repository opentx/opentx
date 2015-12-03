#!/bin/bash

# stops on first error
set -e

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $DIR/version.sh

# make sure we are in the good directory
cd ~opentx/release-$version/

# pull the latest changes
./update-repo.sh

# make the stamp
cd opentx/radio/src
make clean
make opentx-stamp
# make lua fields for X9E which has most fields
make lua_exports.inc PCB=TARANIS PCBREV=REV9E

# create the sources tgz in the release directory
cd ../../..
tar czf ./opentx.tgz opentx/radio/src opentx/radio/util

# copy the stamp and the release-notes to the http server
cd ~opentx/release-$version/
cp opentx/radio/src/stamp-opentx.txt /var/www/html/downloads-$version/firmware/
cp opentx/radio/releasenotes.txt /var/www/html/downloads-$version/firmware/
cp opentx/radio/src/lua_fields.txt /var/www/html/downloads-$version/firmware/

# erase all previous builds
rm -f /var/www/html/downloads-$version/firmware/binaries/opentx*.hex
rm -f /var/www/html/downloads-$version/firmware/binaries/opentx*.bin

# clean the sources
cd opentx/radio/src
make clean

