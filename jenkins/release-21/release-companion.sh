#!/bin/bash

# stops on first error
set -e

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $DIR/version.sh

# make sure we are in the good directory
cd ~opentx/release-$version/

# pull the latest changes
./update-repo.sh

# retrieve the version number (now done in version.sh)
#cd opentx/radio/src
#line=`grep "VERSION = " Makefile`
#release=`expr "$line" : 'VERSION = \([0-9\.]*\).*'`

# create companion rpm
cd ~opentx/release-$version/companion-build/
cmake ../opentx/companion/src
make package
cp ./companion${version}-$release-i686.rpm /var/www/html/downloads-$version/companion/
make stamp
chmod -Rf g+w . || true

# request companion compilation on Windows
cd /var/www/html/downloads-$version/companion/
wget -qO- http://winbox.open-tx.org/companion-builds/compile.php?branch=$branch
wget -O companion-windows-$release.exe http://winbox.open-tx.org/companion-builds/companion-windows-$release.exe
mv ~opentx/release-$version/opentx/companion/companion.stamp ./companion-windows.stamp

chmod -Rf g+w . || true

