#!/bin/bash

# stops on first error
set -e

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $DIR/version.sh

# get voice packs
wget -O /dev/null $winboxurl/compile.php?branch=$branch
cd /var/www/html/voices-$version/opentx-taranis/en/
wget -O english-irish-taranis.zip $winboxurl/english-irish-taranis.zip
wget -O english-scottish-taranis.zip $winboxurl/english-scottish-taranis.zip
wget -O english-american-taranis.zip $winboxurl/english-american-taranis.zip
wget -O english-australian-taranis.zip $winboxurl/english-australian-taranis.zip
# wget -O english-taranis.csv $winboxurl/english-irish-taranis.csv # doesn't seem to work
cd /var/www/html/voices-$version/opentx-taranis/fr/
wget -O french-taranis.zip $winboxurl/french-taranis.zip
# wget -O french-taranis.csv $winboxurl/french-taranis.csv #outdated

