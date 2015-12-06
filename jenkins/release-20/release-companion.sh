#!/bin/bash

# stops on first error
set -e

# make sure we are in the good directory
cd ~opentx/release/

# pull the latest changes
./update-repo.sh

# retrieve the version number
cd opentx/radio/src
line=`grep "VERSION = " Makefile`
version=`expr "$line" : 'VERSION = \([0-9]\..*\)'`

# create companion rpm
cd ~opentx/release/companion-build/
make package
cp ./companion-$version-i686.rpm /var/www/html/downloads-20/companion/
make stamp
chmod -Rf g+w . || true

# request companion compilation on Windows
cd /var/www/html/downloads-20/companion/
wget -qO- http://winbox.open-tx.org/companion-builds/compile.php?branch=2.0
wget -O companionInstall_$version.exe http://winbox.open-tx.org/companion-builds/companionInstall_$version.exe
mv ~opentx/release/opentx/companion/companion.stamp ./companion-windows.stamp

chmod -Rf g+w . || true

