#!/bin/bash

# stops on first error
set -e

# make sure we are in the good directory
cd ~opentx/release-20/

# release all components
./release-companion.sh
#./release-voices.sh
#./release-lua.sh
./release-firmware.sh

