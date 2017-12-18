#!/bin/bash

# stops on first error
set -e
set -x

# make sure we are in the good directory
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd ${DIR}

# pull the latest changes
./update-repo.sh

# retrieve release after the repo update
source ./version.sh

# make the stamp
cd opentx/radio/src
make clean
make stamp.h

# create the sources tgz in the release directory
cd ../../..
tar czf ./opentx.tgz opentx/radio/src opentx/radio/util

# make lua fields for all radio variants
cd opentx/radio/src
make lua/lua_exports_taranis.inc lua/lua_exports_taranis_x9e.inc lua/lua_exports_horus.inc

DESTDIR=/var/www/html/downloads-${version}/nightlies/firmware

# copy the stamp and the release-notes to the http server
cd $DIR
cp opentx/radio/src/stamp.h              ${DESTDIR}/stamp-opentx.txt
cp opentx/radio/releasenotes.txt         ${DESTDIR}
cp opentx/radio/src/lua/lua_fields_*.txt ${DESTDIR}
echo ${OPENTX_VERSION_SUFFIX} >          ${DESTDIR}/suffix.txt

# erase all previous builds
rm -f ${DESTDIR}/binaries/opentx*.hex
rm -f ${DESTDIR}/binaries/opentx*.bin

# clean the sources
cd opentx/radio/src
make clean
