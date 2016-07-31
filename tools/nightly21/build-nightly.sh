#!/bin/bash
 
set -e

branch=master
docker=nightly21
destdir=/var/www/html/2.1/nightly
workdir=/home/opentx/nightly21
version=21

# Incrementnightly index
index=`cat index.txt`
index=`expr $index + 1`
echo $index > index.txt
suffix="N$index"

cd ${workdir}

# Call sdcard generation
code/tools/nightly22/build-sdcard.sh

# Create on-demand build environment
cp code/radio/util/Dockerfile .
docker build -t new-$docker --build-arg OPENTX_VERSION_SUFFIX=$suffix .
docker rmi $docker || true
docker tag new-$docker $docker 
docker rmi new-$docker

# Make stamps
docker run -dit --name compiler21  -v /home/opentx/$docker:/opentx $docker
docker exec compiler21 make -C /opentx/code/radio/src opentx-stamp
cp -f $rundir/code/radio/src/stamp-opentx.txt $destdir/firmware
cp -f $rundir/code/radio/releasenotes.txt $destdir/firmware

# Build Linux companion
cp binaries/stamp-opentx.txt $output/firmware
docker exec compiler21 rm -rf build
docker exec compiler21 /opentx/code/tools/build-companion.sh /opentx/code /opentx/binaries/ $suffix
docker stop compiler21
docker rm compiler21
cp binaries/*.deb $output/companion/linux/

# Clean binaries It will be hosting built on demand firmware
rm -rf binaries/*
rm -rf binaries/.lock

rel=`grep "VERS_STR" $rundir/code/radio/src/stamp-opentx.h | cut -d' ' -f3 | tr -d '"'`

# Request companion compilation on Windows
cd $output/companion/windows
wget -qO- http://winbox.open-tx.org/companion-builds/compile21.php?branch=master\&suffix=${suffix}
wget -O companion-windows-${rel}.exe http://winbox.open-tx.org/companion-builds/companion-windows-${rel}.exe

# Update windows stamp
rm -f companion-windows.stamp
echo "#define VERSION  "'"$rel'"' >> companion-windows.stamp


