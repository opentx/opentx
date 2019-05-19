#!/bin/bash

set -e

branch=2.3
docker=nightly23
workdir=/home/opentx/nightly23
output=/var/www/html/2.3/nightlies
version=2.3.0

echo "Increment nightly index"
index=`cat index.txt`
index=`expr $index + 1`
suffix="N$index"

cd ${workdir}

echo "Create on-demand build environment"
cp code/radio/util/Dockerfile .
docker build -t new-${docker} --build-arg OPENTX_VERSION_SUFFIX=${suffix} .
docker rmi -f ${docker} || true
docker tag new-${docker} ${docker}
docker rmi -f new-${docker}

echo "Call sdcard generation"
code/tools/nightly23/build-sdcard.sh

echo "Delete companion container in case of previous failed atempt"
RUNNING=$(docker inspect --format="{{ .State.Running }}" companion 2> /dev/null)

if [ $? -ne 1 ]; then
  docker rm --force companion
fi

echo "Build firmware stamps"
docker run -dit --name companion -v /home/opentx/${docker}:/opentx ${docker}
docker exec companion sh -c "mkdir -p build && cd build && cmake /opentx/code && cp radio/src/stamp.h /opentx/binaries/stamp-opentx.txt"
cp -f  ${workdir}/binaries/stamp-opentx.txt ${output}/firmware
docker exec companion rm -rf build

echo "Check if Linux companion is needed"
if [ ! -f ${output}/companion/linux/companion23_${version}${suffix}_amd64.deb ]; then
  echo "Build Linux companion"
  docker exec companion /opentx/code/tools/build-companion-nightly.sh /opentx/code /opentx/binaries/ ${suffix}
  cp -f  binaries/*.deb ${output}/companion/linux/companion23_${version}${suffix}_amd64.deb
fi
docker stop companion
docker rm companion

echo "Check if Windows companion is needed"
if [ ! -f ${output}/companion/windows/companion-windows-${version}${suffix}.exe ]; then
  echo "Build Windows companion"
  cd ${output}/companion/windows
  wget -qO- http://winbox.open-tx.org/companion-builds/compile23.php?branch=$branch\&suffix=${suffix}
  wget -O companion-windows-${version}${suffix}.exe http://winbox.open-tx.org/companion-builds/companion-windows-${version}${suffix}.exe
  chmod -Rf g+w companion-windows-${version}${suffix}.exe
fi

echo "Check if Macosc companion is needed"
if [ ! -f ${output}/companion/macosx/opentx-companion-${version}${suffix}.dmg ]; then
  echo "Build Macosx companion"
  cd ${output}/companion/macosx
  wget -qO- http://opentx.blinkt.de:8080/~opentx/build-opentx.py?branch=${branch}\&suffix=${suffix}
  wget -O opentx-companion-${version}${suffix}.dmg http://opentx.blinkt.de:8080/~opentx/builds/opentx-companion-${version}${suffix}.dmg
  chmod -Rf g+w opentx-companion-${version}${suffix}.dmg
fi

echo "Update Companion stamps"
echo "#define VERSION  \"${version}${suffix}\"" > ${output}/companion/companion-windows.stamp
cp -f ${output}/companion/companion-windows.stamp ${output}/companion/companion-linux.stamp
cp -f ${output}/companion/companion-windows.stamp ${output}/companion/companion-macosx.stamp

echo "Clean binaries It will be hosting built on demand firmware"
rm -rf ${workdir}/binaries/*
rm -rf ${workdir}/binaries/.lock

echo "Release $index successfully completed"
echo $index > ${workdir}/index.txt
