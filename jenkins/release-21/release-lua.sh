#!/bin/bash

# stops on first error
set -e

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $DIR/version.sh

# make sure we are in the good directory
cd ~opentx/release-$version/

# pull the latest changes
./update-repo.sh

# make the stamp
cd opentx/radio/wizard/
zip -r /var/www/html/lua-$version/wizard.zip *.lua *.bmp

