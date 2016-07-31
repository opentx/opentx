#!/bin/bash
 
set -e

branch=master
workdir=/home/opentx/nightly21
output=/var/www/html/2.1/nightly

cd ${workdir}

# Copy git sdcard data
rm -Rf ${workdir}/sdcard
cp -r ${workdir}/code/radio/sdcard .

# Request sound pack generation
wget -qO- http://winbox.open-tx.org/voice-builds/compile21.php?branch=${branch}

# Get sounds
cd sdcard
wget http://winbox.open-tx.org/voice-builds/english-irish-taranis.zip
wget http://winbox.open-tx.org/voice-builds/english-scottish-taranis.zip
wget http://winbox.open-tx.org/voice-builds/english-american-taranis.zip
wget http://winbox.open-tx.org/voice-builds/english-australian-taranis.zip
wget http://winbox.open-tx.org/voice-builds/english-english-taranis.zip
wget http://winbox.open-tx.org/voice-builds/french-taranis.zip

# Prepare the sdcard zip files for Taranis
unzip english-irish-taranis.zip -d  ${workdir}/sdcard/taranis
mv ${workdir}/sdcard/horus/SOUNDS/en ${workdir}/sdcard/taranis/SOUNDS/en-irish
unzip english-scottish-taranis.zip -d  ${workdir}/sdcard/taranis
mv ${workdir}/sdcard/horus/SOUNDS/en ${workdir}/sdcard/taranis/SOUNDS/en-scottish
unzip english-australian-taranis.zip -d  ${workdir}/sdcard/taranis
mv ${workdir}/sdcard/horus/SOUNDS/en ${workdir}/sdcard/taranis/SOUNDS/en-australian
unzip english-american-taranis.zip -d  ${workdir}/sdcard/taranis
mv ${workdir}/sdcard/horus/SOUNDS/en ${workdir}/sdcard/taranis/SOUNDS/en-american
unzip english-english-taranis -d  ${workdir}/sdcard/taranis
unzip french-taranis.zip -d  ${workdir}/sdcard/taranis
wget -O ${workdir}/sdcard/taranis/SOUNDS/en/english.cvs http://winbox.open-tx.org/voice-builds/english-english-taranis.csv
wget -O ${workdir}/sdcard/taranis/SOUNDS/fr/french.cvs http://winbox.open-tx.org/voice-builds/french-taranis.csv

rm -f ${workdir}/sdcard/*.zip
cd ${workdir}/sdcard/taranis && zip -r ${output}/sdcard/sdcard-taranis.zip *
rm -Rf ${workdir}/sdcard
