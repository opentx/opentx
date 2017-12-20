#!/bin/bash

set -e

branch=2.2
workdir=/home/opentx/release22
output=/var/www/html/2.2

# Handle opentx.sdcard.version
sdcard_version="2.2V"$(grep 'set(SDCARD_REVISION' ${workdir}/code/CMakeLists.txt | grep -o '".*"' | sed 's/"//g')
echo ${sdcard_version} > ${workdir}/code/radio/sdcard/horus/opentx.sdcard.version
echo ${sdcard_version} > ${workdir}/code/radio/sdcard/taranis-x9/opentx.sdcard.version
echo ${sdcard_version} > ${workdir}/code/radio/sdcard/taranis-x7/opentx.sdcard.version

if cmp --silent ${workdir}/code/radio/sdcard/horus/opentx.sdcard.version ${workdir}/opentx.sdcard.version
then
  exit
else
  cp -r ${workdir}/code/radio/sdcard/horus/opentx.sdcard.version ${workdir}
  cd ${workdir}

  # Copy git sdcard data
  rm -Rf ${workdir}/sdcard
  cp -r ${workdir}/code/radio/sdcard .

  # Get images for Horus
  mkdir -p ${workdir}/sdcard/horus/IMAGES
  cp /home/opentx/horus-bitmaps/* ${workdir}/sdcard/horus/IMAGES/

  # Get images for Taranis x9
  mkdir -p ${workdir}/sdcard/taranis-x9/IMAGES
  cp /home/opentx/x9-bitmaps/* ${workdir}/sdcard/taranis-x9/IMAGES/

  # Request sound pack generation
  python3 -B ${workdir}/code/tools/release22/tts.py en csv files
  python3 -B ${workdir}/code/tools/release22/tts.py fr csv files
  python3 -B ${workdir}/code/tools/release22/tts.py es csv files
  python3 -B ${workdir}/code/tools/release22/tts.py it csv files
  python3 -B ${workdir}/code/tools/release22/tts.py de csv files
  python3 -B ${workdir}/code/tools/nightly22/tts.py cz csv files
  python3 -B ${workdir}/code/tools/nightly22/tts.py pt csv files
  python3 -B ${workdir}/code/tools/nightly22/tts.py ru csv psv files

  # Create sdcards.zips for supported platforms
  mv /tmp/SOUNDS ${workdir}/sdcard/horus/
  mkdir ${workdir}/sdcard/taranis-x9/SOUNDS
  mkdir ${workdir}/sdcard/taranis-x7/SOUNDS
  cp -r ${workdir}/sdcard/horus/SOUNDS ${workdir}/sdcard/taranis-x9/
  cp -r ${workdir}/sdcard/horus/SOUNDS ${workdir}/sdcard/taranis-x7/
  cd ${workdir}/sdcard/horus && zip -r ${output}/sdcard/x12/sdcard-horus-${sdcard_version}.zip *
  cd ${workdir}/sdcard/taranis-x9 && zip -r ${output}/sdcard/x9/sdcard-taranis-x9-${sdcard_version}.zip *
  cd ${workdir}/sdcard/taranis-x7 && zip -r ${output}/sdcard/x7/sdcard-taranis-x7-${sdcard_version}.zip *
  rm -Rf ${workdir}/sdcard
fi
