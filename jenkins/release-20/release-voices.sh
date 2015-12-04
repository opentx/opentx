#!/bin/bash

# stops on first error
set -e

# get voice packs
wget -O /dev/null http://winbox.open-tx.org/voice-packs/compile.php
cd /var/www/html/voices-20/opentx-taranis/en/
wget -O english-irish-taranis.zip http://winbox.open-tx.org/voice-packs/english-irish-taranis.zip
wget -O english-scottish-taranis.zip http://winbox.open-tx.org/voice-packs/english-scottish-taranis.zip
wget -O english-american-taranis.zip http://winbox.open-tx.org/voice-packs/english-american-taranis.zip
wget -O english-australian-taranis.zip http://winbox.open-tx.org/voice-packs/english-australian-taranis.zip
# wget -O english-taranis.csv http://winbox.open-tx.org/voice-packs/english-irish-taranis.csv
cd /var/www/html/voices-20/opentx-taranis/fr/
wget -O french-taranis.zip http://winbox.open-tx.org/voice-packs/french-taranis.zip
# wget -O french-taranis.csv http://winbox.open-tx.org/voice-packs/french-taranis.csv

