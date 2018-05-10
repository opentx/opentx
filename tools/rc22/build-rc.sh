#!/bin/bash

set -e

branch=2.2
docker=rc22
workdir=/home/opentx/rc22
output=/var/www/html/2.2/rc
version=2.2.2

# Increment RC index
index=`cat index.txt`
index=`expr $index + 1`
suffix="RC$index"

cd ${workdir}

# Create on-demand build environment
cp code/radio/util/Dockerfile .
docker build -t new-${docker} --build-arg OPENTX_VERSION_SUFFIX=${suffix} .
set +e
docker rmi -f ${docker}
set -e
docker tag new-${docker} ${docker}
docker rmi -f new-${docker}

# Call sdcard generation
code/tools/rc22/build-sdcard.sh

# Build Linux companion
docker run -dit --name companion -v /home/opentx/${docker}:/opentx ${docker}
docker exec companion sh -c "mkdir -p build && cd build && cmake /opentx/code && cp radio/src/stamp.h /opentx/binaries/stamp-opentx.txt"
docker exec companion rm -rf build
if [ ! -f ${output}/companion/linux/companion22_${version}${suffix}_amd64.deb ]; then
  docker exec companion /opentx/code/tools/build-companion-release.sh /opentx/code /opentx/binaries/
  docker exec companion sh -c "cp /build/radio/src/lua/*.txt /opentx/binaries"
  cp -f binaries/*.deb ${output}/companion/linux/companion22_${version}${suffix}_amd64.deb
  cp -f binaries/lua_fields_*.txt ${output}/firmware
fi
docker stop companion
docker rm companion

# Request companion compilation on Windows
if [ ! -f ${output}/companion/windows/companion-windows-${version}${suffix}.exe ]; then
  cd ${output}/companion/windows
  wget -qO- http://winbox.open-tx.org/companion-builds/compile22.php?branch=$branch\&suffix=${suffix}
  wget -O companion-windows-${version}${suffix}.exe http://winbox.open-tx.org/companion-builds/companion-windows-${version}${suffix}.exe
  chmod -Rf g+w companion-windows-${version}${suffix}.exe
fi

# Request companion compilation on Mac OS X
if [ ! -f ${output}/companion/macosx/opentx-companion-${version}${suffix}.dmg ]; then
  cd ${output}/companion/macosx
  wget -qO- http://opentx.blinkt.de:8080/~opentx/build-opentx.py?branch=${branch}\&suffix=${suffix}
  wget -O opentx-companion-${version}${suffix}.dmg http://opentx.blinkt.de:8080/~opentx/builds/opentx-companion-${version}${suffix}.dmg
  chmod -Rf g+w opentx-companion-${version}${suffix}.dmg
fi

# Update stamps
cp -f  $workdir/binaries/stamp-opentx.txt ${output}/firmware
echo "#define VERSION  \"${version}${suffix}\"" > ${output}/companion/companion-windows.stamp
cp -f ${output}/companion/companion-windows.stamp ${output}/companion/companion-macosx.stamp
cp -f ${output}/companion/companion-windows.stamp ${output}/companion/companion-linux.stamp


# Clean binaries It will be hosting built on demand firmware
rm -rf $workdir/binaries/*
rm -rf $workdir/binaries/.lock

# RC is considered as valid ony if we get to that point
echo $index > ${workdir}/index.txt
