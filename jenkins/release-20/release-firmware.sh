#!/bin/bash

# stops on first error
set -e

# make sure we are in the good directory
cd ~opentx/release-20/

# pull the latest changes
./update-repo.sh

# make the stamp
cd opentx/radio/src
make clean
make opentx-stamp
make lua_exports.cpp

# create the sources tgz in the release directory
cd ../../..
tar czf ./opentx.tgz opentx/radio/src opentx/radio/util

# copy the stamp and the release-notes to the http server
cd ~opentx/release-20/
cp opentx/radio/src/stamp-opentx.txt /var/www/html/downloads-20/firmware/
cp opentx/radio/releasenotes.txt /var/www/html/downloads-20/firmware/
cp opentx/radio/src/lua_fields.txt /var/www/html/downloads-20/firmware/

# erase all previous builds
rm -f /var/www/html/downloads-20/firmware/binaries/opentx*.hex
rm -f /var/www/html/downloads-20/firmware/binaries/opentx*.bin

# clean the sources
cd opentx/radio/src
make clean

