#!/bin/bash

set -e

branch=next
workdir=/home/opentx/nightly22
output=/var/www/html/2.2/nightly

# Handle opentx.sdcard.version
sdcard_version="2.2V"$(grep 'set(SDCARD_REVISION' ${workdir}/code/CMakeLists.txt | grep -o '".*"' | sed 's/"//g')
echo $sdcard_version > ${workdir}/code/radio/sdcard/horus/opentx.sdcard.version
echo $sdcard_version > ${workdir}/code/radio/sdcard/taranis/opentx.sdcard.version

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

  # Request sound pack generation
  python3 -B ${workdir}/code/tools/nightly22/tts.py en csv files
  python3 -B ${workdir}/code/tools/nightly22/tts.py fr csv files
  python3 -B ${workdir}/code/tools/nightly22/tts.py es csv files
  python3 -B ${workdir}/code/tools/nightly22/tts.py it csv files
  python3 -B ${workdir}/code/tools/nightly22/tts.py de csv files

  # Prepare the sdcard zip files for Horus
  mv /tmp/SOUNDS ${workdir}/sdcard/horus/


  # Duplicate for Taranis and create sdcards.zip
  mkdir ${workdir}/sdcard/taranis/SOUNDS
  cp -r ${workdir}/sdcard/horus/SOUNDS ${workdir}/sdcard/taranis/
  cd ${workdir}/sdcard/taranis && zip -r ${output}/sdcard/sdcard-taranis.zip *
  cd ${workdir}/sdcard/horus && zip -r ${output}/sdcard/sdcard-horus.zip *
  mv ${output}/sdcard/sdcard-horus.zip ${output}/sdcard/sdcard-horus-$sdcard_version.zip
  mv ${output}/sdcard/sdcard-taranis.zip ${output}/sdcard/sdcard-taranis-$sdcard_version.zip
  rm -Rf ${workdir}/sdcard

fi
