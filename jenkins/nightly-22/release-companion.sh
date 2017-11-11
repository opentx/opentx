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

DESTDIR=/var/www/html/downloads-${version}/nightlies/companion

# clean radio source
cd opentx/radio/src
make clean

# create companion rpm
rm -rf   ${DIR}/companion-build/
mkdir -p $_
cd       $_
cmake -DPCB=TARANIS ../opentx
make package
cp ./companion${version}-${release}${OPENTX_VERSION_SUFFIX}-i686.rpm ${DESTDIR}/linux
chmod -Rf g+w ${DESTDIR}/linux/companion${version}-${release}${OPENTX_VERSION_SUFFIX}-i686.rpm

# request companion compilation on Windows
cd ${DESTDIR}
wget -qO- http://winbox.open-tx.org/companion-builds/compile22.php?branch=${branch}\&suffix=${OPENTX_VERSION_SUFFIX}
wget -O windows/companion-windows-${release}${OPENTX_VERSION_SUFFIX}.exe http://winbox.open-tx.org/companion-builds/companion-windows-${release}${OPENTX_VERSION_SUFFIX}.exe
chmod -Rf g+w windows/companion-windows-${release}${OPENTX_VERSION_SUFFIX}.exe

# update windows stamp
cp $DIR/companion-build/version.h ./companion-windows.stamp
chmod -Rf g+w ./companion-windows.stamp
