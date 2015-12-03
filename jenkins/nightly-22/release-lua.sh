#!/bin/bash

# stops on first error
set -e

# make sure we are in the good directory
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR
source ./version.sh

# pull the latest changes
./update-repo.sh

# make the stamp
cd opentx/radio/wizard/
zip -r /var/www/html/lua-$version/wizard.zip *.lua *.bmp

