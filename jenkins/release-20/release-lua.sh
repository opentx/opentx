#!/bin/bash

# stops on first error
set -e

# make sure we are in the good directory
cd ~opentx/release-20/

# pull the latest changes
./update-repo.sh

# make the stamp
cd opentx/radio/wizard/
zip -r /var/www/html/lua-20/wizard.zip *.lua *.bmp

