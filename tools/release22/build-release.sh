#!/bin/bash

set -e

branch=2.2
docker=release22
workdir=/home/opentx/release22
output=/var/www/html/2.2/release
version=2.2.2

cd ${workdir}

# Create on-demand build environment
cp code/radio/util/Dockerfile .
docker build -t new-${docker} .
docker rmi -f ${docker} || true
docker tag new-${docker} ${docker}
docker rmi -f new-${docker}

# Call sdcard generation
code/tools/release22/build-sdcard.sh

# Build Linux companion
docker run -dit --name companion -v /home/opentx/${docker}:/opentx ${docker}
docker exec companion sh -c "mkdir -p build && cd build && cmake /opentx/code && cp radio/src/stamp.h /opentx/binaries/stamp-opentx.txt"
docker exec companion rm -rf build
if [ ! -f ${output}/companion/linux/companion22_${version}_amd64.deb ]; then
  docker exec companion /opentx/code/tools/build-companion-release.sh /opentx/code /opentx/binaries/
  docker exec companion sh -c "cp /build/radio/src/lua/*.txt /opentx/binaries"
  cp -f binaries/*.deb ${output}/companion/linux/companion22_${version}_amd64.deb
  cp -f binaries/lua_fields_*.txt ${output}/firmware
fi
docker stop companion
docker rm companion

# Request companion compilation on Windows
if [ ! -f ${output}/companion/windows/companion-windows-${version}.exe ]; then
  cd ${output}/companion/windows
  wget -qO- http://winbox.open-tx.org/companion-builds/compile22.php?branch=${branch}
  wget -O companion-windows-${version}.exe http://winbox.open-tx.org/companion-builds/companion-windows-${version}.exe
  chmod -f g+w companion-windows-${version}.exe
fi

# Request companion compilation on Mac OS X
if [ ! -f ${output}/companion/macosx/opentx-companion-${version}.dmg ]; then
  cd ${output}/companion/macosx
  wget -qO- http://opentx.blinkt.de:8080/~opentx/build-opentx.py?branch=${branch}
  wget -O opentx-companion-${version}.dmg http://opentx.blinkt.de:8080/~opentx/builds/opentx-companion-${version}.dmg
  chmod -f g+w opentx-companion-${version}.dmg
fi

# Update stamps
cp -f  $workdir/binaries/stamp-opentx.txt ${output}/firmware
echo "#define VERSION  "'"2.2.2"' > ${output}/companion/companion-windows.stamp
cp -f ${output}/companion/companion-windows.stamp ${output}/companion/companion-macosx.stamp
cp -f ${output}/companion/companion-windows.stamp ${output}/companion/companion-linux.stamp


# Clean binaries It will be hosting built on demand firmware
rm -rf $workdir/binaries/*
rm -rf $workdir/binaries/.lock
