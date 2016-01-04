#!/bin/bash

# stops on first error
set -e
set -x

# make sure we are in the good directory
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd ${DIR}

# pull the latest changes
./update-repo.sh

# retrieve release after the repo update
source ./version.sh

# release wizard
cd opentx/radio/wizard/
zip -r /var/www/html/lua-${version}/wizard.zip *.lua *.bmp

