#!/bin/bash

# stops on first error
set -e

# make sure we are in the good directory
cd ~opentx/release-20/

# pull the latest changes
./update-repo.sh

# retrieve the version number
cd opentx/radio/src
line=`grep "VERSION = " Makefile`
version=`expr "$line" : 'VERSION = \([0-9]\..*\)'`

# create companion rpm
rm -rf ~opentx/release-20/companion-build/
mkdir -p $_
cd $_
cmake ../opentx/companion/src
make package

cp ./companion-$version-i686.rpm /var/www/html/downloads-20/companion/
make stamp
chmod -Rf g+w . || true

# request companion compilation on Windows
cd /var/www/html/downloads-20/companion/
wget -qO- http://winbox.open-tx.org/companion-builds/compile20.php?branch=2.0
wget -O companionInstall_$version.exe http://winbox.open-tx.org/companion-builds/companionInstall_$version.exe
mv ~opentx/release-20/opentx/companion/companion.stamp ./companion-windows.stamp

chmod -Rf g+w . || true

