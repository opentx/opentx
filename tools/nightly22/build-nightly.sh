#!/bin/bash

set -e

branch=next
docker=nightly22
workdir=/home/opentx/nightly22
output=/var/www/html/2.2/nightly
version=2.2.0

# Incrementnightly index
index=`cat index.txt`
index=`expr $index + 1`
echo $index > index.txt
suffix="N$index"

cd ${workdir}

# Create on-demand build environment
cp code/radio/util/Dockerfile .
docker build -t new-$docker --build-arg OPENTX_VERSION_SUFFIX=$suffix .
docker rmi $docker || true
docker tag new-$docker $docker
docker rmi new-$docker

# Call sdcard generation
code/tools/nightly22/build-sdcard.sh

# Build Linux companion
docker run -dit --name companion -v /home/opentx/$docker:/opentx $docker
docker exec companion sh -c "mkdir -p build && cd build && cmake /opentx/code && cp radio/src/stamp.h /opentx/binaries/stamp-opentx.txt"

cp binaries/stamp-opentx.txt $output/firmware
docker exec companion rm -rf build
docker exec companion /opentx/code/tools/build-companion.sh /opentx/code /opentx/binaries/ $suffix
docker stop companion
docker rm companion
cp binaries/*.deb $output/companion/linux/opentx-companion-linux-${version}$suffix_amd64.deb

# Clean binaries It will be hosting built on demand firmware
rm -rf binaries/*
rm -rf binaries/.lock

# Request companion compilation on Windows
cd $output/companion/windows
wget -qO- http://winbox.open-tx.org/companion-builds/compile22.php?branch=$branch\&suffix=$suffix
wget -O opentx-companion-windows-${version}$suffix.exe http://winbox.open-tx.org/companion-builds/companion-windows-${version}$suffix.exe
chmod -Rf g+w companion-windows-${version}$suffix.exe

# Update windows stamp
rm -f companion-windows.stamp
echo "#define VERSION  "'"2.2.0'$suffix'"' >> companion-windows.stamp
cp $output/companion/windows/companion-windows.stamp $output/companion/linux/companion-windows.stamp

# Request companion compilation on Mac OS X
cd $output/companion/macosx
wget -qO- http://opentx.blinkt.de:8080/~opentx/build-opentx.py?branch=${branch}\&suffix=${suffix}
wget -O opentx-companion-${version}${suffix}.dmg http://opentx.blinkt.de:8080/~opentx/builds/opentx-companion-${version}${suffix}.dmg
chmod -Rf g+w opentx-companion-${version}${suffix}.dmg
