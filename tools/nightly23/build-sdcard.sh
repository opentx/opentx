#!/bin/bash

set -e
set -x

workdir=/home/opentx/nightly23
output=/var/www/html/2.3/nightlies

# Handle opentx.sdcard.version
sdcard_version="2.3V"$(grep 'set(SDCARD_REVISION' ${workdir}/code/CMakeLists.txt | grep -o '".*"' | sed 's/"//g')
echo ${sdcard_version} > ${workdir}/code/radio/sdcard/horus/opentx.sdcard.version
echo ${sdcard_version} > ${workdir}/code/radio/sdcard/taranis-x9/opentx.sdcard.version
echo ${sdcard_version} > ${workdir}/code/radio/sdcard/taranis-x7/opentx.sdcard.version

if cmp --silent ${workdir}/code/radio/sdcard/horus/opentx.sdcard.version ${workdir}/opentx.sdcard.version
then
  exit
else
  cd ${workdir}

  # Copy git sdcard data
  rm -Rf ${workdir}/sdcard
  cp -r ${workdir}/code/radio/sdcard .

  # Get images for Horus
  mkdir -p ${workdir}/sdcard/horus/IMAGES
  cp /home/opentx/horus-bitmaps/* ${workdir}/sdcard/horus/IMAGES/


  # Request sound pack generation
  if [[ ! -d /tmp/SOUNDS/fr ]];then
    ${workdir}/code/tools/nightly23/tts.py en csv files
  fi
  if [[ ! -d /tmp/SOUNDS/es ]];then
    ${workdir}/code/tools/nightly23/tts.py fr csv files
  fi
  if [[ ! -d /tmp/SOUNDS/it ]];then
    ${workdir}/code/tools/nightly23/tts.py es csv files
  fi
  if [[ ! -d /tmp/SOUNDS/de ]];then
    ${workdir}/code/tools/nightly23/tts.py it csv files
  fi
  if [[ ! -d /tmp/SOUNDS/cz ]];then
    ${workdir}/code/tools/nightly23/tts.py de csv files
  fi
  if [[ ! -d /tmp/SOUNDS/pl ]];then
    ${workdir}/code/tools/nightly23/tts.py cz csv files
  fi
  if [[ ! -d /tmp/SOUNDS/ru ]];then
    ${workdir}/code/tools/nightly23/tts.py pt csv files
  fi
  ${workdir}/code/tools/nightly23/tts.py ru csv psv files

  # Create sdcards.zips for supported platforms
  mv /tmp/SOUNDS ${workdir}/sdcard/horus/
  mkdir ${workdir}/sdcard/taranis-x9/SOUNDS
  mkdir ${workdir}/sdcard/taranis-x7/SOUNDS
  cp -r ${workdir}/sdcard/horus/SOUNDS ${workdir}/sdcard/taranis-x9/
  cp -r ${workdir}/sdcard/horus/SOUNDS ${workdir}/sdcard/taranis-x7/
  cd ${workdir}/sdcard/horus && zip -r ${output}/sdcard/sdcard-horus-${sdcard_version}.zip *
  cd ${workdir}/sdcard/taranis-x9 && zip -r ${output}/sdcard/sdcard-taranis-x9-${sdcard_version}.zip *
  cd ${workdir}/sdcard/taranis-x7 && zip -r ${output}/sdcard/sdcard-taranis-x7-${sdcard_version}.zip *

  # Create symlinks for other radios
  ln -s ${output}/sdcard/sdcard-taranis-x7-${sdcard_version}.zip ${output}/sdcard/sdcard-taranis-xlite-${sdcard_version}.zip
  ln -s ${output}/sdcard/sdcard-taranis-x7-${sdcard_version}.zip ${output}/sdcard/sdcard-taranis-x9lite-${sdcard_version}.zip
  ln -s ${output}/sdcard/sdcard-taranis-x7-${sdcard_version}.zip ${output}/sdcard/sdcard-jumper-t12-${sdcard_version}.zip
  ln -s ${output}/sdcard/sdcard-horus-${sdcard_version}.zip ${output}/sdcard/sdcard-jumper-t16-${sdcard_version}.zip

  # remove LUA stuff for 9x platform
  rm -Rf ${workdir}/sdcard/taranis-x7/SCRIPTS
  rm -Rf ${workdir}/sdcard/taranis-x7/FrSky-utilities
  cd ${workdir}/sdcard/taranis-x7 && zip -r ${output}/sdcard/sdcard-9xarm-${sdcard_version}.zip *

  rm -Rf ${workdir}/sdcard
  cp -r ${workdir}/code/radio/sdcard/horus/opentx.sdcard.version ${workdir}
fi
